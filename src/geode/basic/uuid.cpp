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

#include <geode/basic/uuid.hpp>

#include <atomic>
#include <iomanip>
#include <sstream>
#include <thread>

#include <absl/hash/hash.h>
#include <absl/random/random.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>

#include <geode/basic/range.hpp>

namespace
{
    /*
     * Modified and simplified version of https://github.com/mq1n/uuid-v7-cpp.
     */

    class UUIDv7Generator
    {
    public:
        UUIDv7Generator() = default;
        ~UUIDv7Generator() = default;

        // Non-copyable: all instances share global state
        UUIDv7Generator( const UUIDv7Generator & ) = delete;
        UUIDv7Generator( UUIDv7Generator && ) = delete;
        UUIDv7Generator &operator=( const UUIDv7Generator & ) = delete;
        UUIDv7Generator &operator=( UUIDv7Generator && ) = delete;

        static constexpr std::uint16_t kSeqMask =
            0x0FFF; // 12‑bit sequence (0-4095)

        // Returns UUID or nullopt if drift/burst limits exceeded
        [[nodiscard]] std::optional< std::array< std::uint8_t, 16 > >
            generate();

        // Diagnostic: virtual clock drift in milliseconds (+ = ahead, - =
        // behind)
        [[nodiscard]] static std::int64_t current_drift_ms()
        {
            auto real_ms = absl::ToUnixMillis( absl::Now() );
            auto state_ms = g_state.load( std::memory_order_acquire ) >> 16;
            return static_cast< std::int64_t >( state_ms ) - real_ms;
        }

    private:
        // Tuning parameters
        static constexpr std::uint64_t kMaxDriftMs =
            2000; // Max drift ahead of real time
        static constexpr unsigned kBackoffThreshold =
            8; // CAS failures before sleep
        static constexpr unsigned kBackoffSleepUs = 1; // Initial sleep duration
        static constexpr unsigned kBackoffMaxUs = 64; // Max sleep duration

        // Global state: [48-bit timestamp_ms][16-bit sequence]
        // All threads compete to update this atomically
        static std::atomic< std::uint64_t > g_state;

        // Per‑thread RNG to avoid contention
        struct TLS
        {
            absl::BitGen gen;

            uint16_t gen_uint16_t()
            {
                return absl::Uniform< uint16_t >( gen, 0, kSeqMask );
            }

            uint8_t gen_uint8_t()
            {
                return absl::Uniform< uint8_t >( gen, 0, 0xFF );
            }
        };
        static thread_local TLS tls;

        // Generate non-zero random sequence (preserves lexicographic ordering)
        static std::uint16_t fresh_sequence()
        {
            std::uint16_t s;
            do
            {
                s = tls.gen_uint16_t();
            } while( s == 0 );

            return s;
        }

        static std::array< std::uint8_t, 16 > encode_uuid(
            std::uint64_t ts, std::uint16_t seq );
    };

    inline std::optional< std::array< std::uint8_t, 16 > >
        UUIDv7Generator::generate()
    {
        std::uint64_t real_ms = absl::ToUnixMillis( absl::Now() );

        unsigned fail_count = 0;
        unsigned backoff_us = kBackoffSleepUs;

        // Main CAS loop
        for( ;; )
        {
            std::uint64_t old = g_state.load( std::memory_order_acquire );
            std::uint64_t old_ts = old >> 16;
            std::uint16_t old_seq =
                static_cast< std::uint16_t >( old & 0xFFFF );

            // Enforce drift cap: limit how fast we can catch up to real time
            if( real_ms > old_ts + kMaxDriftMs )
                real_ms = old_ts + kMaxDriftMs;

            std::uint64_t ts = old_ts;
            std::uint16_t seq = old_seq;

            if( real_ms > old_ts )
            {
                // Time advanced: use new timestamp with fresh random sequence
                ts = real_ms;
                seq = fresh_sequence();
            }
            else
            {
                // Same millisecond: increment sequence or handle wraparound
                if( old_seq == 0 && real_ms == old_ts )
                    seq = fresh_sequence(); // Reset from zero to preserve
                                            // ordering
                else
                    seq =
                        static_cast< std::uint16_t >( ( seq + 1 ) & kSeqMask );

                if( seq == 0 )
                {
                    // Sequence wrapped: advance virtual time by 1ms
                    if( old_ts + 1 > real_ms + kMaxDriftMs )
                        return std::nullopt; // Would exceed drift limit
                    ts += 1;
                    seq = fresh_sequence();
                }
            }

            if( ts > real_ms + kMaxDriftMs )
                return std::nullopt;

            // Attempt atomic update
            const std::uint64_t next = ( ts << 16 ) | seq;
            if( g_state.compare_exchange_weak( old, next,
                    std::memory_order_acq_rel, std::memory_order_relaxed ) )
                return encode_uuid( ts, seq );

            // CAS failed: exponential backoff to reduce contention
            if( ++fail_count >= kBackoffThreshold )
            {
                std::this_thread::sleep_for(
                    std::chrono::microseconds( backoff_us ) );
                backoff_us = std::min( backoff_us * 2, kBackoffMaxUs );
                fail_count = 0;
            }
            real_ms = absl::ToUnixMillis( absl::Now() );
        }
    }

