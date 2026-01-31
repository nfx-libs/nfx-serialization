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
 * @file Sample_JsonSerializerWithBuilder.cpp
 * @brief Demonstrates using nfx-json Builder API for high-performance serialization
 * @details Shows how to leverage the Builder API from nfx-json for faster JSON generation
 *          compared to the Document-based approach, with 30-150% performance improvements.
 */

#include <nfx/Serialization.h>
#include <nfx/json/Builder.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <map>

using namespace nfx::serialization::json;
using namespace nfx::json;

// Simple timer utility
class Timer
{
public:
    explicit Timer( const std::string& name )
        : m_name{ name },
          m_start{ std::chrono::high_resolution_clock::now() }
    {
    }

    ~Timer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( end - m_start );
        std::cout << "  " << m_name << ": " << duration.count() << " μs\n";
    }

private:
    std::string m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

int main()
{
    std::cout << "=== JSON Serialization: Builder API vs Document API ===\n\n";

    //=====================================================================
    // 1. Basic Example: Serialize a vector with Builder API
    //=====================================================================
    {
        std::cout << "1. Basic Vector Serialization\n";
        std::cout << "------------------------------\n";

        std::vector<int> numbers = { 10, 20, 30, 40, 50 };

        // Traditional approach using Document
        std::cout << "Document API:\n";
        {
            Timer timer( "Document serialization" );
            std::string json = Serializer<std::vector<int>>::toString( numbers );
            std::cout << "  Result: " << json << "\n";
        }

        // New approach using Builder API directly
        std::cout << "\nBuilder API:\n";
        {
            Timer timer( "Builder serialization" );
            Builder builder;
            builder.writeStartArray();
            for( int num : numbers )
            {
                builder.write( num );
            }
            builder.writeEndArray();
            std::string json = builder.toString();
            std::cout << "  Result: " << json << "\n";
        }
    }

    std::cout << "\n";

    //=====================================================================
    // 2. Map Serialization
    //=====================================================================
    {
        std::cout << "2. Map Serialization\n";
        std::cout << "--------------------\n";

        std::map<std::string, int> scores;
        scores["Alice"] = 95;
        scores["Bob"] = 87;
        scores["Charlie"] = 92;

        // Document API
        std::cout << "Document API:\n";
        {
            Timer timer( "Document serialization" );
            std::string json = Serializer<std::map<std::string, int>>::toString( scores );
            std::cout << "  Result: " << json << "\n";
        }

        // Builder API
        std::cout << "\nBuilder API:\n";
        {
            Timer timer( "Builder serialization" );
            Builder builder;
            builder.writeStartObject();
            for( const auto& [key, value] : scores )
            {
                builder.write( key, value );
            }
            builder.writeEndObject();
            std::string json = builder.toString();
            std::cout << "  Result: " << json << "\n";
        }
    }

    std::cout << "\n";

    //=====================================================================
    // 3. Complex Nested Structure
    //=====================================================================
    {
        std::cout << "3. Complex Nested Structure\n";
        std::cout << "----------------------------\n";

        // Create test data
        struct Person
        {
            std::string name;
            int age;
            std::vector<std::string> hobbies;
        };

        std::vector<Person> people = { { "Alice", 30, { "reading", "coding", "gaming" } },
                                       { "Bob", 25, { "sports", "music" } },
                                       { "Charlie", 35, { "cooking", "travel", "photography" } } };

        // Builder API for complex structure
        std::cout << "Builder API (pretty-print):\n";
        {
            Timer timer( "Builder serialization" );
            Builder builder( { .indent = 2 } ); // 2-space indent

            builder.writeStartArray();
            for( const auto& person : people )
            {
                builder.writeStartObject()
                    .write( "name", person.name )
                    .write( "age", person.age )
                    .writeKey( "hobbies" )
                    .writeStartArray();

                for( const auto& hobby : person.hobbies )
                {
                    builder.write( hobby );
                }

                builder.writeEndArray().writeEndObject();
            }
            builder.writeEndArray();

            std::string json = builder.toString();
            std::cout << json << "\n";
        }
    }

    std::cout << "\n";

    //=====================================================================
    // 4. Performance Comparison: Large Dataset
    //=====================================================================
    {
        std::cout << "4. Performance Comparison: Large Dataset\n";
        std::cout << "-----------------------------------------\n";

        // Generate large dataset
        std::vector<int> largeData( 10000 );
        for( size_t i = 0; i < largeData.size(); ++i )
        {
            largeData[i] = static_cast<int>( i );
        }

        std::string docJson, builderJson;

        std::cout << "Serializing " << largeData.size() << " integers...\n\n";

        // Document API
        std::cout << "Document API:\n";
        {
            Timer timer( "Document serialization" );
            docJson = Serializer<std::vector<int>>::toString( largeData );
        }

        // Builder API
        std::cout << "\nBuilder API:\n";
        {
            Timer timer( "Builder serialization" );
            Builder builder;
            builder.writeStartArray();
            for( int num : largeData )
            {
                builder.write( num );
            }
            builder.writeEndArray();
            builderJson = builder.toString();
        }

        // Verify both produce same result
        std::cout << "\nVerification:\n";
        std::cout << "  Document JSON length: " << docJson.length() << " bytes\n";
        std::cout << "  Builder JSON length:  " << builderJson.length() << " bytes\n";
        std::cout << "  Results match: " << ( docJson == builderJson ? "YES" : "NO" ) << "\n";
    }

    std::cout << "\n";

    return 0;
}
