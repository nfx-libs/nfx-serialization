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
     *          instead of falling back to SerializationTraits + Document conversion.
     *
     * @example
     * ```cpp
     * template <>
     * struct BuilderTraits<MyType>
     * {
     *     static void serialize( const MyType& obj, nfx::json::Builder& builder )
     *     {
     *         builder.writeStartObject();
     *         builder.writePropertyName( "field1" );
     *         builder.writeNumberValue( obj.field1 );
     *         builder.writePropertyName( "field2" );
     *         builder.writeStringValue( obj.field2 );
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
     * @details High-performance streaming serialization that traverses the internal
     *          Document DOM and writes directly to the Builder, avoiding the
     *          toString() string allocation and subsequent copy in writeRawJson().
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
            serializeDocument( doc.document(), builder );
        }

    private:
        /**
         * @brief Serialize a Document to Builder by traversing its structure
         * @param doc The Document to serialize
         * @param builder The JSON Builder to write to
         */
        static void serializeDocument( const nfx::json::Document& doc, nfx::json::Builder& builder )
        {
            using namespace nfx::json;

            // Dispatch based on root type
            switch( doc.type() )
            {
                case Type::Null:
                    builder.writeNullValue();
                    break;

                case Type::Boolean:
                    if( auto val = doc.root<bool>() )
                    {
                        builder.writeBooleanValue( *val );
                    }
                    break;

                case Type::Integer:
                    if( auto val = doc.root<int64_t>() )
                    {
                        builder.writeNumberValue( *val );
                    }
                    break;

                case Type::UnsignedInteger:
                    if( auto val = doc.root<uint64_t>() )
                    {
                        builder.writeNumberValue( *val );
                    }
                    break;

                case Type::Double:
                    if( auto val = doc.root<double>() )
                    {
                        builder.writeNumberValue( *val );
                    }
                    break;

                case Type::String:
                    if( auto val = doc.root<std::string>() )
                    {
                        builder.writeStringValue( *val );
                    }
                    break;

                case Type::Array:
                    if( auto arr = doc.root<Array>() )
                    {
                        serializeArray( *arr, builder );
                    }
                    break;

                case Type::Object:
                    if( auto obj = doc.root<Object>() )
                    {
                        serializeObject( *obj, builder );
                    }
                    break;

                default:
                    throw std::runtime_error{ "Unknown Document type" };
            }
        }

        /**
         * @brief Serialize a JSON Object to Builder
         * @param obj The Object to serialize
         * @param builder The JSON Builder to write to
         */
        static void serializeObject( const nfx::json::Object& obj, nfx::json::Builder& builder )
        {
            builder.writeStartObject();

            for( const auto& [key, value] : obj )
            {
                builder.writePropertyName( key );
                serializeDocument( value, builder );
            }

            builder.writeEndObject();
        }

        /**
         * @brief Serialize a JSON Array to Builder
         * @param arr The Array to serialize
         * @param builder The JSON Builder to write to
         */
        static void serializeArray( const nfx::json::Array& arr, nfx::json::Builder& builder )
        {
            builder.writeStartArray();

            for( const auto& value : arr )
            {
                serializeDocument( value, builder );
            }

            builder.writeEndArray();
        }
    };

} // namespace nfx::serialization::json
