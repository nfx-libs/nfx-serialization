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

#include "nfx/serialization/json/Document.h"
#include "nfx/serialization/json/SerializationTraits.h"
#include "nfx/serialization/json/Serializer.h"

//=====================================================================
// PerfectHashMap support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/PerfectHashMap.h>)

#	include <nfx/containers/PerfectHashMap.h>

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
		static void serialize( const nfx::containers::PerfectHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj, Document& doc )
		{
			// Create array to hold key-value pairs
			doc.set<Document::Array>( "" );
			auto arrayRef = doc.get<Document::Array>( "" );

			if ( !arrayRef.has_value() )
			{
				return;
			}

			// Use PerfectHashMap's iterator to traverse all key-value pairs
			for ( auto it = obj.begin(); it != obj.end(); ++it )
			{
				const auto& pair = *it;
				const TKey& key = pair.first;
				const TValue& value = pair.second;

				// Create object for this key-value pair
				Document pairDoc;
				pairDoc.set<Document::Object>( "" );

				// Serialize the key
				Document keyDoc;
				Serializer<TKey> keySerializer;
				keyDoc = keySerializer.serialize( key );

				// Serialize the value
				Document valueDoc;
				Serializer<TValue> valueSerializer;
				valueDoc = valueSerializer.serialize( value );

				// Add key and value to pair object
				if ( keyDoc.is<std::string>( "" ) )
				{
					auto str = keyDoc.get<std::string>( "" );
					pairDoc.set<std::string>( "/key", str.value() );
				}
				else if ( keyDoc.is<int>( "" ) )
				{
					auto val = keyDoc.get<int64_t>( "" );
					pairDoc.set<int64_t>( "/key", val.value() );
				}
				else if ( keyDoc.is<double>( "" ) )
				{
					auto val = keyDoc.get<double>( "" );
					pairDoc.set<double>( "/key", val.value() );
				}
				else if ( keyDoc.is<bool>( "" ) )
				{
					auto val = keyDoc.get<bool>( "" );
					pairDoc.set<bool>( "/key", val.value() );
				}
				else if ( keyDoc.is<Document::Array>( "" ) || keyDoc.is<Document::Object>( "" ) )
				{
					pairDoc.set<Document>( "/key", keyDoc );
				}

				if ( valueDoc.is<std::string>( "" ) )
				{
					auto str = valueDoc.get<std::string>( "" );
					pairDoc.set<std::string>( "/value", str.value() );
				}
				else if ( valueDoc.is<int>( "" ) )
				{
					auto val = valueDoc.get<int64_t>( "" );
					pairDoc.set<int64_t>( "/value", val.value() );
				}
				else if ( valueDoc.is<double>( "" ) )
				{
					auto val = valueDoc.get<double>( "" );
					pairDoc.set<double>( "/value", val.value() );
				}
				else if ( valueDoc.is<bool>( "" ) )
				{
					auto val = valueDoc.get<bool>( "" );
					pairDoc.set<bool>( "/value", val.value() );
				}
				else if ( valueDoc.isNull( "" ) )
				{
					pairDoc.setNull( "/value" );
				}
				else if ( valueDoc.is<Document::Array>( "" ) || valueDoc.is<Document::Object>( "" ) )
				{
					pairDoc.set<Document>( "/value", valueDoc );
				}

				// Add pair to array
				arrayRef->append<Document>( pairDoc );
			}
		}

		/**
		 * @brief Deserialize PerfectHashMap from JSON document
		 * @param obj The PerfectHashMap object to deserialize into
		 * @param doc The document to deserialize from
		 * @details Expects array format with key-value pair objects
		 */
		static void deserialize( nfx::containers::PerfectHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj, const Document& doc )
		{
			if ( !doc.is<Document::Array>( "" ) )
			{
				throw std::runtime_error( "Cannot deserialize non-array JSON value into PerfectHashMap" );
			}

			// Collect key-value pairs for PerfectHashMap construction
			std::vector<std::pair<TKey, TValue>> items;

			// Get array and iterate using STL iterator
			auto arrayOpt = doc.get<Document::Array>( "" );
			if ( arrayOpt.has_value() )
			{
				for ( const auto& pairDoc : arrayOpt.value() )
				{
					// Extract key
					TKey key{};
					if ( pairDoc.contains( "/key" ) )
					{
						Document keyDoc = pairDoc.get<Document>( "/key" ).value_or( Document{} );
						Serializer<TKey> keySerializer;
						key = keySerializer.deserialize( keyDoc );
					}

					// Extract value
					TValue value{};
					if ( pairDoc.contains( "/value" ) )
					{
						Document valueDoc = pairDoc.get<Document>( "/value" ).value_or( Document{} );
						Serializer<TValue> valueSerializer;
						value = valueSerializer.deserialize( valueDoc );
					}

					items.emplace_back( std::move( key ), std::move( value ) );
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

#	include <nfx/containers/FastHashMap.h>

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
		static void serialize( const nfx::containers::FastHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj, Document& doc )
		{
			// Create array to hold key-value pairs
			doc.set<Document::Array>( "" );
			auto arrayRef = doc.get<Document::Array>( "" );

			if ( !arrayRef.has_value() )
			{
				return;
			}

			// Use FastHashMap's iterator to traverse all key-value pairs
			for ( auto it = obj.begin(); it != obj.end(); ++it )
			{
				const auto& pair = *it;
				const TKey& key = pair.first;
				const TValue& value = pair.second;

				// Create object for this key-value pair
				Document pairDoc;
				pairDoc.set<Document::Object>( "" );

				// Serialize the key
				Document keyDoc;
				Serializer<TKey> keySerializer;
				keyDoc = keySerializer.serialize( key );

				// Serialize the value
				Document valueDoc;
				Serializer<TValue> valueSerializer;
				valueDoc = valueSerializer.serialize( value );

				// Add key to pair object
				if ( keyDoc.is<std::string>( "" ) )
				{
					auto str = keyDoc.get<std::string>( "" );
					pairDoc.set<std::string>( "/key", str.value() );
				}
				else if ( keyDoc.is<int>( "" ) )
				{
					auto val = keyDoc.get<int64_t>( "" );
					pairDoc.set<int64_t>( "/key", val.value() );
				}
				else if ( keyDoc.is<double>( "" ) )
				{
					auto val = keyDoc.get<double>( "" );
					pairDoc.set<double>( "/key", val.value() );
				}
				else if ( keyDoc.is<bool>( "" ) )
				{
					auto val = keyDoc.get<bool>( "" );
					pairDoc.set<bool>( "/key", val.value() );
				}
				else if ( keyDoc.is<Document::Array>( "" ) || keyDoc.is<Document::Object>( "" ) )
				{
					pairDoc.set<Document>( "/key", keyDoc );
				}

				// Add value to pair object
				if ( valueDoc.is<std::string>( "" ) )
				{
					auto str = valueDoc.get<std::string>( "" );
					pairDoc.set<std::string>( "/value", str.value() );
				}
				else if ( valueDoc.is<int>( "" ) )
				{
					auto val = valueDoc.get<int64_t>( "" );
					pairDoc.set<int64_t>( "/value", val.value() );
				}
				else if ( valueDoc.is<double>( "" ) )
				{
					auto val = valueDoc.get<double>( "" );
					pairDoc.set<double>( "/value", val.value() );
				}
				else if ( valueDoc.is<bool>( "" ) )
				{
					auto val = valueDoc.get<bool>( "" );
					pairDoc.set<bool>( "/value", val.value() );
				}
				else if ( valueDoc.isNull( "" ) )
				{
					pairDoc.setNull( "/value" );
				}
				else if ( valueDoc.is<Document::Array>( "" ) || valueDoc.is<Document::Object>( "" ) )
				{
					pairDoc.set<Document>( "/value", valueDoc );
				}

				// Add pair to array
				arrayRef->append<Document>( pairDoc );
			}
		}

		/**
		 * @brief Deserialize FastHashMap from JSON document
		 * @param obj The FastHashMap object to deserialize into
		 * @param doc The document to deserialize from
		 * @details Supports both array format and object format for compatibility
		 */
		static void deserialize( nfx::containers::FastHashMap<TKey, TValue, HashType, Seed, Hasher, KeyEqual>& obj, const Document& doc )
		{
			// Clear existing content
			obj.clear();

			// Check if it's an object format (standard JSON map representation)
			if ( doc.is<Document::Object>( "" ) )
			{
				// Use Object iterator for object format
				auto objectOpt = doc.get<Document::Object>( "" );
				if ( objectOpt.has_value() )
				{
					for ( const auto& [keyStr, valueDoc] : objectOpt.value() )
					{
						// Deserialize key (typically string, but support other types)
						TKey key{};
						if constexpr ( std::is_same_v<TKey, std::string> )
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
			else if ( doc.is<Document::Array>( "" ) )
			{
				// Get array and iterate using STL iterator
				auto arrayOpt = doc.get<Document::Array>( "" );
				if ( arrayOpt.has_value() )
				{
					for ( const auto& pairDoc : arrayOpt.value() )
					{
						// Extract key
						TKey key{};
						if ( pairDoc.contains( "/key" ) )
						{
							Document keyDoc = pairDoc.get<Document>( "/key" ).value_or( Document{} );
							Serializer<TKey> keySerializer;
							key = keySerializer.deserialize( keyDoc );
						}

						// Extract value
						TValue value{};
						if ( pairDoc.contains( "/value" ) )
						{
							Document valueDoc = pairDoc.get<Document>( "/value" ).value_or( Document{} );
							Serializer<TValue> valueSerializer;
							value = valueSerializer.deserialize( valueDoc );
						}

						obj.insertOrAssign( std::move( key ), std::move( value ) );
					}
				}
			}
			else
			{
				throw std::runtime_error( "Cannot deserialize JSON value into FastHashMap: must be object or array" );
			}
		}
	};
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/containers/FastHashMap.h>)

//=====================================================================
// FastHashSet support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/containers/FastHashSet.h>)

#	include <nfx/containers/FastHashSet.h>

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
		static void serialize( const nfx::containers::FastHashSet<TKey, HashType, Seed, Hasher, KeyEqual>& obj, Document& doc )
		{
			// Create array document
			doc.set<Document::Array>( "" );
			auto arrayRef = doc.get<Document::Array>( "" );

			if ( arrayRef.has_value() )
			{
				// Use FastHashSet's iterator to traverse all elements
				for ( auto it = obj.begin(); it != obj.end(); ++it )
				{
					const TKey& key = *it;

					// Serialize the key using a temporary serializer
					Document keyDoc;
					Serializer<TKey> keySerializer;
					keyDoc = keySerializer.serialize( key );

					// Add to array based on type
					if ( keyDoc.is<std::string>( "" ) )
					{
						auto str = keyDoc.get<std::string>( "" );
						arrayRef->append<std::string>( str.value() );
					}
					else if ( keyDoc.is<int>( "" ) )
					{
						auto val = keyDoc.get<int64_t>( "" );
						arrayRef->append<int64_t>( val.value() );
					}
					else if ( keyDoc.is<double>( "" ) )
					{
						auto val = keyDoc.get<double>( "" );
						arrayRef->append<double>( val.value() );
					}
					else if ( keyDoc.is<bool>( "" ) )
					{
						auto val = keyDoc.get<bool>( "" );
						arrayRef->append<bool>( val.value() );
					}
					else if ( keyDoc.is<Document::Object>( "" ) || keyDoc.is<Document::Array>( "" ) )
					{
						// Handle nested objects and arrays
						arrayRef->append<Document>( keyDoc );
					}
				}
			}
		}

		/**
		 * @brief Deserialize FastHashSet from JSON document
		 * @param obj The FastHashSet object to deserialize into
		 * @param doc The document to deserialize from
		 */
		static void deserialize( nfx::containers::FastHashSet<TKey, HashType, Seed, Hasher, KeyEqual>& obj, const Document& doc )
		{
			if ( !doc.is<Document::Array>( "" ) )
			{
				throw std::runtime_error( "Cannot deserialize non-array JSON value into FastHashSet" );
			}

			// Clear existing content
			obj.clear();

			// Get array and iterate using STL iterator
			auto arrayOpt = doc.get<Document::Array>( "" );
			if ( arrayOpt.has_value() )
			{
				for ( const auto& elementDoc : arrayOpt.value() )
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
