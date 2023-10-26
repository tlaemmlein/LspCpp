cmake_minimum_required(VERSION 3.8)

if(POLICY CMP0025)
    # detect Apple's Clang
    cmake_policy(SET CMP0025 NEW)
endif()
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

set(LIB_MAJOR_VERSION "1")
set(LIB_MINOR_VERSION "0")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

# Without this, paths are not relative in the sources list
cmake_policy(SET CMP0076 NEW)
project(lspcpp VERSION "${LIB_VERSION_STRING}" LANGUAGES CXX C)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
SET(GOOGLETEST_VERSION "0.00")

# compile in RelWithDebInfo  mode by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

###########################################################
# Options
###########################################################
function (option_if_not_defined name description default)
    if(NOT DEFINED ${name})
        option(${name} ${description} ${default})
    endif()
endfunction()

option_if_not_defined(USE_SYSTEM_RAPIDJSON "Use system RapidJSON instead of the git submodule if exists" OFF)
option_if_not_defined(LSPCPP_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option_if_not_defined(LSPCPP_BUILD_EXAMPLES "Build example applications" OFF)
option_if_not_defined(LSPCPP_BUILD_FUZZER "Build fuzzer" OFF)
option_if_not_defined(LSPCPP_BUILD_WEBSOCKETS "Build websocket server" ON)
option_if_not_defined(LSPCPP_ASAN "Build lsp with address sanitizer" OFF)
option_if_not_defined(LSPCPP_MSAN "Build lsp with memory sanitizer" OFF)
option_if_not_defined(LSPCPP_TSAN "Build lsp with thread sanitizer" OFF)
option_if_not_defined(LSPCPP_INSTALL "Create lsp install target" OFF)

###########################################################
# Directories
###########################################################
function (set_if_not_defined name value)
    if(NOT DEFINED ${name})
        set(${name} ${value} PARENT_SCOPE)
    endif()
endfunction()

set(LSPCPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_if_not_defined(LSPCPP_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

###########################################################
# Functions
###########################################################
function(lspcpp_set_target_options target)

    set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)

    # Enable C++14 (Required)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 14)

    set_property(TARGET ${target} PROPERTY CXX_EXTENSIONS OFF)

    # Enable all warnings
    if(MSVC)
        target_compile_options(${target} PRIVATE "-W4")
    else()
        target_compile_options(${target} PRIVATE "-Wall")
    endif()

    # Disable specific, pedantic warnings
    if(MSVC)
        target_compile_options(${target} PRIVATE
                "-D_CRT_SECURE_NO_WARNINGS"

                # Warnings from nlohmann/json headers.
                "/wd4267" # 'argument': conversion from 'size_t' to 'int', possible loss of data
                "/bigobj" # for visual studio 2022 x64 or later.
                )
    endif()

    # Add define for JSON library in use
    set_target_properties(${target} PROPERTIES
            COMPILE_DEFINITIONS "LSPCPP_JSON_${LSPCPP_JSON_LIBRARY_UPPER}=1"
            )

    # Treat all warnings as errors
    if(LSPCPP_WARNINGS_AS_ERRORS)
        if(MSVC)
            target_compile_options(${target} PRIVATE "/WX")
        else()
            target_compile_options(${target} PRIVATE "-Werror")
        endif()
    endif(LSPCPP_WARNINGS_AS_ERRORS)

    if(LSPCPP_ASAN)
        target_compile_options(${target} PUBLIC "-fsanitize=address")
        target_link_libraries(${target} PUBLIC "-fsanitize=address")
    elseif(LSPCPP_MSAN)
        target_compile_options(${target} PUBLIC "-fsanitize=memory")
        target_link_libraries(${target} PUBLIC "-fsanitize=memory")
    elseif(LSPCPP_TSAN)
        target_compile_options(${target} PUBLIC "-fsanitize=thread")
        target_link_libraries(${target} PUBLIC "-fsanitize=thread")
    endif()

    # Error on undefined symbols
    # if(NOT MSVC)
    #     target_compile_options(${target} PRIVATE "-Wl,--no-undefined")
    # endif()

endfunction()


# Libraries

set(Uri_BUILD_TESTS OFF)
add_subdirectory(third_party/uri)

###########################################################
# boost library
###########################################################
message("Start finding boost...")
if (MSVC)
    message(status "Uri_USE_STATIC_CRT = ${Uri_USE_STATIC_CRT}")
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
endif()
set(Boost_LIBRARIES date_time chrono filesystem system thread program_options)
find_package(Boost COMPONENTS ${Boost_LIBRARIES})
if(NOT Boost_FOUND)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        message(FATAL_ERROR "Can't find boost,lease build boost and install it or install boost with : brew install boost")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        message(FATAL_ERROR "Can't find boost,please build boost and install it. or install boost with : sudo apt-get install libboost-dev")
    else()
        message(FATAL_ERROR "Can't find boost,please set BOOST_ROOT, e.g. -DBOOST_ROOT=<Your Path to Boost>")
    endif()
endif()


###########################################################
# JSON library
###########################################################
if(USE_SYSTEM_RAPIDJSON)
    set(RapidJSON_MIN_VERSION "1.1.0")
    find_package(RapidJSON ${RapidJSON_MIN_VERSION} QUIET)
    if(NOT DEFINED RapidJSON_INCLUDE_DIRS AND DEFINED RAPIDJSON_INCLUDE_DIRS)
        set(RapidJSON_INCLUDE_DIRS "${RAPIDJSON_INCLUDE_DIRS}")
    endif()
endif()
if(NOT RapidJSON_FOUND)
    if(EXISTS "${PROJECT_SOURCE_DIR}/third_party/rapidjson/include")
        message(STATUS "Using local RapidJSON")
        set(RapidJSON_INCLUDE_DIRS third_party/rapidjson/include)
    else()
        message(STATUS "Please initialize rapidJSON git submodule as currently installed version is to old:")
        message(STATUS "git submodule init && git submodule update")
        message(FATAL_ERROR "RapidJSON version is likely too old.")
    endif()
endif()


###########################################################
# Targets
###########################################################

# lsp
add_library(lspcpp STATIC)

### Includes
target_include_directories(lspcpp
        PUBLIC
            ${LSPCPP_INCLUDE_DIR}
            ${RapidJSON_INCLUDE_DIRS}
            ${Uri_SOURCE_DIR}/include
        )

target_link_libraries(lspcpp PUBLIC network-uri  ${Boost_LIBRARIES})

set(LSPCPP_THIRD_PARTY_DIR_LIST
        ${LSPCPP_THIRD_PARTY_DIR}/utfcpp/source
        )

foreach(include_dir  ${LSPCPP_THIRD_PARTY_DIR_LIST})
    get_filename_component(include_dir_realpath ${include_dir} REALPATH)
    # Don't add as SYSTEM if they are in CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES.
    # It would reorder the system search paths and cause issues with libstdc++'s
    # use of #include_next.
    if(NOT "${include_dir_realpath}" IN_LIST CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)
        target_include_directories(lspcpp SYSTEM PRIVATE ${include_dir})
    endif()
endforeach()

### Sources
set(JSONRPC_LIST
        src/jsonrpc/Context.cpp
        src/jsonrpc/Endpoint.cpp
        src/jsonrpc/GCThreadContext.cpp
        src/jsonrpc/message.cpp
        src/jsonrpc/MessageJsonHandler.cpp
        src/jsonrpc/RemoteEndPoint.cpp
        src/jsonrpc/serializer.cpp
        src/jsonrpc/StreamMessageProducer.cpp
        src/jsonrpc/TcpServer.cpp
        src/jsonrpc/threaded_queue.cpp
)
set(LSPCPP_LIST
        src/lsp/initialize.cpp
        src/lsp/lsp.cpp
        src/lsp/lsp_diagnostic.cpp
        src/lsp/Markup.cpp
        src/lsp/ParentProcessWatcher.cpp
        src/lsp/ProtocolJsonHandler.cpp
        src/lsp/textDocument.cpp
        src/lsp/utils.cpp
        src/lsp/working_files.cpp
        )

if(LSPCPP_BUILD_WEBSOCKETS)
    set(JSONRPC_LIST
        ${JSONRPC_LIST}
        src/jsonrpc/WebSocketServer.cpp
    )
endif()

target_sources(lspcpp PRIVATE
        ${JSONRPC_LIST}
        ${LSPCPP_LIST})

### Compile options

lspcpp_set_target_options(lspcpp)

set_target_properties(lspcpp PROPERTIES POSITION_INDEPENDENT_CODE 1)

# install
if(LSPCPP_INSTALL)
    include(GNUInstallDirs)

    install(DIRECTORY ${LSPCPP_INCLUDE_DIR}/LibLsp
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            USE_SOURCE_PERMISSIONS
            )

    install(TARGETS lspcpp
            EXPORT lspcpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )

    install(EXPORT lspcpp-targets
            FILE lspcpp-config.cmake
            NAMESPACE lspcpp::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lspcpp
            )
endif()

# examples
if(LSPCPP_BUILD_EXAMPLES)

    ###########################################################
    # OS libraries
    ###########################################################
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(LSPCPP_OS_LIBS WS2_32)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(LSPCPP_OS_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(LSPCPP_OS_LIBS)
    endif()

    function(build_example target)
        add_executable(${target} "${CMAKE_CURRENT_SOURCE_DIR}/examples/${target}.cpp")
        target_include_directories(${target} PRIVATE ${Uri_SOURCE_DIR}/include)
        set_target_properties(${target} PROPERTIES
                FOLDER "Examples"
                )
        lspcpp_set_target_options(${target})
        target_link_libraries(${target}  PRIVATE lspcpp "${LSPCPP_OS_LIBS}")
    endfunction(build_example)

    set(EXAMPLES
            StdIOClientExample
            StdIOServerExample
            TcpServerExample
            WebsocketExample
            )

    foreach (example ${EXAMPLES})
        build_example(${example})
    endforeach()
endif()
