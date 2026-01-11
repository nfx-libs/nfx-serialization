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
 * @file Document.h
 * @brief Generic document abstraction for JSON serialization
 */

#pragma once

#include <cstdint>
#include <iterator>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "Concepts.h"

namespace nfx::serialization::json
{
    class Document_impl;

    template <typename T>
    class Serializer;

    //=====================================================================
    // Document class
    //=====================================================================

    /**
     * @brief Generic JSON document abstraction for serialization
     * @details Provides a high-level interface for JSON document manipulation with support
     *          for JSON Pointer paths, type-safe value access, and nested object/array operations.
     */
    class Document final
    {
    public:
        //----------------------------------------------
        // Forward declarations
        //----------------------------------------------

        class Array;
        class Object;

        //----------------------------------------------
        // Friends
        //----------------------------------------------

        friend class Document_impl;
        friend class SchemaValidator_impl;

        //----------------------------------------------
        // Construction
        //----------------------------------------------

        /**
         * @brief Default constructor - creates an empty document
         */
        Document();

        /**
         * @brief Copy constructor
         * @param other The document to copy from
         */
        Document( const Document& other );

        /**
         * @brief Move constructor
         * @param other The document to move from
         */
        Document( Document&& other ) noexcept;

        //----------------------------------------------
        // Destruction
        //----------------------------------------------

        /**
         * @brief Destructor - cleans up document resources
         */
        ~Document();

        //----------------------------------------------
        // Assignment
        //----------------------------------------------

        /**
         * @brief Copy assignment operator
         * @param other The document to copy from
         * @return Reference to this document
         */
        Document& operator=( const Document& other );

        /**
         * @brief Move assignment operator
         * @param other The document to move from
         * @return Reference to this document
         */
        Document& operator=( Document&& other ) noexcept;

        //----------------------------------------------
        // Comparison
        //----------------------------------------------

        /**
         * @brief Equality comparison operator
         * @param other The document to compare with
         * @return true if documents are equal, false otherwise
         */
        bool operator==( const Document& other ) const;

        /**
         * @brief Inequality comparison operator
         * @param other The document to compare with
         * @return true if documents are not equal, false otherwise
         */
        bool operator!=( const Document& other ) const;

        //----------------------------------------------
        // Factory
        //----------------------------------------------

        /**
         * @brief Create document from JSON string
         * @param jsonStr JSON string to parse
         * @return Optional document if parsing succeeds, empty optional otherwise
         */
        static std::optional<Document> fromString( std::string_view jsonStr );

        /**
         * @brief Parse JSON string into existing document
         * @param jsonStr JSON string to parse
         * @param[out] doc Document to populate
         * @return true if parsing succeeds, false otherwise
         */
        static bool fromString( std::string_view jsonStr, Document& doc );

        /**
         * @brief Create document from JSON bytes
         * @param bytes JSON bytes to parse (accepts vector, array, or raw pointer+size)
         * @return Optional document if parsing succeeds, empty optional otherwise
         */
        static std::optional<Document> fromBytes( std::span<const uint8_t> bytes );

        /**
         * @brief Parse JSON bytes into existing document
         * @param bytes JSON bytes to parse (accepts vector, array, or raw pointer+size)
         * @param[out] doc Document to populate
         * @return true if parsing succeeds, false otherwise
         */
        static bool fromBytes( std::span<const uint8_t> bytes, Document& doc );

        //----------------------------------------------
        // Output
        //----------------------------------------------

        /**
         * @brief Convert document to JSON string
         * @param indent Indentation level for pretty printing
         *               - 0: compact output (no whitespace)
         *               - >0: pretty-print with specified spaces per level
         * @return JSON string representation
         */
        std::string toString( int indent = 0 ) const;

        /**
         * @brief Convert document to JSON bytes
         * @return JSON byte representation
         */
        std::vector<uint8_t> toBytes() const;

        //----------------------------------------------
        // Merge / update operations
        //----------------------------------------------

        /**
         * @brief Merge another document into this one
         * @param other Document to merge
         * @param overwriteArrays Whether to overwrite arrays or merge them
         */
        void merge( const Document& other, bool overwriteArrays = true );

        /**
         * @brief Update value at specific path
         * @param path JSON Pointer path to update
         * @param value New value to set
         */
        void update( std::string_view path, const Document& value );

        //----------------------------------------------
        // Value existence
        //----------------------------------------------

        /**
         * @brief Check if a value exists at the specified path
         * @param path JSON Pointer path (e.g., "/user/name") or dot notation (e.g., "user.name")
         * @return true if any JSON value exists at the path, false otherwise
         */
        bool contains( std::string_view path ) const;

        //----------------------------------------------
        // Value access
        //----------------------------------------------

