function(_export_library library_name)
    export(TARGETS ${library_name}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${PROJECT_BINARY_DIR}/${CMAKE_CONFIG_DESTINATION}/${PROJECT_NAME}_${library_name}_target.cmake
    )
    install(TARGETS ${library_name}
        EXPORT ${library_name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT public
        ${ARGN}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT public
        ${ARGN}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT public
        ${ARGN}
    )
    install(EXPORT ${library_name}
        FILE ${PROJECT_NAME}_${library_name}_target.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_CONFIG_DESTINATION}
        COMPONENT public
        ${ARGN}
    )
endfunction()

function(add_geode_library)
    cmake_parse_arguments(GEODE_LIB
        "STATIC"
        "NAME;FOLDER"
        "PUBLIC_HEADERS;ADVANCED_HEADERS;PRIVATE_HEADERS;SOURCES;PUBLIC_DEPENDENCIES;PRIVATE_DEPENDENCIES"
        ${ARGN}
    )
    foreach(file ${GEODE_LIB_SOURCES})
        list(APPEND ABSOLUTE_GEODE_LIB_SOURCES 
            "${PROJECT_SOURCE_DIR}/src/${GEODE_LIB_FOLDER}/${file}"
        )
    endforeach()
    foreach(file ${GEODE_LIB_PUBLIC_HEADERS})
        list(APPEND ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS
            "${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/${file}"
        )
    endforeach()
    foreach(file ${GEODE_LIB_ADVANCED_HEADERS})
        list(APPEND ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS
            "${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/${file}"
        )
    endforeach()
    foreach(file ${GEODE_LIB_PRIVATE_HEADERS})
        list(APPEND ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS
            "${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/${file}"
        )
    endforeach()
    if(${GEODE_LIB_STATIC})
        add_library(${GEODE_LIB_NAME} STATIC  
            "${ABSOLUTE_GEODE_LIB_SOURCES}"
            "${ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS}"
            "${ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS}"
            "${ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS}"
        )
    else()
        add_library(${GEODE_LIB_NAME}  
            "${ABSOLUTE_GEODE_LIB_SOURCES}"
            "${ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS}"
            "${ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS}"
            "${ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS}"
        )
        if(CMAKE_STRIP AND BUILD_SHARED_LIBS AND CMAKE_BUILD_TYPE STREQUAL "Release")
            add_custom_command(TARGET ${GEODE_LIB_NAME} 
                POST_BUILD
                COMMAND ${CMAKE_STRIP}
                ARGS --strip-all $<TARGET_FILE:${GEODE_LIB_NAME}>
                COMMENT "Stripping library"
            )
        endif()
    endif()
    add_library(${PROJECT_NAME}::${GEODE_LIB_NAME} ALIAS ${GEODE_LIB_NAME})
    add_dependencies(essential ${GEODE_LIB_NAME})
    string(TOLOWER ${PROJECT_NAME} project-name)
    string(REGEX REPLACE "-" "_" project_name ${project-name})
    set_target_properties(${GEODE_LIB_NAME}
        PROPERTIES
            POSITION_INDEPENDENT_CODE ON
            OUTPUT_NAME ${PROJECT_NAME}_${GEODE_LIB_NAME}
            DEFINE_SYMBOL ${project_name}_${GEODE_LIB_NAME}_EXPORTS
            FOLDER "Libraries"
    )
    source_group("Public Header Files" FILES "${ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS}")
    source_group("Advanced Header Files" FILES "${ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS}")
    source_group("Private Header Files" FILES "${ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS}")
    source_group("Source Files" FILES "${ABSOLUTE_GEODE_LIB_SOURCES}")
    target_include_directories(${GEODE_LIB_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_link_libraries(${GEODE_LIB_NAME}
        PUBLIC ${GEODE_LIB_PUBLIC_DEPENDENCIES}
        PRIVATE ${GEODE_LIB_PRIVATE_DEPENDENCIES}
    )
    _export_library(${GEODE_LIB_NAME})
    generate_export_header(${GEODE_LIB_NAME}
        BASE_NAME ${project_name}_${GEODE_LIB_NAME}
        EXPORT_MACRO_NAME ${project_name}_${GEODE_LIB_NAME}_api
        EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/${GEODE_LIB_FOLDER}/${project_name}_${GEODE_LIB_NAME}_export.h
    )
    install(FILES ${PROJECT_BINARY_DIR}/${GEODE_LIB_FOLDER}/${project_name}_${GEODE_LIB_NAME}_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${GEODE_LIB_FOLDER}
        COMPONENT public
    )
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${GEODE_LIB_FOLDER}
        COMPONENT public
        PATTERN "*/private" EXCLUDE
    )
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${GEODE_LIB_FOLDER}
        COMPONENT private
        FILES_MATCHING
        PATTERN "*/private/*"
    )
    if(MSVC AND BUILD_SHARED_LIBS AND NOT ${GEODE_LIB_STATIC})
        install(FILES $<TARGET_PDB_FILE:${GEODE_LIB_NAME}> 
            DESTINATION bin 
            COMPONENT public
            OPTIONAL
        )
    endif()
