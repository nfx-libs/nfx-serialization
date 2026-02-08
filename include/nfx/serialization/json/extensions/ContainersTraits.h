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
 * @file ContainersTraits.h
 * @brief SerializationTraits specializations for nfx-containers types
 * @details This is an optional extension header that provides JSON serialization support
 *          for nfx::containers types (PerfectHashMap, FastHashMap, FastHashSet, OrderedHashMap, OrderedHashSet).
 *
 *          This header is safe to include even if nfx-containers is not available.
 *          Each container type is independently supported - you can use any subset.
 *
 *          #include <nfx/serialization/json/SerializationTraits.h>
 *          #include <nfx/serialization/json/extensions/ContainersTraits.h>
 *
 * @note Each specialization is only enabled if its corresponding header is available.
 */

#pragma once

#include "nfx/serialization/json/Serializer.h"

//=====================================================================
// PerfectHashMap support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/PerfectHashMap.h>)

#    include <nfx/containers/PerfectHashMap.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::containers::PerfectHashMap
     */
    template <typename TKey, typename TValue, typename HashType, HashType Seed, typename Hasher, typename KeyEqual>
    struct SerializationTraits<nfx::containers::PerfectHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>>
    {
        /**
         * @brief Deserialize PerfectHashMap from JSON document
         * @param doc The document to deserialize from
         * @param obj The PerfectHashMap object to deserialize into
         * @details Expects array format with key-value pair objects
         */
        static void fromDocument(
            const Document& doc, nfx::containers::PerfectHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj )
        {
            if( !doc.is<Array>( "" ) )
            {
                throw std::runtime_error{ "Cannot deserialize non-array JSON value into PerfectHashMap" };
            }

            // Collect key-value pairs for PerfectHashMap construction
            std::vector<std::pair<TKey, TValue>> items;

            // Get array and iterate using STL iterator
            auto arrayOpt = doc.get<Array>( "" );
            if( arrayOpt.has_value() )
            {
                for( const auto& pairDoc : arrayOpt.value() )
                {
                    // Extract key - try different types
                    TKey key{};
                    bool keyFound = false;

                    if( pairDoc.contains( "key" ) )
                    {
                        // Try to deserialize key based on its actual JSON type
                        if constexpr( std::is_same_v<TKey, std::string> )
                        {
                            auto keyOpt = pairDoc.get<std::string>( "key" );
                            if( keyOpt )
                            {
                                key = *keyOpt;
                                keyFound = true;
                            }
                        }
                        else if constexpr( std::is_integral_v<TKey> && !std::is_same_v<TKey, bool> )
                        {
                            auto keyOpt = pairDoc.get<int64_t>( "key" );
                            if( keyOpt )
                            {
                                key = static_cast<TKey>( *keyOpt );
                                keyFound = true;
                            }
                        }
                        else if constexpr( std::is_floating_point_v<TKey> )
                        {
                            auto keyOpt = pairDoc.get<double>( "key" );
                            if( keyOpt )
                            {
                                key = static_cast<TKey>( *keyOpt );
                                keyFound = true;
                            }
                        }
                        else if constexpr( std::is_same_v<TKey, bool> )
                        {
                            auto keyOpt = pairDoc.get<bool>( "key" );
                            if( keyOpt )
                            {
                                key = *keyOpt;
                                keyFound = true;
                            }
                        }
                    }

                    // Extract value - try different types
                    TValue value{};
                    bool valueFound = false;

                    if( pairDoc.contains( "value" ) )
                    {
                        // Try to deserialize value based on its actual JSON type
                        if constexpr( std::is_same_v<TValue, std::string> )
                        {
                            auto valOpt = pairDoc.get<std::string>( "value" );
                            if( valOpt )
                            {
                                value = *valOpt;
                                valueFound = true;
                            }
                        }
                        else if constexpr( std::is_integral_v<TValue> && !std::is_same_v<TValue, bool> )
                        {
                            auto valOpt = pairDoc.get<int64_t>( "value" );
                            if( valOpt )
                            {
                                value = static_cast<TValue>( *valOpt );
                                valueFound = true;
                            }
                        }
                        else if constexpr( std::is_floating_point_v<TValue> )
                        {
                            auto valOpt = pairDoc.get<double>( "value" );
                            if( valOpt )
                            {
                                value = static_cast<TValue>( *valOpt );
                                valueFound = true;
                            }
                        }
                        else if constexpr( std::is_same_v<TValue, bool> )
                        {
                            auto valOpt = pairDoc.get<bool>( "value" );
                            if( valOpt )
                            {
                                value = *valOpt;
                                valueFound = true;
                            }
                        }
                    }

                    if( keyFound && valueFound )
                    {
                        items.emplace_back( std::move( key ), std::move( value ) );
                    }
                }
            }

            obj = nfx::containers::PerfectHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>( std::move( items ) );
        }

        /**
         * @brief High-performance streaming serialization
         * @param obj The PerfectHashMap object to serialize
         * @param builder The builder to write to
         * @details Serializes as array of {key, value} objects: [{key:..., value:...}, ...]
         */
        static void serialize(
            const nfx::containers::PerfectHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj,
            nfx::json::Builder& builder )
        {
            builder.writeStartArray();

            for( auto it = obj.begin(); it != obj.end(); ++it )
            {
                const auto& pair = *it;

                builder.writeStartObject();

                // Write key
                builder.writeKey( "key" );
                Serializer<TKey>{}.serializeValue( pair.first, builder );

                // Write value
                builder.writeKey( "value" );
                Serializer<TValue>{}.serializeValue( pair.second, builder );

                builder.writeEndObject();
            }

            builder.writeEndArray();
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/PerfectHashMap.h>)

//=====================================================================
// FastHashMap support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/FastHashMap.h>)

#    include <nfx/containers/FastHashMap.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::containers::FastHashMap
     */
    template <typename TKey, typename TValue, typename HashType, HashType Seed, typename Hasher, typename KeyEqual>
    struct SerializationTraits<nfx::containers::FastHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>>
    {
        /**
         * @brief Deserialize FastHashMap from JSON document
         * @param doc The document to deserialize from
         * @param obj The FastHashMap object to deserialize into
         * @details Supports both array format and object format for compatibility
         */
        static void fromDocument(
            const Document& doc, nfx::containers::FastHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj )
        {
            // Clear existing content
            obj.clear();

            // Check if it's an object format (standard JSON map representation)
            if( doc.is<Object>( "" ) )
            {
                // Use Object iterator for object format
                auto objectOpt = doc.get<Object>( "" );
                if( objectOpt.has_value() )
                {
                    for( const auto& [keyStr, valueDoc] : objectOpt.value() )
                    {
                        // Deserialize key (typically string, but support other types)
                        TKey key{};
                        if constexpr( std::is_same_v<TKey, std::string> )
                        {
                            key = keyStr;
                        }
                        else
                        {
                            Document keyDoc;
                            keyDoc.set( "", keyStr );
                            Serializer<TKey> keySerializer;
                            keySerializer.deserializeValue( keyDoc, key );
                        }

                        // Extract value
                        TValue value{};
                        Serializer<TValue> valueSerializer;
                        valueSerializer.deserializeValue( valueDoc, value );
                        obj.insertOrAssign( std::move( key ), std::move( value ) );
                    }
                }
            }
            // Check if it's an array format (for backward compatibility)
            else if( doc.is<Array>( "" ) )
            {
                // Get array and iterate using STL iterator
                auto arrayOpt = doc.get<Array>( "" );
                if( arrayOpt.has_value() )
                {
                    for( const auto& pairDoc : arrayOpt.value() )
                    {
                        // Extract key - try different types
                        TKey key{};
                        bool keyFound = false;

                        if( pairDoc.contains( "key" ) )
                        {
                            // Try to deserialize key based on its actual JSON type
                            if constexpr( std::is_same_v<TKey, std::string> )
                            {
                                auto keyOpt = pairDoc.get<std::string>( "key" );
                                if( keyOpt )
                                {
                                    key = *keyOpt;
                                    keyFound = true;
                                }
                            }
                            else if constexpr( std::is_integral_v<TKey> && !std::is_same_v<TKey, bool> )
                            {
                                auto keyOpt = pairDoc.get<int64_t>( "key" );
                                if( keyOpt )
                                {
                                    key = static_cast<TKey>( *keyOpt );
                                    keyFound = true;
                                }
                            }
                            else if constexpr( std::is_floating_point_v<TKey> )
                            {
                                auto keyOpt = pairDoc.get<double>( "key" );
                                if( keyOpt )
                                {
                                    key = static_cast<TKey>( *keyOpt );
                                    keyFound = true;
                                }
                            }
                            else if constexpr( std::is_same_v<TKey, bool> )
                            {
                                auto keyOpt = pairDoc.get<bool>( "key" );
                                if( keyOpt )
                                {
                                    key = *keyOpt;
                                    keyFound = true;
                                }
                            }
                        }

                        // Extract value - try different types
                        TValue value{};
                        bool valueFound = false;

                        if( pairDoc.contains( "value" ) )
                        {
                            // Try to deserialize value based on its actual JSON type
                            if constexpr( std::is_same_v<TValue, std::string> )
                            {
                                auto valOpt = pairDoc.get<std::string>( "value" );
                                if( valOpt )
                                {
                                    value = *valOpt;
                                    valueFound = true;
                                }
                            }
                            else if constexpr( std::is_integral_v<TValue> && !std::is_same_v<TValue, bool> )
                            {
                                auto valOpt = pairDoc.get<int64_t>( "value" );
                                if( valOpt )
                                {
                                    value = static_cast<TValue>( *valOpt );
                                    valueFound = true;
                                }
                            }
                            else if constexpr( std::is_floating_point_v<TValue> )
                            {
                                auto valOpt = pairDoc.get<double>( "value" );
                                if( valOpt )
                                {
                                    value = static_cast<TValue>( *valOpt );
                                    valueFound = true;
                                }
                            }
                            else if constexpr( std::is_same_v<TValue, bool> )
                            {
                                auto valOpt = pairDoc.get<bool>( "value" );
                                if( valOpt )
                                {
                                    value = *valOpt;
                                    valueFound = true;
                                }
                            }
                        }

                        if( keyFound && valueFound )
                        {
                            obj.insertOrAssign( std::move( key ), std::move( value ) );
                        }
                    }
                }
            }
            else
            {
                throw std::runtime_error{ "Cannot deserialize JSON value into FastHashMap: must be object or array" };
            }
        }

        /**
         * @brief High-performance streaming serialization
         * @param obj The FastHashMap object to serialize
         * @param builder The builder to write to
         * @details Serializes as array of {key, value} objects: [{key:..., value:...}, ...]
         */
        static void serialize(
            const nfx::containers::FastHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj,
            nfx::json::Builder& builder )
        {
            builder.writeStartArray();

            for( auto it = obj.begin(); it != obj.end(); ++it )
            {
                const auto& pair = *it;

                builder.writeStartObject();

                // Write key
                builder.writeKey( "key" );
                Serializer<TKey>{}.serializeValue( pair.first, builder );

                // Write value
                builder.writeKey( "value" );
                Serializer<TValue>{}.serializeValue( pair.second, builder );

                builder.writeEndObject();
            }

            builder.writeEndArray();
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/FastHashMap.h>)

//=====================================================================
// FastHashSet support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/FastHashSet.h>)

#    include <nfx/containers/FastHashSet.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::containers::FastHashSet
     */
    template <typename TKey, typename HashType, HashType Seed, typename Hasher, typename KeyEqual>
    struct SerializationTraits<nfx::containers::FastHashSet<TKey, HashType, Seed, Hasher, KeyEqual>>
    {
        /**
         * @brief Deserialize FastHashSet from JSON document
         * @param doc The document to deserialize from
         * @param obj The FastHashSet object to deserialize into
         */
        static void fromDocument(
            const Document& doc, nfx::containers::FastHashSet<TKey, HashType, Seed, Hasher, KeyEqual>& obj )
        {
            if( !doc.is<Array>( "" ) )
            {
                throw std::runtime_error{ "Cannot deserialize non-array JSON value into FastHashSet" };
            }

            // Clear existing content
            obj.clear();

            // Get array and iterate using STL iterator
            auto arrayOpt = doc.get<Array>( "" );
            if( arrayOpt.has_value() )
            {
                for( const auto& itemDoc : arrayOpt.value() )
                {
                    TKey item;
                    Serializer<TKey>{}.deserializeValue( itemDoc, item );
                    obj.insert( std::move( item ) );
                }
            }
        }

        /**
         * @brief High-performance streaming serialization
         * @param obj The FastHashSet object to serialize
         * @param builder The builder to write to
         * @details Serializes as JSON array of elements
         */
        static void serialize(
            const nfx::containers::FastHashSet<TKey, HashType, Seed, Hasher, KeyEqual>& obj,
            nfx::json::Builder& builder )
        {
            builder.writeStartArray();

            for( auto it = obj.begin(); it != obj.end(); ++it )
            {
                Serializer<TKey>{}.serializeValue( *it, builder );
            }

            builder.writeEndArray();
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/FastHashSet.h>)

//=====================================================================
// StackVector support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/StackVector.h>)

#    include <nfx/containers/StackVector.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::containers::StackVector
     */
    template <typename T, std::size_t N>
    struct SerializationTraits<nfx::containers::StackVector<T, N>>
    {
        /**
         * @brief Deserialize StackVector from JSON document
         * @param doc The document to deserialize from
         * @param obj The StackVector object to deserialize into
         */
        static void fromDocument( const Document& doc, nfx::containers::StackVector<T, N>& obj )
        {
            if( !doc.is<Array>( "" ) )
            {
                throw std::runtime_error{ "Cannot deserialize non-array JSON value into StackVector" };
            }

            // Clear existing content
            obj.clear();

            // Get array and iterate using STL iterator
            auto arrayOpt = doc.get<Array>( "" );
            if( arrayOpt.has_value() )
            {
                for( const auto& elementDoc : arrayOpt.value() )
                {
                    // Deserialize the element using a temporary serializer
                    T element{};
                    Serializer<T> elementSerializer;
                    elementSerializer.deserializeValue( elementDoc, element );

                    obj.push_back( std::move( element ) );
                }
            }
        }

        /**
         * @brief High-performance streaming serialization
         * @param obj The StackVector object to serialize
         * @param builder The builder to write to
         * @details Serializes as JSON array of elements
         */
        static void serialize( const nfx::containers::StackVector<T, N>& obj, nfx::json::Builder& builder )
        {
            builder.writeStartArray();

            for( const auto& element : obj )
            {
                Serializer<T>{}.serializeValue( element, builder );
            }

            builder.writeEndArray();
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/StackVector.h>)

//=====================================================================
// OrderedHashMap support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/OrderedHashMap.h>)

#    include <nfx/containers/OrderedHashMap.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::containers::OrderedHashMap
     */
    template <typename TKey, typename TValue, typename HashType, HashType Seed, typename Hasher, typename KeyEqual>
    struct SerializationTraits<nfx::containers::OrderedHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>>
    {
        /**
         * @brief Deserialize OrderedHashMap from JSON document
         * @param doc The document to deserialize from
         * @param obj The OrderedHashMap object to deserialize into
         * @details Expects array format with key-value pair objects. Preserves insertion order.
         */
        static void fromDocument(
            const Document& doc, nfx::containers::OrderedHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj )
        {
            if( !doc.is<Array>( "" ) )
            {
                throw std::runtime_error{ "Cannot deserialize non-array JSON value into OrderedHashMap" };
            }

            obj.clear();

            // Get array and iterate using STL iterator
            auto arrayOpt = doc.get<Array>( "" );
            if( arrayOpt.has_value() )
            {
                for( const auto& pairDoc : arrayOpt.value() )
                {
                    // Extract key - try different types
                    TKey key{};
                    bool keyFound = false;

                    if( pairDoc.contains( "key" ) )
                    {
                        // Try to deserialize key based on its actual JSON type
                        if constexpr( std::is_same_v<TKey, std::string> )
                        {
                            auto keyOpt = pairDoc.get<std::string>( "key" );
                            if( keyOpt )
                            {
                                key = *keyOpt;
                                keyFound = true;
                            }
                        }
                        else if constexpr( std::is_integral_v<TKey> && !std::is_same_v<TKey, bool> )
                        {
                            auto keyOpt = pairDoc.get<int64_t>( "key" );
                            if( keyOpt )
                            {
                                key = static_cast<TKey>( *keyOpt );
                                keyFound = true;
                            }
                        }
                        else if constexpr( std::is_floating_point_v<TKey> )
                        {
                            auto keyOpt = pairDoc.get<double>( "key" );
                            if( keyOpt )
                            {
                                key = static_cast<TKey>( *keyOpt );
                                keyFound = true;
                            }
                        }
                        else if constexpr( std::is_same_v<TKey, bool> )
                        {
                            auto keyOpt = pairDoc.get<bool>( "key" );
                            if( keyOpt )
                            {
                                key = *keyOpt;
                                keyFound = true;
                            }
                        }
                    }

                    // Extract value - try different types
                    TValue value{};
                    bool valueFound = false;

                    if( pairDoc.contains( "value" ) )
                    {
                        // Try to deserialize value based on its actual JSON type
                        if constexpr( std::is_same_v<TValue, std::string> )
                        {
                            auto valOpt = pairDoc.get<std::string>( "value" );
                            if( valOpt )
                            {
                                value = *valOpt;
                                valueFound = true;
                            }
                        }
                        else if constexpr( std::is_integral_v<TValue> && !std::is_same_v<TValue, bool> )
                        {
                            auto valOpt = pairDoc.get<int64_t>( "value" );
                            if( valOpt )
                            {
                                value = static_cast<TValue>( *valOpt );
                                valueFound = true;
                            }
                        }
                        else if constexpr( std::is_floating_point_v<TValue> )
                        {
                            auto valOpt = pairDoc.get<double>( "value" );
                            if( valOpt )
                            {
                                value = static_cast<TValue>( *valOpt );
                                valueFound = true;
                            }
                        }
                        else if constexpr( std::is_same_v<TValue, bool> )
                        {
                            auto valOpt = pairDoc.get<bool>( "value" );
                            if( valOpt )
                            {
                                value = *valOpt;
                                valueFound = true;
                            }
                        }
                    }

                    if( keyFound && valueFound )
                    {
                        obj.insertOrAssign( std::move( key ), std::move( value ) );
                    }
                }
            }
        }

        /**
         * @brief Serialize OrderedHashMap to Builder
         * @param obj The OrderedHashMap object to serialize
         * @param builder The builder to write to
         * @details Serializes as array of {"key": K, "value": V} objects. Preserves insertion order.
         */
        static void serialize(
            const nfx::containers::OrderedHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj,
            Builder& builder )
        {
            builder.writeStartArray();

            for( const auto& [key, value] : obj )
            {
                builder.writeStartObject();
                builder.write( "key", key );
                builder.write( "value", value );
                builder.writeEndObject();
            }

            builder.writeEndArray();
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/OrderedHashMap.h>)

//=====================================================================
// OrderedHashSet support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/OrderedHashSet.h>)

#    include <nfx/containers/OrderedHashSet.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::containers::OrderedHashSet
     */
    template <typename TKey, typename HashType, HashType Seed, typename Hasher, typename KeyEqual>
    struct SerializationTraits<nfx::containers::OrderedHashSet<TKey, HashType, Seed, Hasher, KeyEqual>>
    {
        /**
         * @brief Deserialize OrderedHashSet from JSON document
         * @param doc The document to deserialize from
         * @param obj The OrderedHashSet object to deserialize into
         * @details Expects array format. Preserves insertion order.
         */
        static void fromDocument(
            const Document& doc, nfx::containers::OrderedHashSet<TKey, HashType, Seed, Hasher, KeyEqual>& obj )
        {
            if( !doc.is<Array>( "" ) )
            {
                throw std::runtime_error{ "Cannot deserialize non-array JSON value into OrderedHashSet" };
            }

            obj.clear();

            // Get array and iterate using STL iterator
            auto arrayOpt = doc.get<Array>( "" );
            if( arrayOpt.has_value() )
            {
                for( const auto& elementDoc : arrayOpt.value() )
                {
                    // Extract element - try different types
                    TKey element{};
                    bool elementFound = false;

                    // Try to deserialize element based on its actual JSON type
                    if constexpr( std::is_same_v<TKey, std::string> )
                    {
                        auto elemOpt = elementDoc.get<std::string>( "" );
                        if( elemOpt )
                        {
                            element = *elemOpt;
                            elementFound = true;
                        }
                    }
                    else if constexpr( std::is_integral_v<TKey> && !std::is_same_v<TKey, bool> )
                    {
                        auto elemOpt = elementDoc.get<int64_t>( "" );
                        if( elemOpt )
                        {
                            element = static_cast<TKey>( *elemOpt );
                            elementFound = true;
                        }
                    }
                    else if constexpr( std::is_floating_point_v<TKey> )
                    {
                        auto elemOpt = elementDoc.get<double>( "" );
                        if( elemOpt )
                        {
                            element = static_cast<TKey>( *elemOpt );
                            elementFound = true;
                        }
                    }
                    else if constexpr( std::is_same_v<TKey, bool> )
                    {
                        auto elemOpt = elementDoc.get<bool>( "" );
                        if( elemOpt )
                        {
                            element = *elemOpt;
                            elementFound = true;
                        }
                    }

                    if( elementFound )
                    {
                        obj.insert( std::move( element ) );
                    }
                }
            }
        }

        /**
         * @brief Serialize OrderedHashSet to Builder
         * @param obj The OrderedHashSet object to serialize
         * @param builder The builder to write to
         * @details Serializes as array. Preserves insertion order.
         */
        static void serialize(
            const nfx::containers::OrderedHashSet<TKey, HashType, Seed, Hasher, KeyEqual>& obj, Builder& builder )
        {
            builder.writeStartArray();

            for( const auto& element : obj )
            {
                builder.write( element );
            }

            builder.writeEndArray();
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/OrderedHashSet.h>)
