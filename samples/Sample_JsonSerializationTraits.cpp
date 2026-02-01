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
 * @file Sample_JsonSerializationTraits.cpp
 * @brief Demonstrates custom type serialization with SerializationTraits
 * @details This sample shows how to implement SerializationTraits for custom C++ types
 *          using the asymmetric architecture: Builder API for write, Document API for read.
 *          Progressive tutorial from simple POD to complex nested structures.
 */

#include <nfx/Serialization.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main()
{
    using namespace nfx::json;
    using namespace nfx::serialization::json;

    std::cout << "=== nfx-serialization SerializationTraits ===\n\n";

    //=====================================================================
    // 1. SerializationTraits: Simple POD struct
    //=====================================================================
    {
        std::cout << "1. SerializationTraits: Simple POD struct\n";
        std::cout << "-------------------------------------------\n";

        // Define custom type
        struct Point2D
        {
            double x;
            double y;

            bool operator==( const Point2D& other ) const
            {
                return x == other.x && y == other.y;
            }
        };

        // Specialize SerializationTraits for Point2D
        struct SerializationTraits_Point2D
        {
            static void serialize( const Point2D& point, Builder& builder )
            {
                builder.writeStartObject();
                builder.write( "x", point.x );
                builder.write( "y", point.y );
                builder.writeEndObject();
            }

            static void fromDocument( const Document& doc, Point2D& point )
            {
                if( auto val = doc.get<double>( "x" ) )
                    point.x = *val;
                if( auto val = doc.get<double>( "y" ) )
                    point.y = *val;
            }
        };

        Point2D point{ 3.14, 2.71 };

        // Serialize with Builder
        Builder builder( { .indent = 2 } );
        SerializationTraits_Point2D::serialize( point, builder );
        std::string json = builder.toString();

        std::cout << "Serialized Point2D:\n" << json << "\n";

        // Deserialize with Document
        auto maybeDoc = Document::fromString( json );
        if( maybeDoc.has_value() )
        {
            Point2D deserialized{};
            SerializationTraits_Point2D::fromDocument( maybeDoc.value(), deserialized );

            std::cout << "\nDeserialized: (" << deserialized.x << ", " << deserialized.y << ")\n";

            bool success = ( point == deserialized );
            std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Roundtrip preserves data\n";
            std::cout << "  Note: SerializationTraits enables custom type serialization\n";
        }

        std::cout << "\n";
    }

    //=====================================================================
    // 2. SerializationTraits with STL containers
    //=====================================================================
    {
        std::cout << "2. SerializationTraits with STL containers\n";
        std::cout << "---------------------------------------------\n";

        struct Person
        {
            std::string name;
            int age;
            std::vector<std::string> hobbies;

            bool operator==( const Person& other ) const
            {
                return name == other.name && age == other.age && hobbies == other.hobbies;
            }
        };

        struct SerializationTraits_Person
        {
            static void serialize( const Person& person, Builder& builder )
            {
                builder.writeStartObject();
                builder.write( "name", person.name );
                builder.write( "age", person.age );

                builder.writeKey( "hobbies" );
                builder.writeStartArray();
                for( const auto& hobby : person.hobbies )
                {
                    builder.write( hobby );
                }
                builder.writeEndArray();

                builder.writeEndObject();
            }

            static void fromDocument( const Document& doc, Person& person )
            {
                if( auto val = doc.get<std::string>( "name" ) )
                    person.name = *val;
                if( auto val = doc.get<int64_t>( "age" ) )
                    person.age = static_cast<int>( *val );

                if( auto arrOpt = doc.get<Array>( "hobbies" ) )
                {
                    person.hobbies.clear();
                    for( const auto& itemDoc : arrOpt.value() )
                    {
                        if( auto hobby = itemDoc.get<std::string>( "" ) )
                        {
                            person.hobbies.push_back( *hobby );
                        }
                    }
                }
            }
        };

        Person alice{ "Alice Johnson", 30, { "reading", "coding", "hiking" } };

        Builder builder( { .indent = 2 } );
        SerializationTraits_Person::serialize( alice, builder );
        std::string json = builder.toString();

        std::cout << "Serialized Person:\n" << json << "\n";

        auto maybeDoc = Document::fromString( json );
        if( maybeDoc.has_value() )
        {
            Person deserialized{};
            SerializationTraits_Person::fromDocument( maybeDoc.value(), deserialized );

            std::cout << "\nDeserialized Person:\n";
            std::cout << "  Name:    " << deserialized.name << "\n";
            std::cout << "  Age:     " << deserialized.age << "\n";
            std::cout << "  Hobbies: ";
            for( size_t i = 0; i < deserialized.hobbies.size(); ++i )
            {
                std::cout << deserialized.hobbies[i];
                if( i < deserialized.hobbies.size() - 1 )
                    std::cout << ", ";
            }
            std::cout << "\n";

            bool success = ( alice == deserialized );
            std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": STL containers integrate seamlessly\n";
            std::cout << "  Note: Manually iterate arrays in serialize(), use Array in fromDocument()\n";
        }

        std::cout << "\n";
    }

    //=====================================================================
    // 3. Nested custom types: Company with Address and employees
    //=====================================================================
    {
        std::cout << "3. Nested custom types: Company with Address and employees\n";
        std::cout << "-------------------------------------------------------------\n";

        struct Address
        {
            std::string street;
            std::string city;
            std::string zipCode;

            bool operator==( const Address& other ) const
            {
                return street == other.street && city == other.city && zipCode == other.zipCode;
            }
        };

        struct SerializationTraits_Address
        {
            static void serialize( const Address& addr, Builder& builder )
            {
                builder.writeStartObject();
                builder.write( "street", addr.street );
                builder.write( "city", addr.city );
                builder.write( "zipCode", addr.zipCode );
                builder.writeEndObject();
            }

            static void fromDocument( const Document& doc, Address& addr )
            {
                if( auto val = doc.get<std::string>( "street" ) )
                    addr.street = *val;
                if( auto val = doc.get<std::string>( "city" ) )
                    addr.city = *val;
                if( auto val = doc.get<std::string>( "zipCode" ) )
                    addr.zipCode = *val;
            }
        };

        struct Person
        {
            std::string name;
            int age;
            std::vector<std::string> hobbies;
        };

        struct SerializationTraits_Person
        {
            static void serialize( const Person& person, Builder& builder )
            {
                builder.writeStartObject();
                builder.write( "name", person.name );
                builder.write( "age", person.age );
                builder.writeKey( "hobbies" );
                builder.writeStartArray();
                for( const auto& hobby : person.hobbies )
                    builder.write( hobby );
                builder.writeEndArray();
                builder.writeEndObject();
            }

            static void fromDocument( const Document& doc, Person& person )
            {
                if( auto val = doc.get<std::string>( "name" ) )
                    person.name = *val;
                if( auto val = doc.get<int64_t>( "age" ) )
                    person.age = static_cast<int>( *val );
                if( auto arrOpt = doc.get<Array>( "hobbies" ) )
                {
                    person.hobbies.clear();
                    for( const auto& itemDoc : arrOpt.value() )
                    {
                        if( auto hobby = itemDoc.get<std::string>( "" ) )
                            person.hobbies.push_back( *hobby );
                    }
                }
            }
        };

        struct Company
        {
            std::string name;
            Address headquarters;
            std::vector<Person> employees;
            std::map<std::string, int> departments;

            bool operator==( const Company& other ) const
            {
                return name == other.name && headquarters == other.headquarters &&
                       employees.size() == other.employees.size() && departments == other.departments;
            }
        };

        struct SerializationTraits_Company
        {
            static void serialize( const Company& company, Builder& builder )
            {
                builder.writeStartObject();

                builder.write( "name", company.name );

                // Nested custom type - call its SerializationTraits
                builder.writeKey( "headquarters" );
                SerializationTraits_Address::serialize( company.headquarters, builder );

                // Vector of custom types
                builder.writeKey( "employees" );
                builder.writeStartArray();
                for( const auto& employee : company.employees )
                {
                    SerializationTraits_Person::serialize( employee, builder );
                }
                builder.writeEndArray();

                // Map of primitives
                builder.writeKey( "departments" );
                builder.writeStartObject();
                for( const auto& [dept, count] : company.departments )
                {
                    builder.write( dept, count );
                }
                builder.writeEndObject();

                builder.writeEndObject();
            }

            static void fromDocument( const Document& doc, Company& company )
            {
                if( auto val = doc.get<std::string>( "name" ) )
                    company.name = *val;

                // Nested custom type - extract Object and create Document
                if( auto hqOpt = doc.get<Object>( "headquarters" ) )
                {
                    Document hqDoc{ hqOpt.value() };
                    SerializationTraits_Address::fromDocument( hqDoc, company.headquarters );
                }

                // Vector of custom types
                if( auto arrOpt = doc.get<Array>( "employees" ) )
                {
                    company.employees.clear();
                    for( const auto& itemDoc : arrOpt.value() )
                    {
                        Person employee{};
                        SerializationTraits_Person::fromDocument( itemDoc, employee );
                        company.employees.push_back( std::move( employee ) );
                    }
                }

                // Map of primitives
                if( auto mapOpt = doc.get<Object>( "departments" ) )
                {
                    company.departments.clear();
                    for( const auto& [key, valueDoc] : mapOpt.value() )
                    {
                        if( auto count = valueDoc.get<int64_t>( "" ) )
                            company.departments[key] = static_cast<int>( *count );
                    }
                }
            }
        };

        Company company;
        company.name = "Tech Innovations Inc.";
        company.headquarters = { "123 Silicon Valley Blvd", "San Francisco", "94105" };
        company.employees = { { "Alice Johnson", 45, { "leadership", "strategy", "golf" } },
                              { "Bob Wilson", 28, { "coding", "gaming", "coffee" } } };
        company.departments = { { "Engineering", 50 }, { "Marketing", 15 }, { "Sales", 25 } };

        Builder builder( { .indent = 2 } );
        SerializationTraits_Company::serialize( company, builder );
        std::string json = builder.toString();

        std::cout << "Serialized Company:\n" << json << "\n";

        auto maybeDoc = Document::fromString( json );
        if( maybeDoc.has_value() )
        {
            Company deserialized{};
            SerializationTraits_Company::fromDocument( maybeDoc.value(), deserialized );

            std::cout << "\nDeserialized Company:\n";
            std::cout << "  Name:      " << deserialized.name << "\n";
            std::cout << "  HQ:        " << deserialized.headquarters.street << ", " << deserialized.headquarters.city
                      << "\n";
            std::cout << "  Employees: " << deserialized.employees.size() << "\n";
            for( const auto& emp : deserialized.employees )
            {
                std::cout << "    - " << emp.name << " (age " << emp.age << ")\n";
            }
            std::cout << "  Departments: " << deserialized.departments.size() << "\n";
            for( const auto& [dept, count] : deserialized.departments )
            {
                std::cout << "    - " << std::setw( 15 ) << std::left << dept << ": " << count << " people\n";
            }

            bool success = ( company == deserialized );
            std::cout << "\n  " << ( success ? "OK" : "ERROR" ) << ": Complex nesting works\n";
            std::cout << "  Note: Nested types compose by calling their SerializationTraits\n";
        }

        std::cout << "\n";
    }

    return 0;
}
