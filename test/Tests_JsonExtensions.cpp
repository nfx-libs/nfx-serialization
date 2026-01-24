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
 * @file Tests_JsonExtensions.cpp
 * @brief Unit tests for extension traits (nfx-containers, nfx-datatypes, nfx-datetime)
 * @details Tests covering serialization and deserialization of external nfx library types:
 *          - nfx-containers: PerfectHashMap, FastHashMap, FastHashSet
 *          - nfx-datatypes: Int128, Decimal
 *          - nfx-datetime: DateTime, DateTimeOffset, TimeSpan
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>
#include <nfx/serialization/json/extensions/DateTimeTraits.h>

#include <nfx/Serialization.h>

#include <nfx/Containers.h>
#include <nfx/DataTypes.h>
#include <nfx/DateTime.h>

using namespace nfx::serialization::json;

namespace nfx::serialization::json::test
{
    //=====================================================================
    // nfx-containers: FastHashMap tests
    //=====================================================================

    class FastHashMapExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( FastHashMapExtensionTest, SerializeEmptyMap )
    {
        nfx::containers::FastHashMap<std::string, int> map;

        std::string json = Serializer<decltype( map )>::toString( map );

        EXPECT_FALSE( json.empty() );
        EXPECT_EQ( json, "[]" );
    }

    TEST_F( FastHashMapExtensionTest, SerializeStringIntMap )
    {
        nfx::containers::FastHashMap<std::string, int> map;
        map.insertOrAssign( "Alice", 95 );
        map.insertOrAssign( "Bob", 87 );
        map.insertOrAssign( "Charlie", 92 );

        std::string json = Serializer<decltype( map )>::toString( map );

        EXPECT_FALSE( json.empty() );

        // Deserialize and verify
        auto restored = Serializer<decltype( map )>::fromString( json );
        EXPECT_EQ( restored.size(), 3 );

        // find() returns pointer, not iterator
        const int* alice = restored.find( "Alice" );
        ASSERT_NE( alice, nullptr );
        EXPECT_EQ( *alice, 95 );

        const int* bob = restored.find( "Bob" );
        ASSERT_NE( bob, nullptr );
        EXPECT_EQ( *bob, 87 );

        const int* charlie = restored.find( "Charlie" );
        ASSERT_NE( charlie, nullptr );
        EXPECT_EQ( *charlie, 92 );
    }

    TEST_F( FastHashMapExtensionTest, SerializeIntStringMap )
    {
        nfx::containers::FastHashMap<int, std::string> map;
        map.insertOrAssign( 1, "one" );
        map.insertOrAssign( 2, "two" );
        map.insertOrAssign( 3, "three" );

        std::string json = Serializer<decltype( map )>::toString( map );

        EXPECT_FALSE( json.empty() );

        // Deserialize and verify
        auto restored = Serializer<decltype( map )>::fromString( json );
        EXPECT_EQ( restored.size(), 3 );

        const std::string* one = restored.find( 1 );
        ASSERT_NE( one, nullptr );
        EXPECT_EQ( *one, "one" );
    }

    TEST_F( FastHashMapExtensionTest, RoundTripPreservesData )
    {
        nfx::containers::FastHashMap<std::string, double> original;
        original.insertOrAssign( "pi", 3.14159 );
        original.insertOrAssign( "e", 2.71828 );
        original.insertOrAssign( "phi", 1.61803 );

        std::string json = Serializer<decltype( original )>::toString( original );
        auto restored = Serializer<decltype( original )>::fromString( json );

        EXPECT_EQ( restored.size(), original.size() );

        for( const auto& [key, value] : original )
        {
            const double* ptr = restored.find( key );
            ASSERT_NE( ptr, nullptr );
            EXPECT_DOUBLE_EQ( *ptr, value );
        }
    }

    TEST_F( FastHashMapExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        nfx::containers::FastHashMap<std::string, int> scores;
        scores.insertOrAssign( "Alice", 95 );
        scores.insertOrAssign( "Bob", 87 );

        doc.set<decltype( scores )>( "scores", scores );

        auto retrieved = doc.get<decltype( scores )>( "scores" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value().size(), 2 );

        const int* alice = retrieved.value().find( "Alice" );
        ASSERT_NE( alice, nullptr );
        EXPECT_EQ( *alice, 95 );
    }

    TEST_F( FastHashMapExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        nfx::containers::FastHashMap<std::string, int> data;
        data.insertOrAssign( "key", 42 );

        doc.set<decltype( data )>( "map", data );

        EXPECT_TRUE( doc.is<decltype( data )>( "map" ) );
        EXPECT_FALSE( doc.is<decltype( data )>( "missing" ) );
    }

    //=====================================================================
    // nfx-containers: FastHashSet tests
    //=====================================================================

    class FastHashSetExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( FastHashSetExtensionTest, SerializeEmptySet )
    {
        nfx::containers::FastHashSet<std::string> set;

        std::string json = Serializer<decltype( set )>::toString( set );

        EXPECT_FALSE( json.empty() );
        EXPECT_EQ( json, "[]" );
    }

