/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "../common.hpp"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <variant>

#include <geode/basic/logger.hpp>
#include <geode/basic/logger_client.hpp>
#include <geode/basic/logger_manager.hpp>
#include <geode/basic/progress_logger.hpp>
#include <geode/basic/progress_logger_client.hpp>
#include <geode/basic/progress_logger_manager.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

namespace
{
    double now()
    {
        return std::chrono::duration< double >(
            std::chrono::system_clock::now().time_since_epoch() )
            .count();
    }

    struct LogEvent
    {
        geode::Logger::LEVEL level;
        std::string message;
        double timestamp;
    };

    struct ProgressEvent
    {
        std::string id;
        std::string kind;
        geode::Logger::LEVEL level;
        std::string message;
        geode::index_t current_step;
        geode::index_t nb_steps;
        double timestamp;
    };

    using Event = std::variant< LogEvent, ProgressEvent >;

    /*!
     * Queue of the log and progress events to forward to Python.
     * The clients only push C++ events: they never call into Python since
     * they may be called by C++ worker threads while another thread holds
     * the GIL. Python takes the events with drain().
     */
    class PythonLogBridge
    {
    public:
        static PythonLogBridge& instance()
        {
            // Never destroyed: the registered clients, owned by the logger
            // managers, may push events during static destruction
            static auto* bridge = new PythonLogBridge;
            return *bridge;
        }

        void configure( bool forward_logs, bool forward_progress );

        void push_log( geode::Logger::LEVEL level, const std::string& message )
        {
            {
                const std::lock_guard< std::mutex > lock{ mutex_ };
                if( !forward_logs_ )
                {
                    return;
                }
                events_.emplace_back( LogEvent{ level, message, now() } );
            }
            condition_.notify_one();
        }

        void push_progress( ProgressEvent&& event )
        {
            {
                const std::lock_guard< std::mutex > lock{ mutex_ };
                if( !forward_progress_ )
                {
                    return;
                }
                events_.emplace_back( std::move( event ) );
            }
            condition_.notify_one();
        }

        bool wait( double timeout )
        {
            std::unique_lock< std::mutex > lock{ mutex_ };
            return condition_.wait_for( lock,
                std::chrono::duration< double >( std::max( timeout, 0. ) ),
                [this] {
                    return !events_.empty();
                } );
        }

        std::deque< Event > drain( double timeout )
        {
            std::unique_lock< std::mutex > lock{ mutex_ };
            condition_.wait_for( lock,
                std::chrono::duration< double >( std::max( timeout, 0. ) ),
                [this] {
                    return !events_.empty();
                } );
            return std::exchange( events_, {} );
        }

    private:
        PythonLogBridge() = default;

    private:
        std::once_flag registration_;
        std::mutex mutex_;
        std::condition_variable condition_;
        std::deque< Event > events_;
        bool forward_logs_{ false };
        bool forward_progress_{ false };
    };

    class PythonLoggerClient : public geode::LoggerClient
    {
    public:
        void trace( const std::string& message ) override
        {
            PythonLogBridge::instance().push_log(
                geode::Logger::LEVEL::trace, message );
        }

        void debug( const std::string& message ) override
        {
            PythonLogBridge::instance().push_log(
                geode::Logger::LEVEL::debug, message );
        }

        void info( const std::string& message ) override
        {
            PythonLogBridge::instance().push_log(
                geode::Logger::LEVEL::info, message );
        }

        void warning( const std::string& message ) override
        {
            PythonLogBridge::instance().push_log(
                geode::Logger::LEVEL::warning, message );
        }

        void error( const std::string& message ) override
        {
            PythonLogBridge::instance().push_log(
                geode::Logger::LEVEL::error, message );
        }

        void critical( const std::string& message ) override
        {
            PythonLogBridge::instance().push_log(
                geode::Logger::LEVEL::critical, message );
        }
    };

