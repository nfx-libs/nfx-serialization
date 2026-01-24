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
 * @file SerializableDocument.h
 * @brief SerializableDocument class with serialization support for custom types
 */

#pragma once

#include <nfx/json/Document.h>

#include "Concepts.h"
#include "SerializationTraits.h"

namespace nfx::serialization::json
{
    // Forward declaration
    template <typename T>
    class Serializer;

    /**
     * @brief SerializableDocument class with serialization support
     * @details Wraps nfx::json::Document to support serialization of STL types
     *          (via Serializer<T>) and nfx extension types (via SerializationTraits<T>).
     *
     * @par Usage
     * SerializableDocument extends Document with automatic C++ type serialization:
     * @code
     * SerializableDocument doc;
     * doc.set("/users", std::vector<std::string>{"alice", "bob"}); // STL via Serializer
     * doc.set("/count", std::optional<int>{42});                   // STL optional
     * doc.set("/price", Decimal("99.99"));                         // nfx via SerializationTraits
     * @endcode
     *
     * For low-level Document operations (iteration, visitors, merging), use the document() accessor:
     * @code
     * doc.document().visit([](auto&& val) { std::cout << val; });  // Visitor pattern
     * for (auto [key, val] : doc.document().objectBegin("/users")) // Iteration
     *     std::cout << key << ": " << val;
     * @endcode
     */
    class SerializableDocument final
    {
    public:
        //----------------------------------------------
        // Construction
        //----------------------------------------------

        /**
         * @brief Default constructor
         */
        SerializableDocument() = default;

        /**
         * @brief Constructor from base Document (copy)
         * @param doc The base nfx::json::Document to copy from
         */
        inline SerializableDocument( const nfx::json::Document& doc );

        /**
         * @brief Constructor from base Document (move)
         * @param doc The base nfx::json::Document to move from
         */
        inline SerializableDocument( nfx::json::Document&& doc );

        //----------------------------------------------
        // Conversion operators for compatibility
        //----------------------------------------------

        /**
         * @brief Implicit conversion to base Document reference
         */
        inline operator nfx::json::Document&() noexcept;

        /**
         * @brief Implicit conversion to const base Document reference
         */
        inline operator const nfx::json::Document&() const noexcept;

        /**
         * @brief Get underlying base Document
         * @return Reference to the wrapped nfx::json::Document
         * @details Use this accessor for operations not directly supported by SerializableDocument,
         *          such as iteration, visitors, merging, or advanced path operations.
         */
        inline nfx::json::Document& document() noexcept;

        /**
         * @brief Get underlying base Document (const)
         * @return Const reference to the wrapped nfx::json::Document
         * @details Use this accessor for operations not directly supported by SerializableDocument,
         *          such as iteration, visitors, or advanced path operations.
         */
        inline const nfx::json::Document& document() const noexcept;

        //----------------------------------------------
        // Delegate primitive type methods to base
        //----------------------------------------------

        /**
         * @brief Set primitive value (delegates to base)
         * @tparam T Primitive type (bool, int, double, string, etc.)
         * @param path JSON Pointer path where to set value
         * @param value Primitive value to set
         */
        template <typename T>
            requires( nfx::json::Primitive<T> )
        inline void set( std::string_view path, T value );

        /**
         * @brief Get primitive value (delegates to base)
         * @tparam T Primitive type to get
         * @param path JSON Pointer path to value
         * @return Optional containing value if exists
         */
        template <typename T>
            requires( nfx::json::Primitive<T> )
        inline std::optional<T> get( std::string_view path ) const;

        /**
         * @brief Get primitive value into output parameter (delegates to base)
         * @tparam T Primitive type to get
         * @param path JSON Pointer path to value
         * @param value Output parameter to store the value
         * @return true if value exists and was retrieved, false otherwise
         */
        template <typename T>
            requires( nfx::json::Primitive<T> )
        inline bool get( std::string_view path, T& value ) const;

        /**
         * @brief Check if value is primitive type (delegates to base)
         * @tparam T Primitive type to check for
         * @param path JSON Pointer path to check
         * @return true if value at path is of type T, false otherwise
         */
        template <typename T>
            requires( nfx::json::Primitive<T> )
        inline bool is( std::string_view path ) const;