    TEST_F( FastHashSetExtensionTest, SerializeStringSet )
    {
        nfx::containers::FastHashSet<std::string> set;
        set.insert( "apple" );
        set.insert( "banana" );
        set.insert( "cherry" );

        std::string json = Serializer<decltype( set )>::toString( set );

        EXPECT_FALSE( json.empty() );

        // Deserialize and verify
        auto restored = Serializer<decltype( set )>::fromString( json );
        EXPECT_EQ( restored.size(), 3 );
        EXPECT_TRUE( restored.contains( "apple" ) );
        EXPECT_TRUE( restored.contains( "banana" ) );
        EXPECT_TRUE( restored.contains( "cherry" ) );
    }

    TEST_F( FastHashSetExtensionTest, SerializeIntSet )
    {
        nfx::containers::FastHashSet<int> set;
        set.insert( 10 );
        set.insert( 20 );
        set.insert( 30 );

        std::string json = Serializer<decltype( set )>::toString( set );

        EXPECT_FALSE( json.empty() );

        // Deserialize and verify
        auto restored = Serializer<decltype( set )>::fromString( json );
        EXPECT_EQ( restored.size(), 3 );
        EXPECT_TRUE( restored.contains( 10 ) );
        EXPECT_TRUE( restored.contains( 20 ) );
        EXPECT_TRUE( restored.contains( 30 ) );
    }

    TEST_F( FastHashSetExtensionTest, RoundTripPreservesData )
    {
        nfx::containers::FastHashSet<std::string> original;
        original.insert( "red" );
        original.insert( "green" );
        original.insert( "blue" );

        std::string json = Serializer<decltype( original )>::toString( original );
        auto restored = Serializer<decltype( original )>::fromString( json );

        EXPECT_EQ( restored.size(), original.size() );

        for( const auto& value : original )
        {
            EXPECT_TRUE( restored.contains( value ) );
        }
    }

    TEST_F( FastHashSetExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        nfx::containers::FastHashSet<std::string> tags;
        tags.insert( "cpp" );
        tags.insert( "json" );
        tags.insert( "serialization" );

        doc.set<decltype( tags )>( "tags", tags );

        auto retrieved = doc.get<decltype( tags )>( "tags" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value().size(), 3 );
        EXPECT_TRUE( retrieved.value().contains( "cpp" ) );
    }

    TEST_F( FastHashSetExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        nfx::containers::FastHashSet<int> numbers;
        numbers.insert( 1 );
        numbers.insert( 2 );
        numbers.insert( 3 );

        doc.set<decltype( numbers )>( "numbers", numbers );

        EXPECT_TRUE( doc.is<decltype( numbers )>( "numbers" ) );
        EXPECT_FALSE( doc.is<decltype( numbers )>( "notHere" ) );
    }

    //=====================================================================
    // nfx-containers: PerfectHashMap tests
    //=====================================================================

    class PerfectHashMapExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( PerfectHashMapExtensionTest, SerializeSimpleMap )
    {
        // PerfectHashMap uses constructor with vector
        std::vector<std::pair<std::string, int>> data = { { "one", 1 }, { "two", 2 }, { "three", 3 } };
        nfx::containers::PerfectHashMap<std::string, int> map( std::move( data ) );

        std::string json = Serializer<decltype( map )>::toString( map );

        EXPECT_FALSE( json.empty() );

        // Verify it's valid JSON array format
        auto docOpt = Document::fromString( json );
        ASSERT_TRUE( docOpt.has_value() );
        EXPECT_TRUE( docOpt->is<Array>( "" ) );
    }

    TEST_F( PerfectHashMapExtensionTest, RoundTripPreservesData )
    {
        std::vector<std::pair<std::string, double>> data = { { "pi", 3.14159 }, { "e", 2.71828 } };
        nfx::containers::PerfectHashMap<std::string, double> original( std::move( data ) );

        std::string json = Serializer<decltype( original )>::toString( original );
        auto restored = Serializer<decltype( original )>::fromString( json );

        EXPECT_EQ( restored.size(), original.size() );

        // Verify all original values are in restored (find returns pointer)
        for( auto it = original.begin(); it != original.end(); ++it )
        {
            const double* restoredPtr = restored.find( it->first );
            ASSERT_NE( restoredPtr, nullptr );
            EXPECT_DOUBLE_EQ( *restoredPtr, it->second );
        }
    }

    TEST_F( PerfectHashMapExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        std::vector<std::pair<std::string, int>> data = { { "alpha", 1 }, { "beta", 2 }, { "gamma", 3 } };
        nfx::containers::PerfectHashMap<std::string, int> map( std::move( data ) );

        doc.set<decltype( map )>( "perfectMap", map );

        auto retrieved = doc.get<decltype( map )>( "perfectMap" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value().count(), 3 );

        const int* alpha = retrieved.value().find( "alpha" );
        ASSERT_NE( alpha, nullptr );
        EXPECT_EQ( *alpha, 1 );
    }

    TEST_F( PerfectHashMapExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        std::vector<std::pair<int, std::string>> data = { { 1, "one" }, { 2, "two" } };
        nfx::containers::PerfectHashMap<int, std::string> map( std::move( data ) );

        doc.set<decltype( map )>( "lookup", map );

        EXPECT_TRUE( doc.is<decltype( map )>( "lookup" ) );
        EXPECT_FALSE( doc.is<decltype( map )>( "nowhere" ) );
    }

    //=====================================================================
    // nfx-containers: SmallVector tests
    //=====================================================================

    class SmallVectorExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( SmallVectorExtensionTest, SerializeEmptyVector )
    {
        nfx::containers::SmallVector<int, 8> vec;

