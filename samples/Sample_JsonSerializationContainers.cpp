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
 * @file Sample_JsonSerializationContainers.cpp
 * @brief Demonstrates automatic STL container serialization
 * @details This sample shows how nfx-serialization automatically handles standard
 *          C++ containers without custom code:
 *          - Sequential containers: vector, array, list, deque, set
 *          - Associative containers: map, unordered_map
 *          - Smart pointers: unique_ptr, shared_ptr
 *          - Optional types: std::optional
 */

#include <nfx/Serialization.h>

#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

int main()
{
    using namespace nfx::json;
    using namespace nfx::serialization::json;

    std::cout << "=== nfx-serialization STL Containers ===\n";
    std::cout << "=========================================\n\n";

    //=====================================================================
    // 1. Sequential containers: JSON arrays
    //=====================================================================
    {
        std::cout << "1. Sequential containers: JSON arrays\n";
        std::cout << "-----------------------------------------\n";

        // std::vector
        std::vector<int> fibonacci = { 1, 1, 2, 3, 5, 8, 13, 21 };
        Serializer<std::vector<int>>::Options opts1;
        opts1.prettyPrint = true;
        std::string vecJson = Serializer<std::vector<int>>::toString( fibonacci, opts1 );
        std::cout << "std::vector<int>:\n" << vecJson << "\n";

        // std::list
        std::list<std::string> cities = { "Paris", "London", "Tokyo", "New York" };
        Serializer<std::list<std::string>>::Options opts2;
        opts2.prettyPrint = true;
        std::string listJson = Serializer<std::list<std::string>>::toString( cities, opts2 );
        std::cout << "\nstd::list<string>:\n" << listJson << "\n";

        // std::set (sorted, unique)
        std::set<double> temperatures = { 23.5, 18.2, 25.7, 18.2, 21.3 };
        Serializer<std::set<double>>::Options opts3;
        opts3.prettyPrint = true;
        std::string setJson = Serializer<std::set<double>>::toString( temperatures, opts3 );
        std::cout << "\nstd::set<double> (note: duplicates removed, sorted):\n" << setJson << "\n";

        std::cout << "\n  OK: All sequential containers serialize as JSON arrays\n";
        std::cout << "  Note: No custom serialization code needed!\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 2. Associative containers: JSON objects
    //=====================================================================
    {
        std::cout << "2. Associative containers: JSON objects\n";
        std::cout << "-------------------------------------------\n";

        // std::map
        std::map<std::string, int> inventory = {
            { "apples", 50 }, { "oranges", 30 }, { "bananas", 75 }, { "grapes", 20 }
        };
        Serializer<std::map<std::string, int>>::Options opts;
        opts.prettyPrint = true;
        std::string mapJson = Serializer<std::map<std::string, int>>::toString( inventory, opts );
        std::cout << "std::map<string, int>:\n" << mapJson << "\n";

        // std::unordered_map
        std::unordered_map<std::string, double> prices = { { "laptop", 1299.99 },
                                                           { "mouse", 29.99 },
                                                           { "keyboard", 89.99 } };
        Serializer<std::unordered_map<std::string, double>>::Options opts2;
        opts2.prettyPrint = true;
        std::string umapJson = Serializer<std::unordered_map<std::string, double>>::toString( prices, opts2 );
        std::cout << "\nstd::unordered_map<string, double>:\n" << umapJson << "\n";

        std::cout << "\n  OK: Map-like containers serialize as JSON objects\n";
        std::cout << "  Note: Keys must be convertible to string\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 3. std::optional: JSON (null handling)
    //=====================================================================
    {
        std::cout << "3. std::optional: JSON (null handling)\n";
        std::cout << "------------------------------------------\n";

        // With value
        std::optional<std::string> nickname = "Johnny";
        std::string withValueJson = Serializer<std::optional<std::string>>::toString( nickname );
        std::cout << "optional with value: " << withValueJson << "\n";

        // Without value (null)
        std::optional<std::string> middleName = std::nullopt;
        std::string nullJson = Serializer<std::optional<std::string>>::toString( middleName );
        std::cout << "optional without value: " << nullJson << "\n";

        // Roundtrip test
        auto parsedNickname = Serializer<std::optional<std::string>>::fromString( withValueJson );
        auto parsedMiddleName = Serializer<std::optional<std::string>>::fromString( nullJson );

        std::cout << "\nRoundtrip results:\n";
        std::cout << "  Nickname:    " << ( parsedNickname.has_value() ? parsedNickname.value() : "null" ) << "\n";
        std::cout << "  Middle name: " << ( parsedMiddleName.has_value() ? parsedMiddleName.value() : "null" ) << "\n";

        bool optionalSuccess =
            parsedNickname.has_value() && parsedNickname.value() == "Johnny" && !parsedMiddleName.has_value();

        std::cout << "\n  " << ( optionalSuccess ? "OK" : "ERROR" ) << ": std::optional handles null correctly\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 4. Smart pointers: JSON
    //=====================================================================
    {
        std::cout << "4. Smart pointers: JSON\n";
        std::cout << "--------------------------\n";

        // unique_ptr with value
        auto uniqueValue = std::make_unique<int>( 42 );
        std::string uniqueJson = Serializer<std::unique_ptr<int>>::toString( uniqueValue );
        std::cout << "unique_ptr with value: " << uniqueJson << "\n";

        // unique_ptr null
        std::unique_ptr<int> uniqueNull = nullptr;
        std::string uniqueNullJson = Serializer<std::unique_ptr<int>>::toString( uniqueNull );
        std::cout << "unique_ptr null:       " << uniqueNullJson << "\n";

        // shared_ptr with value
        auto sharedValue = std::make_shared<std::string>( "Hello, smart pointers!" );
        std::string sharedJson = Serializer<std::shared_ptr<std::string>>::toString( sharedValue );
        std::cout << "\nshared_ptr with value: " << sharedJson << "\n";

        // shared_ptr null
        std::shared_ptr<std::string> sharedNull = nullptr;
        std::string sharedNullJson = Serializer<std::shared_ptr<std::string>>::toString( sharedNull );
        std::cout << "shared_ptr null:       " << sharedNullJson << "\n";

        std::cout << "\n  OK: Smart pointers serialize transparently\n";
        std::cout << "  Note: Null pointers serialize as JSON null\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 5. Nested containers: vector<vector<T>>
    //=====================================================================
    {
        std::cout << "5. Nested containers: vector<vector<T>>\n";
        std::cout << "------------------------------------------\n";

        // 2D matrix representation
        std::vector<std::vector<int>> matrix = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };

        Serializer<std::vector<std::vector<int>>>::Options opts;
        opts.prettyPrint = true;
        std::string matrixJson = Serializer<std::vector<std::vector<int>>>::toString( matrix, opts );
        std::cout << "2D matrix (vector<vector<int>>):\n" << matrixJson << "\n";

        // Deserialize back
        auto parsedMatrix = Serializer<std::vector<std::vector<int>>>::fromString( matrixJson );

        std::cout << "\nDeserialized matrix:\n";
        for( const auto& row : parsedMatrix )
        {
            std::cout << "  [ ";
            for( size_t i = 0; i < row.size(); ++i )
            {
                std::cout << row[i];
                if( i < row.size() - 1 )
                    std::cout << ", ";
            }
            std::cout << " ]\n";
        }

        bool matrixSuccess = ( parsedMatrix == matrix );

        std::cout << "\n  " << ( matrixSuccess ? "OK" : "ERROR" ) << ": Nested containers roundtrip correctly\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 6. Complex nesting: map<string, vector<T>>
    //=====================================================================
    {
        std::cout << "6. Complex nesting: map<string, vector<T>>\n";
        std::cout << "---------------------------------------------\n";

        // Department employees
        std::map<std::string, std::vector<std::string>> departments = {
            { "Engineering", { "Alice", "Bob", "Charlie" } },
            { "Marketing", { "David", "Eve" } },
            { "Sales", { "Frank", "Grace", "Henry", "Ivy" } }
        };

        Serializer<std::map<std::string, std::vector<std::string>>>::Options opts;
        opts.prettyPrint = true;
        std::string deptJson =
            Serializer<std::map<std::string, std::vector<std::string>>>::toString( departments, opts );
        std::cout << "Departments (map<string, vector<string>>):\n" << deptJson << "\n";

        // Deserialize back
        auto parsedDepts = Serializer<std::map<std::string, std::vector<std::string>>>::fromString( deptJson );

        std::cout << "\nDeserialized departments:\n";
        for( const auto& [dept, employees] : parsedDepts )
        {
            std::cout << "  " << std::setw( 15 ) << std::left << dept << ": " << employees.size() << " employees\n";
        }

        bool deptSuccess = ( parsedDepts == departments );

        std::cout << "\n  " << ( deptSuccess ? "OK" : "ERROR" ) << ": Complex nested structures work seamlessly\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 7. Real-world example: Application metrics
    //=====================================================================
    {
        std::cout << "7. Real-world example: Application metrics\n";
        std::cout << "---------------------------------------------\n";

        // Metrics data structure
        std::map<std::string, std::vector<double>> metrics = { { "response_time_ms", { 42.3, 38.1, 45.7, 41.2, 39.8 } },
                                                               { "cpu_usage_percent",
                                                                 { 23.5, 28.2, 25.1, 27.9, 24.3 } },
                                                               { "memory_mb", { 512.4, 518.7, 515.2, 520.1, 516.8 } } };

        Serializer<std::map<std::string, std::vector<double>>>::Options opts;
        opts.prettyPrint = true;
        std::string metricsJson = Serializer<std::map<std::string, std::vector<double>>>::toString( metrics, opts );
        std::cout << "Application metrics:\n" << metricsJson << "\n";

        // Deserialize and compute statistics
        auto parsedMetrics = Serializer<std::map<std::string, std::vector<double>>>::fromString( metricsJson );

        std::cout << "\nMetrics summary:\n";
        for( const auto& [metric, values] : parsedMetrics )
        {
            double sum = 0.0;
            for( double val : values )
            {
                sum += val;
            }
            double avg = sum / values.size();
            std::cout << "  " << std::setw( 20 ) << std::left << metric << ": avg = " << std::fixed
                      << std::setprecision( 2 ) << avg << " (" << values.size() << " samples)\n";
        }

        std::cout << "\n  OK: Perfect for logs, telemetry, and monitoring data\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 8. JSON → STL: Flexible deserialization
    //=====================================================================
    {
        std::cout << "8. JSON → STL: Flexible deserialization\n";
        std::cout << "------------------------------------------\n";

        // JSON array → vector
        std::string arrayJson = R"([10, 20, 30, 40, 50])";
        auto vec = Serializer<std::vector<int>>::fromString( arrayJson );
        std::cout << "JSON array → vector<int>: [ ";
        for( size_t i = 0; i < vec.size(); ++i )
        {
            std::cout << vec[i];
            if( i < vec.size() - 1 )
                std::cout << ", ";
        }
        std::cout << " ]\n";

        // JSON object → map
        std::string objectJson = R"({"x": 100, "y": 200, "z": 300})";
        auto map = Serializer<std::map<std::string, int>>::fromString( objectJson );
        std::cout << "\nJSON object → map<string, int>:\n";
        for( const auto& [key, val] : map )
        {
            std::cout << "  " << key << " = " << val << "\n";
        }

        // Nested structure
        std::string nestedJson = R"({
			"team_a": [1, 2, 3],
			"team_b": [4, 5],
			"team_c": [6, 7, 8, 9]
		})";
        auto teams = Serializer<std::map<std::string, std::vector<int>>>::fromString( nestedJson );
        std::cout << "\nJSON nested → map<string, vector<int>>:\n";
        for( const auto& [team, members] : teams )
        {
            std::cout << "  " << team << ": " << members.size() << " members\n";
        }

        std::cout << "\n  OK: Automatic type conversion from JSON to STL\n";
        std::cout << "  Note: Type mismatches throw exceptions for safety\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 9. Roundtrip testing: Complex nested structures
    //=====================================================================
    {
        std::cout << "9. Roundtrip testing: Complex nested structures\n";
        std::cout << "--------------------------------------------------\n";

        // Complex data structure
        std::map<std::string, std::map<std::string, std::vector<int>>> complexData = {
            { "region_north", { { "store_1", { 100, 150, 120 } }, { "store_2", { 200, 180, 210 } } } },
            { "region_south", { { "store_3", { 300, 320, 310 } }, { "store_4", { 400, 390, 410 } } } }
        };

        using ComplexType = std::map<std::string, std::map<std::string, std::vector<int>>>;

        // Serialize
        Serializer<ComplexType>::Options opts;
        opts.prettyPrint = true;
        std::string complexJson = Serializer<ComplexType>::toString( complexData, opts );
        std::cout << "Original complex structure:\n" << complexJson << "\n";

        // Deserialize
        auto parsedComplex = Serializer<ComplexType>::fromString( complexJson );

        // Verify
        bool complexSuccess = ( parsedComplex == complexData );

        std::cout << "\nRoundtrip verification:\n";
        std::cout << "  Regions:     " << parsedComplex.size() << "\n";
        std::cout << "  Stores:      "
                  << ( parsedComplex["region_north"].size() + parsedComplex["region_south"].size() ) << "\n";
        std::cout << "  Data points: "
                  << ( parsedComplex["region_north"]["store_1"].size() +
                       parsedComplex["region_north"]["store_2"].size() +
                       parsedComplex["region_south"]["store_3"].size() +
                       parsedComplex["region_south"]["store_4"].size() )
                  << "\n";

        std::cout << "\n  " << ( complexSuccess ? "OK" : "ERROR" ) << ": Deep nesting preserves all data\n";
        std::cout << "\n";
    }

    return 0;
}