        /**
         * @brief Get typed value at specified path
         * @tparam T Type to retrieve (string, int, double, bool, Document, Object, Array)
         * @param path JSON Pointer path to value
         * @return Optional containing value if exists and correct type, empty otherwise
         */
        template <JsonValue T>
        std::optional<T> get( std::string_view path ) const;

        /**
         * @brief Get typed value at specified path into output parameter
         * @tparam T Type to retrieve (string, int, double, bool, Document, Object, Array)
         * @param path JSON Pointer path to value
         * @param[out] value Output parameter to store the result
         * @return true if value exists and was successfully retrieved, false otherwise
         */
        template <JsonValue T>
        bool get( std::string_view path, T& value ) const;

        //----------------------------------------------
        // Value modification
        //----------------------------------------------

        /**
         * @brief Set typed value at specified path (copy version)
         * @tparam T Type to set (string, int, double, bool, Document, Object, Array)
         * @param path JSON Pointer path where to set value
         * @param value Value to set (copied)
         */
        template <JsonValue T>
        void set( std::string_view path, const T& value );

        /**
         * @brief Set typed value at specified path (move version)
         * @tparam T Type to set (string, int, double, bool, Document, Object, Array)
         * @param path JSON Pointer path where to set value
         * @param value Value to set (moved)
         */
        template <JsonValue T>
        void set( std::string_view path, T&& value );

        /**
         * @brief Set value from C-string for types with explicit constructors
         * @tparam T Type constructible from const char* (e.g., Decimal, custom types)
         * @param path JSON Pointer path where to set value
         * @param value C-string to construct value from
         * @note This overload enables setting types with explicit constructors from string literals
         */
        template <typename T>
            requires std::is_constructible_v<T, const char*> && (!JsonValue<T>)
        void set( std::string_view path, const char* value )
        {
            set( path, T( value ) );
        }

        /**
         * @brief Set value from arithmetic type for types with explicit constructors
         * @tparam T Type constructible from arithmetic type (e.g., Decimal from double)
         * @tparam U Arithmetic type (int, double, float, etc.)
         * @param path JSON Pointer path where to set value
         * @param value Arithmetic value to construct value from
         * @note This overload enables setting types with explicit constructors from numeric literals
         */
        template <typename T, typename U>
            requires std::is_arithmetic_v<U> && std::is_constructible_v<T, U> && (!JsonValue<T>)
        void set( std::string_view path, U value )
        {
            set( path, T( value ) );
        }

        //-----------------------------
        // Type-only creation
        //-----------------------------

        /**
         * @brief Create empty container at specified path
         * @tparam T Container type (Document, Object, Array)
         * @param path JSON Pointer path where to create container
         */
        template <JsonContainer T>
        void set( std::string_view path );

        //-----------------------------
        // Null operations
        //-----------------------------

        /**
         * @brief Set null value at specified path
         * @param path JSON Pointer path where to set null
         */
        void setNull( std::string_view path );

        //----------------------------------------------
        // Type checking
        //----------------------------------------------

        /**
         * @brief Check if value at path is of specified type
         * @tparam T Type to check for
         * @param path JSON Pointer path to check
         * @return true if value exists and is of type T, false otherwise
         */
        template <JsonCheckable T>
        bool is( std::string_view path ) const;

        /**
         * @brief Check if value at path is null
         * @param path JSON Pointer path to check
         * @return true if value is null, false otherwise
         */
        bool isNull( std::string_view path ) const;

        //----------------------------------------------
        // Validation and error handling
        //----------------------------------------------

        /**
         * @brief Check if document is in valid state
         * @return true if document is valid, false otherwise
         */
        bool isValid() const;

        /**
         * @brief Get last error message
         * @return Error message string
         */
        std::string lastError() const;

        //-----------------------------
        // Serializer support for STL types
        //-----------------------------

