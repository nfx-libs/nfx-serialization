/*
 * MIT License
 *
 * Copyright (c) 2025 nfx
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file SchemaValidator_impl.h
 * @brief Pimpl implementation for SchemaValidator, providing JSON Schema validation
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "nfx/serialization/json/SchemaValidator.h"

namespace nfx::serialization::json
{
    //=====================================================================
    // Forward declarations
    //=====================================================================

    class ValidationResult;
    class Document;
    class Document_impl;

    //=====================================================================
    // SchemaValidator_impl class
    //=====================================================================

    class SchemaValidator_impl final
    {
    public:
        //----------------------------------------------
        // Construction
        //----------------------------------------------

        /**
         * @brief Default constructor
         */
        SchemaValidator_impl();

        /**
         * @brief Constructor with schema and options
         * @param schema The schema document to load
         * @param strictMode If true, unknown properties cause validation failure
         * @param maxDepth Maximum recursion depth
         */
        explicit SchemaValidator_impl( const Document& schema, bool strictMode = false, size_t maxDepth = 64 );

        /**
         * @brief Copy constructor
         * @param other The validator to copy from
         */
        SchemaValidator_impl( const SchemaValidator_impl& other );

        /**
         * @brief Move constructor
         * @param other The validator to move from
         */
        SchemaValidator_impl( SchemaValidator_impl&& other ) noexcept;

        //----------------------------------------------
        // Destruction
        //----------------------------------------------

        /** @brief Destructor */
        ~SchemaValidator_impl() = default;

        //----------------------------------------------
        // Assignment
        //----------------------------------------------

        /**
         * @brief Copy assignment operator
         * @param other The validator to copy from
         * @return Reference to this validator
         */
        SchemaValidator_impl& operator=( const SchemaValidator_impl& other );

        /**
         * @brief Move assignment operator
         * @param other The validator to move from
         * @return Reference to this validator
         */
        SchemaValidator_impl& operator=( SchemaValidator_impl&& other ) noexcept;

        //----------------------------------------------
        // Main validation interface
        //----------------------------------------------
        /**
         * @brief Validate document against loaded schema
         * @param document Document to validate
         * @return ValidationResult with errors if any
         */
        ValidationResult validate( const Document& document ) const;

        /**
         * @brief Validate document at specific path against schema at path
         * @param document Document to validate
         * @param documentPath Path within document to validate
         * @param schemaPath Path within schema to validate against
         * @return ValidationResult with errors if any
         */
        ValidationResult validateAtPath( const Document& document, std::string_view documentPath, std::string_view schemaPath ) const;

        //----------------------------------------------
        // Accessor methods
        //----------------------------------------------

        /**
         * @brief Check if schema is loaded
         * @return True if schema is loaded, false otherwise
         */
        bool isSchemaLoaded() const noexcept { return m_schemaLoaded; }

        /**
         * @brief Get the loaded schema
         * @return Reference to the schema document
         */
        const Document& schema() const { return *m_schema; }

        /**
         * @brief Get strict mode setting
         * @return True if strict mode is enabled, false otherwise
         */
        bool isStrictMode() const noexcept { return m_strictMode; }

        /**
         * @brief Get maximum validation depth
         * @return Maximum depth setting (0 = unlimited)
         */
        size_t maxDepth() const noexcept { return m_maxDepth; }

        /**
         * @brief Get detected schema draft version
         * @return Schema draft version enum
         */
        SchemaDraft draft() const noexcept { return m_schemaDraft; }

        /**
         * @brief Get schema draft version as string
         * @return Draft version string (e.g., "draft-07", "2020-12")
         */
        std::string draftString() const noexcept;

        /**
         * @brief Set schema loaded state
         * @param loaded New loaded state
         */
        void setSchemaLoaded( bool loaded ) noexcept { m_schemaLoaded = loaded; }

        /**
         * @brief Set the schema document
         * @param schema New schema document
         */
        void setSchema( const Document& schema );

        /**
         * @brief Set strict mode
         * @param strict New strict mode setting
         */
        void setStrictMode( bool strict ) noexcept { m_strictMode = strict; }

        /**
         * @brief Set maximum validation depth
         * @param maxDepth New maximum depth setting
         */
        void setMaxDepth( size_t maxDepth ) noexcept { m_maxDepth = maxDepth; }

    private:
        //----------------------------------------------
        // Internal validation methods
        //----------------------------------------------
        /**
         * @brief Validate a document node against schema node
         * @param document Document to validate
         * @param schema Schema to validate against
         * @param path Current JSON path for error reporting
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateNode( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        /**
         * @brief Validate JSON type constraint
         * @param document Document to validate
         * @param schema Schema containing type constraint
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         */
        void validateType( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

        /**
         * @brief Validate required fields for objects
         * @param document Document to validate
         * @param schema Schema containing required constraint
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         */
        void validateRequired( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

        /**
         * @brief Validate object properties
         * @param document Document to validate
         * @param schema Schema containing properties definition
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateProperties( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        /**
         * @brief Validate numeric constraints (minimum, maximum)
         * @param document Document to validate
         * @param schema Schema containing numeric constraints
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         */
        void validateNumericConstraints( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

        /**
         * @brief Validate string constraints (minLength, maxLength, pattern, format)
         * @param document Document to validate
         * @param schema Schema containing string constraints
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         */
        void validateStringConstraints( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

        /**
         * @brief Validate enum constraint (value must be one of allowed values)
         * @param document Document to validate
         * @param schema Schema containing enum constraint
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         */
        void validateEnum( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

        /**
         * @brief Validate const constraint (value must equal exact constant)
         * @param document Document to validate
         * @param schema Schema containing const constraint
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         */
        void validateConst( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

        /**
         * @brief Check if a schema reference exists
         * @param reference Reference path (e.g., "#/definitions/Package")
         * @return True if reference exists, false otherwise
         */
        bool referenceExists( std::string_view reference ) const noexcept;

        /**
         * @brief Resolve schema reference ($ref)
         * @param reference Reference path (e.g., "#/definitions/Package")
         * @return Resolved schema Document, or empty Document if not found
         */
        Document resolveReference( std::string_view reference ) const;

        /**
         * @brief Get actual type name of document value at path
         * @param document Document to check
         * @param path JSON path to check
         * @return Type name string (e.g., "string", "number", "object", "array")
         */
        std::string actualType( const Document& document, std::string_view path ) const noexcept;

        /**
         * @brief Validate additionalProperties constraint (strict validation)
         * @param document Document to validate
         * @param schema Schema to validate against
         * @param path Current JSON path for error reporting
         * @param result ValidationResult to accumulate errors
         */
        void validateAdditionalProperties( const Document& document, const Document& schema, std::string_view path, ValidationResult& result ) const;

        /**
         * @brief Extract a sub-document from a document at specified path
         * @param document Source document
         * @param path Path to extract (supports both dot notation and JSON Pointer)
         * @return Sub-document at path, or empty document if not found
         */
        Document extractSubDocument( const Document& sourceDocument, std::string_view path ) const;

        /**
         * @brief Validate array items
         * @param document Document to validate
         * @param schema Schema containing items definition
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateArray( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        /**
         * @brief Validate object constraints (required, properties, additionalProperties, etc.)
         * @param document Document to validate
         * @param schema Schema containing object constraints
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateObject( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        /**
         * @brief Validate allOf constraint - instance must validate against ALL schemas
         * @param document Document to validate
         * @param schema Schema containing allOf array
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateAllOf( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        /**
         * @brief Validate anyOf constraint - instance must validate against AT LEAST ONE schema
         * @param document Document to validate
         * @param schema Schema containing anyOf array
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateAnyOf( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        /**
         * @brief Validate oneOf constraint - instance must validate against EXACTLY ONE schema
         * @param document Document to validate
         * @param schema Schema containing oneOf array
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateOneOf( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        /**
         * @brief Validate not constraint - instance must NOT validate against schema
         * @param document Document to validate
         * @param schema Schema containing not subschema
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateNot( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        /**
         * @brief Validate if/then/else conditional constraint
         * @details If document validates against "if" schema, it must also validate against "then" schema.
         *          If document does NOT validate against "if" schema, it must validate against "else" schema (if present).
         * @param document Document to validate
         * @param schema Schema containing if/then/else subschemas
         * @param path Current JSON path
         * @param result ValidationResult to accumulate errors
         * @param currentDepth Current recursion depth for circular reference protection
         */
        void validateIfThenElse( const Document& document, const Document& schema, std::string_view path, ValidationResult& result, size_t currentDepth = 0 ) const;

        //----------------------------------------------
        // Internal helper methods
        //----------------------------------------------

        /**
         * @brief Build anchor index by scanning schema for $anchor keywords
         * @details Recursively scans the schema document to find all $anchor definitions
         *          and stores their JSON Pointer paths for later resolution.
         *          Called automatically when schema is set.
         */
        void buildAnchorIndex();

        /**
         * @brief Recursively scan schema node for anchors
         * @param schema Schema node to scan
         * @param currentPath Current JSON Pointer path
         */
        void scanForAnchors( const Document& schema, const std::string& currentPath );

        /**
         * @brief Detect schema draft version from $schema URI
         * @details Parses the $schema keyword to determine which JSON Schema draft
         *          the schema was written for. This affects validation behavior.
         */
        void detectdraft();

        //----------------------------------------------
        // Member variables
        //----------------------------------------------

        static constexpr size_t DEFAULT_MAX_DEPTH = 64;               ///< Default maximum validation depth
        std::unique_ptr<Document> m_schema;                           ///< Owned schema document
        bool m_schemaLoaded = false;                                  ///< Whether a schema has been loaded
        bool m_strictMode = false;                                    ///< Whether strict validation mode is enabled
        size_t m_maxDepth = DEFAULT_MAX_DEPTH;                        ///< Maximum validation depth (0 = unlimited)
        SchemaDraft m_schemaDraft = SchemaDraft::Unknown;             ///< Detected schema draft version
        mutable std::unordered_map<std::string, Document> m_refCache; ///< Cache for resolved $ref references
        std::unordered_map<std::string, std::string> m_anchorIndex;   ///< Map of anchor names to JSON Pointer paths
    };
} // namespace nfx::serialization::json