endfunction()

macro(_find_dependency_directories directories projects)
    foreach(dependency ${ARGN})
        if(NOT TARGET ${dependency})
            continue()
        endif()
        get_target_property(TARGET_TYPE ${dependency} TYPE)
        if(TARGET_TYPE STREQUAL "SHARED_LIBRARY")
            get_target_property(name ${dependency} NAME)
            string(REPLACE "::" ";" name_list ${name})
            list(GET name_list 0 project)
            list(FIND projects ${project} output)
            if(${output} EQUAL -1)
                list(APPEND projects ${project})
                list(APPEND directories $<TARGET_FILE_DIR:${dependency}>)
            endif()
            get_target_property(dependencies ${dependency} LINK_LIBRARIES)
            if(dependencies)
                _find_dependency_directories(directories projects ${dependencies})
            endif()
        endif()
    endforeach()
endmacro()

function(_add_geode_executable exe_path folder_name)
    get_filename_component(target_name ${exe_path} NAME_WE)

    add_executable(${target_name} ${exe_path})
    foreach(dependency ${ARGN})
        target_link_libraries(${target_name} PRIVATE ${dependency})
    endforeach()
    
    if(WIN32)
        _find_dependency_directories(directories projects ${ARGN})
        list(JOIN directories "\\;" directories)
        set_target_properties(${target_name}
            PROPERTIES
                VS_DEBUGGER_ENVIRONMENT "PATH=${directories}\\;$ENV{Path}"
        )
        if(NOT BUILD_SHARED_LIBS)
            set(pdb_file "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<$<BOOL:${isMultiConfig}>:$<CONFIG>>/${target_name}.pdb")
            add_custom_command(
                TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E remove ${pdb_file}
            )
        endif()
        add_custom_target(run-${target_name}
            COMMAND ${CMAKE_COMMAND} -E env "PATH=${directories}\\;$ENV{Path}" $<TARGET_FILE:${target_name}>
        )
    endif()

    set_target_properties(${target_name}
        PROPERTIES
            FOLDER ${folder_name}
            INSTALL_RPATH "${OS_RPATH}/../${CMAKE_INSTALL_LIBDIR}"
    )
    set(target_name ${target_name} PARENT_SCOPE)
endfunction()

function(add_geode_binary)
    cmake_parse_arguments(GEODE_BINARY
        ""
        "SOURCE"
        "DEPENDENCIES"
        ${ARGN}
    )
    _add_geode_executable(${GEODE_BINARY_SOURCE} "Utilities" ${GEODE_BINARY_DEPENDENCIES})
    install(TARGETS ${target_name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT public
        OPTIONAL
    )
endfunction()

option(USE_BENCHMARK "Toggle benchmarking of tests" OFF)
function(add_geode_test)
    cmake_parse_arguments(GEODE_TEST
        "ESSENTIAL"
        "SOURCE"
        "DEPENDENCIES"
        ${ARGN}
    )
    _add_geode_executable(${GEODE_TEST_SOURCE} "Tests" ${GEODE_TEST_DEPENDENCIES})
    add_test(NAME ${target_name} COMMAND ${target_name})
    if(${GEODE_TEST_ESSENTIAL})
        add_dependencies(essential ${target_name})
        set_tests_properties(${target_name} PROPERTIES LABELS essential) 
    endif()
    set_tests_properties(${target_name} 
        PROPERTIES 
            TIMEOUT 300
            FAIL_REGULAR_EXPRESSION "Sanitizer"
    )
    _find_dependency_directories(directories projects ${GEODE_TEST_DEPENDENCIES})
    if(WIN32)
        list(JOIN directories "\\;" directories)
        set_tests_properties(${target_name}
            PROPERTIES
                ENVIRONMENT "Path=${directories}\\;$ENV{Path}"
        )
    else()
        list(JOIN directories ":" directories)
        set_tests_properties(${target_name}
            PROPERTIES
                ENVIRONMENT "LD_LIBRARY_PATH=${directories}:$ENV{LD_LIBRARY_PATH}"
        )
    endif()
    if(USE_BENCHMARK)
        target_compile_definitions(${target_name} PRIVATE OPENGEODE_BENCHMARK)
    endif()
endfunction()
