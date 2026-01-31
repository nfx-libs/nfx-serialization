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
 * @file BuilderTraits.h
 * @brief Builder traits for high-performance JSON serialization
 * @details Contains the BuilderTraits template that provides direct JSON Builder
 *          serialization for types, avoiding the Document→JSON conversion overhead.
 *
 *          Types can specialize BuilderTraits to provide optimized serialization
 *          directly to the JSON Builder, bypassing the intermediate Document representation.
 *
 *          For extension types (datetime, datatypes, containers), include the appropriate
 *          extension headers that provide BuilderTraits specializations:
 *          - #include <nfx/serialization/json/extensions/DateTimeTraits.h>
 *          - #include <nfx/serialization/json/extensions/DatatypesTraits.h>
 *          - #include <nfx/serialization/json/extensions/ContainersTraits.h>
 */

#pragma once

#include "nfx/serialization/json/SerializableDocument.h"

#include <nfx/json/Builder.h>
#include <nfx/json/Document.h>

#include <type_traits>

namespace nfx::serialization::json
{
    //=====================================================================
    // Forward declarations
    //=====================================================================

    template <typename T>
    class Serializer;

    template <typename T>
    struct BuilderTraits;

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
    // Builder Traits (extensible by users)
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

    /**
     * @brief BuilderTraits specialization for SerializableDocument
     * @details Delegates to Builder's native write(Document) method for optimal performance.
     *          Builder handles the Document traversal internally with zero-copy efficiency.
     */
    template <>
    struct BuilderTraits<SerializableDocument>
    {
        /**
         * @brief Serialize SerializableDocument to JSON Builder
         * @param doc The SerializableDocument to serialize
         * @param builder The JSON Builder to write to
         */
        static void serialize( const SerializableDocument& doc, nfx::json::Builder& builder )
        {
            builder.write( doc.document() );
        }
    };

} // namespace nfx::serialization::json