    // Pack timestamp and sequence into RFC 9562 UUID v7 format
    inline std::array< std::uint8_t, 16 > UUIDv7Generator::encode_uuid(
        std::uint64_t ts, std::uint16_t seq )
    {
        std::array< std::uint8_t, 16 > bytes;
        auto *d = bytes.data();

        // 48‑bit big‑endian timestamp (bytes 0‑5)
        d[0] = static_cast< std::uint8_t >( ( ts >> 40 ) & 0xFF );
        d[1] = static_cast< std::uint8_t >( ( ts >> 32 ) & 0xFF );
        d[2] = static_cast< std::uint8_t >( ( ts >> 24 ) & 0xFF );
        d[3] = static_cast< std::uint8_t >( ( ts >> 16 ) & 0xFF );
        d[4] = static_cast< std::uint8_t >( ( ts >> 8 ) & 0xFF );
        d[5] = static_cast< std::uint8_t >( ts & 0xFF );

        // version (0111) + sequence high nibble (byte 6)
        d[6] = static_cast< std::uint8_t >( 0x70 | ( ( seq >> 8 ) & 0x0F ) );
        // sequence low byte (byte 7)
        d[7] = static_cast< std::uint8_t >( seq & 0xFF );

        // variant (10xx) + 6 random bits (byte 8)
        d[8] =
            static_cast< std::uint8_t >( 0x80 | ( tls.gen_uint8_t() & 0x3F ) );
        // remaining 56 random bits (bytes 9‑15)
        for( int i = 9; i < 16; ++i )
            d[i] = tls.gen_uint8_t();

        return bytes;
    }

    // Generate non-zero initial sequence for process startup
    inline std::uint16_t initial_seq()
    {
        std::random_device rd;
        const auto mono = absl::GetCurrentTimeNanos();
        std::uint16_t s = static_cast< std::uint16_t >(
            ( rd() ^ mono ) & UUIDv7Generator::kSeqMask );
        return s ? s : 1;
    }

    // Initialize global state with current time + random sequence
    inline std::atomic< std::uint64_t > UUIDv7Generator::g_state{
        ( static_cast< std::uint64_t >( absl::ToUnixMillis( absl::Now() ) )
            << 16 )
        | initial_seq()
    };

    inline thread_local UUIDv7Generator::TLS UUIDv7Generator::tls{};
} // namespace

namespace geode
{
    uuid::uuid()
    {
        static constexpr index_t MAX_SAFETY_COUNT = 1000;
        static constexpr absl::Duration INITIAL_SLEEP =
            absl::Microseconds( 100 );
        static constexpr absl::Duration MAX_SLEEP = absl::Milliseconds( 50 );
        UUIDv7Generator gen;
        bool generated{ false };
        auto sleep = INITIAL_SLEEP;
        for( const auto i : Range{ MAX_SAFETY_COUNT } )
        {
            geode_unused( i );
            if( auto bytes = gen.generate() )
            {
                bytes_ = bytes.value();
                generated = true;
                break;
            }
            absl::SleepFor( sleep );
            sleep = std::min( sleep * 2., MAX_SLEEP );
        }
        OPENGEODE_EXCEPTION( generated, "[uuid] could not generate uuid" );
    }

    uuid::uuid( std::string_view string )
    {
        OPENGEODE_EXCEPTION( string.size() == 36, "[uuid] wrong string size" );
        OPENGEODE_EXCEPTION( string[8] == '-' && string[13] == '-'
                                 && string[18] == '-' && string[23] == '-',
            "[uuid] unknown string format" );
        std::sscanf( to_string( string ).c_str(),
            "%2hhx%2hhx%2hhx%2hhx-"
            "%2hhx%2hhx-"
            "%2hhx%2hhx-"
            "%2hhx%2hhx-"
            "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
            &bytes_[0], &bytes_[1], &bytes_[2], &bytes_[3], &bytes_[4],
            &bytes_[5], &bytes_[6], &bytes_[7], &bytes_[8], &bytes_[9],
            &bytes_[10], &bytes_[11], &bytes_[12], &bytes_[13], &bytes_[14],
            &bytes_[15] );
    }

    bool uuid::operator==( const uuid &other ) const
    {
        return bytes_ == other.bytes_;
    }

    bool uuid::operator!=( const uuid &other ) const
    {
        return !operator==( other );
    }

    bool uuid::operator<( const uuid &other ) const
    {
        return bytes_ < other.bytes_;
    }

    std::string uuid::string() const
    {
        static constexpr char hex[] = "0123456789abcdef";

        // Positions for each byte in 36-char string (accounting for hyphens)
        static constexpr int pos[16] = { 0, 2, 4, 6, 9, 11, 14, 16, 19, 21, 24,
            26, 28, 30, 32, 34 };

        std::array< char, 36 > buf{};
        buf[8] = buf[13] = buf[18] = buf[23] = '-';

        for( int i = 0; i < 16; ++i )
        {
            unsigned b = bytes_[i];

            buf[pos[i]] = hex[b >> 4];
            buf[pos[i] + 1] = hex[b & 0x0F];
        }

        return { buf.data(), buf.size() };
    }
} // namespace geode

namespace std
{
    size_t hash< geode::uuid >::operator()( const geode::uuid &uuid ) const
    {
        return absl::HashOf( uuid.bytes_ );
    }
} // namespace std