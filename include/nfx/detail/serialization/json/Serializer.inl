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

        /** @brief Specialization for std::unordered_map */
        template <typename K, typename V>
        struct is_container<std::unordered_map<K, V>> : std::true_type
        {
        };

        /** @brief Specialization for std::set */
        template <typename T>
        struct is_container<std::set<T>> : std::true_type
        {
        };

        /** @brief Specialization for std::unordered_set */
        template <typename T>
        struct is_container<std::unordered_set<T>> : std::true_type
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
        auto optDoc = SerializableDocument::fromString( jsonStr );
        if( !optDoc )
        {
            throw std::runtime_error{ "Failed to parse JSON string" };
        }
        return serializer.fromDocument( *optDoc );
    }

    //----------------------------------------------
    // Instance serialization methods
    //----------------------------------------------

    template <typename T>
    inline SerializableDocument Serializer<T>::toDocument( const T& obj ) const
    {
        SerializableDocument doc;
        if constexpr( detail::has_toDocument_method<T>::value )
        {
            // Initialize document as empty object for custom serialization
            doc.set<nfx::json::Object>( "" );

            // Use custom serialize method
            obj.toDocument( *this, doc );
        }
        else
        {
            // Use unified templated serialization
            serializeValue( obj, doc );
        }
        return doc;
    }

    template <typename T>
    inline T Serializer<T>::fromDocument( const SerializableDocument& doc ) const
    {
        T obj{};
        if constexpr( detail::has_fromDocument_method<T>::value )
        {
            // Use custom fromDocument method if available
            obj.fromDocument( doc, *this );
        }
        else
        {
            // Use unified templated deserialization
            deserializeValue( doc, obj );
        }
        return obj;
    }

    //----------------------------------------------
    // Private methods
    //----------------------------------------------

    template <typename T>
    template <typename U>
    inline void Serializer<T>::serializeValue( const U& obj, SerializableDocument& doc ) const
    {
        // Handle built-in types with library-defined logic
        if constexpr( std::is_same_v<U, bool> )
        {
            // Handle bool separately (before is_integral check)
            doc.set<bool>( "", obj );
        }
        else if constexpr( std::is_integral_v<U> )
        {
            // Handle integral types (int, long, etc.)
            doc.set<int64_t>( "", static_cast<int64_t>( obj ) );
        }
        else if constexpr( std::is_floating_point_v<U> )
        {
            // Handle floating point types
            doc.set<double>( "", static_cast<double>( obj ) );
        }
        else if constexpr( std::is_same_v<U, std::string> )
        {
            // Handle std::string
            doc.set<std::string>( "", obj );
        }
        else if constexpr( detail::is_optional<U>::value )
        {
            if( obj.has_value() )
            {
                serializeValue( obj.value(), doc );
            }
            else
            {
                doc.setNull( "" );
            }
        }
        else if constexpr( detail::is_smart_pointer<U>::value )
        {
            if( obj )
            {
                serializeValue( *obj, doc );
            }
            else
            {
                doc.setNull( "" );
            }
        }

        else if constexpr( detail::is_container<U>::value )
        {
            // Handle std::pair - serialize as array [first, second]
            if constexpr( detail::is_pair<U>::value )
            {
                doc.set<nfx::json::Array>( "" );

                // Serialize first at index 0
                SerializableDocument firstDoc;
                serializeValue( obj.first, firstDoc );
                if( firstDoc.is<std::string>( "" ) )
                {
                    auto str = firstDoc.get<std::string>( "" );
                    doc.set<std::string>( "/0", *str );
                }
                else if( firstDoc.is<int>( "" ) )
                {
                    auto val = firstDoc.get<int64_t>( "" );
                    doc.set<int64_t>( "/0", *val );
                }
                else if( firstDoc.is<double>( "" ) )
                {
                    auto val = firstDoc.get<double>( "" );
                    doc.set<double>( "/0", *val );
                }
                else if( firstDoc.is<bool>( "" ) )
                {
                    auto val = firstDoc.get<bool>( "" );
                    doc.set<bool>( "/0", *val );
                }
                else if( firstDoc.isNull( "" ) )
                {
                    doc.setNull( "/0" );
                }
                else if( firstDoc.is<Array>( "" ) || firstDoc.is<Object>( "" ) )
                {
                    doc.set<nfx::json::Document>( "/0", std::move( static_cast<nfx::json::Document&>( firstDoc ) ) );
                }

                // Serialize second at index 1
                SerializableDocument secondDoc;
                serializeValue( obj.second, secondDoc );
                if( secondDoc.is<std::string>( "" ) )
                {
                    auto str = secondDoc.get<std::string>( "" );
                    doc.set<std::string>( "/1", *str );
                }
                else if( secondDoc.is<int>( "" ) )
                {
                    auto val = secondDoc.get<int64_t>( "" );
                    doc.set<int64_t>( "/1", *val );
                }
                else if( secondDoc.is<double>( "" ) )
                {
                    auto val = secondDoc.get<double>( "" );
                    doc.set<double>( "/1", *val );
                }
                else if( secondDoc.is<bool>( "" ) )
                {
                    auto val = secondDoc.get<bool>( "" );
                    doc.set<bool>( "/1", *val );
                }
                else if( secondDoc.isNull( "" ) )
                {
                    doc.setNull( "/1" );
                }
                else if( secondDoc.is<Array>( "" ) || secondDoc.is<Object>( "" ) )
                {
                    doc.set<nfx::json::Document>( "/1", std::move( static_cast<nfx::json::Document&>( secondDoc ) ) );
                }
            }
            else if constexpr( requires { typename U::mapped_type; } )
            {
                // Map-like containers (std::map, std::unordered_map) - serialize as JSON object
                doc.set<nfx::json::Object>( "" );

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

                    SerializableDocument valueDoc;
                    serializeValue( pair.second, valueDoc );

                    // Set field in object using JSON Pointer syntax
                    std::string fieldPath = "/" + key;
                    if( valueDoc.is<std::string>( "" ) )
                    {
                        auto str = valueDoc.get<std::string>( "" );
                        doc.set<std::string>( fieldPath, *str );
                    }
                    else if( valueDoc.is<int>( "" ) )
                    {
                        auto val = valueDoc.get<int64_t>( "" );
                        doc.set<int64_t>( fieldPath, *val );
                    }
                    else if( valueDoc.is<double>( "" ) )
                    {
                        auto val = valueDoc.get<double>( "" );
                        doc.set<double>( fieldPath, *val );
                    }
                    else if( valueDoc.is<bool>( "" ) )
                    {
                        auto val = valueDoc.get<bool>( "" );
                        doc.set<bool>( fieldPath, *val );
                    }
                    else if( valueDoc.isNull( "" ) )
                    {
                        doc.setNull( fieldPath );
                    }
                    else if( valueDoc.is<Array>( "" ) || valueDoc.is<Object>( "" ) )
                    {
                        // Handle nested arrays and objects (e.g., std::vector<int>, std::map<string, int>)
                        doc.set<nfx::json::Document>(
                            fieldPath, std::move( static_cast<nfx::json::Document&>( valueDoc ) ) );
                    }
                }
            }
            else
            {
                doc.set<nfx::json::Array>( "" );

                size_t index = 0;
                for( const auto& item : obj )
                {
                    SerializableDocument itemDoc;
                    serializeValue( item, itemDoc );

                    // Buffer sized for "/%zu" format: 1 (slash) + 20 (max digits for size_t) + 1 (null) = 22 bytes
                    // minimum Using 32 bytes for comfortable margin and power-of-2 alignment
                    char arrayPath[32];
                    std::snprintf( arrayPath, sizeof( arrayPath ), "/%zu", index );
                    if( itemDoc.is<std::string>( "" ) )
                    {
                        auto str = itemDoc.get<std::string>( "" );
                        doc.set<std::string>( arrayPath, *str );
                    }
                    else if( itemDoc.is<int>( "" ) )
                    {
                        auto val = itemDoc.get<int64_t>( "" );
                        doc.set<int64_t>( arrayPath, *val );
                    }
                    else if( itemDoc.is<double>( "" ) )
                    {
                        auto val = itemDoc.get<double>( "" );
                        doc.set<double>( arrayPath, *val );
                    }
                    else if( itemDoc.is<bool>( "" ) )
                    {
                        auto val = itemDoc.get<bool>( "" );
                        doc.set<bool>( arrayPath, *val );
                    }
                    else if( itemDoc.isNull( "" ) )
                    {
                        doc.setNull( arrayPath );
                    }
                    else if( itemDoc.is<Array>( "" ) || itemDoc.is<Object>( "" ) )
                    {
                        // Handle nested arrays and objects in array elements
                        doc.set<nfx::json::Document>(
                            arrayPath, std::move( static_cast<nfx::json::Document&>( itemDoc ) ) );
                    }
                    ++index;
                }
            }
        }
        else
        {
            // Fall back to DocumentTraits (handles custom types: nfx extensions and user types)
            DocumentTraits<U>::toDocument( obj, doc );
        }
    }

    template <typename T>
    template <typename U>
    inline void Serializer<T>::serializeValue( const U& obj, Builder& builder ) const
    {
        if constexpr( detail::has_builder_traits_v<U> )
        {
            BuilderTraits<U>::serialize( obj, builder );
            return;
        }

        if constexpr( detail::has_toDocument_method<U>::value )
        {
            // Use Document-based serialization for custom methods, then convert to Builder
            // NOTE: Custom serialize() methods are Document-based by design (API contract)
            // For performance, user types should implement BuilderTraits instead
            // Create a properly-typed serializer for this object
            typename Serializer<U>::Options objOptions;
            objOptions.includeNullFields = m_options.includeNullFields;
            objOptions.prettyPrint = m_options.prettyPrint;
            objOptions.validateOnDeserialize = m_options.validateOnDeserialize;
            Serializer<U> objSerializer( objOptions );

            SerializableDocument tempDoc;
            tempDoc.set<nfx::json::Object>( "" );
            obj.toDocument( objSerializer, tempDoc );

            std::string tempJson = tempDoc.toString( m_options.prettyPrint ? 2 : 0 );
            builder.writeRawJson( tempJson );
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
            if constexpr( detail::has_builder_traits_v<U> )
            {
                BuilderTraits<U>::serialize( obj, builder );
            }
            else
            {
                // This path is for user-defined types that only implement DocumentTraits
                SerializableDocument tempDoc;
                DocumentTraits<U>::toDocument( obj, tempDoc );
                std::string tempJson = tempDoc.toString( m_options.prettyPrint ? 2 : 0 );
                builder.writeRawJson( tempJson );
            }
        }
    }

    template <typename T>
    template <typename U>
    inline void Serializer<T>::deserializeValue( const SerializableDocument& doc, U& obj ) const
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
            // Handle STL containers with flexible JSON input types
            else if constexpr( requires { obj.clear(); } )
            {
                obj.clear();
            }

            if constexpr( requires { typename U::mapped_type; } )
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
                    // Handle null → empty map (obj.clear() already called above)
                }
                else
                {
                    throw std::runtime_error{ "Cannot deserialize non-object JSON value into map container" };
                }
            }
            else
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
            // Fall back to DocumentTraits (custom types: nfx extensions and user types)
            DocumentTraits<U>::fromDocument( doc, obj );
        }
    }
} // namespace nfx::serialization::json
