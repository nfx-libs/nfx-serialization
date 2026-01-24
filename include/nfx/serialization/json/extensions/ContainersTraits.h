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
 *          for nfx::containers types (PerfectHashMap, FastHashMap, FastHashSet).
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

#include "nfx/serialization/json/SerializableDocument.h"
#include "nfx/serialization/json/SerializationTraits.h"
#include "nfx/serialization/json/Serializer.h"

#include <utility> // for std::pair

//=====================================================================
// std::pair support
//=====================================================================

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for std::pair
     * @details Serializes std::pair as an object with "first" and "second" fields
     */
    template <typename TFirst, typename TSecond>
    struct SerializationTraits<std::pair<TFirst, TSecond>>
    {
        /**
         * @brief Serialize std::pair to JSON document
         * @param obj The pair object to serialize
         * @param doc The document to serialize into
         */
        static void serialize( const std::pair<TFirst, TSecond>& obj, SerializableDocument& doc )
        {
            // Create object to hold the pair
            doc.document().set<nfx::json::Object>( "" );

            // Serialize first
            Document firstDoc;
            Serializer<TFirst> firstSerializer;
            firstDoc = firstSerializer.serialize( obj.first );
            doc.document().set<nfx::json::Document>(
                "first", std::move( static_cast<nfx::json::Document&>( firstDoc ) ) );

            // Serialize second
            Document secondDoc;
            Serializer<TSecond> secondSerializer;
            secondDoc = secondSerializer.serialize( obj.second );
            doc.document().set<nfx::json::Document>(
                "second", std::move( static_cast<nfx::json::Document&>( secondDoc ) ) );
        }

        /**
         * @brief Deserialize std::pair from JSON document
         * @param doc The document to deserialize from
         * @param obj The pair object to deserialize into
         */
        static void deserialize( const SerializableDocument& doc, std::pair<TFirst, TSecond>& obj )
        {
            // Deserialize first
            auto firstDoc = doc.document().get<nfx::json::Document>( "first" );
            if( firstDoc.has_value() )
            {
                Serializer<TFirst> firstSerializer;
                obj.first = firstSerializer.deserialize( firstDoc.value() );
            }

            // Deserialize second
            auto secondDoc = doc.document().get<nfx::json::Document>( "second" );
            if( secondDoc.has_value() )
            {
                Serializer<TSecond> secondSerializer;
                obj.second = secondSerializer.deserialize( secondDoc.value() );
            }
        }
    };
} // namespace nfx::serialization::json

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
         * @brief Serialize PerfectHashMap to JSON document as an array of key-value pairs
         * @param obj The PerfectHashMap object to serialize
         * @param doc The document to serialize into
         * @details Uses array format to avoid JSON Pointer issues with empty/special character keys
         */
        static void serialize(
            const nfx::containers::PerfectHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj,
            SerializableDocument& doc )
        {
            // Create array to hold key-value pairs
            doc.document().set<nfx::json::Array>( "" );

            size_t index = 0;
            // Use PerfectHashMap's iterator to traverse all key-value pairs
            for( auto it = obj.begin(); it != obj.end(); ++it, ++index )
            {
                const auto& pair = *it;
                const TKey& key = pair.first;
                const TValue& value = pair.second;

                // Build JSON Pointer path for this array index
                char arrayPath[32];
                std::snprintf( arrayPath, sizeof( arrayPath ), "/%zu", index );

                // Initialize this array element as an object
                doc.document().set<nfx::json::Object>( arrayPath );

                // Serialize the key
                Document keyDoc;
                Serializer<TKey> keySerializer;
                keyDoc = keySerializer.serialize( key );

                // Serialize the value
                Document valueDoc;
                Serializer<TValue> valueSerializer;
                valueDoc = valueSerializer.serialize( value );

                // Build paths for key and value fields
                std::string keyPath = std::string( arrayPath ) + "/key";
                std::string valuePath = std::string( arrayPath ) + "/value";

                // Add key and value to pair object
                if( keyDoc.is<std::string>( "" ) )
                {
                    auto str = keyDoc.get<std::string>( "" );
                    doc.set<std::string>( keyPath, str.value() );
                }
                else if( keyDoc.is<int>( "" ) )
                {
                    auto val = keyDoc.get<int64_t>( "" );
                    doc.set<int64_t>( keyPath, val.value() );
                }
                else if( keyDoc.is<double>( "" ) )
                {
                    auto val = keyDoc.get<double>( "" );
                    doc.set<double>( keyPath, val.value() );
                }
                else if( keyDoc.is<bool>( "" ) )
                {
                    auto val = keyDoc.get<bool>( "" );
                    doc.set<bool>( keyPath, val.value() );
                }
                else if( keyDoc.is<Array>( "" ) || keyDoc.is<Object>( "" ) )
                {
                    doc.document().set<nfx::json::Document>(
                        keyPath, std::move( static_cast<nfx::json::Document&>( keyDoc ) ) );
                }

                if( valueDoc.is<std::string>( "" ) )
                {
                    auto str = valueDoc.get<std::string>( "" );
                    doc.set<std::string>( valuePath, str.value() );
                }
                else if( valueDoc.is<int>( "" ) )
                {
                    auto val = valueDoc.get<int64_t>( "" );
                    doc.set<int64_t>( valuePath, val.value() );
                }
                else if( valueDoc.is<double>( "" ) )
                {
                    auto val = valueDoc.get<double>( "" );
                    doc.set<double>( valuePath, val.value() );
                }
                else if( valueDoc.is<bool>( "" ) )
                {
                    auto val = valueDoc.get<bool>( "" );
                    doc.set<bool>( valuePath, val.value() );
                }
                else if( valueDoc.isNull( "" ) )
                {
                    doc.setNull( valuePath );
                }
                else if( valueDoc.is<Array>( "" ) || valueDoc.is<Object>( "" ) )
                {
                    doc.document().set<nfx::json::Document>(
                        valuePath, std::move( static_cast<nfx::json::Document&>( valueDoc ) ) );
                }
            }
        }

        /**
         * @brief Deserialize PerfectHashMap from JSON document
         * @param obj The PerfectHashMap object to deserialize into
         * @param doc The document to deserialize from
         * @details Expects array format with key-value pair objects
         */
        static void deserialize(
            nfx::containers::PerfectHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj,
            const SerializableDocument& doc )
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
         * @brief Serialize FastHashMap to JSON document as an array of key-value pairs
         * @param obj The FastHashMap object to serialize
         * @param doc The document to serialize into
         * @details Uses array format to avoid JSON Pointer issues with empty/special character keys
         */
        static void serialize(
            const nfx::containers::FastHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj,
            SerializableDocument& doc )
        {
            // Create array to hold key-value pairs
            doc.document().set<nfx::json::Array>( "" );

            size_t index = 0;
            // Use FastHashMap's iterator to traverse all key-value pairs
            for( auto it = obj.begin(); it != obj.end(); ++it, ++index )
            {
                const auto& pair = *it;
                const TKey& key = pair.first;
                const TValue& value = pair.second;

                // Build JSON Pointer path for this array index
                char arrayPath[32];
                std::snprintf( arrayPath, sizeof( arrayPath ), "/%zu", index );

                // Initialize this array element as an object
                doc.document().set<nfx::json::Object>( arrayPath );

                // Serialize the key
                Document keyDoc;
                Serializer<TKey> keySerializer;
                keyDoc = keySerializer.serialize( key );

                // Serialize the value
                Document valueDoc;
                Serializer<TValue> valueSerializer;
                valueDoc = valueSerializer.serialize( value );

                // Build paths for key and value fields
                std::string keyPath = std::string( arrayPath ) + "/key";
                std::string valuePath = std::string( arrayPath ) + "/value";

                // Add key to pair object
                if( keyDoc.is<std::string>( "" ) )
                {
                    auto str = keyDoc.get<std::string>( "" );
                    doc.set<std::string>( keyPath, str.value() );
                }
                else if( keyDoc.is<int>( "" ) )
                {
                    auto val = keyDoc.get<int64_t>( "" );
                    doc.set<int64_t>( keyPath, val.value() );
                }
                else if( keyDoc.is<double>( "" ) )
                {
                    auto val = keyDoc.get<double>( "" );
                    doc.set<double>( keyPath, val.value() );
                }
                else if( keyDoc.is<bool>( "" ) )
                {
                    auto val = keyDoc.get<bool>( "" );
                    doc.set<bool>( keyPath, val.value() );
                }
                else if( keyDoc.is<Array>( "" ) || keyDoc.is<Object>( "" ) )
                {
                    doc.document().set<nfx::json::Document>(
                        keyPath, std::move( static_cast<nfx::json::Document&>( keyDoc ) ) );
                }

                // Add value to pair object
                if( valueDoc.is<std::string>( "" ) )
                {
                    auto str = valueDoc.get<std::string>( "" );
                    doc.set<std::string>( valuePath, str.value() );
                }
                else if( valueDoc.is<int>( "" ) )
                {
                    auto val = valueDoc.get<int64_t>( "" );
                    doc.set<int64_t>( valuePath, val.value() );
                }
                else if( valueDoc.is<double>( "" ) )
                {
                    auto val = valueDoc.get<double>( "" );
                    doc.set<double>( valuePath, val.value() );
                }
                else if( valueDoc.is<bool>( "" ) )
                {
                    auto val = valueDoc.get<bool>( "" );
                    doc.set<bool>( valuePath, val.value() );
                }
                else if( valueDoc.isNull( "" ) )
                {
                    doc.setNull( valuePath );
                }
                else if( valueDoc.is<Array>( "" ) || valueDoc.is<Object>( "" ) )
                {
                    doc.document().set<nfx::json::Document>(
                        valuePath, std::move( static_cast<nfx::json::Document&>( valueDoc ) ) );
                }
            }
        }

        /**
         * @brief Deserialize FastHashMap from JSON document
         * @param obj The FastHashMap object to deserialize into
         * @param doc The document to deserialize from
         * @details Supports both array format and object format for compatibility
         */
        static void deserialize(
            nfx::containers::FastHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj,
            const SerializableDocument& doc )
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
                            key = keySerializer.deserialize( keyDoc );
                        }

                        // Extract value
                        TValue value{};
                        Serializer<TValue> valueSerializer;
                        value = valueSerializer.deserialize( valueDoc );
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
         * @brief Serialize FastHashSet to JSON document as an array
         * @param obj The FastHashSet object to serialize
         * @param doc The document to serialize into
         */
        static void serialize(
            const nfx::containers::FastHashSet<TKey, HashType, Seed, Hasher, KeyEqual>& obj, SerializableDocument& doc )
        {
            // Create array document
            doc.document().set<nfx::json::Array>( "" );

            size_t index = 0;
            // Use FastHashSet's iterator to traverse all elements
            for( auto it = obj.begin(); it != obj.end(); ++it, ++index )
            {
                const TKey& key = *it;

                // Serialize the key using a temporary serializer
                Document keyDoc;
                Serializer<TKey> keySerializer;
                keyDoc = keySerializer.serialize( key );

                // Build JSON Pointer path for this index
                char arrayPath[32];
                std::snprintf( arrayPath, sizeof( arrayPath ), "/%zu", index );

                // Add to array based on type
                if( keyDoc.is<std::string>( "" ) )
                {
                    auto str = keyDoc.get<std::string>( "" );
                    doc.set<std::string>( arrayPath, str.value() );
                }
                else if( keyDoc.is<int>( "" ) )
                {
                    auto val = keyDoc.get<int64_t>( "" );
                    doc.set<int64_t>( arrayPath, val.value() );
                }
                else if( keyDoc.is<double>( "" ) )
                {
                    auto val = keyDoc.get<double>( "" );
                    doc.set<double>( arrayPath, val.value() );
                }
                else if( keyDoc.is<bool>( "" ) )
                {
                    auto val = keyDoc.get<bool>( "" );
                    doc.set<bool>( arrayPath, val.value() );
                }
                else if( keyDoc.is<Object>( "" ) || keyDoc.is<Array>( "" ) )
                {
                    // Handle nested objects and arrays
                    doc.document().set<nfx::json::Document>(
                        arrayPath, std::move( static_cast<nfx::json::Document&>( keyDoc ) ) );
                }
            }
        }

        /**
         * @brief Deserialize FastHashSet from JSON document
         * @param obj The FastHashSet object to deserialize into
         * @param doc The document to deserialize from
         */
        static void deserialize(
            nfx::containers::FastHashSet<TKey, HashType, Seed, Hasher, KeyEqual>& obj, const SerializableDocument& doc )
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
                for( const auto& elementDoc : arrayOpt.value() )
                {
                    // Deserialize the key using a temporary serializer
                    TKey key{};
                    Serializer<TKey> keySerializer;
                    key = keySerializer.deserialize( elementDoc );

                    obj.insert( std::move( key ) );
                }
            }
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/FastHashSet.h>)

