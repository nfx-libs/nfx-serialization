#==============================================================================
# nfx-serialization - CMake Sources
#==============================================================================

#----------------------------------------------
# Source files
#----------------------------------------------

set(private_sources)

if(NFX_SERIALIZATION_WITH_JSON)
	list(APPEND private_sources
		${NFX_SERIALIZATION_SOURCE_DIR}/json/impl/Document_impl.cpp
		${NFX_SERIALIZATION_SOURCE_DIR}/json/impl/SchemaValidator_impl.cpp

		${NFX_SERIALIZATION_SOURCE_DIR}/json/Document.cpp
		${NFX_SERIALIZATION_SOURCE_DIR}/json/SchemaGenerator.cpp
		${NFX_SERIALIZATION_SOURCE_DIR}/json/SchemaValidator.cpp
	)
endif()

