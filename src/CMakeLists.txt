include(FetchContent)
include(FindPkgConfig)

add_library(netspeak)
target_sources(netspeak PRIVATE
    antlr4/parse.cpp
    antlr4/QueryErrorHandler.cpp

    cli/BuildCommand.cpp
    cli/Commands.cpp
    cli/logging.cpp
    cli/ProxyCommand.cpp
    cli/RegexCommand.cpp
    cli/ServeCommand.cpp
    cli/ShellCommand.cpp
    cli/StressCommand.cpp
    cli/util.cpp

    netspeak/Configuration.cpp
    netspeak/Dictionaries.cpp
    netspeak/error.cpp
    netspeak/indexing.cpp
    netspeak/Netspeak.cpp
    netspeak/PhraseCorpus.cpp
    netspeak/Properties.cpp
    netspeak/QueryNormalizer.cpp

    netspeak/invertedindex/ByteBuffer.cpp
    netspeak/invertedindex/Properties.cpp

    netspeak/model/LengthRange.cpp
    netspeak/model/NormQuery.cpp
    netspeak/model/Phrase.cpp
    netspeak/model/Query.cpp
    netspeak/model/RawRefResult.cpp
    netspeak/model/RawResult.cpp
    netspeak/model/SimpleQuery.cpp
    netspeak/model/Words.cpp

    netspeak/regex/DefaultRegexIndex.cpp
    netspeak/regex/parsers.cpp
    netspeak/regex/RegexQuery.cpp

    netspeak/service/LoadBalanceProxy.cpp
    netspeak/service/RequestLogger.cpp
    netspeak/service/tracking.cpp
    netspeak/service/UniqueMap.cpp

    netspeak/util/Config.cpp
    netspeak/util/FileDescriptor.cpp
    netspeak/util/glob.cpp
    netspeak/util/logging.cpp
    netspeak/util/PropertiesFormat.cpp
    netspeak/util/service.cpp
    netspeak/util/string.cpp
)
target_compile_features(netspeak PUBLIC cxx_std_23)
target_include_directories(netspeak PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
set_target_properties(netspeak PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(netspeakcli)
target_sources(netspeakcli PRIVATE
    CommandLineTool.cpp
)

target_compile_features(netspeakcli PUBLIC cxx_std_23)
target_link_libraries(netspeakcli PUBLIC netspeak)

##########################################################################################
# Libraries
##########################################################################################
# ANTLR4
set(ANTLR_VERSION 4.13.1)
# Download the jar file
FetchContent_Declare(antlrjar URL https://www.antlr.org/download/antlr-${ANTLR_VERSION}-complete.jar DOWNLOAD_NO_EXTRACT TRUE)
FetchContent_MakeAvailable(antlrjar)
set(ANTLR_EXECUTABLE ${antlrjar_SOURCE_DIR}/antlr-${ANTLR_VERSION}-complete.jar)
# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)
# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)
set(ANTLR_BUILD_CPP_TESTS OFF)
FetchContent_Declare(
    antlr
    GIT_REPOSITORY https://github.com/antlr/antlr4
    GIT_TAG ${ANTLR_VERSION}
    SOURCE_SUBDIR "runtime/Cpp"
)
FetchContent_MakeAvailable(antlr)
set(ANTLR4CPP_INCLUDE_DIRS ${antlr_SOURCE_DIR}/runtime/Cpp/runtime/src)
include(${antlr_SOURCE_DIR}/runtime/Cpp/cmake/FindANTLR.cmake)

antlr_target(nsgrammarparser ${CMAKE_CURRENT_LIST_DIR}/../conf/grammar/Query.g4 LEXER PARSER VISITOR LISTENER PACKAGE antlr4)

target_link_libraries(netspeak PUBLIC antlr4_static)
target_include_directories(netspeak PUBLIC ${ANTLR4CPP_INCLUDE_DIRS} ${ANTLR_nsgrammarparser_OUTPUT_DIR})
target_sources(netspeak PRIVATE ${ANTLR_nsgrammarparser_CXX_OUTPUTS})

# absl (needed by protobuf)
FetchContent_Declare(absl GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git GIT_TAG 20240116.2 OVERRIDE_FIND_PACKAGE)
set(ABSL_ENABLE_INSTALL ON)  # https://github.com/protocolbuffers/protobuf/issues/12185#issuecomment-1594685860
FetchContent_MakeAvailable(absl)


# gRPC
FetchContent_Declare(gRPC GIT_REPOSITORY https://github.com/grpc/grpc GIT_TAG v1.62.1)
set(gRPC_SSL_PROVIDER package)
# set(gRPC_PROTOBUF_PROVIDER package)
FetchContent_MakeAvailable(gRPC)
target_link_libraries(netspeak PUBLIC grpc++)

# Protobuf and gRPC sources
find_package(Protobuf REQUIRED)
target_link_libraries(netspeak PUBLIC ${Protobuf_LIBRARIES})
set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(netspeak PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

target_sources(netspeak PRIVATE netspeak/NetSpeakService.proto)
protobuf_generate(TARGET netspeak)
protobuf_generate(
    TARGET netspeak
    LANGUAGE grpc
    PLUGIN protoc-gen-grpc=$<TARGET_FILE:grpc_cpp_plugin>
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)
# target_sources(netspeak PUBLIC ${NS_PROTO_CPP})
# target_include_directories(netspeak PUBLIC ${NS_PROTO_HPP})

# Boost
find_package(Boost 1.40 COMPONENTS program_options regex REQUIRED)
target_link_libraries(netspeak PUBLIC ${Boost_LIBRARIES})

# cmph
pkg_check_modules(CMPH REQUIRED cmph)
target_link_libraries(netspeak PUBLIC cmph)

# ICU
find_package(ICU COMPONENTS data uc REQUIRED)
target_link_libraries(netspeak PUBLIC ${ICU_LIBRARIES})

if (0)
## OLD:
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../build/dependencies/pybind11-2.9.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-m64")
set(CMAKE_EXE_LINKER_FLAGS "-m64")

include_directories("build/dependencies/pybind11-2.9.0/include")

set(NETSPEAK_LINK_LIBS
dl
z
rt
pthread

# boost
-l:libboost_date_time.a
-l:libboost_filesystem.a
-l:libboost_system.a
-l:libboost_program_options.a
-l:libboost_regex.a
-l:libboost_stacktrace_basic.a
-l:libboost_stacktrace_backtrace.a
-l:libboost_stacktrace_addr2line.a

# protobuf
-l:libprotobuf.a

# grpc
-l:libgrpc++.a
-l:libgrpc_unsecure.a
-l:libcares.a
-l:libgrpc.a
-l:libgpr.a
-l:libabsl_bad_optional_access.a
-l:libabsl_base.a
-l:libabsl_log_severity.a
-l:libabsl_spinlock_wait.a
-l:libabsl_str_format_internal.a
-l:libabsl_strings.a
-l:libabsl_raw_logging_internal.a
-l:libabsl_strings_internal.a
-l:libabsl_int128.a
-l:libabsl_throw_delegate.a
-l:libaddress_sorting.a
-l:libupb.a
)

set(NETSPEAK_TEST_SOURCES "${NETSPEAK_SOURCES}"
"test/netspeak/ManagedDirectory"
"test/netspeak/paths"
"test/netspeak/test_ChainCutter"
"test/netspeak/test_LfuCache"
"test/netspeak/test_Netspeak"
"test/netspeak/test_normalization"
"test/netspeak/test_parse"
"test/netspeak/test_phrase"
"test/netspeak/test_PhraseCorpus"
"test/netspeak/test_PropertiesFormat"
"test/netspeak/test_QueryParser"
"test/netspeak/test_regex"

"test/netspeak/bighashmap/test_big_hash_map"
"test/netspeak/bighashmap/test_value_traits"

"test/netspeak/invertedindex/test_Indexer"
"test/netspeak/invertedindex/test_InvertedFileReader"
"test/netspeak/invertedindex/test_ManagedIndexer"
"test/netspeak/invertedindex/test_Postlist"
"test/netspeak/invertedindex/test_PostlistReader"
"test/netspeak/invertedindex/test_PostlistSorter"
"test/netspeak/invertedindex/test_Record"
)
set(NETSPEAK_TEST_LINK_LIBS "${NETSPEAK_LINK_LIBS}")


add_executable(netspeak4 src/CommandLineTool.cpp)
target_sources(netspeak4 PRIVATE "${NETSPEAK_SOURCES}")
target_link_libraries(netspeak4 "${NETSPEAK_LINK_LIBS}")

add_executable(netspeak4-test test/netspeak/runner.cpp)
target_sources(netspeak4-test PRIVATE "${NETSPEAK_TEST_SOURCES}")
target_link_libraries(netspeak4-test "${NETSPEAK_TEST_LINK_LIBS}")

set(NETSPEAK_PY_LINK_LIBS
dl
z
rt

# boost
boost_system
boost_regex

# antlr4
antlr4-runtime
)

set(NETSPEAK_PY_SOURCES
"src/py/QueryParser"

"src/antlr4/parse"
"src/antlr4/QueryErrorHandler"

"src/antlr4/generated/QueryBaseListener"
"src/antlr4/generated/QueryBaseVisitor"
"src/antlr4/generated/QueryLexer"
"src/antlr4/generated/QueryListener"
"src/antlr4/generated/QueryParser"
"src/antlr4/generated/QueryVisitor"

"src/netspeak/Dictionaries"
"src/netspeak/error"
"src/netspeak/QueryNormalizer"

"src/netspeak/model/LengthRange"
"src/netspeak/model/NormQuery"
"src/netspeak/model/SimpleQuery"
"src/netspeak/model/Query"
"src/netspeak/model/QuerySyntax"

"src/netspeak/regex/DefaultRegexIndex"
"src/netspeak/regex/parsers"
"src/netspeak/regex/RegexIndex"
"src/netspeak/regex/RegexQuery"
)

pybind11_add_module(netspeak4py src/PythonBindings.cpp "${NETSPEAK_PY_SOURCES}")
set_property(TARGET netspeak4py PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(netspeak4py PRIVATE pybind11::module "${NETSPEAK_PY_LINK_LIBS}")
endif()