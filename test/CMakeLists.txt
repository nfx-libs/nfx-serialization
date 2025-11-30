#==============================================================================
# nfx-serialization - Test suite CMake configuration
#==============================================================================

#----------------------------------------------
# Test condition check
#----------------------------------------------

if(NOT NFX_SERIALIZATION_BUILD_TESTS)
	message(STATUS "Tests disabled, skipping...")
	return()
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
	find_package(nfx-containers QUIET)
	find_package(nfx-datatypes  QUIET)
	find_package(nfx-datetime   QUIET)

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
			TESTS_Extensions.cpp
		)
	endif()

	# Core JSON tests
	list(APPEND test_sources
		TESTS_JSONArrayIterator.cpp
		TESTS_JSONArrays.cpp
		TESTS_JSONDocument.cpp
		TESTS_JSONFileLoading.cpp
		TESTS_JSONObjectFieldIterator.cpp
		TESTS_JSONObjects.cpp
		TESTS_JSONPathView.cpp
		TESTS_JSONSchemaGenerator.cpp
		TESTS_JSONSchemaValidator.cpp
		TESTS_JSONSerializer.cpp
	)
endif()

if(NOT test_sources)
	message(STATUS "No test sources available, skipping...")
	return()
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
		# Additional linking for extension tests
		#----------------------------------------------

		if(${test_target_name} STREQUAL "TESTS_Extensions")
			target_link_libraries(${test_target_name} PRIVATE
				nfx-containers::nfx-containers
				nfx-datatypes::static
				nfx-datetime::static
			)

		target_compile_options(${test_target_name} PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
			$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-msse4.2>
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

