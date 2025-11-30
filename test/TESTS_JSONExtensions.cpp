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
 * @file TESTS_JSONExtensions.cpp
 * @brief Comprehensive tests for extension traits (nfx-containers, nfx-datatypes, nfx-datetime)
 * @details Tests covering serialization and deserialization of external nfx library types:
 *          - nfx-containers: PerfectHashMap, FastHashMap, FastHashSet
 *          - nfx-datatypes: Int128, Decimal
 *          - nfx-datetime: DateTime, DateTimeOffset, TimeSpan
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/Serializer.h>

// Extension headers
#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>
#include <nfx/serialization/json/extensions/DateTimeTraits.h>

// External library headers
#include <nfx/Containers.h>
#include <nfx/DataTypes.h>
#include <nfx/DateTime.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// nfx-containers: FastHashMap tests
	//=====================================================================

	class FastHashMapExtensionTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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

		for ( const auto& [key, value] : original )
		{
			const double* ptr = restored.find( key );
			ASSERT_NE( ptr, nullptr );
			EXPECT_DOUBLE_EQ( *ptr, value );
		}
	}

	//=====================================================================
	// nfx-containers: FastHashSet tests
	//=====================================================================

	class FastHashSetExtensionTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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

		for ( const auto& value : original )
		{
			EXPECT_TRUE( restored.contains( value ) );
		}
	}

	//=====================================================================
	// nfx-containers: PerfectHashMap tests
	//=====================================================================

	class PerfectHashMapExtensionTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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
		EXPECT_TRUE( docOpt->is<Document::Array>( "" ) );
	}

	TEST_F( PerfectHashMapExtensionTest, RoundTripPreservesData )
	{
		std::vector<std::pair<std::string, double>> data = { { "pi", 3.14159 }, { "e", 2.71828 } };
		nfx::containers::PerfectHashMap<std::string, double> original( std::move( data ) );

		std::string json = Serializer<decltype( original )>::toString( original );
		auto restored = Serializer<decltype( original )>::fromString( json );

		EXPECT_EQ( restored.size(), original.size() );

		// Verify all original values are in restored (find returns pointer)
		for ( auto it = original.begin(); it != original.end(); ++it )
		{
			const double* restoredPtr = restored.find( it->first );
			ASSERT_NE( restoredPtr, nullptr );
			EXPECT_DOUBLE_EQ( *restoredPtr, it->second );
		}
	}

	//=====================================================================
	// nfx-datatypes: Int128 tests
	//=====================================================================

	class Int128ExtensionTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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
		[[maybe_unused]] auto result = nfx::datatypes::Int128::fromString( "170141183460469231731687303715884105727", value ); // Max signed 128-bit

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

	//=====================================================================
	// nfx-datatypes: Decimal tests
	//=====================================================================

	class DecimalExtensionTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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

	//=====================================================================
	// nfx-datetime: TimeSpan tests
	//=====================================================================

	class TimeSpanExtensionTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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

	//=====================================================================
	// nfx-datetime: DateTime tests
	//=====================================================================

	class DateTimeExtensionTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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

	//=====================================================================
	// nfx-datetime: DateTimeOffset tests
	//=====================================================================

	class DateTimeOffsetExtensionTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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
		EXPECT_TRUE( json.find( "+" ) != std::string::npos || json.find( "-" ) != std::string::npos ||
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

	//=====================================================================
	// Integration tests - README sample validation
	//=====================================================================

	class ExtensionsReadmeSampleTest : public ::testing::Test
	{
	protected:
		void SetUp() override {}

		void TearDown() override {}
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
} // namespace nfx::serialization::json::test
