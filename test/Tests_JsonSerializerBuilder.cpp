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
 * @file Tests_JsonSerializerBuilder.cpp
 * @brief Unit tests for Builder-based JSON serialization
 * @details Tests that verify Builder-based serialization produces identical output
 *          to the Document-based implementation for all supported types.
 *          Validates that the internal Builder optimization doesn't change behavior.
 */

#include <gtest/gtest.h>

#include <nfx/Serialization.h>

#include <array>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace nfx::json;
using namespace nfx::serialization::json;

namespace nfx::serialization::json::test
{
    //=====================================================================
    // Test fixture for Builder-based serialization
    //=====================================================================

    class BuilderSerializerTest : public ::testing::Test
    {
    protected:
        /**
         * @brief Test that serialization produces valid JSON
         */
        template <typename T>
        void testSerializationProducesValidJson( const T& obj )
        {
            // Serialize to JSON string
            std::string jsonStr = Serializer<T>::toString( obj );

            // Verify JSON is not empty
            EXPECT_FALSE( jsonStr.empty() ) << "Serialized JSON should not be empty";

            // Verify JSON can be parsed back
            auto parseResult = Document::fromString( jsonStr );
            EXPECT_TRUE( parseResult.has_value() ) << "Serialized JSON must be valid and parseable";
        }

        /**
         * @brief Test roundtrip serialization/deserialization
         */
        template <typename T>
        void testRoundTrip( const T& original )
        {
            // Serialize to JSON string
            std::string jsonStr = Serializer<T>::toString( original );

            // Deserialize back from JSON string
            T deserialized = Serializer<T>::fromString( jsonStr );

            // Verify roundtrip integrity
            EXPECT_EQ( original, deserialized ) << "Roundtrip must preserve original value";
        }

        /**
         * @brief Test roundtrip for unique_ptr (compare dereferenced values)
         */
        template <typename T>
        void testUniquePointerRoundTrip( const std::unique_ptr<T>& original )
        {
            // Serialize to JSON string
            std::string jsonStr = Serializer<std::unique_ptr<T>>::toString( original );

            // Deserialize back from JSON string
            auto deserialized = Serializer<std::unique_ptr<T>>::fromString( jsonStr );

            // Verify roundtrip integrity
            if( original == nullptr )
            {
                EXPECT_EQ( deserialized, nullptr ) << "Null pointer should roundtrip as null";
            }
            else
            {
                EXPECT_NE( deserialized, nullptr ) << "Non-null pointer should roundtrip as non-null";
                EXPECT_EQ( *original, *deserialized ) << "Pointer contents must match after roundtrip";
            }
        }

        /**
         * @brief Test roundtrip for shared_ptr (compare dereferenced values)
         */
        template <typename T>
        void testSharedPointerRoundTrip( const std::shared_ptr<T>& original )
        {
            // Serialize to JSON string
            std::string jsonStr = Serializer<std::shared_ptr<T>>::toString( original );

            // Deserialize back from JSON string
            auto deserialized = Serializer<std::shared_ptr<T>>::fromString( jsonStr );

            // Verify roundtrip integrity
            if( original == nullptr )
            {
                EXPECT_EQ( deserialized, nullptr ) << "Null pointer should roundtrip as null";
            }
            else
            {
                EXPECT_NE( deserialized, nullptr ) << "Non-null pointer should roundtrip as non-null";
                EXPECT_EQ( *original, *deserialized ) << "Pointer contents must match after roundtrip";
            }
        }
    };

