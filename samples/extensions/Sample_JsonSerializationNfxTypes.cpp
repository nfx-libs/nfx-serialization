/*
 * MIT License
 *
 * Copyright (c) 2026 nfx
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
 * @file Sample_JsonSerializationNfxTypes.cpp
 * @brief Complete guide to nfx library type serialization
 * @details Progressive tutorial covering all nfx library types:
 *          - nfx-containers: PerfectHashMap, FastHashMap, FastHashSet, OrderedHashMap, OrderedHashSet, SmallVector
 *          - nfx-datatypes: Int128, Decimal
 *          - nfx-datetime: DateTime, DateTimeOffset, TimeSpan
 *          Uses compile-time detection to show only available types.
 */

#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>
#include <nfx/serialization/json/extensions/DateTimeTraits.h>

#include <nfx/Serialization.h>

#if __has_include( <nfx/containers/FastHashMap.h>)
#    define NFX_CONTAINERS_AVAILABLE
#endif

#if __has_include( <nfx/datatypes/Int128.h>)
#    define NFX_DATATYPES_AVAILABLE
#endif

#if __has_include( <nfx/datetime/DateTime.h>)
#    define NFX_DATETIME_AVAILABLE
#endif

#include <iomanip>
#include <iostream>
#include <string>

int main()
{
    using namespace nfx::json;
    using namespace nfx::serialization::json;

    std::cout << "=== nfx-serialization NFX Types Guide ===\n";
    std::cout << "==========================================\n\n";

    //=====================================================================
    // nfx-containers: High-performance container types
    //=====================================================================

#ifdef NFX_CONTAINERS_AVAILABLE
    //=====================================================================
    // 1. nfx::containers::PerfectHashMap - Immutable perfect hash map
    //=====================================================================
    {
        std::cout << "1. nfx::containers::PerfectHashMap<K, V> - Immutable perfect hash map\n";
        std::cout << "-----------------------------------------------------------------------\n";

        std::vector<std::pair<std::string, int>> data = { { "width", 1920 }, { "height", 1080 }, { "fps", 60 } };
        nfx::containers::PerfectHashMap<std::string, int> config( std::move( data ) );

        Serializer<nfx::containers::PerfectHashMap<std::string, int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::containers::PerfectHashMap<std::string, int>>::toString( config, opts );

        std::cout << "PerfectHashMap<string, int>:\n" << json << "\n";

        auto restored = Serializer<nfx::containers::PerfectHashMap<std::string, int>>::fromString( json );

        const int* widthPtr = restored.find( "width" );
        bool success = ( restored.count() == 3 ) && widthPtr && ( *widthPtr == 1920 );
        std::cout << "\nDeserialized " << restored.count() << " entries (table size: " << restored.size() << ")\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Perfect hash with no collisions (CHD algorithm)\n";
        std::cout << "  Note: Immutable, O(1) guaranteed, count()=" << restored.count() << " vs size()=" << restored.size() << " (sparse table)\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 2. nfx::containers::FastHashMap - High-performance hash map
    //=====================================================================
    {
        std::cout << "2. nfx::containers::FastHashMap<K, V> - High-performance hash map\n";
        std::cout << "--------------------------------------------------------------------\n";

        nfx::containers::FastHashMap<std::string, int> scores;
        scores["Alice"] = 95;
        scores["Bob"] = 87;
        scores["Charlie"] = 92;

        Serializer<nfx::containers::FastHashMap<std::string, int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::containers::FastHashMap<std::string, int>>::toString( scores, opts );

        std::cout << "FastHashMap<string, int>:\n" << json << "\n";

        auto restored = Serializer<nfx::containers::FastHashMap<std::string, int>>::fromString( json );

        bool success = ( restored.size() == 3 ) && ( restored["Alice"] == 95 );
        std::cout << "\nDeserialized " << restored.size() << " entries\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": FastHashMap serializes as JSON object\n";
        std::cout << "  Note: Optimized for performance with inline storage\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 3. nfx::containers::FastHashSet - High-performance hash set
    //=====================================================================
    {
        std::cout << "3. nfx::containers::FastHashSet<T> - High-performance hash set\n";
        std::cout << "----------------------------------------------------------------\n";

        nfx::containers::FastHashSet<std::string> tags;
        tags.insert( "cpp" );
        tags.insert( "json" );
        tags.insert( "performance" );

        Serializer<nfx::containers::FastHashSet<std::string>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::containers::FastHashSet<std::string>>::toString( tags, opts );

        std::cout << "FastHashSet<string>:\n" << json << "\n";

        auto restored = Serializer<nfx::containers::FastHashSet<std::string>>::fromString( json );

        bool success = ( restored.size() == 3 ) && restored.contains( "cpp" );
        std::cout << "\nDeserialized " << restored.size() << " elements\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": FastHashSet serializes as JSON array\n";
        std::cout << "  Note: Order may vary (hash-based)\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 4. nfx::containers::OrderedHashMap - Insertion-order preserving map
    //=====================================================================
    {
        std::cout << "4. nfx::containers::OrderedHashMap<K, V> - Insertion-order preserving map\n";
        std::cout << "--------------------------------------------------------------------------\n";

        nfx::containers::OrderedHashMap<std::string, int> sequence;
        sequence.insert( "first", 1 );
        sequence.insert( "second", 2 );
        sequence.insert( "third", 3 );

        Serializer<nfx::containers::OrderedHashMap<std::string, int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::containers::OrderedHashMap<std::string, int>>::toString( sequence, opts );

        std::cout << "OrderedHashMap<string, int>:\n" << json << "\n";

        auto restored = Serializer<nfx::containers::OrderedHashMap<std::string, int>>::fromString( json );

        bool success = ( restored.size() == 3 );
        std::cout << "\nDeserialized " << restored.size() << " entries in order\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": OrderedHashMap preserves insertion order\n";
        std::cout << "  Note: Serialized as array of {\"key\": K, \"value\": V} objects\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 5. nfx::containers::OrderedHashSet - Insertion-order preserving set
    //=====================================================================
    {
        std::cout << "5. nfx::containers::OrderedHashSet<T> - Insertion-order preserving set\n";
        std::cout << "------------------------------------------------------------------------\n";

        nfx::containers::OrderedHashSet<std::string> steps;
        steps.insert( "init" );
        steps.insert( "process" );
        steps.insert( "finalize" );

        Serializer<nfx::containers::OrderedHashSet<std::string>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::containers::OrderedHashSet<std::string>>::toString( steps, opts );

        std::cout << "OrderedHashSet<string>:\n" << json << "\n";

        auto restored = Serializer<nfx::containers::OrderedHashSet<std::string>>::fromString( json );

        bool success = ( restored.size() == 3 );
        std::cout << "\nDeserialized " << restored.size() << " elements in order\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": OrderedHashSet preserves insertion order\n";
        std::cout << "  Note: Perfect for maintaining ordered unique collections\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 6. nfx::containers::SmallVector - Stack-optimized vector
    //=====================================================================
    {
        std::cout << "6. nfx::containers::SmallVector<T, N> - Stack-optimized vector\n";
        std::cout << "---------------------------------------------------------------\n";

        nfx::containers::SmallVector<int, 8> numbers; // 8 elements on stack
        numbers.push_back( 10 );
        numbers.push_back( 20 );
        numbers.push_back( 30 );

        Serializer<nfx::containers::SmallVector<int, 8>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::containers::SmallVector<int, 8>>::toString( numbers, opts );

        std::cout << "SmallVector<int, 8>:\n" << json << "\n";

        auto restored = Serializer<nfx::containers::SmallVector<int, 8>>::fromString( json );

        bool success = ( restored.size() == 3 ) && ( restored[0] == 10 );
        std::cout << "\nDeserialized " << restored.size() << " elements\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": SmallVector serializes as JSON array\n";
        std::cout << "  Note: Avoids heap allocation for small sizes\n";
        std::cout << "\n";
    }
#endif

    //=====================================================================
    // nfx-datatypes: Extended numeric types
    //=====================================================================

#ifdef NFX_DATATYPES_AVAILABLE
    //=====================================================================
    // 7. nfx::datatypes::Int128 - 128-bit integer
    //=====================================================================
    {
        std::cout << "7. nfx::datatypes::Int128 - 128-bit integer\n";
        std::cout << "--------------------------------------------\n";

        nfx::datatypes::Int128 bigNumber( "123456789012345678901234567890" );

        Serializer<nfx::datatypes::Int128>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::datatypes::Int128>::toString( bigNumber, opts );

        std::cout << "Int128:\n" << json << "\n";

        auto restored = Serializer<nfx::datatypes::Int128>::fromString( json );

        bool success = ( restored == bigNumber );
        std::cout << "\nDeserialized: " << restored.toString() << "\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Int128 serializes as JSON string\n";
        std::cout << "  Note: String format preserves full precision beyond JSON number limits\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 8. nfx::datatypes::Decimal - Arbitrary-precision decimal
    //=====================================================================
    {
        std::cout << "8. nfx::datatypes::Decimal - Arbitrary-precision decimal\n";
        std::cout << "---------------------------------------------------------\n";

        nfx::datatypes::Decimal price( "123.456789" );

        Serializer<nfx::datatypes::Decimal>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::datatypes::Decimal>::toString( price, opts );

        std::cout << "Decimal:\n" << json << "\n";

        auto restored = Serializer<nfx::datatypes::Decimal>::fromString( json );

        bool success = ( restored == price );
        std::cout << "\nDeserialized: " << restored.toString() << "\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Decimal serializes as JSON string\n";
        std::cout << "  Note: Perfect for financial calculations (no rounding errors)\n";
        std::cout << "\n";
    }
#endif

    //=====================================================================
    // nfx-datetime: Date and time types
    //=====================================================================

#ifdef NFX_DATETIME_AVAILABLE
    //=====================================================================
    // 9. nfx::datetime::DateTime - Date and time
    //=====================================================================
    {
        std::cout << "9. nfx::datetime::DateTime - Date and time\n";
        std::cout << "-------------------------------------------\n";

        nfx::time::DateTime now = nfx::time::DateTime::now();

        Serializer<nfx::time::DateTime>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::time::DateTime>::toString( now, opts );

        std::cout << "DateTime:\n" << json << "\n";

        auto restored = Serializer<nfx::time::DateTime>::fromString( json );

        bool success = ( restored.year() == now.year() && restored.month() == now.month() );
        std::cout << "\nDeserialized: " << restored.toString( nfx::time::DateTime::Format::Iso8601Precise ) << "\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": DateTime serializes as ISO 8601 string\n";
        std::cout << "  Note: Use Format::Iso8601Precise for 7 decimal digits (tick precision)\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 10. nfx::datetime::DateTimeOffset - Date, time, and timezone offset
    //=====================================================================
    {
        std::cout << "10. nfx::datetime::DateTimeOffset - Date, time, and timezone offset\n";
        std::cout << "-------------------------------------------------------------------\n";

        nfx::time::DateTimeOffset timestamp = nfx::time::DateTimeOffset::now();

        Serializer<nfx::time::DateTimeOffset>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::time::DateTimeOffset>::toString( timestamp, opts );

        std::cout << "DateTimeOffset:\n" << json << "\n";

        auto restored = Serializer<nfx::time::DateTimeOffset>::fromString( json );

        bool success = ( restored.year() == timestamp.year() && restored.month() == timestamp.month() );
        std::cout << "\nDeserialized: " << restored.toString( nfx::time::DateTime::Format::Iso8601Precise ) << "\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": DateTimeOffset serializes with timezone\n";
        std::cout << "  Note: Use Format::Iso8601Precise for full tick precision (7 decimals)\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 11. nfx::datetime::TimeSpan - Duration/interval
    //=====================================================================
    {
        std::cout << "11. nfx::datetime::TimeSpan - Duration/interval\n";
        std::cout << "------------------------------------------------\n";

        nfx::time::TimeSpan duration = nfx::time::TimeSpan::fromHours( 2.5 );

        Serializer<nfx::time::TimeSpan>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<nfx::time::TimeSpan>::toString( duration, opts );

        std::cout << "TimeSpan:\n" << json << "\n";

        auto restored = Serializer<nfx::time::TimeSpan>::fromString( json );

        bool success = ( restored.ticks() == duration.ticks() );
        std::cout << "\nDeserialized: " << restored.toString() << " (" << restored.ticks() << " ticks)\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": TimeSpan serializes as ticks (int64)\n";
        std::cout << "  Note: Precise duration representation\n";
        std::cout << "\n";
    }
#endif

    return 0;
}
