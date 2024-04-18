include(CTest)
include(FetchContent)

add_executable(tests
    netspeak/runner.cpp

	# Add test files here
    netspeak/test_ChainCutter.cpp
    netspeak/test_LfuCache.cpp
    netspeak/test_Netspeak.cpp
    netspeak/test_normalization.cpp
    netspeak/test_parse.cpp
    netspeak/test_phrase.cpp
    netspeak/test_PhraseCorpus.cpp
    netspeak/test_PropertiesFormat.cpp
    netspeak/test_QueryParser.cpp
    netspeak/test_regex.cpp

    netspeak/bighashmap/test_big_hash_map.cpp
    netspeak/bighashmap/test_value_traits.cpp

    netspeak/invertedindex/test_Indexer.cpp
    netspeak/invertedindex/test_InvertedFileReader.cpp
    netspeak/invertedindex/test_ManagedIndexer.cpp
    netspeak/invertedindex/test_Postlist.cpp
    netspeak/invertedindex/test_PostlistReader.cpp
    netspeak/invertedindex/test_PostlistSorter.cpp
    netspeak/invertedindex/test_Record.cpp
)
target_compile_features(tests PRIVATE cxx_std_23)
target_link_libraries(tests PRIVATE netspeak)

# Boost Test
find_package(Boost 1.40 COMPONENTS unit_test_framework REQUIRED)
target_link_libraries(tests PUBLIC ${Boost_LIBRARIES})


add_test(NAME netspeak_test COMMAND tests WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})