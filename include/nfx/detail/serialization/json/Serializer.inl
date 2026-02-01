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
 * @file Serializer.inl
 * @brief Templated JSON serializer implementation file
 * @details Contains the template method implementations for the Serializer class.
 *          This file provides the actual implementation of serialization and
 *          deserialization methods for all supported types.
 */

#include <array>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nfx::serialization::json
{
    //=====================================================================
    // Internal implementation type traits
    //=====================================================================

    namespace detail
    {
        /**
         * @brief Type trait to detect if a type has a toDocument method
         * @tparam T The type to check
         * @details Uses SFINAE to detect if type T has a toDocument method that accepts
         *          a Serializer<T>& parameter and Document& parameter. Used for compile-time
         *          dispatch to custom serialization methods.
         */
        template <typename T>
        struct has_toDocument_method
        {
        private:
            template <typename U>
            static auto test( int )
                -> decltype( std::declval<const U&>().toDocument( std::declval<const Serializer<U>&>(), std::declval<Document&>() ), std::true_type{} );
            template <typename>
            static std::false_type test( ... );

        public:
            /** @brief True if type T has a toDocument method, false otherwise */
            static constexpr bool value = decltype( test<T>( 0 ) )::value;
        };

        /**
         * @brief Type trait to detect if a type has a fromDocument method
         * @tparam T The type to check
         * @details Uses SFINAE to detect if type T has a fromDocument method that accepts
         *          a const Document& and const Serializer<T>& parameter. Used for compile-time
         *          dispatch to custom deserialization methods.
         */
        template <typename T>
        struct has_fromDocument_method
        {
        private:
            template <typename U>
            static auto test( int )
                -> decltype( std::declval<U>().fromDocument( std::declval<const Document&>(), std::declval<const Serializer<U>&>() ), std::true_type{} );
            template <typename>
            static std::false_type test( ... );

        public:
            /** @brief True if type T has a fromDocument method, false otherwise */
            static constexpr bool value = decltype( test<T>( 0 ) )::value;
        };

        /**
         * @brief Type trait to detect if a type is a container
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for supported
         *          container types including vector, array, map, and unordered_map.
         */
        template <typename T>
        struct is_container : std::false_type
        {
        };

        /** @brief Specialization for std::vector */
        template <typename T>
        struct is_container<std::vector<T>> : std::true_type
        {
        };

        /** @brief Specialization for std::array */
        template <typename T, std::size_t N>
        struct is_container<std::array<T, N>> : std::true_type
        {
        };

        /** @brief Specialization for std::map */
        template <typename K, typename V>
        struct is_container<std::map<K, V>> : std::true_type
        {
        };

        /** @brief Specialization for std::multimap */
        template <typename K, typename V, typename... Args>
        struct is_container<std::multimap<K, V, Args...>> : std::true_type
        {
        };

        /** @brief Specialization for std::unordered_map */
        template <typename K, typename V>
        struct is_container<std::unordered_map<K, V>> : std::true_type
        {
        };

        /** @brief Specialization for std::unordered_multimap */
        template <typename K, typename V, typename... Args>
        struct is_container<std::unordered_multimap<K, V, Args...>> : std::true_type
        {
        };

        /** @brief Specialization for std::set */
        template <typename T>
        struct is_container<std::set<T>> : std::true_type
        {
        };

        /** @brief Specialization for std::multiset */
        template <typename T, typename... Args>
        struct is_container<std::multiset<T, Args...>> : std::true_type
        {
        };

        /** @brief Specialization for std::unordered_set */
        template <typename T>
        struct is_container<std::unordered_set<T>> : std::true_type
        {
        };

        /** @brief Specialization for std::unordered_multiset */
        template <typename T, typename... Args>
        struct is_container<std::unordered_multiset<T, Args...>> : std::true_type
        {
        };

        /** @brief Specialization for std::list */
        template <typename T>
        struct is_container<std::list<T>> : std::true_type
        {
        };

        /** @brief Specialization for std::deque */
        template <typename T>
        struct is_container<std::deque<T>> : std::true_type
        {
        };

        /** @brief Specialization for std::pair */
        template <typename TFirst, typename TSecond>
        struct is_container<std::pair<TFirst, TSecond>> : std::true_type
        {
        };

        /**
         * @brief Type trait to detect if a type is std::pair
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for std::pair<T1, T2>.
         */
        template <typename T>
        struct is_pair : std::false_type
        {
        };

        /** @brief Specialization for std::pair */
        template <typename TFirst, typename TSecond>
        struct is_pair<std::pair<TFirst, TSecond>> : std::true_type
        {
        };

        /**
         * @brief Type trait to detect if a type is std::tuple
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for std::tuple<Ts...>.
         */
        template <typename T>
        struct is_tuple : std::false_type
        {
        };

        /** @brief Specialization for std::tuple */
        template <typename... Ts>
        struct is_tuple<std::tuple<Ts...>> : std::true_type
        {
        };

        /**
         * @brief Type trait to detect if a type is std::multimap
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for std::multimap<K, V>.
         */
        template <typename T>
        struct is_multimap : std::false_type
        {
        };

        /** @brief Specialization for std::multimap */
        template <typename K, typename V, typename... Args>
        struct is_multimap<std::multimap<K, V, Args...>> : std::true_type
        {
        };

        /**
         * @brief Type trait to detect if a type is std::unordered_multimap
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for std::unordered_multimap<K, V>.
         */
        template <typename T>
        struct is_unordered_multimap : std::false_type
        {
        };

        /** @brief Specialization for std::unordered_multimap */
        template <typename K, typename V, typename... Args>
        struct is_unordered_multimap<std::unordered_multimap<K, V, Args...>> : std::true_type
        {
        };

        /**
         * @brief Type trait to detect if a type is std::multiset
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for std::multiset<T>.
         */
        template <typename T>
        struct is_multiset : std::false_type
        {
        };

        /** @brief Specialization for std::multiset */
        template <typename T, typename... Args>
        struct is_multiset<std::multiset<T, Args...>> : std::true_type
        {
        };

        /**
         * @brief Type trait to detect if a type is std::unordered_multiset
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for std::unordered_multiset<T>.
         */
        template <typename T>
        struct is_unordered_multiset : std::false_type
        {
        };

        /** @brief Specialization for std::unordered_multiset */
        template <typename T, typename... Args>
        struct is_unordered_multiset<std::unordered_multiset<T, Args...>> : std::true_type
        {
        };

        /**
         * @brief Type trait to detect if a type is std::optional
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for std::optional<T>.
         */
        template <typename T>
        struct is_optional : std::false_type
        {
        };

        /** @brief Specialization for std::optional */
        template <typename T>
        struct is_optional<std::optional<T>> : std::true_type
        {
        };

        /**
         * @brief Type trait to detect if a type is a smart pointer
         * @tparam T The type to check
         * @details Base template that evaluates to false. Specialized for std::unique_ptr
         *          and std::shared_ptr types.
         */
        template <typename T>
        struct is_smart_pointer : std::false_type
        {
        };

        /** @brief Specialization for std::unique_ptr */
        template <typename T>
        struct is_smart_pointer<std::unique_ptr<T>> : std::true_type
        {
        };

        /** @brief Specialization for std::shared_ptr */
        template <typename T>
        struct is_smart_pointer<std::shared_ptr<T>> : std::true_type
        {
        };
    } // namespace detail

    //=====================================================================
    // Serializer class
    //=====================================================================

    //----------------------------------------------
    // Serialization options and context
    //----------------------------------------------

    template <typename T>
    template <typename U>
    inline void Serializer<T>::Options::copyFrom( const typename Serializer<U>::Options& other )
    {
        includeNullFields = other.includeNullFields;
        prettyPrint = other.prettyPrint;
        validateOnDeserialize = other.validateOnDeserialize;
    }

    template <typename T>
    template <typename U>
    inline typename Serializer<T>::Options Serializer<T>::Options::createFrom(
        const typename Serializer<U>::Options& other )
    {
        Options result;
        result.includeNullFields = other.includeNullFields;
        result.prettyPrint = other.prettyPrint;
        result.validateOnDeserialize = other.validateOnDeserialize;
        return result;
    }

    //----------------------------------------------
    // Construction
    //----------------------------------------------

    template <typename T>
    inline Serializer<T>::Serializer( const Options& options ) noexcept
        : m_options{ options }
    {
    }

    //----------------------------------------------
    // Options management
    //----------------------------------------------

    template <typename T>
    inline const Serializer<T>::Options& Serializer<T>::options() const noexcept
    {
        return m_options;
    }

    template <typename T>
    inline void Serializer<T>::setOptions( const typename Serializer<T>::Options& options ) noexcept
    {
        m_options = options;
    }

    //----------------------------------------------
    // Static convenience serialization methods
    //----------------------------------------------

    template <typename T>
    inline std::string Serializer<T>::toString( const T& obj, const Serializer<T>::Options& options )
    {
        Serializer<T> serializer( options );
        Builder builder( { .indent = options.prettyPrint ? 2 : 0 } );
        serializer.serializeValue( obj, builder );

        return builder.toString();
    }

    template <typename T>
    inline T Serializer<T>::fromString( std::string_view jsonStr, const Serializer<T>::Options& options )
    {
        Serializer<T> serializer( options );
        auto optDoc = Document::fromString( jsonStr );
        if( !optDoc )
        {
            throw std::runtime_error{ "Failed to parse JSON string" };
        }
        T obj{};
        serializer.deserializeValue( *optDoc, obj );
        return obj;
    }

    //----------------------------------------------
    // Private methods
    //----------------------------------------------

    template <typename T>
    template <typename U>
    inline void Serializer<T>::serializeValue( const U& obj, Builder& builder ) const
    {
        // Priority order (performance-optimized):
        // 1. SerializationTraits::serialize() - optimal streaming serialization
        // 2. Built-in types - efficient direct serialization
        // 3. Custom toDocument() - user override (performance hit: Document → JSON → Builder)

        // Fast path
        if constexpr( detail::has_streaming_serialization_v<U> )
        {
            SerializationTraits<U>::serialize( obj, builder );
            return;
        }

        // Handle built-in types with library-defined logic
        if constexpr( std::is_same_v<U, bool> )
        {
            // Handle bool separately (before is_integral check)
            builder.write( obj );
        }
        else if constexpr( std::is_integral_v<U> )
        {
            // Handle integral types (int, long, etc.)
            builder.write( static_cast<int64_t>( obj ) );
        }
        else if constexpr( std::is_floating_point_v<U> )
        {
            // Handle floating point types
            builder.write( static_cast<double>( obj ) );
        }
        else if constexpr( std::is_same_v<U, std::string> )
        {
            // Handle std::string
            builder.write( obj );
        }
        else if constexpr( detail::is_optional<U>::value )
        {
            if( obj.has_value() )
            {
                serializeValue( obj.value(), builder );
            }
            else
            {
                builder.write( nullptr );
            }
        }
        else if constexpr( detail::is_smart_pointer<U>::value )
        {
            if( obj )
            {
                serializeValue( *obj, builder );
            }
            else
            {
                builder.write( nullptr );
            }
        }
        else if constexpr( detail::is_tuple<U>::value )
        {
            // Handle std::tuple - serialize as array [elem0, elem1, ...]
            builder.writeStartArray();
            std::apply(
                [&]( const auto&... elems ) {
                    ( serializeValue( elems, builder ), ... ); // fold expression
                },
                obj );
            builder.writeEndArray();
        }
        else if constexpr( detail::is_container<U>::value )
        {
            // Handle std::pair - serialize as array [first, second]
            if constexpr( detail::is_pair<U>::value )
            {
                builder.writeStartArray();
                serializeValue( obj.first, builder );
                serializeValue( obj.second, builder );
                builder.writeEndArray();
            }
            // Handle std::multimap/std::unordered_multimap - serialize as array of {"key": K, "value": V}
            else if constexpr( detail::is_multimap<U>::value || detail::is_unordered_multimap<U>::value )
            {
                builder.writeStartArray();

                for( const auto& pair : obj )
                {
                    builder.writeStartObject();
                    builder.writeKey( "key" );
                    serializeValue( pair.first, builder );
                    builder.writeKey( "value" );
                    serializeValue( pair.second, builder );
                    builder.writeEndObject();
                }

                builder.writeEndArray();
            }
            // Handle std::multiset/std::unordered_multiset - serialize as array (allows duplicates)
            else if constexpr( detail::is_multiset<U>::value || detail::is_unordered_multiset<U>::value )
            {
                builder.writeStartArray();

                for( const auto& item : obj )
                {
                    serializeValue( item, builder );
                }

                builder.writeEndArray();
            }
            else if constexpr( requires { typename U::mapped_type; } )
            {
                // Map-like containers (std::map, std::unordered_map) - serialize as JSON object
                builder.writeStartObject();

                for( const auto& pair : obj )
                {
                    std::string key;
                    if constexpr( std::is_convertible_v<decltype( pair.first ), std::string> )
                    {
                        key = std::string( pair.first );
                    }
                    else
                    {
                        key = std::to_string( pair.first );
                    }

                    builder.writeKey( key );
                    serializeValue( pair.second, builder );
                }

                builder.writeEndObject();
            }
            else
            {
                // Sequence containers - serialize as JSON array
                builder.writeStartArray();

                for( const auto& item : obj )
                {
                    serializeValue( item, builder );
                }

                builder.writeEndArray();
            }
        }
        else
        {
            // User-defined types - check for custom toDocument() method first
            if constexpr( detail::has_toDocument_method<U>::value )
            {
                // Custom toDocument() method - performance hit (Document → JSON → Builder)
                // NOTE: For better performance, implement SerializationTraits::serialize() instead
                // Create a properly-typed serializer for this object
                typename Serializer<U>::Options objOptions;
                objOptions.includeNullFields = m_options.includeNullFields;
                objOptions.prettyPrint = m_options.prettyPrint;
                objOptions.validateOnDeserialize = m_options.validateOnDeserialize;
                Serializer<U> objSerializer( objOptions );

                Document tempDoc;
                tempDoc.set<nfx::json::Object>( "" );
                obj.toDocument( objSerializer, tempDoc );

                std::string tempJson = tempDoc.toString( m_options.prettyPrint ? 2 : 0 );
                builder.writeRawJson( tempJson );
            }
            // NOTE: No fallback to SerializationTraits::toDocument() - all types must either:
            //       - Have SerializationTraits::serialize() (checked above)
            //       - Have custom toDocument() method (checked above)
            //       - Be built-in types (checked above)
            // If none match, compilation will fail (by design)
        }
    }

    template <typename T>
    template <typename U>
    inline void Serializer<T>::deserializeValue( const Document& doc, U& obj ) const
    {
        // Handle built-in types with library-defined logic
        if constexpr( std::is_same_v<U, bool> )
        {
            // Handle bool
            auto val = doc.get<bool>( "" );
            if( !val )
                throw std::runtime_error{ "Cannot deserialize value as bool" };
            obj = *val;
        }
        else if constexpr( std::is_integral_v<U> )
        {
            // Handle integral types
            auto val = doc.get<int64_t>( "" );
            if( !val )
            {
                throw std::runtime_error{ "Cannot deserialize value as integral type" };
            }
            obj = static_cast<U>( *val );
        }
        else if constexpr( std::is_floating_point_v<U> )
        {
            // Handle floating point types
            auto val = doc.get<double>( "" );
            if( !val )
                throw std::runtime_error{ "Cannot deserialize value as floating point type" };
            obj = static_cast<U>( *val );
        }
        else if constexpr( std::is_same_v<U, std::string> )
        {
            // Handle std::string
            auto val = doc.get<std::string>( "" );
            if( !val )
                throw std::runtime_error{ "Cannot deserialize value as string" };
            obj = *val;
        }
        else if constexpr( detail::is_optional<U>::value )
        {
            // Handle std::optional types
            if( doc.isNull( "" ) )
            {
                obj = std::nullopt;
            }
            else
            {
                typename U::value_type value{};
                deserializeValue( doc, value );
                obj = std::move( value );
            }
        }
        else if constexpr( detail::is_smart_pointer<U>::value )
        {
            if( doc.isNull( "" ) )
            {
                obj = nullptr;
            }
            else
            {
                auto value = std::make_unique<typename U::element_type>();
                deserializeValue( doc, *value );
                if constexpr( std::is_same_v<U, std::unique_ptr<typename U::element_type>> )
                {
                    obj = std::move( value );
                }
                else
                {
                    obj = std::shared_ptr<typename U::element_type>( value.release() );
                }
            }
        }

        else if constexpr( detail::is_tuple<U>::value )
        {
            // Handle std::tuple - expect array [elem0, elem1, ...]
            if( doc.is<Array>( "" ) )
            {
                auto arrOpt = doc.get<Array>( "" );
                if( arrOpt.has_value() )
                {
                    const auto& arr = arrOpt.value();
                    constexpr std::size_t tupleSize = std::tuple_size_v<U>;

                    if( arr.size() != tupleSize )
                    {
                        throw std::runtime_error{ "Cannot deserialize array with " + std::to_string( arr.size() ) +
                                                  " elements into std::tuple with " + std::to_string( tupleSize ) +
                                                  " elements" };
                    }

                    // Use index_sequence to deserialize each element
                    [&]<std::size_t... Indices>( std::index_sequence<Indices...> ) {
                        ( deserializeValue( arr[Indices], std::get<Indices>( obj ) ), ... );
                    }( std::make_index_sequence<tupleSize>{} );
                }
            }
            else if( !doc.isNull( "" ) )
            {
                throw std::runtime_error{ "Cannot deserialize non-array JSON value into std::tuple" };
            }
        }

        else if constexpr( detail::is_container<U>::value )
        {
            // Handle std::pair - expect array [first, second]
            if constexpr( detail::is_pair<U>::value )
            {
                if( doc.is<Array>( "" ) )
                {
                    auto arrOpt = doc.get<Array>( "" );
                    if( arrOpt.has_value() && arrOpt.value().size() >= 2 )
                    {
                        // Deserialize from array elements
                        deserializeValue( arrOpt.value()[0], obj.first );
                        deserializeValue( arrOpt.value()[1], obj.second );
                    }
                    else if( arrOpt.has_value() )
                    {
                        throw std::runtime_error{ "Cannot deserialize array with less than 2 elements into std::pair" };
                    }
                }
                else if( !doc.isNull( "" ) )
                {
                    throw std::runtime_error{ "Cannot deserialize non-array JSON value into std::pair" };
                }
            }
            // Handle std::multimap/std::unordered_multimap - expect array of {"key": K, "value": V}
            else if constexpr( detail::is_multimap<U>::value || detail::is_unordered_multimap<U>::value )
            {
                obj.clear();

                if( doc.is<Array>( "" ) )
                {
                    auto arrOpt = doc.get<Array>( "" );
                    if( arrOpt.has_value() )
                    {
                        for( const auto& elementDoc : arrOpt.value() )
                        {
                            if( elementDoc.is<Object>( "" ) )
                            {
                                typename U::key_type key{};
                                typename U::mapped_type value{};

                                auto keyDoc = elementDoc.get<Document>( "key" );
                                auto valueDoc = elementDoc.get<Document>( "value" );

                                if( keyDoc && valueDoc )
                                {
                                    deserializeValue( *keyDoc, key );
                                    deserializeValue( *valueDoc, value );
                                    obj.insert( { std::move( key ), std::move( value ) } );
                                }
                            }
                        }
                    }
                }
                else if( !doc.isNull( "" ) )
                {
                    throw std::runtime_error{ "Cannot deserialize non-array JSON value into multimap container" };
                }
            }
            // Handle std::multiset/std::unordered_multiset - expect array (allows duplicates)
            else if constexpr( detail::is_multiset<U>::value || detail::is_unordered_multiset<U>::value )
            {
                obj.clear();

                if( doc.is<Array>( "" ) )
                {
                    auto arrOpt = doc.get<Array>( "" );
                    if( arrOpt.has_value() )
                    {
                        for( const auto& elementDoc : arrOpt.value() )
                        {
                            typename U::value_type item{};
                            deserializeValue( elementDoc, item );
                            obj.insert( std::move( item ) );
                        }
                    }
                }
                else if( !doc.isNull( "" ) )
                {
                    throw std::runtime_error{ "Cannot deserialize non-array JSON value into multiset container" };
                }
            }
            // Handle std::array (fixed-size, no .clear() method) - special case
            else if constexpr(
                requires {
                    typename U::value_type;
                    std::tuple_size<U>::value;
                } && !requires { obj.clear(); } && !detail::is_tuple<U>::value )
            {
                // std::array deserialization
                if( doc.is<Array>( "" ) )
                {
                    auto arrOpt = doc.get<Array>( "" );
                    if( arrOpt.has_value() )
                    {
                        const auto& arr = arrOpt.value();
                        constexpr std::size_t arraySize = std::tuple_size_v<U>;

                        if( arr.size() != arraySize )
                        {
                            throw std::runtime_error{ "Cannot deserialize array with " + std::to_string( arr.size() ) +
                                                      " elements into std::array with " + std::to_string( arraySize ) +
                                                      " elements" };
                        }

                        for( std::size_t i = 0; i < arraySize; ++i )
                        {
                            deserializeValue( arr[i], obj[i] );
                        }
                    }
                }
                else if( !doc.isNull( "" ) )
                {
                    throw std::runtime_error{ "Cannot deserialize non-array JSON value into std::array" };
                }
            }
            // Handle STL containers with flexible JSON input types
            // (Skip for multimap/multiset which were already handled above)
            else if constexpr( requires { obj.clear(); } )
            {
                obj.clear();
            }

            // Regular map-like containers (NOT multimap) - only accept JSON objects
            if constexpr( !detail::is_multimap<U>::value && !detail::is_unordered_multimap<U>::value && requires {
                              typename U::mapped_type;
                          } )
            {
                // Map-like containers: only accept JSON objects
                if( doc.is<Object>( "" ) )
                {
                    // Object → map: iterate over object fields using Object::iterator
                    auto objOpt = doc.get<Object>( "" );
                    if( objOpt.has_value() )
                    {
                        for( const auto& [key, valueDoc] : objOpt.value() )
                        {
                            typename U::mapped_type value{};
                            deserializeValue( valueDoc, value );
                            obj[key] = std::move( value );
                        }
                    }
                }
                else if( doc.isNull( "" ) )
                {
                    // Handle null → empty map
                }
                else
                {
                    throw std::runtime_error{ "Cannot deserialize non-object JSON value into map container" };
                }
            }

            // Non-map, non-multimap/multiset, non-pair containers: accept arrays and single values
            if constexpr(
                !detail::is_pair<U>::value && !detail::is_multimap<U>::value &&
                !detail::is_unordered_multimap<U>::value && !detail::is_multiset<U>::value &&
                !detail::is_unordered_multiset<U>::value && !requires { typename U::mapped_type; } )
            {
                // Non-map containers: accept arrays and single values
                if( doc.is<Array>( "" ) )
                {
                    // Standard case: JSON array → container using Array::iterator
                    auto arrOpt = doc.get<Array>( "" );
                    if( arrOpt.has_value() )
                    {
                        Array& arr = arrOpt.value();

                        if constexpr( std::is_same_v<U, std::vector<typename U::value_type>> )
                        {
                            obj.reserve( arr.size() );
                        }

                        size_t arrayIndex = 0;

                        for( const auto& elementDoc : arr )
                        {
                            typename U::value_type item{};

                            deserializeValue( elementDoc, item );

                            if constexpr( requires { obj.push_back( std::move( item ) ); } )
                            {
                                obj.push_back( std::move( item ) );
                            }
                            else if constexpr( requires { obj.insert( std::move( item ) ); } )
                            {
                                obj.insert( std::move( item ) );
                            }
                            else if constexpr( requires { obj.insert( obj.end(), std::move( item ) ); } )
                            {
                                obj.insert( obj.end(), std::move( item ) );
                            }
                            else if constexpr( requires { obj[arrayIndex] = std::move( item ); } )
                            {
                                if( arrayIndex < obj.size() )
                                {
                                    obj[arrayIndex] = std::move( item );
                                }
                            }
                            else
                            {
                                static_assert(
                                    std::is_void_v<U>,
                                    "Container doesn't support push_back, insert, or indexed assignment" );
                            }

                            ++arrayIndex;
                        }
                    }
                }
                else if( doc.isNull( "" ) )
                {
                    // Handle null → empty container (obj.clear() already called above)
                }
                else
                {
                    // Single value → container
                    typename U::value_type item{};
                    deserializeValue( doc, item );

                    if constexpr( std::is_same_v<U, std::vector<typename U::value_type>> )
                    {
                        obj.push_back( std::move( item ) );
                    }
                    else if constexpr( requires { obj.insert( std::move( item ) ); } )
                    {
                        obj.insert( std::move( item ) );
                    }
                    else if constexpr( requires { obj.insert( obj.end(), std::move( item ) ); } )
                    {
                        obj.insert( obj.end(), std::move( item ) );
                    }
                    else
                    {
                        // Fixed-size containers (like std::array) don't support insertion - skip
                    }
                }
            }
        }
        else
        {
            // Fall back to SerializationTraits::fromDocument() (custom types: nfx extensions and user types)
            SerializationTraits<U>::fromDocument( doc, obj );
        }
    }
} // namespace nfx::serialization::json
