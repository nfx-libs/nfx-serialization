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
 * @brief Serialization traits for JSON serialization/deserialization
 * @details Contains both BuilderTraits and DocumentTraits templates that provide
 *          the extensible serialization framework for nfx-serialization library.
 *
 *          Two complementary trait systems:
 *          - **BuilderTraits**: High-performance streaming serialization (write-only)
 *          - **DocumentTraits**: Bidirectional DOM-based serialization (read/write)
 *
 *          Users can specialize these traits to customize serialization behavior
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
    struct BuilderTraits;

    template <typename T>
    struct DocumentTraits;

    //=====================================================================
    // SFINAE detectors
    //=====================================================================

    namespace detail
    {
        /**
         * @brief SFINAE detector for BuilderTraits specialization
         * @tparam T Type to check
         */
        template <typename T, typename = void>
        struct has_builder_traits : std::false_type
        {
        };

        /**
         * @brief SFINAE detector for BuilderTraits specialization (specialized version)
         * @tparam T Type to check
         * @details Checks if BuilderTraits<T>::serialize(const T&, Builder&) is valid
         */
        template <typename T>
        struct has_builder_traits<
            T,
            std::void_t<decltype( BuilderTraits<T>::serialize(
                std::declval<const T&>(), std::declval<nfx::json::Builder&>() ) )>> : std::true_type
        {
        };

        /**
         * @brief Helper variable template for has_builder_traits
         */
        template <typename T>
        inline constexpr bool has_builder_traits_v = has_builder_traits<T>::value;
    } // namespace detail

    //=====================================================================
    // BuilderTraits - High-performance streaming serialization
    //=====================================================================

    /**
     * @brief Builder traits for direct JSON serialization
     * @tparam T The type to serialize
     * @details This is the extension point for high-performance serialization.
     *          Users can specialize this template to provide direct Builder serialization
     *          for their types, avoiding the Document→JSON conversion overhead.
     *
     *          When BuilderTraits<T> is specialized, the serializer will use it directly
     *          instead of falling back to DocumentTraits + Document conversion.
     *
     *          Provides write-only streaming serialization with no intermediate DOM.
     *
     * @example
     * ```cpp
     * template <>
     * struct BuilderTraits<MyType>
     * {
     *     static void serialize( const MyType& obj, nfx::json::Builder& builder )
     *     {
     *         builder.writeStartObject();
     *         builder.write( "field1", obj.field1 );
     *         builder.write( "field2", obj.field2 );
     *         builder.writeEndObject();
     *     }
     * };
     * ```
     */
    template <typename T>
    struct BuilderTraits
    {
        // No default implementation - must be specialized for each type
        // The has_builder_traits SFINAE detector will catch if this isn't specialized
    };

    //=====================================================================
    // DocumentTraits - Bidirectional DOM-based serialization
    //=====================================================================

    /**
     * @brief Document-based serialization traits - users can specialize this
     * @tparam T The type to serialize/deserialize
     * @details This is the extension point for users to define custom DOM-based serialization.
     *          Users can specialize this template for their types or even override
     *          library types with custom serialization logic.
     *
     *          DocumentTraits provides bidirectional serialization through the Document API:
     *          - toDocument(obj, doc): Convert object → Document (serialization)
     *          - fromDocument(doc, obj): Convert Document → object (deserialization)
     *
     *          User types must provide member methods with these signatures:
     *          - void toDocument(const Serializer<T>&, Document&) const
     *          - void fromDocument(const Document&, const Serializer<T>&)
     *
     *          For write-only streaming serialization, see BuilderTraits.
     */
    template <typename T>
    struct DocumentTraits
    {
        /**
         * @brief Convert object to Document (serialization)
         * @param obj Object to convert (source)
         * @param doc Document to populate (destination)
         */
        static void toDocument( const T& obj, Document& doc )
        {
            Serializer<T> serializer;
            obj.toDocument( serializer, doc );
        }

        /**
         * @brief Convert Document to object (deserialization)
         * @param doc Document to read from (source)
         * @param obj Object to populate (destination)
         */
        static void fromDocument( const Document& doc, T& obj )
        {
            Serializer<T> serializer;
            obj.fromDocument( doc, serializer );
        }
    };
} // namespace nfx::serialization::json