        /**
         * @brief Set value using Serializer for STL types (copy version)
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path where to set value
         * @param value Value to serialize
         */
        template <typename T>
            requires( !detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        void set( std::string_view path, const T& value )
        {
            Serializer<T> serializer;
            Document temp = serializer.serialize( value );
            this->set<Document>( path, std::move( temp ) );
        }

        /**
         * @brief Set value using Serializer for STL types (move version)
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path where to set value
         * @param value Value to serialize
         */
        template <typename T>
            requires( !detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        void set( std::string_view path, T&& value )
        {
            Serializer<std::remove_cvref_t<T>> serializer;
            Document temp = serializer.serialize( std::forward<T>( value ) );
            this->set<Document>( path, std::move( temp ) );
        }

        /**
         * @brief Get value using Serializer for STL types
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path to value
         * @return Optional containing deserialized value if exists
         */
        template <typename T>
            requires( !detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        std::optional<T> get( std::string_view path ) const
        {
            auto docOpt = get<Document>( path );
            if ( !docOpt.has_value() )
            {
                return std::nullopt;
            }

            Serializer<T> serializer;
            return serializer.deserialize( docOpt.value() );
        }

        /**
         * @brief Get value using Serializer for STL types (output parameter version)
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path to value
         * @param[out] value Output parameter to store deserialized value
         * @return true if value exists and was successfully deserialized
         */
        template <typename T>
            requires( !detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        bool get( std::string_view path, T& value ) const
        {
            auto result = get<T>( path );
            if ( result.has_value() )
            {
                value = std::move( result.value() );
                return true;
            }
            return false;
        }

        /**
         * @brief Check if value at path can be deserialized as STL type T
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path to check
         * @return true if value exists and can be deserialized as T
         */
        template <typename T>
            requires( !detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        bool is( std::string_view path ) const
        {
            auto docOpt = get<Document>( path );
            if ( !docOpt.has_value() )
                return false;
            try
            {
                Serializer<T> serializer;
                serializer.deserialize( docOpt.value() );
                return true;
            }
            catch ( ... )
            {
                return false;
            }
        }

        //-----------------------------
        // SerializationTraits support
        //-----------------------------

        /**
         * @brief Set value using SerializationTraits (copy version)
         * @tparam T Type with SerializationTraits defined (e.g., Decimal, DateTime)
         * @param path JSON Pointer path where to set value
         * @param value Value to serialize using its SerializationTraits
         * @details This overload is selected when T has a SerializationTraits specialization
         *          but is not a JSON primitive or container. The value is serialized into a
         *          temporary Document and then inserted at the specified path.
         */
        template <typename T>
            requires( detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        void set( std::string_view path, const T& value )
        {
            Document temp;
            SerializationTraits<T>::serialize( value, temp );
            this->set<Document>( path, std::move( temp ) );
        }

        /**
         * @brief Set value using SerializationTraits (move version)
         * @tparam T Type with SerializationTraits defined (e.g., Decimal, DateTime)
         * @param path JSON Pointer path where to set value
         * @param value Value to serialize using its SerializationTraits
         * @details This overload is selected when T has a SerializationTraits specialization
         *          but is not a JSON primitive or container. The value is serialized into a
         *          temporary Document and then inserted at the specified path.
         */
        template <typename T>
            requires( detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        void set( std::string_view path, T&& value )
        {
            Document temp;
            SerializationTraits<T>::serialize( value, temp );
            this->set<Document>( path, std::move( temp ) );
        }

        /**
         * @brief Get value using SerializationTraits
         * @tparam T Type with SerializationTraits defined (e.g., Decimal, DateTime)
         * @param path JSON Pointer path to value
         * @return Optional containing deserialized value if exists
         */
        template <typename T>
            requires( detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        std::optional<T> get( std::string_view path ) const
        {
            auto docOpt = get<Document>( path );
            if ( !docOpt.has_value() )
            {
                return std::nullopt;
            }

            T result;
            SerializationTraits<T>::deserialize( result, docOpt.value() );
            return result;
        }

        /**
         * @brief Get value using SerializationTraits into output parameter
         * @tparam T Type with SerializationTraits defined (e.g., Decimal, DateTime)
         * @param path JSON Pointer path to value
         * @param[out] value Output parameter to store deserialized value
         * @return true if value exists and was successfully deserialized
         */
        template <typename T>
            requires( detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        bool get( std::string_view path, T& value ) const
        {
            auto result = get<T>( path );
            if ( result.has_value() )
            {
                value = std::move( result.value() );
                return true;
            }
            return false;
        }

        /**
         * @brief Check if value at path can be deserialized as type T
         * @tparam T Type with SerializationTraits to check for
         * @param path JSON Pointer path to check
         * @return true if value exists and can be deserialized as T, false otherwise
         */
        template <typename T>
            requires( detail::is_nfx_extension_type_v<T> &&
                      !JsonPrimitive<T> &&
                      !is_json_container_v<T> )
        bool is( std::string_view path ) const
        {
            auto docOpt = get<Document>( path );
            if ( !docOpt.has_value() )
                return false;
            try
            {
                T temp;
                SerializationTraits<T>::deserialize( temp, docOpt.value() );
                return true;
            }
            catch ( ... )
            {
                return false;
            }
        }

        //----------------------------------------------
        // Document::Object class
        //----------------------------------------------

        /**
         * @brief JSON object wrapper for Document
         * @details Provides type-safe access to JSON object fields with support for
         *          field access, modification, removal, and nested operations using JSON Pointer paths.
         */
        class Object final
        {
            //----------------------------------------------
            // Friends
            //----------------------------------------------

            friend class Document;
            friend class Document_impl;

            //-----------------------------
            // Construction
            //-----------------------------

        private:
            Object( Document* doc, std::string_view path );

        public:
            /**
             * @brief Default constructor - creates invalid object
             */
            Object();

            /**
             * @brief Copy constructor
             * @param other Object to copy from
             */
            Object( const Document::Object& other );

            /**
             * @brief Move constructor
             * @param other Object to move from
             */
            Object( Document::Object&& other ) noexcept;

            //-----------------------------
            // Destruction
            //-----------------------------

            /** @brief Destructor */
            ~Object() = default;

            //-----------------------------
            // Assignment
            //-----------------------------

            /**
             * @brief Copy assignment operator
             * @param other Object to copy from
             * @return Reference to this object
             */
            Document::Object& operator=( const Document::Object& other );

            /**
             * @brief Move assignment operator
             * @param other Object to move from
             * @return Reference to this object
             */
            Document::Object& operator=( Document::Object&& other ) noexcept;

            //-----------------------------
            // Comparison
            //-----------------------------

            /**
             * @brief Equality comparison operator
             * @param other Object to compare with
             * @return true if objects are equal
             */
            bool operator==( const Document::Object& other ) const;

            /**
             * @brief Inequality comparison operator
             * @param other Object to compare with
             * @return true if objects are not equal
             */
            bool operator!=( const Document::Object& other ) const;

            //-----------------------------
            // Output
            //-----------------------------

            /**
             * @brief Convert object to JSON string
             * @param indent Indentation level for pretty printing
             *               - 0: compact output (no whitespace)
             *               - >0: pretty-print with specified spaces per level
             * @return JSON string representation
             */
            std::string toString( int indent = 0 ) const;

            /**
             * @brief Convert object to JSON bytes
             * @return JSON byte representation
             */
            std::vector<uint8_t> toBytes() const;

            //-----------------------------
            // Size
            //-----------------------------

            /**
             * @brief Get number of fields in object
             * @return Number of key-value pairs
             */
            size_t size() const;

            //-----------------------------
            // Clearing
            //-----------------------------

            /**
             * @brief Clear all fields from object
             */
            void clear();

            //-----------------------------
            // Field removal
            //-----------------------------

            /**
             * @brief Remove field from object
             * @param key Field name to remove
             * @return true if field was removed
             */
            bool removeField( std::string_view key );

            //-----------------------------
            // Field existence
            //-----------------------------

            /**
             * @brief Check if a field exists in this object
             * @param fieldName Name of the field to check (simple name, not a path)
             * @return true if the field exists, false otherwise
             */
            bool contains( std::string_view fieldName ) const;

            //-----------------------------
            // Field access
            //-----------------------------

            /**
             * @brief Get field value by key
             * @tparam T Type to retrieve
             * @param path Field key or nested path
             * @return Optional containing field value if exists and correct type
             */
            template <JsonValue T>
            std::optional<T> get( std::string_view path ) const;

            /**
             * @brief Get field value into output parameter
             * @tparam T Type to retrieve
             * @param path Field key or nested path
             * @param[out] value Output parameter to store the result
             * @return true if field exists and was successfully retrieved, false otherwise
             */
            template <JsonValue T>
            bool get( std::string_view path, T& value ) const;

            //-----------------------------
            // Field modification
            //-----------------------------

            /**
             * @brief Set field value (copy version)
             * @tparam T Type to set
             * @param path Field key or nested path
             * @param value Value to copy and set
             */
            template <JsonValue T>
            void set( std::string_view path, const T& value );

            /**
             * @brief Set field value (move version)
             * @tparam T Type to set
             * @param path Field key or nested path
             * @param value Value to move and set
             */
            template <JsonValue T>
            void set( std::string_view path, T&& value );

            //-----------------------------
            // Validation and error handling
            //-----------------------------

            /**
             * @brief Check if object is valid
             * @return true if object is valid and accessible
             */
            bool isValid() const;

            /**
             * @brief Get last error message
             * @return String describing the last error
             */
            std::string lastError() const;

            //----------------------------------------------
            // Document::Object::Iterator class
            //----------------------------------------------

            /**
             * @brief Forward iterator for Object fields
             * @details Provides STL-compatible iteration over object key-value pairs.
             *          Yields std::pair<std::string, Document> for each field.
             */
            class Iterator
            {
            public:
                //-----------------------------
                // Iterator traits
                //-----------------------------

                /** @brief Iterator category tag for STL compatibility */
                using iterator_category = std::forward_iterator_tag;
                /** @brief Type of value yielded by iterator */
                using value_type = std::pair<std::string, Document>;
                /** @brief Type for iterator difference */
                using difference_type = std::ptrdiff_t;
                /** @brief Pointer to value type */
                using pointer = value_type*;
                /** @brief Reference to value type */
                using reference = value_type&;

                //-----------------------------
                // Construction
                //-----------------------------

                /** @brief Default constructor - creates end iterator */
                Iterator();

                /**
                 * @brief Construct iterator at specific position
                 * @param obj Pointer to parent Object
                 * @param index Current position (0 = begin, size = end)
                 */
                Iterator( const Object* obj, size_t index );

                //-----------------------------
                // Operators
                //-----------------------------

                /**
                 * @brief Dereference operator
                 * @return Current key-value pair (field name and Document value)
                 */
                value_type operator*() const;

                /**
                 * @brief Pre-increment operator
                 * @return Reference to this iterator after increment
                 */
                Iterator& operator++();

                /**
                 * @brief Post-increment operator
                 * @return Copy of iterator before increment
                 */
                Iterator operator++( int );

                /**
                 * @brief Equality comparison
                 * @param other Iterator to compare with
                 * @return true if iterators point to same position
                 */
                bool operator==( const Iterator& other ) const;

                /**
                 * @brief Inequality comparison
                 * @param other Iterator to compare with
                 * @return true if iterators point to different positions
                 */
                bool operator!=( const Iterator& other ) const;

            private:
                const Object* m_obj; ///< Pointer to parent object
                size_t m_index;      ///< Current position
            };

            /**
             * @brief Get iterator to first field
             * @return Iterator pointing to first key-value pair
             */
            Iterator begin() const;

            /**
             * @brief Get iterator past last field
             * @return Iterator representing end position
             */
            Iterator end() const;

        private:
            //----------------------------------------------
            // Private helper methods for iterator
            //----------------------------------------------

            /**
             * @brief Get field key at specific index
             * @param index Zero-based index
             * @return Field name at index
             */
            std::string keyAt( size_t index ) const;

            /**
             * @brief Get field value at specific index as Document
             * @param index Zero-based index
             * @return Document containing the field value
             */
            Document valueAt( size_t index ) const;

            //----------------------------------------------
            // Private data members
            //----------------------------------------------

            Document* m_doc;    ///< Pointer to the original document
            std::string m_path; ///< Path to the object within the document
        };

        //----------------------------------------------
        // Document::Array class
        //----------------------------------------------

        /**
         * @brief JSON array wrapper for Document
         * @details Provides type-safe access to JSON array elements with support for
         *          indexed access, element addition/insertion/removal, and nested operations.
         */
        class Array final
        {
            //-----------------------------
            // Friends
            //-----------------------------

            friend class Document;
            friend class Document_impl;

            //-----------------------------
            // Construction
            //-----------------------------

        private:
            Array( Document* doc, std::string_view path );

        public:
            /**
             * @brief Default constructor - creates empty array
             */
            Array();

            /**
             * @brief Copy constructor
             * @param other Array to copy from
             */
            Array( const Document::Array& other );

            /**
             * @brief Move constructor
             * @param other Array to move from
             */
            Array( Document::Array&& other ) noexcept;

            //-----------------------------
            // Destruction
            //-----------------------------

            /** @brief Destructor */
            ~Array() = default;

            //-----------------------------
            // Assignment
            //-----------------------------

            /**
             * @brief Copy assignment operator
             * @param other Array to copy from
             * @return Reference to this array
             */
            Document::Array& operator=( const Document::Array& other );

            /**
             * @brief Move assignment operator
             * @param other Array to move from
             * @return Reference to this array
             */
            Document::Array& operator=( Document::Array&& other ) noexcept;

            //-----------------------------
            // Comparison
            //-----------------------------

            /**
             * @brief Equality comparison
             * @param other Array to compare with
             * @return true if arrays are equal
             */
            bool operator==( const Document::Array& other ) const;

            /**
             * @brief Inequality comparison
             * @param other Array to compare with
             * @return true if arrays are not equal
             */
            bool operator!=( const Document::Array& other ) const;

            //-----------------------------
            // Output
            //-----------------------------

            /**
             * @brief Convert array to JSON string
             * @param indent Indentation level for pretty printing
             *               - 0: compact output (no whitespace)
             *               - >0: pretty-print with specified spaces per level
             * @return JSON string representation
             */
            std::string toString( int indent = 0 ) const;

            /**
             * @brief Convert array to JSON bytes
             * @return JSON byte representation
             */
            std::vector<uint8_t> toBytes() const;

            //-----------------------------
            // Size
            //-----------------------------

            /**
             * @brief Get number of elements in array
             * @return Size of array
             */
            size_t size() const;

            //-----------------------------
            // Clearing
            //-----------------------------

            /**
             * @brief Clear all elements from array
             */
            void clear();

            //-----------------------------
            // Element removal
            //-----------------------------

            /**
             * @brief Remove element at index
             * @param index Array index
             * @return true if element was removed
             */
            bool remove( size_t index );

            //-----------------------------
            // Element existence
            //-----------------------------

            /**
             * @brief Check if an element exists at the specified index
             * @param indexStr Index as string (e.g., "0", "1") or JSON Pointer (e.g., "/0", "/1")
             * @return true if the element exists, false otherwise
             */
            bool contains( std::string_view indexStr ) const;

            //-----------------------------
            // Element access
            //-----------------------------

            /**
             * @brief Get element at index
             * @tparam T Type to retrieve
             * @param index Array index
             * @return Optional containing element if exists and correct type
             */
            template <JsonValue T>
            std::optional<T> get( size_t index ) const;

            /**
             * @brief Get element at index into output parameter
             * @tparam T Type to retrieve
             * @param index Array index
             * @param[out] value Output parameter to store the result
             * @return true if element exists and was successfully retrieved, false otherwise
             */
            template <JsonValue T>
            bool get( size_t index, T& value ) const;

            //-----------------------------
            // Nested element access
            //-----------------------------

            /**
             * @brief Get nested element at path
             * @tparam T Type to retrieve
             * @param path JSON pointer path
             * @return Optional containing element if exists and correct type
             */
            template <JsonValue T>
            std::optional<T> get( std::string_view path ) const;

            /**
             * @brief Get nested element at path into output parameter
             * @tparam T Type to retrieve
             * @param path JSON pointer path
             * @param[out] value Output parameter to store the result
             * @return true if element exists and was successfully retrieved, false otherwise
             */
            template <JsonValue T>
            bool get( std::string_view path, T& value ) const;

            //-----------------------------
            // Element modification
            //-----------------------------

            /**
             * @brief Set element at index (copy version)
             * @tparam T Type to set
             * @param index Array index
             * @param value Value to copy
             */
            template <JsonValue T>
            void set( size_t index, const T& value );

            /**
             * @brief Set element at index (move version)
             * @tparam T Type to set
             * @param index Array index
             * @param value Value to move
             */
            template <JsonValue T>
            void set( size_t index, T&& value );

            //-----------------------------
            // Nested element modification
            //-----------------------------

            /**
             * @brief Set nested element at path (copy version)
             * @tparam T Type to set
             * @param path JSON pointer path
             * @param value Value to copy
             */
            template <JsonValue T>
            void set( std::string_view path, const T& value );

            /**
             * @brief Set nested element at path (move version)
             * @tparam T Type to set
             * @param path JSON pointer path
             * @param value Value to move
             */
            template <JsonValue T>
            void set( std::string_view path, T&& value );

            //-----------------------------
            // Element addition
            //-----------------------------

            /**
             * @brief Append element to end of array (copy version)
             * @tparam T Type to append
             * @param value Value to copy and append
             */
            template <JsonValue T>
            void append( const T& value );

            /**
             * @brief Append element to end of array (move version)
             * @tparam T Type to append
             * @param value Value to move and append
             */
            template <JsonValue T>
            void append( T&& value );

            /**
             * @brief Append Document to end of array (reference version)
             * @param value Document reference to append
             */
            void append( Document& value );

            /**
             * @brief Append Array to end of array (reference version)
             * @param value Array reference to append
             */
            void append( Document::Array& value );

            /**
             * @brief Append Object to end of array (reference version)
             * @param value Object reference to append
             */
            void append( Document::Object& value );

            //-----------------------------
            // Element insertion
            //-----------------------------

            /**
             * @brief Insert element at index (copy version)
             * @tparam T Type to insert
             * @param index Position to insert at
             * @param value Value to copy and insert
             */
            template <JsonValue T>
            void insert( size_t index, const T& value );

            /**
             * @brief Insert element at index (move version)
             * @tparam T Type to insert
             * @param index Position to insert at
             * @param value Value to move and insert
             */
            template <JsonValue T>
            void insert( size_t index, T&& value );

            /**
             * @brief Insert Document at index (reference version)
             * @param index Position to insert at
             * @param value Document reference to insert
             */
            void insert( size_t index, Document& value );

            /**
             * @brief Insert Array at index (reference version)
             * @param index Position to insert at
             * @param value Array reference to insert
             */
            void insert( size_t index, Document::Array& value );

            /**
             * @brief Insert Object at index (reference version)
             * @param index Position to insert at
             * @param value Object reference to insert
             */
            void insert( size_t index, Document::Object& value );

            //-----------------------------
            // Validation and error handling
            //-----------------------------

            /**
             * @brief Check if array is valid
             * @return true if array is valid and accessible
             */
            bool isValid() const;

            /**
             * @brief Get last error message
             * @return String describing the last error
             */
            std::string lastError() const;

            //----------------------------------------------
            // Document::Array::Iterator class
            //----------------------------------------------

            /**
             * @brief Forward iterator for Array elements
             * @details Provides STL-compatible iteration over array elements.
             *          Yields Document for each element.
             */
            class Iterator
            {
            public:
                //-----------------------------
                // Iterator traits
                //-----------------------------

                /** @brief Iterator category tag for STL compatibility */
                using iterator_category = std::forward_iterator_tag;
                /** @brief Type of value yielded by iterator */
                using value_type = Document;
                /** @brief Type for iterator difference */
                using difference_type = std::ptrdiff_t;
                /** @brief Pointer to value type */
                using pointer = value_type*;
                /** @brief Reference to value type */
                using reference = value_type&;

                //-----------------------------
                // Construction
                //-----------------------------

                /** @brief Default constructor - creates end iterator */
                Iterator();

                /**
                 * @brief Construct iterator at specific position
                 * @param arr Pointer to parent Array
                 * @param index Current position (0 = begin, size = end)
                 */
                Iterator( const Array* arr, size_t index );

                //-----------------------------
                // Operators
                //-----------------------------

                /**
                 * @brief Dereference operator
                 * @return Current element as Document
                 */
                value_type operator*() const;

                /**
                 * @brief Pre-increment operator
                 * @return Reference to this iterator after increment
                 */
                Iterator& operator++();

                /**
                 * @brief Post-increment operator
                 * @return Copy of iterator before increment
                 */
                Iterator operator++( int );

                /**
                 * @brief Equality comparison
                 * @param other Iterator to compare with
                 * @return true if iterators point to same position
                 */
                bool operator==( const Iterator& other ) const;

                /**
                 * @brief Inequality comparison
                 * @param other Iterator to compare with
                 * @return true if iterators point to different positions
                 */
                bool operator!=( const Iterator& other ) const;

            private:
                const Array* m_arr; ///< Pointer to parent array
                size_t m_index;     ///< Current position
            };

            /**
             * @brief Get iterator to first element
             * @return Iterator pointing to first element
             */
            Iterator begin() const;

            /**
             * @brief Get iterator past last element
             * @return Iterator representing end position
             */
            Iterator end() const;

        private:
            //-----------------------------
            // Private data members
            //-----------------------------

            Document* m_doc;    ///< Pointer to the original document
            std::string m_path; ///< Path to the array within the document
        };

        //----------------------------------------------
        // Document::PathView class
        //----------------------------------------------

        /**
         * @brief Path iterator for traversing all paths in a JSON document
         * @details Provides depth-first traversal of all JSON paths with their values.
         *          Supports both JSON Pointer format ("/user/name") and dot notation ("user.name").
         *
         * Example usage:
         * @code
         * for (const auto& entry : Document::PathView(doc)) {
         *     std::cout << entry.path << " = " << entry.value().toString() << "\n";
         * }
         * @endcode
         */
        class PathView final
        {
        public:
            //-----------------------------
            // Forward declaration
            //-----------------------------

            class Iterator;

            //-----------------------------
            // Format
            //-----------------------------

            /**
             * @brief Format for path string representation
             */
            enum class Format : bool
            {
                JsonPointer = 0, ///< RFC 6901 JSON Pointer format (e.g., "/user/addresses/0/city")
                DotNotation      ///< Dot notation format (e.g., "user.addresses[0].city")
            };

            //-----------------------------
            // Path entry structure
            //-----------------------------

            /**
             * @brief Represents a single path entry in the document
             */
            struct Entry
            {
                std::string path;                   ///< Full path to this value
                std::unique_ptr<Document> valuePtr; ///< The value at this path (owned pointer)
                size_t depth;                       ///< Nesting depth (0 = root level)
                bool isLeaf;                        ///< True if value is a primitive (not object/array)

                /**
                 * @brief Get the value as a Document reference
                 * @return Const reference to the Document value
                 */
                const Document& value() const { return *valuePtr; }

                /// @brief Default constructor
                Entry() : depth{ 0 }, isLeaf{ false } {}

                /**
                 * @brief Move constructor
                 * @param other Entry to move from
                 */
                Entry( Entry&& other ) noexcept = default;

                /**
                 * @brief Move assignment
                 * @param other Entry to move from
                 * @return Reference to this entry
                 */
                Entry& operator=( Entry&& other ) noexcept = default;

                /**
                 * @brief Copy constructor (deep copy)
                 * @param other Entry to copy from
                 */
                Entry( const Entry& other );

                /**
                 * @brief Copy assignment (deep copy)
                 * @param other Entry to copy from
                 * @return Reference to this entry
                 */
                Entry& operator=( const Entry& other );
            };

            //----------------------------------------------
            // Construction
            //----------------------------------------------

            /**
             * @brief Construct PathView for a document
             * @param doc Document to iterate
             * @param format Path format to use
             * @param includeContainers Whether to include object/array container paths
             *
             * Example usage:
             * @code
             * for (const auto& entry : Document::PathView(doc)) {
             *     std::cout << entry.path << "\n";
             * }
             * @endcode
             */
            explicit PathView(
                const Document& doc,
                Format format = Format::JsonPointer,
                bool includeContainers = true );

            //----------------------------------------------
            // Range interface (for range-for loops)
            //----------------------------------------------

            /**
             * @brief Get iterator to first entry
             * @return Iterator pointing to first path entry
             */
            Iterator begin() const { return Iterator{ &m_entries, 0 }; }

            /**
             * @brief Get iterator past last entry
             * @return Iterator representing end position
             */
            Iterator end() const { return Iterator{ &m_entries, m_entries.size() }; }

            //----------------------------------------------
            // Direct access
            //----------------------------------------------

            /**
             * @brief Get number of path entries
             * @return Number of entries
             */
            size_t size() const { return m_entries.size(); }

            /**
             * @brief Check if empty
             * @return true if no entries
             */
            bool empty() const { return m_entries.empty(); }

            /**
             * @brief Access entry by index
             * @param index Index of entry
             * @return Reference to entry at index
             */
            const Entry& operator[]( size_t index ) const { return m_entries[index]; }

            //----------------------------------------------
            // Filtering
            //----------------------------------------------

            /**
             * @brief Get only leaf entries (primitives)
             * @return Vector of leaf entries
             */
            std::vector<Entry> leaves() const;

            //----------------------------------------------
            // Document::PathView::Iterator class
            //----------------------------------------------

            /**
             * @brief Forward iterator for path entries
             */
            class Iterator
            {
            public:
                /** @brief Iterator category tag for STL compatibility */
                using iterator_category = std::forward_iterator_tag;
                /** @brief Type of value yielded by iterator */
                using value_type = Entry;
                /** @brief Type for iterator difference */
                using difference_type = std::ptrdiff_t;
                /** @brief Pointer to value type */
                using pointer = const Entry*;
                /** @brief Reference to value type */
                using reference = const Entry&;

                /// @brief Default constructor - creates end iterator
                Iterator()
                    : m_entries{ nullptr },
                      m_index{ 0 }
                {
                }

                /**
                 * @brief Construct iterator at position
                 * @param entries Pointer to entries vector
                 * @param index Current position
                 */
                Iterator( const std::vector<Entry>* entries, size_t index )
                    : m_entries{ entries },
                      m_index{ index }
                {
                }

                /**
                 * @brief Dereference operator
                 * @return Reference to current entry
                 */
                reference operator*() const { return ( *m_entries )[m_index]; }

                /**
                 * @brief Arrow operator
                 * @return Pointer to current entry
                 */
                pointer operator->() const { return &( *m_entries )[m_index]; }

                /**
                 * @brief Pre-increment operator
                 * @return Reference to this iterator
                 */
                Iterator& operator++()
                {
                    ++m_index;
                    return *this;
                }

                /**
                 * @brief Post-increment operator
                 * @return Copy of iterator before increment
                 */
                Iterator operator++( int )
                {
                    Iterator tmp = *this;
                    ++m_index;
                    return tmp;
                }

                /**
                 * @brief Equality comparison
                 * @param other Iterator to compare with
                 * @return true if equal
                 */
                bool operator==( const Iterator& other ) const
                {
                    if ( !m_entries && !other.m_entries )
                    {
                        return true;
                    }
                    if ( !m_entries )
                    {
                        return other.m_index >= other.m_entries->size();
                    }
                    if ( !other.m_entries )
                    {
                        return m_index >= m_entries->size();
                    }
                    return m_entries == other.m_entries && m_index == other.m_index;
                }

                /**
                 * @brief Inequality comparison
                 * @param other Iterator to compare with
                 * @return true if not equal
                 */
                bool operator!=( const Iterator& other ) const { return !( *this == other ); }

            private:
                const std::vector<Entry>* m_entries; ///< Pointer to entries
                size_t m_index;                      ///< Current position
            };

        private:
            //----------------------------------------------
            // Private helper methods
            //----------------------------------------------

            /**
             * @brief Build entries via depth-first traversal
             * @param doc Document to traverse
             */
            void buildEntries( const Document& doc );

            /**
             * @brief Format path string based on format setting
             * @param segments Path segments
             * @return Formatted path string
             */
            std::string formatPath( const std::vector<std::string>& segments ) const;

            //----------------------------------------------
            // Private data members
            //----------------------------------------------

            Format m_format;              ///< Path format to use
            bool m_includeContainers;     ///< Whether to include container nodes
            std::vector<Entry> m_entries; ///< Pre-computed entries
        };

    private:
        //----------------------------------------------
        // Pimpl
        //----------------------------------------------

        std::shared_ptr<Document_impl> m_impl;
    };

    //=====================================================================
    // Type trait specializations for Document nested classes
    //=====================================================================

    /**
     * @brief Specialization for Document::Object
     */
    template <>
    struct is_json_container<Document::Object> : std::true_type
    {
    };

    /**
     * @brief Specialization for Document::Array
     */
    template <>
    struct is_json_container<Document::Array> : std::true_type
    {
    };
} // namespace nfx::serialization::json