    class PythonProgressLoggerClient : public geode::ProgressLoggerClient
    {
    public:
        // ProgressLoggerManager serializes the calls to its clients
        void start( const geode::uuid& progress_logger_id,
            geode::Logger::LEVEL level,
            const std::string& message,
            geode::index_t nb_steps ) override
        {
            messages_.emplace( progress_logger_id, message );
            push( progress_logger_id, "start", level, 0, nb_steps );
        }

        void update( const geode::uuid& progress_logger_id,
            geode::Logger::LEVEL level,
            geode::index_t current_step,
            geode::index_t nb_steps ) override
        {
            push( progress_logger_id, "update", level, current_step, nb_steps );
        }

        void completed( const geode::uuid& progress_logger_id,
            geode::Logger::LEVEL level ) override
        {
            push( progress_logger_id, "completed", level, 0, 0 );
            messages_.erase( progress_logger_id );
        }

        void failed( const geode::uuid& progress_logger_id,
            geode::Logger::LEVEL level ) override
        {
            push( progress_logger_id, "failed", level, 0, 0 );
            messages_.erase( progress_logger_id );
        }

    private:
        void push( const geode::uuid& progress_logger_id,
            const char* kind,
            geode::Logger::LEVEL level,
            geode::index_t current_step,
            geode::index_t nb_steps )
        {
            const auto message = messages_.find( progress_logger_id );
            PythonLogBridge::instance().push_progress( ProgressEvent{
                progress_logger_id.string(), kind, level,
                message == messages_.end() ? std::string{} : message->second,
                current_step, nb_steps, now() } );
        }

    private:
        absl::flat_hash_map< geode::uuid, std::string > messages_;
    };

    void PythonLogBridge::configure( bool forward_logs, bool forward_progress )
    {
        std::call_once( registration_, [] {
            geode::LoggerManager::register_client(
                std::make_unique< PythonLoggerClient >() );
            geode::ProgressLoggerManager::register_client(
                std::make_unique< PythonProgressLoggerClient >() );
        } );
        const std::lock_guard< std::mutex > lock{ mutex_ };
        forward_logs_ = forward_logs;
        forward_progress_ = forward_progress;
        events_.erase( std::remove_if( events_.begin(), events_.end(),
                           [this]( const Event& event ) {
                               return std::holds_alternative< LogEvent >(
                                          event )
                                          ? !forward_logs_
                                          : !forward_progress_;
                           } ),
            events_.end() );
    }

    pybind11::list drain_events( double timeout )
    {
        std::deque< Event > events;
        {
            const pybind11::gil_scoped_release release;
            events = PythonLogBridge::instance().drain( timeout );
        }
        pybind11::list result;
        for( auto& event : events )
        {
            if( const auto* log = std::get_if< LogEvent >( &event ) )
            {
                result.append( pybind11::make_tuple(
                    "log", log->level, log->message, log->timestamp ) );
            }
            else
            {
                const auto& progress = std::get< ProgressEvent >( event );
                result.append( pybind11::make_tuple( "progress", progress.id,
                    progress.kind, progress.level, progress.message,
                    progress.current_step, progress.nb_steps,
                    progress.timestamp ) );
            }
        }
        return result;
    }
} // namespace