        std::string json = Serializer<decltype( vec )>::toString( vec );

        EXPECT_FALSE( json.empty() );
        EXPECT_EQ( json, "[]" );
    }

    TEST_F( SmallVectorExtensionTest, SerializeIntVector )
    {
        nfx::containers::SmallVector<int, 8> vec;
        vec.push_back( 10 );
        vec.push_back( 20 );
        vec.push_back( 30 );

        std::string json = Serializer<decltype( vec )>::toString( vec );

        EXPECT_FALSE( json.empty() );

        // Deserialize and verify
        auto restored = Serializer<decltype( vec )>::fromString( json );
        EXPECT_EQ( restored.size(), 3 );
        EXPECT_EQ( restored[0], 10 );
        EXPECT_EQ( restored[1], 20 );
        EXPECT_EQ( restored[2], 30 );
    }

    TEST_F( SmallVectorExtensionTest, SerializeStringVector )
    {
        nfx::containers::SmallVector<std::string, 4> vec;
        vec.push_back( "apple" );
        vec.push_back( "banana" );
        vec.push_back( "cherry" );

        std::string json = Serializer<decltype( vec )>::toString( vec );

        EXPECT_FALSE( json.empty() );

        // Deserialize and verify
        auto restored = Serializer<decltype( vec )>::fromString( json );
        EXPECT_EQ( restored.size(), 3 );
        EXPECT_EQ( restored[0], "apple" );
        EXPECT_EQ( restored[1], "banana" );
        EXPECT_EQ( restored[2], "cherry" );
    }

    TEST_F( SmallVectorExtensionTest, SerializeDoubleVector )
    {
        nfx::containers::SmallVector<double, 8> vec;
        vec.push_back( 3.14 );
        vec.push_back( 2.71 );
        vec.push_back( 1.41 );

        std::string json = Serializer<decltype( vec )>::toString( vec );

        EXPECT_FALSE( json.empty() );

        // Deserialize and verify
        auto restored = Serializer<decltype( vec )>::fromString( json );
        EXPECT_EQ( restored.size(), 3 );
        EXPECT_DOUBLE_EQ( restored[0], 3.14 );
        EXPECT_DOUBLE_EQ( restored[1], 2.71 );
        EXPECT_DOUBLE_EQ( restored[2], 1.41 );
    }

    TEST_F( SmallVectorExtensionTest, RoundTripPreservesData )
    {
        nfx::containers::SmallVector<int, 8> original;
        original.push_back( 100 );
        original.push_back( 200 );
        original.push_back( 300 );
        original.push_back( 400 );

        std::string json = Serializer<decltype( original )>::toString( original );
        auto restored = Serializer<decltype( original )>::fromString( json );

        EXPECT_EQ( restored.size(), original.size() );

        for( size_t i = 0; i < original.size(); ++i )
        {
            EXPECT_EQ( restored[i], original[i] );
        }
    }

    TEST_F( SmallVectorExtensionTest, SmallCapacityStackStorage )
    {
        // Test with size within stack capacity (N=8)
        nfx::containers::SmallVector<int, 8> vec;
        for( int i = 0; i < 5; ++i )
        {
            vec.push_back( i * 10 );
        }

        std::string json = Serializer<decltype( vec )>::toString( vec );
        auto restored = Serializer<decltype( vec )>::fromString( json );

        EXPECT_EQ( restored.size(), 5 );
        for( int i = 0; i < 5; ++i )
        {
            EXPECT_EQ( restored[i], i * 10 );
        }
    }

    TEST_F( SmallVectorExtensionTest, LargeCapacityHeapStorage )
    {
        // Test with size exceeding stack capacity (N=8)
        nfx::containers::SmallVector<int, 8> vec;
        for( int i = 0; i < 20; ++i )
        {
            vec.push_back( i );
        }

        std::string json = Serializer<decltype( vec )>::toString( vec );
        auto restored = Serializer<decltype( vec )>::fromString( json );

        EXPECT_EQ( restored.size(), 20 );
        for( int i = 0; i < 20; ++i )
        {
            EXPECT_EQ( restored[i], i );
        }
    }

    TEST_F( SmallVectorExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        nfx::containers::SmallVector<std::string, 4> tags;
        tags.push_back( "cpp" );
        tags.push_back( "json" );
        tags.push_back( "serialization" );

        doc.set<decltype( tags )>( "tags", tags );

        auto retrieved = doc.get<decltype( tags )>( "tags" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value().size(), 3 );
        EXPECT_EQ( retrieved.value()[0], "cpp" );
        EXPECT_EQ( retrieved.value()[1], "json" );
        EXPECT_EQ( retrieved.value()[2], "serialization" );
    }

    TEST_F( SmallVectorExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        nfx::containers::SmallVector<int, 8> numbers;
        numbers.push_back( 1 );
        numbers.push_back( 2 );
        numbers.push_back( 3 );

        doc.set<decltype( numbers )>( "numbers", numbers );

        EXPECT_TRUE( doc.is<decltype( numbers )>( "numbers" ) );
        EXPECT_FALSE( doc.is<decltype( numbers )>( "notHere" ) );
    }