        /**
         * @brief Set JSON container (delegates to base)
         * @tparam T JSON container type (Object or Array)
         * @param path JSON Pointer path where to set value
         * @param value Container value to set
         */
        template <typename T>
            requires( is_json_container_v<T> )
        inline void set( std::string_view path, T&& value );

        /**
         * @brief Create empty JSON container at path (delegates to base)
         * @tparam T JSON container type (Object or Array)
         * @param path JSON Pointer path where to create empty container
         */
        template <typename T>
            requires( is_json_container_v<T> )
        inline void set( std::string_view path );

        /**
         * @brief Get JSON container (delegates to base)
         * @tparam T JSON container type to get
         * @param path JSON Pointer path to value
         * @return Optional containing value if exists
         */
        template <typename T>
            requires( is_json_container_v<T> )
        inline std::optional<T> get( std::string_view path ) const;

        /**
         * @brief Get JSON container into output parameter (delegates to base)
         * @tparam T JSON container type to get
         * @param path JSON Pointer path to value
         * @param value Output parameter to store the container
         * @return true if value exists and was retrieved, false otherwise
         */
        template <typename T>
            requires( is_json_container_v<T> )
        inline bool get( std::string_view path, T& value ) const;

        /**
         * @brief Check if value is JSON container type (delegates to base)
         * @tparam T JSON container type to check for
         * @param path JSON Pointer path to check
         * @return true if value at path is of type T, false otherwise
         */
        template <typename T>
            requires( is_json_container_v<T> )
        inline bool is( std::string_view path ) const;

        //----------------------------------------------
        // Additional delegation methods for base functionality
        //----------------------------------------------

        /**
         * @brief Convert Document to JSON string
         * @param indent Number of spaces for indentation (0 for compact)
         * @return JSON string representation
         */
        inline std::string toString( int indent = 0 ) const;

        /**
         * @brief Convert Document to JSON bytes
         * @return JSON bytes representation
         */
        inline std::vector<uint8_t> toBytes() const;

        /**
         * @brief Set value to null at path
         * @param path JSON Pointer path
         */
        inline void setNull( std::string_view path );

        /**
         * @brief Check if value at path is null
         * @param path JSON Pointer path
         * @return true if value is null
         */
        inline bool isNull( std::string_view path ) const;

        /**
         * @brief Get the type of the root JSON value
         * @return Type enum (Object, Array, String, Number, Boolean, Null)
         */
        inline nfx::json::Type type() const noexcept;

        /**
         * @brief Check if the document's container is empty
         * @return true if the document is an empty array or object
         */
        inline bool isEmpty() const noexcept;

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
            requires StlSerializable<T>
        inline void set( std::string_view path, const T& value );

        /**
         * @brief Set value using Serializer for STL types (move version)
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path where to set value
         * @param value Value to serialize
         */
        template <typename T>
            requires StlSerializable<T>
        inline void set( std::string_view path, T&& value );

        /**
         * @brief Get value using Serializer for STL types
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path to value
         * @return Optional containing deserialized value if exists
         */
        template <typename T>
            requires StlSerializable<T>
        inline std::optional<T> get( std::string_view path ) const;

        /**
         * @brief Get value using Serializer for STL types (output parameter version)
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path to value
         * @param[out] value Output parameter to store deserialized value
         * @return true if value exists and was successfully deserialized
         */
        template <typename T>
            requires StlSerializable<T>
        inline bool get( std::string_view path, T& value ) const;

        /**
         * @brief Check if value at path can be deserialized as STL type T
         * @tparam T STL type (e.g., std::vector, std::optional, std::unique_ptr)
         * @param path JSON Pointer path to check
         * @return true if value exists and can be deserialized as T
         */
        template <typename T>
            requires StlSerializable<T>
        inline bool is( std::string_view path ) const;

        //-----------------------------
        // SerializationTraits support for nfx extension types
        //-----------------------------

        /**
         * @brief Set value using SerializationTraits (copy version)
         * @tparam T Type with SerializationTraits defined (e.g., Decimal, DateTime, FastHashMap)
         * @param path JSON Pointer path where to set value
         * @param value Value to serialize using its SerializationTraits
         */
        template <typename T>
            requires NfxSerializable<T>
        inline void set( std::string_view path, const T& value );