//=====================================================================
// SmallVector support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/SmallVector.h>)

#    include <nfx/containers/SmallVector.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::containers::SmallVector
     */
    template <typename T, std::size_t N>
    struct SerializationTraits<nfx::containers::SmallVector<T, N>>
    {
        /**
         * @brief Serialize SmallVector to JSON document as an array
         * @param obj The SmallVector object to serialize
         * @param doc The document to serialize into
         */
        static void serialize( const nfx::containers::SmallVector<T, N>& obj, SerializableDocument& doc )
        {
            // Create array document
            doc.document().set<nfx::json::Array>( "" );

            size_t index = 0;
            // Use SmallVector's iterator to traverse all elements
            for( auto it = obj.begin(); it != obj.end(); ++it, ++index )
            {
                const T& element = *it;

                // Serialize the element using a temporary serializer
                Document elementDoc;
                Serializer<T> elementSerializer;
                elementDoc = elementSerializer.serialize( element );

                // Build JSON Pointer path for this index
                char arrayPath[32];
                std::snprintf( arrayPath, sizeof( arrayPath ), "/%zu", index );

                // Add to array based on type
                if( elementDoc.is<std::string>( "" ) )
                {
                    auto str = elementDoc.get<std::string>( "" );
                    doc.set<std::string>( arrayPath, str.value() );
                }
                else if( elementDoc.is<int>( "" ) )
                {
                    auto val = elementDoc.get<int64_t>( "" );
                    doc.set<int64_t>( arrayPath, val.value() );
                }
                else if( elementDoc.is<double>( "" ) )
                {
                    auto val = elementDoc.get<double>( "" );
                    doc.set<double>( arrayPath, val.value() );
                }
                else if( elementDoc.is<bool>( "" ) )
                {
                    auto val = elementDoc.get<bool>( "" );
                    doc.set<bool>( arrayPath, val.value() );
                }
                else if( elementDoc.isNull( "" ) )
                {
                    doc.setNull( arrayPath );
                }
                else if( elementDoc.is<Object>( "" ) || elementDoc.is<Array>( "" ) )
                {
                    // Handle nested objects and arrays
                    doc.document().set<nfx::json::Document>(
                        arrayPath, std::move( static_cast<nfx::json::Document&>( elementDoc ) ) );
                }
            }
        }

        /**
         * @brief Deserialize SmallVector from JSON document
         * @param obj The SmallVector object to deserialize into
         * @param doc The document to deserialize from
         */
        static void deserialize( nfx::containers::SmallVector<T, N>& obj, const SerializableDocument& doc )
        {
            if( !doc.is<Array>( "" ) )
            {
                throw std::runtime_error{ "Cannot deserialize non-array JSON value into SmallVector" };
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
                    element = elementSerializer.deserialize( elementDoc );

                    obj.push_back( std::move( element ) );
                }
            }
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/SmallVector.h>)