    TEST_F( SmallVectorExtensionTest, NestedSmallVectors )
    {
        SerializableDocument doc;
        nfx::containers::SmallVector<nfx::containers::SmallVector<int, 4>, 4> nested;

        nfx::containers::SmallVector<int, 4> row1;
        row1.push_back( 1 );
        row1.push_back( 2 );
        nested.push_back( row1 );

        nfx::containers::SmallVector<int, 4> row2;
        row2.push_back( 3 );
        row2.push_back( 4 );
        nested.push_back( row2 );

        doc.set<decltype( nested )>( "matrix", nested );

        auto retrieved = doc.get<decltype( nested )>( "matrix" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value().size(), 2 );
        EXPECT_EQ( retrieved.value()[0].size(), 2 );
        EXPECT_EQ( retrieved.value()[0][0], 1 );
        EXPECT_EQ( retrieved.value()[0][1], 2 );
        EXPECT_EQ( retrieved.value()[1][0], 3 );
        EXPECT_EQ( retrieved.value()[1][1], 4 );
    }

    //=====================================================================
    // nfx-datatypes: Int128 tests
    //=====================================================================

    class Int128ExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( Int128ExtensionTest, SerializeZero )
    {
        nfx::datatypes::Int128 value( 0 );

        std::string json = Serializer<nfx::datatypes::Int128>::toString( value );

        EXPECT_EQ( json, "\"0\"" );
    }

    TEST_F( Int128ExtensionTest, SerializePositiveValue )
    {
        nfx::datatypes::Int128 value( 123456789 );

        std::string json = Serializer<nfx::datatypes::Int128>::toString( value );

        EXPECT_EQ( json, "\"123456789\"" );
    }

    TEST_F( Int128ExtensionTest, SerializeNegativeValue )
    {
        nfx::datatypes::Int128 value( -987654321 );

        std::string json = Serializer<nfx::datatypes::Int128>::toString( value );

        EXPECT_EQ( json, "\"-987654321\"" );
    }

    TEST_F( Int128ExtensionTest, SerializeLargeValue )
    {
        // Create a large 128-bit value
        nfx::datatypes::Int128 value;
        [[maybe_unused]] auto result = nfx::datatypes::Int128::fromString(
            "170141183460469231731687303715884105727", value ); // Max signed 128-bit

        std::string json = Serializer<nfx::datatypes::Int128>::toString( value );

        EXPECT_FALSE( json.empty() );
        EXPECT_NE( json, "\"0\"" );
    }

    TEST_F( Int128ExtensionTest, RoundTripPreservesValue )
    {
        nfx::datatypes::Int128 original( static_cast<std::int64_t>( 9876543210 ) );

        std::string json = Serializer<nfx::datatypes::Int128>::toString( original );
        auto restored = Serializer<nfx::datatypes::Int128>::fromString( json );

        EXPECT_EQ( restored, original );
    }

    TEST_F( Int128ExtensionTest, RoundTripLargeValue )
    {
        nfx::datatypes::Int128 original;
        [[maybe_unused]] auto result = nfx::datatypes::Int128::fromString( "12345678901234567890", original );

        std::string json = Serializer<nfx::datatypes::Int128>::toString( original );
        auto restored = Serializer<nfx::datatypes::Int128>::fromString( json );

        EXPECT_EQ( restored, original );
    }

    TEST_F( Int128ExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        nfx::datatypes::Int128 value( static_cast<std::int64_t>( 1234567890 ) );

        doc.set<nfx::datatypes::Int128>( "int128Value", value );

        auto retrieved = doc.get<nfx::datatypes::Int128>( "int128Value" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value(), value );
    }

    TEST_F( Int128ExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        nfx::datatypes::Int128 value( static_cast<std::int64_t>( 9876543210 ) );

        doc.set<nfx::datatypes::Int128>( "largeNumber", value );

        EXPECT_TRUE( doc.is<nfx::datatypes::Int128>( "largeNumber" ) );
        EXPECT_FALSE( doc.is<nfx::datatypes::Int128>( "nonExistent" ) );
    }

    //=====================================================================
    // nfx-datatypes: Decimal tests
    //=====================================================================

    class DecimalExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( DecimalExtensionTest, SerializeZero )
    {
        nfx::datatypes::Decimal value( 0 );

        std::string json = Serializer<nfx::datatypes::Decimal>::toString( value );

        EXPECT_FALSE( json.empty() );
    }

    TEST_F( DecimalExtensionTest, SerializePositiveValue )
    {
        nfx::datatypes::Decimal value( "123.45" );

        std::string json = Serializer<nfx::datatypes::Decimal>::toString( value );

        EXPECT_FALSE( json.empty() );
    }

    TEST_F( DecimalExtensionTest, SerializeNegativeValue )
    {
        nfx::datatypes::Decimal value( "-987.654" );

        std::string json = Serializer<nfx::datatypes::Decimal>::toString( value );

        EXPECT_FALSE( json.empty() );
    }

    TEST_F( DecimalExtensionTest, RoundTripPreservesValue )
    {
        nfx::datatypes::Decimal original( "12345.6789" );

        std::string json = Serializer<nfx::datatypes::Decimal>::toString( original );
        auto restored = Serializer<nfx::datatypes::Decimal>::fromString( json );

        EXPECT_EQ( restored, original );
    }

    TEST_F( DecimalExtensionTest, RoundTripPrecision )
    {
        nfx::datatypes::Decimal original( "0.00000001" );

        std::string json = Serializer<nfx::datatypes::Decimal>::toString( original );
        auto restored = Serializer<nfx::datatypes::Decimal>::fromString( json );

        EXPECT_EQ( restored, original );
    }

