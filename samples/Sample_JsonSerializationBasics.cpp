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
 * @file Sample_JsonSerializationBasics.cpp
 * @brief Demonstrates fundamental JSON serialization with Builder and Document APIs
 * @details This sample shows the core nfx-json APIs for creating and reading JSON:
 *          - Document API: DOM-based navigation (type-safe random access)
 *          - Builder API: Streaming JSON generation (SAX-like, optimal performance)
 */

#include <nfx/Serialization.h>

#include <iomanip>
#include <iostream>
#include <string>

int main()
{
    using namespace nfx::json;
    using namespace nfx::serialization::json;

    std::cout << "=== nfx-serialization JSON Basics ===\n";
    std::cout << "======================================\n\n";

    //=====================================================================
    // 1. Document API: Parsing and type-safe extraction
    //=====================================================================
    {
        std::cout << "1. Document API: Parsing and type-safe extraction\n";
        std::cout << "----------------------------------------------------\n";

        std::string jsonStr = R"({
			"name": "Bob Wilson",
			"age": 28,
			"salary": 75000.50,
			"active": true,
			"manager": null
		})";

        auto maybeDoc = Document::fromString( jsonStr );
        if( !maybeDoc.has_value() )
        {
            std::cout << "  ERROR: Failed to parse JSON\n";
            return 1;
        }

        Document doc = maybeDoc.value();

        std::cout << "Parsed employee data:\n";
        std::cout << "  Name:    " << doc.get<std::string>( "name" ).value_or( "N/A" ) << "\n";
        std::cout << "  Age:     " << doc.get<int64_t>( "age" ).value_or( 0 ) << "\n";
        std::cout << "  Salary:  $" << std::fixed << std::setprecision( 2 )
                  << doc.get<double>( "salary" ).value_or( 0.0 ) << "\n";
        std::cout << "  Active:  " << std::boolalpha << doc.get<bool>( "active" ).value_or( false ) << "\n";
        std::cout << "  Manager: " << ( doc.isNull( "manager" ) ? "None" : "Assigned" ) << "\n";

        std::cout << "\n  OK: Document provides type-safe field access\n";
        std::cout << "  Note: get<T>() returns std::optional<T> for safety\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 2. Document API: Path navigation with dot notation
    //=====================================================================
    {
        std::cout << "2. Document API: Path navigation with dot notation\n";
        std::cout << "-----------------------------------------------------\n";

        std::string jsonStr = R"({
			"company": {
				"name": "TechCorp",
				"founded": 2010,
				"headquarters": {
					"city": "San Francisco",
					"state": "CA",
					"zipCode": "94105"
				}
			}
		})";

        auto maybeDoc = Document::fromString( jsonStr );
        if( !maybeDoc.has_value() )
        {
            std::cout << "  ERROR: Failed to parse JSON\n";
            return 1;
        }

        Document doc = maybeDoc.value();

        std::cout << "Accessing nested fields with dot notation:\n";
        std::cout << "  Company:    " << doc.get<std::string>( "company.name" ).value_or( "N/A" ) << "\n";
        std::cout << "  Founded:    " << doc.get<int64_t>( "company.founded" ).value_or( 0 ) << "\n";
        std::cout << "  HQ City:    " << doc.get<std::string>( "company.headquarters.city" ).value_or( "N/A" ) << "\n";
        std::cout << "  HQ State:   " << doc.get<std::string>( "company.headquarters.state" ).value_or( "N/A" ) << "\n";
        std::cout << "  HQ ZipCode: " << doc.get<std::string>( "company.headquarters.zipCode" ).value_or( "N/A" )
                  << "\n";

        std::cout << "\n  OK: Dot notation simplifies nested field access\n";
        std::cout << "  Note: No need to manually navigate intermediate objects\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 3. Document API: JSON Pointer navigation (RFC 6901)
    //=====================================================================
    {
        std::cout << "3. Document API: JSON Pointer navigation (RFC 6901)\n";
        std::cout << "------------------------------------------------------\n";

        std::string jsonStr = R"({
			"users": [
				{"id": 1, "name": "Alice", "role": "admin"},
				{"id": 2, "name": "Bob", "role": "user"},
				{"id": 3, "name": "Charlie", "role": "moderator"}
			]
		})";

        auto maybeDoc = Document::fromString( jsonStr );
        if( !maybeDoc.has_value() )
        {
            std::cout << "  ERROR: Failed to parse JSON\n";
            return 1;
        }

        Document doc = maybeDoc.value();

        std::cout << "Accessing array elements with JSON Pointer:\n";
        std::cout << "  User 0: " << doc.get<std::string>( "/users/0/name" ).value_or( "N/A" ) << " ("
                  << doc.get<std::string>( "/users/0/role" ).value_or( "N/A" ) << ")\n";
        std::cout << "  User 1: " << doc.get<std::string>( "/users/1/name" ).value_or( "N/A" ) << " ("
                  << doc.get<std::string>( "/users/1/role" ).value_or( "N/A" ) << ")\n";
        std::cout << "  User 2: " << doc.get<std::string>( "/users/2/name" ).value_or( "N/A" ) << " ("
                  << doc.get<std::string>( "/users/2/role" ).value_or( "N/A" ) << ")\n";

        std::cout << "\n  OK: JSON Pointer provides array indexing\n";
        std::cout << "  Note: Both dot notation and RFC 6901 are supported\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 4. Document API: Iterating over arrays and objects
    //=====================================================================
    {
        std::cout << "4. Document API: Iterating over arrays and objects\n";
        std::cout << "-----------------------------------------------------\n";

        std::string jsonStr = R"({
			"tags": ["cpp", "json", "serialization", "performance"],
			"metrics": {
				"requests": 15420,
				"errors": 3,
				"latency": 42.5
			}
		})";

        auto maybeDoc = Document::fromString( jsonStr );
        if( !maybeDoc.has_value() )
        {
            std::cout << "  ERROR: Failed to parse JSON\n";
            return 1;
        }

        Document doc = maybeDoc.value();

        // Iterate over array
        auto tagsOpt = doc.get<Array>( "tags" );
        if( tagsOpt.has_value() )
        {
            std::cout << "Tags (" << tagsOpt->size() << " items):\n";
            for( const auto& tag : tagsOpt.value() )
            {
                std::cout << "  - " << tag.get<std::string>( "" ).value_or( "N/A" ) << "\n";
            }
        }

        std::cout << "\n";

        // Iterate over object
        auto metricsOpt = doc.get<Object>( "metrics" );
        if( metricsOpt.has_value() )
        {
            std::cout << "Metrics:\n";
            for( const auto& [key, value] : metricsOpt.value() )
            {
                if( value.is<int64_t>( "" ) )
                {
                    std::cout << "  " << std::setw( 12 ) << std::left << key << ": " << value.get<int64_t>( "" ).value()
                              << "\n";
                }
                else if( value.is<double>( "" ) )
                {
                    std::cout << "  " << std::setw( 12 ) << std::left << key << ": " << std::fixed
                              << std::setprecision( 1 ) << value.get<double>( "" ).value() << "ms\n";
                }
            }
        }

        std::cout << "\n  OK: Arrays and Objects support range-based iteration\n";
        std::cout << "  Note: Use is<T>() to check type before extraction\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 5. Builder API: Creating JSON from primitives
    //=====================================================================
    {
        std::cout << "5. Builder API: Creating JSON from primitives\n";
        std::cout << "------------------------------------------------\n";

        // 1.1 Simple values
        Builder builder1( { .indent = 2 } );
        builder1.write( 42 );
        std::cout << "Integer: " << builder1.toString() << "\n";

        Builder builder2( { .indent = 2 } );
        builder2.write( 3.14159 );
        std::cout << "Double:  " << builder2.toString() << "\n";

        Builder builder3( { .indent = 2 } );
        builder3.write( "Hello, nfx-json!" );
        std::cout << "String:  " << builder3.toString() << "\n";

        Builder builder4( { .indent = 2 } );
        builder4.write( true );
        std::cout << "Boolean: " << builder4.toString() << "\n";

        Builder builder5( { .indent = 2 } );
        builder5.write( nullptr );
        std::cout << "Null:    " << builder5.toString() << "\n";

        std::cout << "\n  OK: Builder supports all JSON primitive types\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 6. Builder API: Creating JSON objects
    //=====================================================================
    {
        std::cout << "6. Builder API: Creating JSON objects\n";
        std::cout << "----------------------------------------\n";

        Builder builder( { .indent = 2 } );

        builder.writeStartObject();
        builder.write( "name", "Alice Johnson" );
        builder.write( "age", 30 );
        builder.write( "active", true );
        builder.write( "department", "Engineering" );
        builder.writeEndObject();

        std::string json = builder.toString();
        std::cout << "User object:\n" << json << "\n";

        std::cout << "\n  OK: Builder creates objects with key-value pairs\n";
        std::cout << "  Note: Streaming API - no intermediate DOM needed\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 7. Builder API: Creating JSON arrays
    //=====================================================================
    {
        std::cout << "7. Builder API: Creating JSON arrays\n";
        std::cout << "---------------------------------------\n";

        Builder builder( { .indent = 2 } );

        builder.writeStartArray();
        builder.write( "reading" );
        builder.write( "coding" );
        builder.write( "hiking" );
        builder.write( "photography" );
        builder.writeEndArray();

        std::string json = builder.toString();
        std::cout << "Hobbies array:\n" << json << "\n";

        std::cout << "\n  OK: Builder creates arrays with sequential values\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 8. Builder API: Nested structures
    //=====================================================================
    {
        std::cout << "8. Builder API: Nested structures\n";
        std::cout << "------------------------------------\n";

        Builder builder( { .indent = 2 } );

        builder.writeStartObject();

        // Application metadata
        builder.write( "appName", "MyApp" );
        builder.write( "version", "2.1.0" );

        // Nested server configuration
        builder.writeKey( "server" );
        builder.writeStartObject();
        builder.write( "host", "localhost" );
        builder.write( "port", 8080 );
        builder.write( "ssl", true );
        builder.writeEndObject();

        // Array of allowed origins
        builder.writeKey( "allowedOrigins" );
        builder.writeStartArray();
        builder.write( "https://example.com" );
        builder.write( "https://api.example.com" );
        builder.writeEndArray();

        builder.writeEndObject();

        std::string json = builder.toString();
        std::cout << "Configuration:\n" << json << "\n";

        std::cout << "\n  OK: Builder handles arbitrary nesting depth\n";
        std::cout << "  Performance: Single-pass streaming, minimal memory\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 9. Roundtrip validation: Builder → Document → Builder
    //=====================================================================
    {
        std::cout << "9. Roundtrip validation: Builder → Document → Builder\n";
        std::cout << "--------------------------------------------------------\n";

        // Original data
        Builder builder( { .indent = 2 } );
        builder.writeStartObject();
        builder.write( "product", "Widget Pro" );
        builder.write( "price", 29.99 );
        builder.write( "inStock", true );
        builder.write( "quantity", 150 );
        builder.writeEndObject();

        std::string originalJson = builder.toString();
        std::cout << "Original JSON:\n" << originalJson << "\n";

        // Parse back
        auto maybeDoc = Document::fromString( originalJson );
        if( !maybeDoc.has_value() )
        {
            std::cout << "  ERROR: Failed to parse roundtrip JSON\n";
            return 1;
        }

        Document doc = maybeDoc.value();

        // Verify values
        bool roundtripSuccess = true;
        roundtripSuccess &= ( doc.get<std::string>( "product" ).value() == "Widget Pro" );
        roundtripSuccess &= ( std::abs( doc.get<double>( "price" ).value() - 29.99 ) < 0.001 );
        roundtripSuccess &= ( doc.get<bool>( "inStock" ).value() == true );
        roundtripSuccess &= ( doc.get<int64_t>( "quantity" ).value() == 150 );

        std::cout << "\nRoundtrip validation:\n";
        std::cout << "  Product:  " << doc.get<std::string>( "product" ).value_or( "FAIL" ) << "\n";
        std::cout << "  Price:    $" << std::fixed << std::setprecision( 2 )
                  << doc.get<double>( "price" ).value_or( 0.0 ) << "\n";
        std::cout << "  In Stock: " << std::boolalpha << doc.get<bool>( "inStock" ).value_or( false ) << "\n";
        std::cout << "  Quantity: " << doc.get<int64_t>( "quantity" ).value_or( 0 ) << "\n";

        std::cout << "\n  " << ( roundtripSuccess ? "OK" : "ERROR" ) << ": Roundtrip preserves all data and types\n";
        std::cout << "\n";
    }

    //=====================================================================
    // 10. Serializer API: High-level convenience methods
    //=====================================================================
    {
        std::cout << "10. Serializer API: High-level convenience methods\n";
        std::cout << "-----------------------------------------------------\n";

        // Serialize primitive types
        int age = 35;
        std::string ageJson = Serializer<int>::toString( age );
        std::cout << "Serialized int:    " << ageJson << "\n";

        double temperature = 36.6;
        Serializer<double>::Options opts;
        opts.prettyPrint = false;
        std::string tempJson = Serializer<double>::toString( temperature, opts );
        std::cout << "Serialized double: " << tempJson << "\n";

        std::string name = "David Chen";
        std::string nameJson = Serializer<std::string>::toString( name );
        std::cout << "Serialized string: " << nameJson << "\n";

        // Deserialize back
        int parsedAge = Serializer<int>::fromString( ageJson );
        double parsedTemp = Serializer<double>::fromString( tempJson );
        std::string parsedName = Serializer<std::string>::fromString( nameJson );

        std::cout << "\nDeserialized values:\n";
        std::cout << "  Age:         " << parsedAge << "\n";
        std::cout << "  Temperature: " << std::fixed << std::setprecision( 1 ) << parsedTemp << "°C\n";
        std::cout << "  Name:        " << parsedName << "\n";

        bool serializerSuccess = ( parsedAge == age && parsedTemp == temperature && parsedName == name );

        std::cout << "\n  " << ( serializerSuccess ? "OK" : "ERROR" )
                  << ": Serializer provides convenient one-liners\n";
        std::cout << "  Note: For custom types, use SerializationTraits\n";
        std::cout << "        → See Sample_JsonSerializationTraits.cpp\n";
        std::cout << "\n";
    }

    return 0;
}