        /**
         * @brief Set value using SerializationTraits (move version)
         * @tparam T Type with SerializationTraits defined (e.g., Decimal, DateTime, FastHashMap)
         * @param path JSON Pointer path where to set value
         * @param value Value to serialize using its SerializationTraits
         */
        template <typename T>
            requires NfxSerializable<T>
        inline void set( std::string_view path, T&& value );

        /**
         * @brief Get value using SerializationTraits
         * @tparam T Type with SerializationTraits defined (e.g., Decimal, DateTime, FastHashMap)
         * @param path JSON Pointer path to value
         * @return Optional containing deserialized value if exists
         */
        template <typename T>
            requires NfxSerializable<T>
        inline std::optional<T> get( std::string_view path ) const;

        /**
         * @brief Get value using SerializationTraits into output parameter
         * @tparam T Type with SerializationTraits defined (e.g., Decimal, DateTime, FastHashMap)
         * @param path JSON Pointer path to value
         * @param[out] value Output parameter to store deserialized value
         * @return true if value exists and was successfully deserialized
         */
        template <typename T>
            requires NfxSerializable<T>
        inline bool get( std::string_view path, T& value ) const;

        /**
         * @brief Check if value at path can be deserialized as type T
         * @tparam T Type with SerializationTraits to check for
         * @param path JSON Pointer path to check
         * @return true if value exists and can be deserialized as T, false otherwise
         */
        template <typename T>
            requires NfxSerializable<T>
        inline bool is( std::string_view path ) const;

        //-----------------------------
        // Conversion helpers for types with explicit constructors
        //-----------------------------

        /**
         * @brief Set value from const char* for types constructible from string
         * @tparam T Type with SerializationTraits and constructible from const char*
         * @param path JSON Pointer path where to set value
         * @param value String value to construct T from
         */
        template <typename T>
            requires( NfxSerializable<T> && std::is_constructible_v<T, const char*> )
        inline void set( std::string_view path, const char* value );

        /**
         * @brief Set value from arithmetic type for types with explicit constructors
         * @tparam T Type with SerializationTraits constructible from arithmetic (e.g., Decimal from double)
         * @tparam U Arithmetic type (int, double, float, etc.)
         * @param path JSON Pointer path where to set value
         * @param value Arithmetic value to construct T from
         */
        template <typename T, typename U>
            requires( NfxSerializable<T> && std::is_arithmetic_v<U> && std::is_constructible_v<T, U> )
        inline void set( std::string_view path, U value );

        //-----------------------------
        // Static factory methods override
        //-----------------------------

        /**
         * @brief Create SerializableDocument from JSON string
         * @param jsonStr JSON string to parse
         * @return Optional SerializableDocument if parsing succeeds, empty optional otherwise
         * @details Wraps base class method to return extended Document type
         */
        inline static std::optional<SerializableDocument> fromString( std::string_view jsonStr );

        /**
         * @brief Parse JSON string into existing SerializableDocument
         * @param jsonStr JSON string to parse
         * @param[out] value SerializableDocument to populate
         * @return true if parsing succeeds, false otherwise
         * @details Wraps base class method with type conversion
         */
        inline static bool fromString( std::string_view jsonStr, SerializableDocument& value );

        /**
         * @brief Create SerializableDocument from JSON bytes
         * @param bytes JSON bytes to parse
         * @return Optional SerializableDocument if parsing succeeds, empty optional otherwise
         * @details Wraps base class method to return extended Document type
         */
        inline static std::optional<SerializableDocument> fromBytes( const std::vector<uint8_t>& bytes );

        /**
         * @brief Parse JSON bytes into existing SerializableDocument
         * @param bytes JSON bytes to parse
         * @param[out] value SerializableDocument to populate
         * @return true if parsing succeeds, false otherwise
         * @details Wraps base class method with type conversion
         */
        inline static bool fromBytes( const std::vector<uint8_t>& bytes, SerializableDocument& value );

    private:
        nfx::json::Document m_doc;
    };
} // namespace nfx::serialization::json

#include "nfx/detail/serialization/json/SerializableDocument.inl"
#include "Serializer.h"