    TEST_F( DecimalExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        nfx::datatypes::Decimal value( 123.456 );

        doc.set<nfx::datatypes::Decimal>( "decimalValue", value );

        auto retrieved = doc.get<nfx::datatypes::Decimal>( "decimalValue" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value(), value );
    }

    TEST_F( DecimalExtensionTest, DirectDocumentSetGetMove )
    {
        SerializableDocument doc;

        doc.set<nfx::datatypes::Decimal>( "price", nfx::datatypes::Decimal( 99.99 ) );

        auto retrieved = doc.get<nfx::datatypes::Decimal>( "price" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value(), nfx::datatypes::Decimal( 99.99 ) );
    }

    TEST_F( DecimalExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        nfx::datatypes::Decimal value( "123.456789" );

        doc.set<nfx::datatypes::Decimal>( "amount", value );

        EXPECT_TRUE( doc.is<nfx::datatypes::Decimal>( "amount" ) );
        EXPECT_FALSE( doc.is<nfx::datatypes::Decimal>( "missing" ) );
    }

    TEST_F( DecimalExtensionTest, DirectDocumentSetWithCString )
    {
        SerializableDocument doc;

        // Test with explicit type parameter and const char*
        doc.set<nfx::datatypes::Decimal>( "value1", "123456789.12345678913456789" );

        auto retrieved = doc.get<nfx::datatypes::Decimal>( "value1" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value(), nfx::datatypes::Decimal( "123456789.12345678913456789" ) );
    }

    TEST_F( DecimalExtensionTest, DirectDocumentSetWithCStringHighPrecision )
    {
        SerializableDocument doc;

        // Test with very high precision decimal
        const char* highPrecision = "0.123456789123456789123456789";
        doc.set<nfx::datatypes::Decimal>( "precision", highPrecision );

        auto retrieved = doc.get<nfx::datatypes::Decimal>( "precision" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value(), nfx::datatypes::Decimal( highPrecision ) );
    }

    TEST_F( DecimalExtensionTest, DirectDocumentSetWithCStringNegative )
    {
        SerializableDocument doc;

        // Test with negative value
        doc.set<nfx::datatypes::Decimal>( "negative", "-999999.999999" );

        auto retrieved = doc.get<nfx::datatypes::Decimal>( "negative" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value(), nfx::datatypes::Decimal( "-999999.999999" ) );
    }

    TEST_F( DecimalExtensionTest, DirectDocumentSetWithDouble )
    {
        SerializableDocument doc;

        // Test with explicit type parameter and double
        doc.set<nfx::datatypes::Decimal>( "offset", 2.5 );

        auto retrieved = doc.get<nfx::datatypes::Decimal>( "offset" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value(), nfx::datatypes::Decimal( 2.5 ) );
    }

    TEST_F( DecimalExtensionTest, DirectDocumentSetWithInt )
    {
        SerializableDocument doc;

        // Test with integer value
        doc.set<nfx::datatypes::Decimal>( "count", 42 );

        auto retrieved = doc.get<nfx::datatypes::Decimal>( "count" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value(), nfx::datatypes::Decimal( 42 ) );
    }

    //=====================================================================
    // nfx-datetime: TimeSpan tests
    //=====================================================================

    class TimeSpanExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( TimeSpanExtensionTest, SerializeZero )
    {
        nfx::time::TimeSpan value( 0 );

        std::string json = Serializer<nfx::time::TimeSpan>::toString( value );

        EXPECT_EQ( json, "0" );
    }

    TEST_F( TimeSpanExtensionTest, SerializePositiveTicks )
    {
        nfx::time::TimeSpan value( 10000000 ); // 1 second in ticks

        std::string json = Serializer<nfx::time::TimeSpan>::toString( value );

        EXPECT_EQ( json, "10000000" );
    }

    TEST_F( TimeSpanExtensionTest, SerializeNegativeTicks )
    {
        nfx::time::TimeSpan value( -10000000 ); // -1 second in ticks

        std::string json = Serializer<nfx::time::TimeSpan>::toString( value );

        EXPECT_EQ( json, "-10000000" );
    }

    TEST_F( TimeSpanExtensionTest, SerializeFromHours )
    {
        nfx::time::TimeSpan value = nfx::time::TimeSpan::fromHours( 1 );

        std::string json = Serializer<nfx::time::TimeSpan>::toString( value );

        EXPECT_FALSE( json.empty() );
        EXPECT_NE( json, "0" );
    }

    TEST_F( TimeSpanExtensionTest, RoundTripPreservesValue )
    {
        nfx::time::TimeSpan original = nfx::time::TimeSpan::fromMinutes( 90 );

        std::string json = Serializer<nfx::time::TimeSpan>::toString( original );
        auto restored = Serializer<nfx::time::TimeSpan>::fromString( json );

        EXPECT_EQ( restored.ticks(), original.ticks() );
    }

    TEST_F( TimeSpanExtensionTest, RoundTripComplexDuration )
    {
        // 1 day, 2 hours, 30 minutes, 45 seconds
        nfx::time::TimeSpan original = nfx::time::TimeSpan::fromDays( 1 ) + nfx::time::TimeSpan::fromHours( 2 ) +
                                       nfx::time::TimeSpan::fromMinutes( 30 ) + nfx::time::TimeSpan::fromSeconds( 45 );

        std::string json = Serializer<nfx::time::TimeSpan>::toString( original );
        auto restored = Serializer<nfx::time::TimeSpan>::fromString( json );

        EXPECT_EQ( restored.ticks(), original.ticks() );
    }

