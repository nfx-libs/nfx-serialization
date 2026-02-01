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
 * @file Sample_JsonSerializationStlContainers.cpp
 * @brief Comprehensive guide to all STL container serialization in nfx-serialization
 * @details Progressive tutorial covering every standard C++ container type:
 *          - Simple types: pair, tuple, optional, variant
 *          - Sequential: array, vector, deque, list, forward_list
 *          - Associative: set, multiset, map, multimap
 *          - Unordered: unordered_set, unordered_multiset, unordered_map, unordered_multimap
 *          - Smart pointers: unique_ptr, shared_ptr
 *          - Views: span (C++20, serialization only)
 */

#include <nfx/Serialization.h>

#include <array>
#include <deque>
#include <forward_list>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

int main()
{
    using namespace nfx::json;
    using namespace nfx::serialization::json;

    std::cout << "=== nfx-serialization Complete STL Guide ===\n";
    std::cout << "============================================\n\n";

    //=====================================================================
    // 1. std::pair - Serialized as array [first, second]
    //=====================================================================
    {
        std::cout << "1. std::pair<T1, T2> - Serialized as array [first, second]\n";
        std::cout << "-------------------------------------------------------------\n";

        std::pair<std::string, int> coordinate{ "latitude", 45 };

        Serializer<std::pair<std::string, int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::pair<std::string, int>>::toString( coordinate, opts );

        std::cout << "std::pair<string, int>:\n" << json << "\n";

        auto restored = Serializer<std::pair<std::string, int>>::fromString( json );

        bool success = ( restored.first == "latitude" && restored.second == 45 );
        std::cout << "\nDeserialized: (" << restored.first << ", " << restored.second << ")\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Pair serializes as 2-element array\n";
        std::cout << "  Format: [first, second]\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 2. std::tuple - Serialized as array [elem0, elem1, ...]
    //=====================================================================
    {
        std::cout << "2. std::tuple<Ts...> - Serialized as array [elem0, elem1, ...]\n";
        std::cout << "-----------------------------------------------------------------\n";

        std::tuple<int, std::string, double, bool> record{ 42, "Alice", 3.14, true };

        Serializer<std::tuple<int, std::string, double, bool>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::tuple<int, std::string, double, bool>>::toString( record, opts );

        std::cout << "std::tuple<int, string, double, bool>:\n" << json << "\n";

        auto restored = Serializer<std::tuple<int, std::string, double, bool>>::fromString( json );

        bool success =
            ( std::get<0>( restored ) == 42 && std::get<1>( restored ) == "Alice" &&
              std::abs( std::get<2>( restored ) - 3.14 ) < 0.01 && std::get<3>( restored ) == true );

        std::cout << "\nDeserialized: (" << std::get<0>( restored ) << ", \"" << std::get<1>( restored ) << "\", "
                  << std::get<2>( restored ) << ", " << std::boolalpha << std::get<3>( restored ) << ")\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Tuple preserves element order and types\n";
        std::cout << "  Format: [elem0, elem1, ...]\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 3. std::optional - Serialized as value or null
    //=====================================================================
    {
        std::cout << "3. std::optional<T> - Serialized as value or null\n";
        std::cout << "----------------------------------------------------\n";

        std::optional<std::string> withValue = "Hello";
        std::optional<std::string> empty = std::nullopt;

        std::string jsonWithValue = Serializer<std::optional<std::string>>::toString( withValue );
        std::string jsonEmpty = Serializer<std::optional<std::string>>::toString( empty );

        std::cout << "optional with value: " << jsonWithValue << "\n";
        std::cout << "optional empty:      " << jsonEmpty << "\n";

        auto restoredWithValue = Serializer<std::optional<std::string>>::fromString( jsonWithValue );
        auto restoredEmpty = Serializer<std::optional<std::string>>::fromString( jsonEmpty );

        bool success =
            restoredWithValue.has_value() && restoredWithValue.value() == "Hello" && !restoredEmpty.has_value();

        std::cout << "\nDeserialized:\n";
        std::cout << "  With value: " << ( restoredWithValue.has_value() ? restoredWithValue.value() : "null" ) << "\n";
        std::cout << "  Empty:      " << ( restoredEmpty.has_value() ? restoredEmpty.value() : "null" ) << "\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Optional handles presence/absence correctly\n";
        std::cout << "  Format: value or null\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 4. std::variant - Serialized as {"tag": "TypeName", "data": value}
    //=====================================================================
    {
        std::cout << "4. std::variant<Ts...> - Serialized as {\"tag\": \"TypeName\", \"data\": value}\n";
        std::cout << "-------------------------------------------------------------------------\n";

        std::variant<int, std::string, double> varInt = 42;
        std::variant<int, std::string, double> varString = "Hello";
        std::variant<int, std::string, double> varDouble = 3.14;

        Serializer<std::variant<int, std::string, double>>::Options opts;
        opts.prettyPrint = true;

        std::string jsonInt = Serializer<std::variant<int, std::string, double>>::toString( varInt, opts );
        std::string jsonString = Serializer<std::variant<int, std::string, double>>::toString( varString, opts );
        std::string jsonDouble = Serializer<std::variant<int, std::string, double>>::toString( varDouble, opts );

        std::cout << "variant<int, string, double> holding int:\n" << jsonInt << "\n";
        std::cout << "\nvariant<int, string, double> holding string:\n" << jsonString << "\n";
        std::cout << "\nvariant<int, string, double> holding double:\n" << jsonDouble << "\n";

        auto restoredInt = Serializer<std::variant<int, std::string, double>>::fromString( jsonInt );
        auto restoredString = Serializer<std::variant<int, std::string, double>>::fromString( jsonString );
        auto restoredDouble = Serializer<std::variant<int, std::string, double>>::fromString( jsonDouble );

        bool success = std::holds_alternative<int>( restoredInt ) && std::get<int>( restoredInt ) == 42 &&
                       std::holds_alternative<std::string>( restoredString ) &&
                       std::get<std::string>( restoredString ) == "Hello" &&
                       std::holds_alternative<double>( restoredDouble ) &&
                       std::abs( std::get<double>( restoredDouble ) - 3.14 ) < 0.01;

        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Variant preserves active type\n";
        std::cout << "  Format: {\"tag\": \"TypeName\", \"data\": value}\n";
        std::cout << "  Note: Cross-language compatible (TypeScript, Rust, Python)\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 5. std::array - Fixed-size array serialized as JSON array
    //=====================================================================
    {
        std::cout << "5. std::array<T, N> - Fixed-size array serialized as JSON array\n";
        std::cout << "------------------------------------------------------------------\n";

        std::array<int, 5> fibonacci{ 1, 1, 2, 3, 5 };

        Serializer<std::array<int, 5>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::array<int, 5>>::toString( fibonacci, opts );

        std::cout << "std::array<int, 5>:\n" << json << "\n";

        auto restored = Serializer<std::array<int, 5>>::fromString( json );

        bool success = ( restored == fibonacci );
        std::cout << "\nDeserialized: [ ";
        for( size_t i = 0; i < restored.size(); ++i )
        {
            std::cout << restored[i];
            if( i < restored.size() - 1 )
                std::cout << ", ";
        }
        std::cout << " ]\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Fixed-size array preserves all elements\n";
        std::cout << "  Note: Size must match exactly during deserialization\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 6. std::vector - Dynamic array serialized as JSON array
    //=====================================================================
    {
        std::cout << "6. std::vector<T> - Dynamic array serialized as JSON array\n";
        std::cout << "-------------------------------------------------------------\n";

        std::vector<std::string> cities{ "Paris", "London", "Tokyo", "New York" };

        Serializer<std::vector<std::string>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::vector<std::string>>::toString( cities, opts );

        std::cout << "std::vector<string>:\n" << json << "\n";

        auto restored = Serializer<std::vector<std::string>>::fromString( json );

        bool success = ( restored == cities );
        std::cout << "\nDeserialized (" << restored.size() << " elements): [ ";
        for( size_t i = 0; i < restored.size(); ++i )
        {
            std::cout << "\"" << restored[i] << "\"";
            if( i < restored.size() - 1 )
                std::cout << ", ";
        }
        std::cout << " ]\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Vector handles dynamic sizing\n";
        std::cout << "  Note: Most common STL container\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 7. std::deque - Double-ended queue serialized as JSON array
    //=====================================================================
    {
        std::cout << "7. std::deque<T> - Double-ended queue serialized as JSON array\n";
        std::cout << "----------------------------------------------------------------\n";

        std::deque<int> queue{ 10, 20, 30, 40, 50 };

        Serializer<std::deque<int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::deque<int>>::toString( queue, opts );

        std::cout << "std::deque<int>:\n" << json << "\n";

        auto restored = Serializer<std::deque<int>>::fromString( json );

        bool success = ( restored == queue );
        std::cout << "\nDeserialized: [ ";
        for( size_t i = 0; i < restored.size(); ++i )
        {
            std::cout << restored[i];
            if( i < restored.size() - 1 )
                std::cout << ", ";
        }
        std::cout << " ]\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Deque serializes like vector\n";
        std::cout << "  Note: Efficient for front/back insertion\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 8. std::list - Doubly-linked list serialized as JSON array
    //=====================================================================
    {
        std::cout << "8. std::list<T> - Doubly-linked list serialized as JSON array\n";
        std::cout << "---------------------------------------------------------------\n";

        std::list<double> values{ 1.1, 2.2, 3.3, 4.4 };

        Serializer<std::list<double>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::list<double>>::toString( values, opts );

        std::cout << "std::list<double>:\n" << json << "\n";

        auto restored = Serializer<std::list<double>>::fromString( json );

        bool success = ( restored == values );
        std::cout << "\nDeserialized: [ ";
        for( auto it = restored.begin(); it != restored.end(); ++it )
        {
            std::cout << *it;
            if( std::next( it ) != restored.end() )
                std::cout << ", ";
        }
        std::cout << " ]\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": List preserves order\n";
        std::cout << "  Note: No random access, bidirectional iteration\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 9. std::forward_list - Singly-linked list serialized as JSON array
    //=====================================================================
    {
        std::cout << "9. std::forward_list<T> - Singly-linked list serialized as JSON array\n";
        std::cout << "-----------------------------------------------------------------------\n";

        std::forward_list<int> flist{ 10, 20, 30, 40, 50 };

        Serializer<std::forward_list<int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::forward_list<int>>::toString( flist, opts );

        std::cout << "std::forward_list<int>:\n" << json << "\n";

        auto restored = Serializer<std::forward_list<int>>::fromString( json );

        bool success = ( restored == flist );
        std::cout << "\nDeserialized: [ ";
        for( auto it = restored.begin(); it != restored.end(); ++it )
        {
            std::cout << *it;
            if( std::next( it ) != restored.end() )
                std::cout << ", ";
        }
        std::cout << " ]\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Forward_list preserves order\n";
        std::cout << "  Note: Singly-linked (forward-only iteration), no size() method\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 10. std::set - Ordered unique set serialized as JSON array
    //=====================================================================
    {
        std::cout << "10. std::set<T> - Ordered unique set serialized as JSON array\n";
        std::cout << "---------------------------------------------------------------\n";

        std::set<int> uniqueNumbers{ 5, 2, 8, 2, 1, 9, 1 }; // Duplicates removed, sorted

        Serializer<std::set<int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::set<int>>::toString( uniqueNumbers, opts );

        std::cout << "std::set<int>:\n" << json << "\n";

        auto restored = Serializer<std::set<int>>::fromString( json );

        bool success = ( restored == uniqueNumbers );
        std::cout << "\nDeserialized (sorted, unique): [ ";
        for( auto it = restored.begin(); it != restored.end(); ++it )
        {
            std::cout << *it;
            if( std::next( it ) != restored.end() )
                std::cout << ", ";
        }
        std::cout << " ]\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Set maintains sorted order\n";
        std::cout << "  Note: Automatically removes duplicates and sorts\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 11. std::multiset - Ordered set allowing duplicates
    //=====================================================================
    {
        std::cout << "11. std::multiset<T> - Ordered set allowing duplicates\n";
        std::cout << "--------------------------------------------------------\n";

        std::multiset<int> numbers{ 3, 1, 4, 1, 5, 9, 2, 6, 5, 3 };

        Serializer<std::multiset<int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::multiset<int>>::toString( numbers, opts );

        std::cout << "std::multiset<int>:\n" << json << "\n";

        auto restored = Serializer<std::multiset<int>>::fromString( json );

        bool success = ( restored == numbers );
        std::cout << "\nDeserialized (sorted, with duplicates): [ ";
        for( auto it = restored.begin(); it != restored.end(); ++it )
        {
            std::cout << *it;
            if( std::next( it ) != restored.end() )
                std::cout << ", ";
        }
        std::cout << " ]\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Multiset preserves duplicates\n";
        std::cout << "  Note: Sorted order, duplicates allowed\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 12. std::unordered_set - Hash-based unique set
    //=====================================================================
    {
        std::cout << "12. std::unordered_set<T> - Hash-based unique set\n";
        std::cout << "----------------------------------------------------\n";

        std::unordered_set<std::string> tags{ "cpp", "json", "performance", "cpp" }; // Duplicates removed

        Serializer<std::unordered_set<std::string>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::unordered_set<std::string>>::toString( tags, opts );

        std::cout << "std::unordered_set<string>:\n" << json << "\n";

        auto restored = Serializer<std::unordered_set<std::string>>::fromString( json );

        bool success = ( restored == tags );
        std::cout << "\nDeserialized (" << restored.size() << " unique elements)\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Unordered_set removes duplicates\n";
        std::cout << "  Note: Faster than set, no ordering guarantee\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 13. std::unordered_multiset - Hash-based set allowing duplicates
    //=====================================================================
    {
        std::cout << "13. std::unordered_multiset<T> - Hash-based set allowing duplicates\n";
        std::cout << "----------------------------------------------------------------------\n";

        std::unordered_multiset<int> counts{ 1, 2, 2, 3, 3, 3, 4, 4, 4, 4 };

        Serializer<std::unordered_multiset<int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::unordered_multiset<int>>::toString( counts, opts );

        std::cout << "std::unordered_multiset<int>:\n" << json << "\n";

        auto restored = Serializer<std::unordered_multiset<int>>::fromString( json );

        bool success = ( restored == counts );
        std::cout << "\nDeserialized (" << restored.size() << " elements with duplicates)\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Unordered_multiset preserves duplicates\n";
        std::cout << "  Note: Fast, unordered, duplicates allowed\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 14. std::map - Ordered key-value map serialized as JSON object
    //=====================================================================
    {
        std::cout << "14. std::map<K, V> - Ordered key-value map serialized as JSON object\n";
        std::cout << "-----------------------------------------------------------------------\n";

        std::map<std::string, int> scores{ { "Alice", 95 }, { "Bob", 87 }, { "Charlie", 92 } };

        Serializer<std::map<std::string, int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::map<std::string, int>>::toString( scores, opts );

        std::cout << "std::map<string, int>:\n" << json << "\n";

        auto restored = Serializer<std::map<std::string, int>>::fromString( json );

        bool success = ( restored == scores );
        std::cout << "\nDeserialized (" << restored.size() << " key-value pairs):\n";
        for( const auto& [key, val] : restored )
        {
            std::cout << "  " << std::setw( 10 ) << std::left << key << ": " << val << "\n";
        }
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Map serializes as JSON object\n";
        std::cout << "  Format: {\"key1\": value1, \"key2\": value2}\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 15. std::multimap - Ordered map allowing duplicate keys
    //=====================================================================
    {
        std::cout << "15. std::multimap<K, V> - Ordered map allowing duplicate keys\n";
        std::cout << "----------------------------------------------------------------\n";

        std::multimap<std::string, int> grades;
        grades.insert( { "Alice", 95 } );
        grades.insert( { "Alice", 92 } ); // Duplicate key
        grades.insert( { "Bob", 87 } );
        grades.insert( { "Bob", 90 } ); // Duplicate key

        Serializer<std::multimap<std::string, int>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::multimap<std::string, int>>::toString( grades, opts );

        std::cout << "std::multimap<string, int>:\n" << json << "\n";

        auto restored = Serializer<std::multimap<std::string, int>>::fromString( json );

        bool success = ( restored.size() == grades.size() );
        std::cout << "\nDeserialized (" << restored.size() << " key-value pairs):\n";
        for( const auto& [key, val] : restored )
        {
            std::cout << "  " << std::setw( 10 ) << std::left << key << ": " << val << "\n";
        }
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Multimap preserves duplicate keys\n";
        std::cout << "  Format: [{\"key\": K, \"value\": V}, ...]\n";
        std::cout << "  Note: Serialized as array to preserve duplicates\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 16. std::unordered_map - Hash-based key-value map
    //=====================================================================
    {
        std::cout << "16. std::unordered_map<K, V> - Hash-based key-value map\n";
        std::cout << "----------------------------------------------------------\n";

        std::unordered_map<std::string, double> prices{ { "apple", 1.99 }, { "banana", 0.59 }, { "orange", 1.29 } };

        Serializer<std::unordered_map<std::string, double>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::unordered_map<std::string, double>>::toString( prices, opts );

        std::cout << "std::unordered_map<string, double>:\n" << json << "\n";

        auto restored = Serializer<std::unordered_map<std::string, double>>::fromString( json );

        bool success = ( restored == prices );
        std::cout << "\nDeserialized (" << restored.size() << " items):\n";
        for( const auto& [key, val] : restored )
        {
            std::cout << "  " << std::setw( 10 ) << std::left << key << ": $" << std::fixed << std::setprecision( 2 )
                      << val << "\n";
        }
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Unordered_map serializes as JSON object\n";
        std::cout << "  Note: Faster than map, no ordering guarantee\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 17. std::unordered_multimap - Hash-based map allowing duplicate keys
    //=====================================================================
    {
        std::cout << "17. std::unordered_multimap<K, V> - Hash-based map allowing duplicate keys\n";
        std::cout << "-----------------------------------------------------------------------------\n";

        std::unordered_multimap<std::string, std::string> phonebook;
        phonebook.insert( { "Emergency", "911" } );
        phonebook.insert( { "Emergency", "112" } ); // Duplicate key
        phonebook.insert( { "Support", "555-1234" } );

        Serializer<std::unordered_multimap<std::string, std::string>>::Options opts;
        opts.prettyPrint = true;
        std::string json = Serializer<std::unordered_multimap<std::string, std::string>>::toString( phonebook, opts );

        std::cout << "std::unordered_multimap<string, string>:\n" << json << "\n";

        auto restored = Serializer<std::unordered_multimap<std::string, std::string>>::fromString( json );

        bool success = ( restored.size() == phonebook.size() );
        std::cout << "\nDeserialized (" << restored.size() << " entries):\n";
        for( const auto& [key, val] : restored )
        {
            std::cout << "  " << std::setw( 15 ) << std::left << key << ": " << val << "\n";
        }
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Unordered_multimap preserves duplicates\n";
        std::cout << "  Format: [{\"key\": K, \"value\": V}, ...]\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 18. std::unique_ptr - Smart pointer serialized as value or null
    //=====================================================================
    {
        std::cout << "18. std::unique_ptr<T> - Smart pointer serialized as value or null\n";
        std::cout << "---------------------------------------------------------------------\n";

        auto ptrWithValue = std::make_unique<int>( 42 );
        std::unique_ptr<int> ptrNull = nullptr;

        std::string jsonWithValue = Serializer<std::unique_ptr<int>>::toString( ptrWithValue );
        std::string jsonNull = Serializer<std::unique_ptr<int>>::toString( ptrNull );

        std::cout << "unique_ptr with value: " << jsonWithValue << "\n";
        std::cout << "unique_ptr null:       " << jsonNull << "\n";

        auto restoredWithValue = Serializer<std::unique_ptr<int>>::fromString( jsonWithValue );
        auto restoredNull = Serializer<std::unique_ptr<int>>::fromString( jsonNull );

        bool success = restoredWithValue && *restoredWithValue == 42 && !restoredNull;

        std::cout << "\nDeserialized:\n";
        std::cout << "  With value: " << ( restoredWithValue ? std::to_string( *restoredWithValue ) : "null" ) << "\n";
        std::cout << "  Null:       " << ( restoredNull ? std::to_string( *restoredNull ) : "null" ) << "\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Unique_ptr handles ownership transfer\n";
        std::cout << "  Note: nullptr serializes as null\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 19. std::shared_ptr - Shared smart pointer serialized as value or null
    //=====================================================================
    {
        std::cout << "19. std::shared_ptr<T> - Shared smart pointer serialized as value or null\n";
        std::cout << "---------------------------------------------------------------------------\n";

        auto ptrWithValue = std::make_shared<std::string>( "Shared data" );
        std::shared_ptr<std::string> ptrNull = nullptr;

        std::string jsonWithValue = Serializer<std::shared_ptr<std::string>>::toString( ptrWithValue );
        std::string jsonNull = Serializer<std::shared_ptr<std::string>>::toString( ptrNull );

        std::cout << "shared_ptr with value: " << jsonWithValue << "\n";
        std::cout << "shared_ptr null:       " << jsonNull << "\n";

        auto restoredWithValue = Serializer<std::shared_ptr<std::string>>::fromString( jsonWithValue );
        auto restoredNull = Serializer<std::shared_ptr<std::string>>::fromString( jsonNull );

        bool success = restoredWithValue && *restoredWithValue == "Shared data" && !restoredNull;

        std::cout << "\nDeserialized:\n";
        std::cout << "  With value: " << ( restoredWithValue ? *restoredWithValue : "null" ) << "\n";
        std::cout << "  Null:       " << ( restoredNull ? *restoredNull : "null" ) << "\n";
        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Shared_ptr serializes transparently\n";
        std::cout << "  Note: Deserialization creates new shared_ptr (no shared ownership across JSON)\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 20. std::span - Non-owning view (serialization only)
    //=====================================================================
    {
        std::cout << "20. std::span<T, Extent> - Non-owning view (serialization only)\n";
        std::cout << "----------------------------------------------------------------\n";

        // std::span is a view over existing data - cannot deserialize into it
        std::vector<int> data = { 10, 20, 30, 40, 50 };
        std::span<int> fullSpan( data );
        auto subSpan = fullSpan.subspan( 1, 3 ); // [20, 30, 40]

        std::string jsonFull = Serializer<std::span<int>>::toString( fullSpan );
        std::string jsonSub = Serializer<std::span<int>>::toString( subSpan );

        std::cout << "Full span:  " << jsonFull << "\n";
        std::cout << "Subspan:    " << jsonSub << "\n";

        // Deserialize to vector (span cannot own memory)
        auto restoredVector = Serializer<std::vector<int>>::fromString( jsonFull );
        std::span<int> newSpan( restoredVector );

        bool success = ( restoredVector.size() == 5 ) && ( restoredVector[0] == 10 );

        std::cout << "\nDeserialized to vector: [";
        for( size_t i = 0; i < restoredVector.size(); ++i )
        {
            std::cout << restoredVector[i];
            if( i < restoredVector.size() - 1 )
                std::cout << ", ";
        }
        std::cout << "]\n";

        std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Span serializes as array\n";
        std::cout << "  Note: std::span is serialization-only (non-owning view)\n";
        std::cout << "        Deserialize to std::vector then create span if needed\n";
        std::cout << "\n";
    }

    return 0;
}
