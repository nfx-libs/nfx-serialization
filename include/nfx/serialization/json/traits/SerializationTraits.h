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
 * @file SerializationTraits.h
 * @brief Unified serialization traits for JSON serialization/deserialization
 * @details Contains SerializationTraits template that provides the extensible
 *          serialization framework for nfx-serialization library.
 *
 *          SerializationTraits provides two complementary methods:
 *          - **serialize()**: High-performance streaming serialization (write)
 *          - **fromDocument()**: DOM-based deserialization (read)
 *
 *          Users can specialize this trait to customize serialization behavior
 *          for their own types.
 *
 *          For serialization support of nfx framework types (datetime, datatypes, containers),
 *          include the appropriate extension headers:
 *          - #include <nfx/serialization/json/extensions/DateTimeTraits.h>
 *          - #include <nfx/serialization/json/extensions/DatatypesTraits.h>
 *          - #include <nfx/serialization/json/extensions/ContainersTraits.h>
 */

#pragma once

#include <nfx/json/Builder.h>
#include <nfx/json/Document.h>

#include <stdexcept>
#include <variant>
#include <string>
#include <type_traits>
#include <vector>

using namespace nfx::json;

namespace nfx::serialization::json
{
    //=====================================================================
    // Forward declarations
    //=====================================================================

    template <typename T>
    class Serializer;

    template <typename T>
    struct SerializationTraits;

    //=====================================================================
    // SFINAE detectors
    //=====================================================================

    namespace detail
    {
        /**
         * @brief SFINAE detector for streaming serialization
         * @tparam T Type to check
         */
        template <typename T, typename = void>
        struct has_streaming_serialization : std::false_type
        {
        };

        /**
         * @brief SFINAE detector for streaming serialization (specialized version)
         * @tparam T Type to check
         * @details Checks if SerializationTraits<T>::serialize(const T&, Builder&) is valid
         */
        template <typename T>
        struct has_streaming_serialization<
            T,
            std::void_t<decltype( SerializationTraits<T>::serialize(
                std::declval<const T&>(), std::declval<nfx::json::Builder&>() ) )>> : std::true_type
        {
        };

        /**
         * @brief Helper variable template for has_streaming_serialization
         */
        template <typename T>
        inline constexpr bool has_streaming_serialization_v = has_streaming_serialization<T>::value;
    } // namespace detail

    //=====================================================================
    // SerializationTraits - Read/Write serialization interface
    //=====================================================================

    /**
     * @brief Unified serialization traits for JSON serialization/deserialization
     * @tparam T The type to serialize/deserialize
     * @details This is the extension point for users to define custom serialization.
     *          Users can specialize this template for their types with one or both methods:
     *
     *          1. **serialize()** - High-performance streaming (write, no DOM overhead)
     *          2. **fromDocument()** - DOM-based deserialization (read)
     *
     *          The serializer will prefer serialize() when available (detected via SFINAE),
     *          falling back to user types with member method fromDocument().
     *
     *          User types can provide member method with this signature:
     *          - void fromDocument(const Document&, const Serializer<T>&)
     *
     * **Example: High-performance streaming serialization**
     * ```cpp
     * template <>
     * struct SerializationTraits<MyType>
     * {
     *     static void serialize( const MyType& obj, nfx::json::Builder& builder )
     *     {
     *         builder.writeStartObject();
     *         builder.write( "field1", obj.field1 );
     *         builder.write( "field2", obj.field2 );
     *         builder.writeEndObject();
     *     }
     *
     *     static void fromDocument( const Document& doc, MyType& obj )
     *     {
     *         obj.field1 = doc.get<int>("field1").value();
     *         obj.field2 = doc.get<string>("field2").value();
     *     }
     * };
     * ```
     */
    template <typename T>
    struct SerializationTraits
    {
        /**
         * @brief Convert Document to object (deserialization)
         * @param doc Document to read from (source)
         * @param obj Object to populate (destination)
         * @details Default implementation calls member method fromDocument()
         */
        static void fromDocument( const Document& doc, T& obj )
        {
            Serializer<T> serializer;
            obj.fromDocument( doc, serializer );
        }

        // No default implementation for serialize() - must be specialized if needed
        // SFINAE detector will check if serialize() is available
    };

    /**
     * @brief Specialization for std::monostate (empty variant alternative)
     * @details Serializes as null, deserializes from null.
     *          std::monostate is used as the first alternative in std::variant
     *          to represent an empty state (similar to std::optional).
     */
    template <>
    struct SerializationTraits<std::monostate>
    {
        /**
         * @brief Serialize std::monostate to JSON null
         * @param builder JSON builder to write to
         */
        static void serialize( const std::monostate&, Builder& builder )
        {
            builder.write( nullptr );
        }

        /**
         * @brief Deserialize std::monostate from JSON null
         * @param doc JSON document to read from
         * @throws std::runtime_error if JSON is not null
         */
        static void fromDocument( const Document& doc, std::monostate& )
        {
            // monostate is always empty - no data to deserialize
            // Just verify the JSON is null
            if( !doc.isNull( "" ) )
            {
                throw std::runtime_error{ "Expected null for std::monostate" };
            }
        }
    };
} // namespace nfx::serialization::json