    TEST_F( TimeSpanExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        nfx::time::TimeSpan duration = nfx::time::TimeSpan::fromHours( 3 ) + nfx::time::TimeSpan::fromMinutes( 45 );

        doc.set<nfx::time::TimeSpan>( "duration", duration );

        auto retrieved = doc.get<nfx::time::TimeSpan>( "duration" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value().ticks(), duration.ticks() );
    }

    TEST_F( TimeSpanExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        nfx::time::TimeSpan value = nfx::time::TimeSpan::fromSeconds( 30 );

        doc.set<nfx::time::TimeSpan>( "elapsed", value );

        EXPECT_TRUE( doc.is<nfx::time::TimeSpan>( "elapsed" ) );
        EXPECT_FALSE( doc.is<nfx::time::TimeSpan>( "notThere" ) );
    }

    //=====================================================================
    // nfx-datetime: DateTime tests
    //=====================================================================

    class DateTimeExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( DateTimeExtensionTest, SerializeNow )
    {
        nfx::time::DateTime now = nfx::time::DateTime::now();

        std::string json = Serializer<nfx::time::DateTime>::toString( now );

        EXPECT_FALSE( json.empty() );
        // Should be ISO 8601 format in quotes
        EXPECT_EQ( json.front(), '"' );
        EXPECT_EQ( json.back(), '"' );
    }

    TEST_F( DateTimeExtensionTest, SerializeSpecificDate )
    {
        nfx::time::DateTime dt( 2025, 11, 30, 10, 30, 45 );

        std::string json = Serializer<nfx::time::DateTime>::toString( dt );

        EXPECT_FALSE( json.empty() );
        // Should contain the date parts
        EXPECT_NE( json.find( "2025" ), std::string::npos );
        EXPECT_NE( json.find( "11" ), std::string::npos );
        EXPECT_NE( json.find( "30" ), std::string::npos );
    }

    TEST_F( DateTimeExtensionTest, RoundTripPreservesValue )
    {
        nfx::time::DateTime original( 2025, 6, 15, 14, 30, 0 );

        std::string json = Serializer<nfx::time::DateTime>::toString( original );
        auto restored = Serializer<nfx::time::DateTime>::fromString( json );

        EXPECT_EQ( restored.year(), original.year() );
        EXPECT_EQ( restored.month(), original.month() );
        EXPECT_EQ( restored.day(), original.day() );
        EXPECT_EQ( restored.hour(), original.hour() );
        EXPECT_EQ( restored.minute(), original.minute() );
        EXPECT_EQ( restored.second(), original.second() );
    }

    TEST_F( DateTimeExtensionTest, SerializeMinValue )
    {
        nfx::time::DateTime dt = nfx::time::DateTime::min();

        std::string json = Serializer<nfx::time::DateTime>::toString( dt );

        EXPECT_FALSE( json.empty() );
    }

    TEST_F( DateTimeExtensionTest, SerializeMaxValue )
    {
        nfx::time::DateTime dt = nfx::time::DateTime::max();

        std::string json = Serializer<nfx::time::DateTime>::toString( dt );

        EXPECT_FALSE( json.empty() );
    }

    TEST_F( DateTimeExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        nfx::time::DateTime value( 2025, 12, 25, 10, 30, 0 );

        doc.set<nfx::time::DateTime>( "timestamp", value );

        auto retrieved = doc.get<nfx::time::DateTime>( "timestamp" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value().year(), value.year() );
        EXPECT_EQ( retrieved.value().month(), value.month() );
        EXPECT_EQ( retrieved.value().day(), value.day() );
    }

    TEST_F( DateTimeExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        nfx::time::DateTime value( 2025, 1, 1, 0, 0, 0 );

        doc.set<nfx::time::DateTime>( "newYear", value );

        EXPECT_TRUE( doc.is<nfx::time::DateTime>( "newYear" ) );
        EXPECT_FALSE( doc.is<nfx::time::DateTime>( "notPresent" ) );
    }

    //=====================================================================
    // nfx-datetime: DateTimeOffset tests
    //=====================================================================

    class DateTimeOffsetExtensionTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( DateTimeOffsetExtensionTest, SerializeNow )
    {
        nfx::time::DateTimeOffset now = nfx::time::DateTimeOffset::now();

        std::string json = Serializer<nfx::time::DateTimeOffset>::toString( now );

        EXPECT_FALSE( json.empty() );
        // Should be ISO 8601 format in quotes
        EXPECT_EQ( json.front(), '"' );
        EXPECT_EQ( json.back(), '"' );
    }

    TEST_F( DateTimeOffsetExtensionTest, SerializeWithOffset )
    {
        nfx::time::DateTime dt( 2025, 11, 30, 10, 30, 45 );
        nfx::time::TimeSpan offset = nfx::time::TimeSpan::fromHours( 2 ); // UTC+2
        nfx::time::DateTimeOffset dto( dt, offset );

        std::string json = Serializer<nfx::time::DateTimeOffset>::toString( dto );

        EXPECT_FALSE( json.empty() );
        // Should contain offset indicator
        EXPECT_TRUE(
            json.find( "+" ) != std::string::npos || json.find( "-" ) != std::string::npos ||
            json.find( "Z" ) != std::string::npos );
    }

