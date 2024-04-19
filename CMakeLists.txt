cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

option(NETSPEAK_BUILD_TESTS "Build tests" ON)
option(NETSPEAK_BUILD_DOCS "Build documentation" ON)
option(NETSPEAK_ONLY_DOCS "Build only documentation -- this disables tests and others" OFF)

project(Netspeak VERSION 4.2.0 LANGUAGES CXX)

if (NOT NETSPEAK_ONLY_DOCS)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	add_subdirectory(src)
	add_subdirectory(bindings/python)
endif()

##########################################################################################
# Testing
##########################################################################################
# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if(NOT NETSPEAK_ONLY_DOCS AND (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND NETSPEAK_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

##########################################################################################
# Documentation
##########################################################################################
# Documentation only available if this is the main app
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND NETSPEAK_BUILD_DOCS)
	find_package(Doxygen REQUIRED)

	## Set Doxygen configuration options (THIS REPLACES THE DOXYFILE!)
	set(DOXYGEN_PROJECT_BRIEF "One word leads to another.")
	set(DOXYGEN_PROJECT_LOGO "netspeak_logo.svg")
	set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/documentation)
	set(DOXYGEN_STRIP_FROM_PATH "include")
	set(DOXYGEN_EXTRACT_PRIVATE "YES")
	set(DOXYGEN_EXTRACT_STATIC "YES")
	set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
	set(DOXYGEN_SOURCE_BROWSER "YES")
	set(DOXYGEN_INLINE_SOURCES "YES")

	set(DOXYGEN_GENERATE_HTML "YES")

    ## Install and configure the "Doxygen Awesome" theme
	include(FetchContent)
	FetchContent_Declare(doxawesome GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css GIT_TAG v2.3.2)
	FetchContent_MakeAvailable(doxawesome)
	set(DOXYGEN_GENERATE_TREEVIEW "YES")
	set(DOXYGEN_DISABLE_INDEX "NO")
	set(DOXYGEN_FULL_SIDEBAR "NO")
	set(DOXYGEN_HTML_EXTRA_STYLESHEET ${doxawesome_SOURCE_DIR}/doxygen-awesome.css ${doxawesome_SOURCE_DIR}/doxygen-awesome-sidebar-only.css ${doxawesome_SOURCE_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css ${doxawesome_SOURCE_DIR}/doxygen-awesome-tabs.js ${doxawesome_SOURCE_DIR}/doxygen-custom/custom.css)
	set(DOXYGEN_HTML_COLORSTYLE "LIGHT")
	set(DOXYGEN_HTML_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-header.html)
	set(DOXYGEN_HTML_EXTRA_FILES ${doxawesome_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js ${doxawesome_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js ${doxawesome_SOURCE_DIR}/doxygen-awesome-interactive-toc.js ${doxawesome_SOURCE_DIR}/doxygen-awesome-paragraph-link.js)

	## Build Documentation
	doxygen_add_docs(
		netspeak_docs
		${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/README.md
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
endif()