namespace geode
{
    void define_logger( pybind11::module& module )
    {
        pybind11::class_< Logger,
            std::unique_ptr< Logger, pybind11::nodelete > >
            logger( module, "Logger" );
        pybind11::native_enum< Logger::LEVEL >( logger, "LEVEL", "enum.Enum" )
            .value( "trace", Logger::LEVEL::trace )
            .value( "debug", Logger::LEVEL::debug )
            .value( "info", Logger::LEVEL::info )
            .value( "warning", Logger::LEVEL::warning )
            .value( "error", Logger::LEVEL::error )
            .value( "critical", Logger::LEVEL::critical )
            .value( "off", Logger::LEVEL::off )
            .finalize();
        logger.def_static( "level", &Logger::level, "Current logging level" )
            .def_static( "set_level", &Logger::set_level,
                pybind11::arg( "level" ),
                "Set the minimum level of the messages to display, e.g. "
                "Logger.set_level(Logger.LEVEL.warning) to hide info "
                "messages" )
            .def_static(
                "is_enabled", &Logger::is_enabled, pybind11::arg( "level" ) )
            .def_static( "trace", &Logger::trace< std::string >,
                pybind11::arg( "message" ) )
            .def_static( "debug", &Logger::debug< std::string >,
                pybind11::arg( "message" ) )
            .def_static( "info", &Logger::info< std::string >,
                pybind11::arg( "message" ) )
            .def_static( "warning", &Logger::warning< std::string >,
                pybind11::arg( "message" ) )
            .def_static( "error", &Logger::error< std::string >,
                pybind11::arg( "message" ) )
            .def_static( "critical", &Logger::critical< std::string >,
                pybind11::arg( "message" ) )
            .def_static( "_set_console_output",
                &LoggerManager::set_console_output, pybind11::arg( "enabled" ) )
            .def_static( "_is_console_output_enabled",
                &LoggerManager::is_console_output_enabled )
            .def_static(
                "_set_bridge",
                []( bool forward_logs, bool forward_progress ) {
                    PythonLogBridge::instance().configure(
                        forward_logs, forward_progress );
                },
                pybind11::arg( "forward_logs" ),
                pybind11::arg( "forward_progress" ),
                "Use Logger.forward_to_python and ProgressLogger.add_listener "
                "instead" )
            .def_static(
                "_log_from_threads",
                []( Logger::LEVEL level, const std::string& message,
                    index_t nb_threads, index_t nb_messages ) {
                    // Keeps the GIL on purpose: the threads must log without
                    // ever needing it
                    std::vector< std::thread > threads;
                    for( const auto thread_id : Range{ nb_threads } )
                    {
                        threads.emplace_back( [&, thread_id] {
                            for( const auto message_id : Range{ nb_messages } )
                            {
                                Logger::log( level, message, " ", thread_id,
                                    " ", message_id );
                            }
                        } );
                    }
                    for( auto& thread : threads )
                    {
                        thread.join();
                    }
                },
                pybind11::arg( "level" ), pybind11::arg( "message" ),
                pybind11::arg( "nb_threads" ), pybind11::arg( "nb_messages" ),
                "Test helper: log '<message> <thread> <index>' from C++ worker "
                "threads while the calling thread keeps the GIL" )
            .def_static(
                "_wait",
                []( double timeout ) {
                    const pybind11::gil_scoped_release release;
                    return PythonLogBridge::instance().wait( timeout );
                },
                pybind11::arg( "timeout" ),
                "Wait up to timeout seconds, without the GIL, for log and "
                "progress events and return True if there are some" )
            .def_static( "_drain", &drain_events, pybind11::arg( "timeout" ),
                "Wait up to timeout seconds, without the GIL, for log and "
                "progress events and return them" );

        pybind11::class_< ProgressLogger >( module, "ProgressLogger" )
            .def(
                pybind11::init< Logger::LEVEL, const std::string&, index_t >(),
                pybind11::arg( "level" ), pybind11::arg( "message" ),
                pybind11::arg( "nb_steps" ) )
            .def( "increment", static_cast< index_t ( ProgressLogger::* )() >(
                                   &ProgressLogger::increment ) )
            .def( "increment",
                static_cast< index_t ( ProgressLogger::* )( index_t ) >(
                    &ProgressLogger::increment ),
                pybind11::arg( "nb_increments" ) )
            .def( "increment_nb_steps",
                static_cast< index_t ( ProgressLogger::* )() >(
                    &ProgressLogger::increment_nb_steps ) )
            .def( "increment_nb_steps",
                static_cast< index_t ( ProgressLogger::* )( index_t ) >(
                    &ProgressLogger::increment_nb_steps ),
                pybind11::arg( "nb_steps" ) )
            .def(
                "set_refresh_interval",
                []( ProgressLogger& progress, double seconds ) {
                    progress.set_refresh_interval( absl::Seconds( seconds ) );
                },
                pybind11::arg( "seconds" ) );
    }
} // namespace geode