    TEST_F( DateTimeOffsetExtensionTest, RoundTripPreservesValue )
    {
        nfx::time::DateTime dt( 2025, 6, 15, 14, 30, 0 );
        nfx::time::TimeSpan offset = nfx::time::TimeSpan::fromHours( -5 ); // UTC-5
        nfx::time::DateTimeOffset original( dt, offset );

        std::string json = Serializer<nfx::time::DateTimeOffset>::toString( original );
        auto restored = Serializer<nfx::time::DateTimeOffset>::fromString( json );

        // Compare the UTC time (which should be identical)
        EXPECT_EQ( restored.utcDateTime().year(), original.utcDateTime().year() );
        EXPECT_EQ( restored.utcDateTime().month(), original.utcDateTime().month() );
        EXPECT_EQ( restored.utcDateTime().day(), original.utcDateTime().day() );
    }

    TEST_F( DateTimeOffsetExtensionTest, SerializeUtc )
    {
        nfx::time::DateTimeOffset utcNow = nfx::time::DateTimeOffset::utcNow();

        std::string json = Serializer<nfx::time::DateTimeOffset>::toString( utcNow );

        EXPECT_FALSE( json.empty() );
    }

    TEST_F( DateTimeOffsetExtensionTest, DirectDocumentSetGet )
    {
        SerializableDocument doc;
        nfx::time::DateTime dt( 2025, 6, 15, 14, 30, 0 );
        nfx::time::TimeSpan offset = nfx::time::TimeSpan::fromHours( 2 );
        nfx::time::DateTimeOffset value( dt, offset );

        doc.set<nfx::time::DateTimeOffset>( "timestampWithZone", value );

        auto retrieved = doc.get<nfx::time::DateTimeOffset>( "timestampWithZone" );
        ASSERT_TRUE( retrieved.has_value() );
        EXPECT_EQ( retrieved.value().utcDateTime().year(), value.utcDateTime().year() );
        EXPECT_EQ( retrieved.value().utcDateTime().month(), value.utcDateTime().month() );
    }

    TEST_F( DateTimeOffsetExtensionTest, DirectDocumentIsCheck )
    {
        SerializableDocument doc;
        nfx::time::DateTimeOffset value = nfx::time::DateTimeOffset::now();

        doc.set<nfx::time::DateTimeOffset>( "currentTime", value );

        EXPECT_TRUE( doc.is<nfx::time::DateTimeOffset>( "currentTime" ) );
        EXPECT_FALSE( doc.is<nfx::time::DateTimeOffset>( "absent" ) );
    }

    //=====================================================================
    // Integration tests - README sample validation
    //=====================================================================

    class ExtensionsReadmeSampleTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        void TearDown() override
        {
        }
    };

    TEST_F( ExtensionsReadmeSampleTest, FastHashMapSerialization )
    {
        // From README.md Optional Extensions section
        nfx::containers::FastHashMap<std::string, int> scores;
        scores.insertOrAssign( "Alice", 95 );
        scores.insertOrAssign( "Bob", 87 );

        std::string json = Serializer<decltype( scores )>::toString( scores );

        EXPECT_FALSE( json.empty() );

        // Verify round-trip
        auto restored = Serializer<decltype( scores )>::fromString( json );
        EXPECT_EQ( restored.size(), 2 );
    }

    TEST_F( ExtensionsReadmeSampleTest, DateTimeSerialization )
    {
        // From README.md Optional Extensions section
        nfx::time::DateTime now = nfx::time::DateTime::now();
        std::string timeJson = Serializer<nfx::time::DateTime>::toString( now );

        // Result should be ISO 8601 format
        EXPECT_FALSE( timeJson.empty() );
        EXPECT_EQ( timeJson.front(), '"' );
        EXPECT_EQ( timeJson.back(), '"' );

        // Should contain T separator for ISO 8601
        EXPECT_NE( timeJson.find( "T" ), std::string::npos );
    }

    //=====================================================================
    // STL Types via Document API (using Serializer)
    //=====================================================================

    TEST( DirectDocument_StlTypes, VectorIntSetGet )
    {
        SerializableDocument doc;
        std::vector<int> vec = { 1, 2, 3, 4, 5 };

        // Set via Document API
        doc.set( "/data", vec );

        // Get via Document API
        auto result = doc.get<std::vector<int>>( "/data" );
        ASSERT_TRUE( result.has_value() );
        EXPECT_EQ( result.value(), vec );
    }

    TEST( DirectDocument_StlTypes, VectorIntIsCheck )
    {
        SerializableDocument doc;
        std::vector<int> vec = { 10, 20, 30 };

        doc.set( "/numbers", vec );

        EXPECT_TRUE( doc.is<std::vector<int>>( "/numbers" ) );
        EXPECT_FALSE( doc.is<std::vector<std::string>>( "/numbers" ) );
        EXPECT_FALSE( doc.is<std::vector<int>>( "/nonexistent" ) );
    }

    TEST( DirectDocument_StlTypes, VectorStringSetGet )
    {
        SerializableDocument doc;
        std::vector<std::string> vec = { "hello", "world", "test" };

        doc.set( "/strings", vec );

        auto result = doc.get<std::vector<std::string>>( "/strings" );
        ASSERT_TRUE( result.has_value() );
        EXPECT_EQ( result.value(), vec );
    }

    TEST( DirectDocument_StlTypes, OptionalIntSetGet )
    {
        SerializableDocument doc;

        // Test with value
        std::optional<int> opt = 42;
        doc.set( "/opt_value", opt );

        auto result1 = doc.get<std::optional<int>>( "/opt_value" );
        ASSERT_TRUE( result1.has_value() );
        EXPECT_TRUE( result1.value().has_value() );
        EXPECT_EQ( result1.value().value(), 42 );

        // Test with nullopt
        std::optional<int> empty;
        doc.set( "/opt_empty", empty );

        auto result2 = doc.get<std::optional<int>>( "/opt_empty" );
        ASSERT_TRUE( result2.has_value() );
        EXPECT_FALSE( result2.value().has_value() );
    }

    TEST( DirectDocument_StlTypes, OptionalStringIsCheck )
    {
        SerializableDocument doc;
        std::optional<std::string> opt = "test";

        doc.set( "/opt", opt );

        EXPECT_TRUE( doc.is<std::optional<std::string>>( "/opt" ) );
        EXPECT_FALSE( doc.is<std::optional<int>>( "/opt" ) );
    }

    TEST( DirectDocument_StlTypes, UniquePtrSetGet )
    {
        SerializableDocument doc;

        // Test with value
        auto ptr = std::make_unique<int>( 123 );
        doc.set( "/ptr_value", ptr );

        auto result1 = doc.get<std::unique_ptr<int>>( "/ptr_value" );
        ASSERT_TRUE( result1.has_value() );
        ASSERT_NE( result1.value(), nullptr );
        EXPECT_EQ( *result1.value(), 123 );

        // Test with nullptr
        std::unique_ptr<int> nullPtr;
        doc.set( "/ptr_null", nullPtr );

        auto result2 = doc.get<std::unique_ptr<int>>( "/ptr_null" );
        ASSERT_TRUE( result2.has_value() );
        EXPECT_EQ( result2.value(), nullptr );
    }

    TEST( DirectDocument_StlTypes, SharedPtrSetGet )
    {
        SerializableDocument doc;

        auto ptr = std::make_shared<std::string>( "shared data" );
        doc.set( "/shared", ptr );

        auto result = doc.get<std::shared_ptr<std::string>>( "/shared" );
        ASSERT_TRUE( result.has_value() );
        ASSERT_NE( result.value(), nullptr );
        EXPECT_EQ( *result.value(), "shared data" );
    }

    TEST( DirectDocument_StlTypes, MapStringIntSetGet )
    {
        SerializableDocument doc;
        std::map<std::string, int> map = { { "one", 1 }, { "two", 2 }, { "three", 3 } };

        doc.set( "/map", map );

        auto result = doc.get<std::map<std::string, int>>( "/map" );
        ASSERT_TRUE( result.has_value() );
        EXPECT_EQ( result.value(), map );
    }

    TEST( DirectDocument_StlTypes, SetIntSetGet )
    {
        SerializableDocument doc;
        std::set<int> set = { 1, 2, 3, 4, 5 };

        doc.set( "/set", set );

        auto result = doc.get<std::set<int>>( "/set" );
        ASSERT_TRUE( result.has_value() );
        EXPECT_EQ( result.value(), set );
    }

    TEST( DirectDocument_StlTypes, NestedVectorSetGet )
    {
        SerializableDocument doc;
        std::vector<std::vector<int>> nested = { { 1, 2 }, { 3, 4 }, { 5, 6 } };

        doc.set( "/nested", nested );

        auto result = doc.get<std::vector<std::vector<int>>>( "/nested" );
        ASSERT_TRUE( result.has_value() );
        EXPECT_EQ( result.value(), nested );
    }

    TEST( DirectDocument_StlTypes, ComplexTypeSetGet )
    {
        SerializableDocument doc;
        std::map<std::string, std::vector<int>> complex = { { "first", { 1, 2, 3 } }, { "second", { 4, 5, 6 } } };

        doc.set( "/complex", complex );

        auto result = doc.get<std::map<std::string, std::vector<int>>>( "/complex" );
        ASSERT_TRUE( result.has_value() );
        EXPECT_EQ( result.value(), complex );
    }

    TEST( DirectDocument_StlTypes, MixedWithExtensionTypes )
    {
        SerializableDocument doc;

        // Extension type
        nfx::datatypes::Decimal dec( "123.45" );
        doc.set( "/decimal", dec );

        // STL type
        std::vector<int> vec = { 1, 2, 3 };
        doc.set( "/vector", vec );

        // Both should work
        EXPECT_TRUE( doc.is<nfx::datatypes::Decimal>( "/decimal" ) );
        EXPECT_TRUE( doc.is<std::vector<int>>( "/vector" ) );

        auto decResult = doc.get<nfx::datatypes::Decimal>( "/decimal" );
        auto vecResult = doc.get<std::vector<int>>( "/vector" );

        ASSERT_TRUE( decResult.has_value() );
        ASSERT_TRUE( vecResult.has_value() );
        EXPECT_EQ( decResult.value().toString(), "123.45" );
        EXPECT_EQ( vecResult.value(), vec );
    }
} // namespace nfx::serialization::json::test
