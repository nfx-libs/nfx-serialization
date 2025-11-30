#==============================================================================
# nfx-serialization - CMake targets
#==============================================================================

#----------------------------------------------
# Check if we have sources to build
#----------------------------------------------

if(NOT private_sources)
	message(STATUS "No library sources available, skipping target creation...")
	return()
endif()

#----------------------------------------------
# Targets definition
#----------------------------------------------

# --- Create shared library if requested ---
if(NFX_SERIALIZATION_BUILD_SHARED)
	add_library(${PROJECT_NAME} SHARED)
	target_sources(${PROJECT_NAME}
		PRIVATE
			${private_sources}
	)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

	add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
endif()

# --- Create static library if requested ---
if(NFX_SERIALIZATION_BUILD_STATIC)
	add_library(${PROJECT_NAME}-static STATIC)
	target_sources(${PROJECT_NAME}-static
		PRIVATE
			${private_sources}
	)

	set_target_properties(${PROJECT_NAME}-static PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}-static-${PROJECT_VERSION}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

	add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}-static)
endif()

#----------------------------------------------
# Targets properties
#----------------------------------------------

function(configure_target target_name)
	# --- Include directories ---
	target_include_directories(${target_name}
		PUBLIC
			$<BUILD_INTERFACE:${NFX_SERIALIZATION_INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${NFX_SERIALIZATION_SOURCE_DIR}
	)

	# --- Add nlohmann_json include directory ---
	if(NFX_SERIALIZATION_WITH_JSON AND DEFINED NLOHMANN_JSON_INCLUDE_DIR)
		target_include_directories(${target_name}
			PRIVATE ${NLOHMANN_JSON_INCLUDE_DIR}
		)
	endif()

	# --- Add nfx-stringutils include directory ---
	if(NFX_SERIALIZATION_WITH_JSON AND DEFINED NFX_STRINGUTILS_INCLUDE_DIR)
		target_include_directories(${target_name}
			PRIVATE ${NFX_STRINGUTILS_INCLUDE_DIR}
		)
	endif()

	# --- Properties ---
	set_target_properties(${target_name} PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		POSITION_INDEPENDENT_CODE ON
		DEBUG_POSTFIX "-d"
	)
endfunction()

# --- Apply configuration to both targets ---
if(NFX_SERIALIZATION_BUILD_SHARED)
	configure_target(${PROJECT_NAME})
	if(WIN32)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			WINDOWS_EXPORT_ALL_SYMBOLS TRUE
		)

		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/nfxSerializationVersion.rc.in
			${CMAKE_BINARY_DIR}/nfxSerializationVersion.rc
			@ONLY
		)
		target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/nfxSerializationVersion.rc)
	endif()
endif()

if(NFX_SERIALIZATION_BUILD_STATIC)
	configure_target(${PROJECT_NAME}-static)
endif()
