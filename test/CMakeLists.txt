#==============================================================================
# nfx-serialization - Test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Test condition check
#----------------------------------------------

if(NOT NFX_SERIALIZATION_BUILD_TESTS)
	return()
endif()

#----------------------------------------------
# GoogleTest dependency
#----------------------------------------------

include(FetchContent)

set(NFX_SERIALIZATION_DEPS_GOOGLETEST_VERSION "1.17.0")

find_package(GTest ${NFX_SERIALIZATION_DEPS_GOOGLETEST_VERSION} QUIET)
if(NOT GTest_FOUND)
	set(_SAVED_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
	set(BUILD_SHARED_LIBS           OFF CACHE BOOL  "Build shared libraries"     FORCE)

	set(BUILD_GMOCK                 OFF CACHE BOOL "Build GMock"                 FORCE)
	set(INSTALL_GTEST               OFF CACHE BOOL "Install GoogleTest"          FORCE)
	set(GTEST_HAS_ABSL              OFF CACHE BOOL "Use Abseil library"          FORCE)
	set(gtest_build_samples         OFF CACHE BOOL "Build samples"               FORCE)
	set(gtest_build_tests           OFF CACHE BOOL "Build tests"                 FORCE)
	set(gtest_disable_pthreads      OFF CACHE BOOL "Disable pthreads"            FORCE)
	set(gtest_force_shared_crt      ON  CACHE BOOL "Force shared CRT on Windows" FORCE)
	set(gtest_hide_internal_symbols OFF CACHE BOOL "Hide internal symbols"       FORCE)

	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG        v${NFX_SERIALIZATION_DEPS_GOOGLETEST_VERSION}
		GIT_SHALLOW    TRUE
	)

	set(BUILD_SHARED_LIBS ${_SAVED_BUILD_SHARED_LIBS} CACHE BOOL "Build shared libraries" FORCE)

	FetchContent_MakeAvailable(googletest)
endif()

#----------------------------------------------
# GoogleTest integration
#----------------------------------------------

include(GoogleTest)

#----------------------------------------------
# Fetch optional nfx extension libraries for testing
#----------------------------------------------

if(NFX_SERIALIZATION_BUILD_EXTENSION_TESTS)
	include(FetchContent)

	# Try to find libraries first, then fetch if not found
	find_package(nfx-containers 0.1.1 QUIET)
	find_package(nfx-datatypes  0.1.1 QUIET)
	find_package(nfx-datetime   0.2.0 QUIET)

	# nfx-containers (PerfectHashMap, FastHashMap, FastHashSet) - header-only
	if(NOT nfx-containers_FOUND)
		FetchContent_Declare(
			nfx-containers
			GIT_REPOSITORY https://github.com/nfx-libs/nfx-containers.git
			GIT_TAG        main
		)
		list(APPEND nfx_libs_to_fetch nfx-containers)
	endif()

	# nfx-datatypes (Int128, Decimal) - requires static library
	if(NOT nfx-datatypes_FOUND)
		set(NFX_DATATYPES_BUILD_STATIC ON CACHE BOOL "" FORCE)
		FetchContent_Declare(
			nfx-datatypes
			GIT_REPOSITORY https://github.com/nfx-libs/nfx-datatypes.git
			GIT_TAG        main
		)
		list(APPEND nfx_libs_to_fetch nfx-datatypes)
	endif()

	# nfx-datetime (DateTime, DateTimeOffset, TimeSpan) - requires static library
	if(NOT nfx-datetime_FOUND)
		set(NFX_DATETIME_BUILD_STATIC ON CACHE BOOL "" FORCE)
		FetchContent_Declare(
			nfx-datetime
			GIT_REPOSITORY https://github.com/nfx-libs/nfx-datetime.git
			GIT_TAG        main
		)
		list(APPEND nfx_libs_to_fetch nfx-datetime)
	endif()

	# Only fetch libraries that weren't found
	if(nfx_libs_to_fetch)
		FetchContent_MakeAvailable(${nfx_libs_to_fetch})
	endif()
endif()

#----------------------------------------------
# Parallel test configuration
#----------------------------------------------

include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
	set(CTEST_PARALLEL_LEVEL ${N})
	message(STATUS "Setting parallel test level to ${N} cores")
else()
	set(CTEST_PARALLEL_LEVEL 4)
	message(STATUS "Could not detect CPU count, setting parallel test level to 4")
endif()

#----------------------------------------------
# Tests source files
#----------------------------------------------

set(test_sources)

if(NFX_SERIALIZATION_WITH_JSON)
	if(NFX_SERIALIZATION_BUILD_EXTENSION_TESTS)
		list(APPEND test_sources
			TESTS_JsonExtensions.cpp
		)
	endif()

	# Core JSON tests
	list(APPEND test_sources
		TESTS_JsonArrayIterator.cpp
		TESTS_JsonArrays.cpp
		TESTS_JsonDocument.cpp
		TESTS_JsonFileLoading.cpp
		TESTS_JsonFormats.cpp
		TESTS_JsonObjectFieldIterator.cpp
		TESTS_JsonObjects.cpp
		TESTS_JsonPathView.cpp
		TESTS_JsonSchemaGenerator.cpp
		TESTS_JsonSchemaValidator.cpp
		TESTS_JsonSerializer.cpp
	)
endif()

#----------------------------------------------
# Configure test executable
#----------------------------------------------

foreach(test_source ${test_sources})
	get_filename_component(test_target_name ${test_source} NAME_WE)

	if(NOT TARGET ${test_target_name})
		add_executable(${test_target_name} ${test_source})

		#----------------------------------------------
		# Target linking
		#----------------------------------------------

		target_link_libraries(${test_target_name} PRIVATE
			nfx-serialization::static
			GTest::gtest_main
		)

		#----------------------------------------------
		# Enable specific CPU features
		#----------------------------------------------

		target_compile_options(${test_target_name} PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
		)

		#----------------------------------------------
		# Additional linking for extension tests
		#----------------------------------------------

		if(${test_target_name} STREQUAL "TESTS_JsonExtensions")
			target_link_libraries(${test_target_name} PRIVATE
				nfx-containers::nfx-containers
				nfx-datatypes::static
				nfx-datetime::static
			)
		endif()

		#----------------------------------------------
		# Properties
		#----------------------------------------------

		set_target_properties(${test_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
		)

		#----------------------------------------------
		# Test discovery and registration
		#----------------------------------------------

		gtest_discover_tests(${test_target_name}
			WORKING_DIRECTORY "$<TARGET_FILE_DIR:${test_target_name}>"
			DISCOVERY_MODE POST_BUILD
			PROPERTIES
				TIMEOUT 120
				RUN_SERIAL OFF
		)
	endif()
endforeach()

#----------------------------------------------
# Copy testdata to output directory
#----------------------------------------------

if(EXISTS "${NFX_SERIALIZATION_TESTDATA_DIR}")
	add_custom_target(copy_testdata_tests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${NFX_SERIALIZATION_TESTDATA_DIR}"
			"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/testdata"
		COMMENT "Copying testdata to tests output directory"
	)
endif()