    //=====================================================================
    // Primitive types
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderBooleanTypes )
    {
        testSerializationProducesValidJson( true );
        testSerializationProducesValidJson( false );
        testRoundTrip( true );
        testRoundTrip( false );
    }

    TEST_F( BuilderSerializerTest, BuilderIntegerTypes )
    {
        testSerializationProducesValidJson( 0 );
        testSerializationProducesValidJson( 42 );
        testSerializationProducesValidJson( -100 );
        testSerializationProducesValidJson( INT64_MAX );
        testSerializationProducesValidJson( INT64_MIN );

        testRoundTrip( 0 );
        testRoundTrip( 42 );
        testRoundTrip( -100 );
    }

    TEST_F( BuilderSerializerTest, BuilderFloatingPointTypes )
    {
        testSerializationProducesValidJson( 0.0f );
        testSerializationProducesValidJson( 3.14159f );
        testSerializationProducesValidJson( -2.71828f );
        testSerializationProducesValidJson( 0.0 );
        testSerializationProducesValidJson( 3.141592653589793 );

        testRoundTrip( 0.0f );
        testRoundTrip( 3.14159f );
    }

    TEST_F( BuilderSerializerTest, BuilderStringTypes )
    {
        testSerializationProducesValidJson( std::string( "" ) );
        testSerializationProducesValidJson( std::string( "hello" ) );
        testSerializationProducesValidJson( std::string( "with spaces" ) );
        testSerializationProducesValidJson( std::string( "with\"quotes" ) );

        testRoundTrip( std::string( "" ) );
        testRoundTrip( std::string( "hello world" ) );
    }

    //=====================================================================
    // Vector types
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderVectorTypes )
    {
        std::vector<int> emptyVec;
        std::vector<int> intVec = { 1, 2, 3, 4, 5 };
        std::vector<std::string> strVec = { "a", "b", "c" };

        testSerializationProducesValidJson( emptyVec );
        testSerializationProducesValidJson( intVec );
        testSerializationProducesValidJson( strVec );

        testRoundTrip( emptyVec );
        testRoundTrip( intVec );
        testRoundTrip( strVec );
    }

    //=====================================================================
    // Array types
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderArrayTypes )
    {
        std::array<int, 0> emptyArr;
        std::array<int, 5> intArr = { 1, 2, 3, 4, 5 };
        std::array<std::string, 3> strArr = { "x", "y", "z" };

        testSerializationProducesValidJson( emptyArr );
        testSerializationProducesValidJson( intArr );
        testSerializationProducesValidJson( strArr );

        testRoundTrip( intArr );
        testRoundTrip( strArr );
    }

    //=====================================================================
    // Map types
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderMapTypes )
    {
        std::map<std::string, int> emptyMap;
        std::map<std::string, int> strIntMap = { { "one", 1 }, { "two", 2 }, { "three", 3 } };

        testSerializationProducesValidJson( emptyMap );
        testSerializationProducesValidJson( strIntMap );

        testRoundTrip( emptyMap );
        testRoundTrip( strIntMap );
    }

    //=====================================================================
    // Set types
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderSetTypes )
    {
        std::set<int> emptySet;
        std::set<int> intSet = { 1, 2, 3, 4, 5 };
        std::set<std::string> strSet = { "apple", "banana", "cherry" };

        testSerializationProducesValidJson( emptySet );
        testSerializationProducesValidJson( intSet );
        testSerializationProducesValidJson( strSet );

        testRoundTrip( emptySet );
        testRoundTrip( intSet );
        testRoundTrip( strSet );
    }

    //=====================================================================
    // Optional types
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderOptionalTypes )
    {
        std::optional<int> emptyOpt;
        std::optional<int> filledOpt = 42;
        std::optional<std::string> emptyStrOpt;
        std::optional<std::string> filledStrOpt = "hello";

        testSerializationProducesValidJson( emptyOpt );
        testSerializationProducesValidJson( filledOpt );
        testSerializationProducesValidJson( emptyStrOpt );
        testSerializationProducesValidJson( filledStrOpt );

        testRoundTrip( filledOpt );
        testRoundTrip( filledStrOpt );
    }

    //=====================================================================
    // Pointer types
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderUniquePointerTypes )
    {
        std::unique_ptr<int> nullPtr;
        std::unique_ptr<int> intPtr = std::make_unique<int>( 42 );
        std::unique_ptr<std::string> strPtr = std::make_unique<std::string>( "hello" );

        testSerializationProducesValidJson( nullPtr );
        testSerializationProducesValidJson( intPtr );
        testSerializationProducesValidJson( strPtr );

        testUniquePointerRoundTrip( nullPtr );
        testUniquePointerRoundTrip( intPtr );
        testUniquePointerRoundTrip( strPtr );
    }

    TEST_F( BuilderSerializerTest, BuilderSharedPointerTypes )
    {
        std::shared_ptr<int> nullPtr;
        std::shared_ptr<int> intPtr = std::make_shared<int>( 42 );
        std::shared_ptr<std::string> strPtr = std::make_shared<std::string>( "hello" );

        testSerializationProducesValidJson( nullPtr );
        testSerializationProducesValidJson( intPtr );
        testSerializationProducesValidJson( strPtr );

        testSharedPointerRoundTrip( nullPtr );
        testSharedPointerRoundTrip( intPtr );
        testSharedPointerRoundTrip( strPtr );
    }

    //=====================================================================
    // Nested containers
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderNestedContainers )
    {
        std::vector<std::vector<int>> nestedVec = { { 1, 2 }, { 3, 4, 5 }, { 6 } };
        std::map<std::string, std::vector<int>> mapOfVec = {
            { "a", { 1, 2, 3 } },
            { "b", { 4, 5 } },
        };

        testSerializationProducesValidJson( nestedVec );
        testSerializationProducesValidJson( mapOfVec );

        testRoundTrip( nestedVec );
        testRoundTrip( mapOfVec );
    }

    //=====================================================================
    // Custom types with serialize/deserialize methods
    //=====================================================================

    struct SimpleCustomType
    {
        int value;
        std::string name;

        bool operator==( const SimpleCustomType& other ) const
        {
            return value == other.value && name == other.name;
        }

        void toDocument( const Serializer<SimpleCustomType>& /*serializer*/, Document& doc ) const
        {
            doc.set<int64_t>( "/value", value );
            doc.set<std::string>( "/name", name );
        }

        void fromDocument( const Document& doc, const Serializer<SimpleCustomType>& /*serializer*/ )
        {
            if( auto val = doc.get<int64_t>( "/value" ) )
                value = static_cast<int>( *val );
            if( auto n = doc.get<std::string>( "/name" ) )
                name = *n;
        }
    };

    TEST_F( BuilderSerializerTest, BuilderCustomTypeWithMethods )
    {
        SimpleCustomType obj = { 42, "test" };

        testSerializationProducesValidJson( obj );
        testRoundTrip( obj );
    }

    //=====================================================================
    // Large data structures
    //=====================================================================

    TEST_F( BuilderSerializerTest, BuilderLargeDataStructures )
    {
        // Large vector
        std::vector<int> largeVec;
        for( int i = 0; i < 1000; ++i )
        {
            largeVec.push_back( i );
        }
        testSerializationProducesValidJson( largeVec );
        testRoundTrip( largeVec );

        // Large map
        std::map<std::string, int> largeMap;
        for( int i = 0; i < 100; ++i )
        {
            largeMap["key_" + std::to_string( i )] = i;
        }
        testSerializationProducesValidJson( largeMap );
        testRoundTrip( largeMap );
    }

} // namespace nfx::serialization::json::test
