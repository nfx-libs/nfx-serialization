#==============================================================================
# nfx-serialization - Benchmark suite CMake configuration
#==============================================================================

#----------------------------------------------
# Benchmark condition check
#----------------------------------------------

if(NOT NFX_SERIALIZATION_BUILD_BENCHMARKS)
	message(STATUS "Benchmarks disabled, skipping...")
	return()
endif()

#----------------------------------------------
# Benchmarks source files
#----------------------------------------------

set(benchmark_sources)

if(NFX_SERIALIZATION_WITH_JSON)
	list(APPEND benchmark_sources
		BM_JSONAccess.cpp
		BM_JSONIteration.cpp
		BM_JSONModification.cpp
		BM_JSONObjectSerialization.cpp
		BM_JSONParsing.cpp
		BM_JSONSchemaGeneration.cpp
		BM_JSONSchemaValidation.cpp
		BM_JSONSerialization.cpp
	)
endif()

if(NOT benchmark_sources)
	message(STATUS "No benchmark sources available, skipping...")
	return()
endif()

#----------------------------------------------
# Configure benchmark executables
#----------------------------------------------

foreach(benchmark_source ${benchmark_sources})
	get_filename_component(benchmark_target_name ${benchmark_source} NAME_WE)
	if(NOT TARGET ${benchmark_target_name})
		add_executable(${benchmark_target_name} ${benchmark_source})

		#----------------------------------------------
		# Target linking
		#----------------------------------------------

		target_link_libraries(${benchmark_target_name} PRIVATE
			nfx-serialization::static
			benchmark::benchmark
		)

		#----------------------------------------------
		# Properties
		#----------------------------------------------

		set_target_properties(${benchmark_target_name} PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
			CXX_EXTENSIONS OFF
			POSITION_INDEPENDENT_CODE ON
			DEBUG_POSTFIX "-d"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/benchmarks"
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/benchmarks"
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/benchmarks"
		)
	endif()
endforeach()

#----------------------------------------------
# Copy testdata to output directory
#----------------------------------------------

if(EXISTS "${NFX_SERIALIZATION_TESTDATA_DIR}")
	add_custom_target(copy_testdata_benchmarks ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${NFX_SERIALIZATION_TESTDATA_DIR}"
			"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/benchmarks/testdata"
		COMMENT "Copying testdata to benchmarks output directory"
	)
endif()
