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
 * @file Sample_JsonSerializer.cpp
 * @brief Sample demonstrating JSON Serializer functionality
 * @details Real-world examples showcasing all serializer features including primitive types,
 *          STL containers, custom objects, and serialization options.
 */

#include <nfx/Serialization.h>

#include <array>
#include <iostream>
#include <deque>
#include <map>
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace nfx::json;

using namespace nfx::serialization::json;

//=====================================================================
// Custom data structures for demonstration
//=====================================================================

// Simple Date/Time class for demonstration (using std::chrono internally)
struct DateTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

    DateTime()
        : year( 2024 ),
          month( 1 ),
          day( 1 ),
          hour( 0 ),
          minute( 0 ),
          second( 0 )
    {
    }

    DateTime( int y, int m, int d, int h = 0, int min = 0, int s = 0 )
        : year( y ),
          month( m ),
          day( d ),
          hour( h ),
          minute( min ),
          second( s )
    {
    }

    std::string toIso8601() const
    {
        std::ostringstream oss;
        oss << std::setfill( '0' ) << std::setw( 4 ) << year << "-" << std::setw( 2 ) << month << "-" << std::setw( 2 )
            << day << "T" << std::setw( 2 ) << hour << ":" << std::setw( 2 ) << minute << ":" << std::setw( 2 )
            << second << "Z";
        return oss.str();
    }

    static bool fromString( const std::string& str, DateTime& dt )
    {
        // Simple ISO8601 parser: YYYY-MM-DDTHH:MM:SSZ
        if( str.length() < 19 )
            return false;
        try
        {
            dt.year = std::stoi( str.substr( 0, 4 ) );
            dt.month = std::stoi( str.substr( 5, 2 ) );
            dt.day = std::stoi( str.substr( 8, 2 ) );
            dt.hour = std::stoi( str.substr( 11, 2 ) );
            dt.minute = std::stoi( str.substr( 14, 2 ) );
            dt.second = std::stoi( str.substr( 17, 2 ) );
            return true;
        }
        catch( ... )
        {
            return false;
        }
    }

    bool operator==( const DateTime& other ) const
    {
        return year == other.year && month == other.month && day == other.day && hour == other.hour &&
               minute == other.minute && second == other.second;
    }
};

struct Person
{
    std::string name;
    int age;
    bool isActive;
    std::optional<std::string> email;
    std::vector<std::string> hobbies;

    // Equality operator for comparison
    bool operator==( const Person& other ) const
    {
        return name == other.name && age == other.age && isActive == other.isActive && email == other.email &&
               hobbies == other.hobbies;
    }

    // Custom serialization method - no parameters
    Document serialize() const
    {
        Document doc;
        doc.set<std::string>( "/name", name );
        doc.set<int64_t>( "/age", age );
        doc.set<bool>( "/isActive", isActive );

        if( email.has_value() )
        {
            doc.set<std::string>( "/email", *email );
        }
        // Note: includeNullFields not available without serializer parameter

        // Serialize hobbies array
        if( !hobbies.empty() )
        {
            doc.set<Array>( "/hobbies" );
            for( const auto& hobby : hobbies )
            {
                auto hobbiesArrayRef = doc.get<Array>( "/hobbies" );
                if( hobbiesArrayRef.has_value() )
                {
                    hobbiesArrayRef->push_back( Document( hobby ) );
                    doc.set<Array>( "/hobbies", hobbiesArrayRef.value() );
                }
            }
        }

        return doc;
    }

    // Serialization with dependency injection
    void serialize( Serializer<Person>& serializer, Document& doc ) const
    {
        doc.set<std::string>( "/name", name );
        doc.set<int64_t>( "/age", age );
        doc.set<bool>( "/isActive", isActive );

        if( email.has_value() )
        {
            doc.set<std::string>( "/email", *email );
        }
        else if( serializer.options().includeNullFields )
        {
            doc.set<std::string>( "/email", "" );
        }

        // Serialize hobbies array
        if( !hobbies.empty() )
        {
            doc.set<Array>( "/hobbies" );
            for( const auto& hobby : hobbies )
            {
                auto hobbiesArrayRef = doc.get<Array>( "/hobbies" );
                if( hobbiesArrayRef.has_value() )
                {
                    hobbiesArrayRef->push_back( Document( hobby ) );
                    doc.set<Array>( "/hobbies", hobbiesArrayRef.value() );
                }
            }
        }
    }

    // Custom deserialization method
    void deserialize( const Serializer<Person>& serializer, const Document& doc )
    {
        if( auto nameVal = doc.get<std::string>( "/name" ) )
        {
            name = *nameVal;
        }
        if( auto ageVal = doc.get<int64_t>( "/age" ) )
        {
            age = static_cast<int>( *ageVal );
        }
        if( auto activeVal = doc.get<bool>( "/isActive" ) )
        {
            isActive = *activeVal;
        }

        // Handle optional email
        if( auto emailVal = doc.get<std::string>( "/email" ) )
        {
            if( !emailVal->empty() )
            {
                email = *emailVal;
            }
            else
            {
                email = std::nullopt;
            }
        }
        else
        {
            email = std::nullopt;
        }

        // Deserialize hobbies array manually since we have custom deserialization
        hobbies.clear();
        if( auto hobbiesDoc = doc.get<Document>( "/hobbies" ) )
        {
            if( hobbiesDoc->is<Array>( "" ) )
            {
                auto arrOpt = hobbiesDoc->get<Array>( "" );
                if( arrOpt.has_value() )
                {
                    for( const auto& hobbyDoc : arrOpt.value() )
                    {
                        if( auto hobbyStr = hobbyDoc.get<std::string>( "" ) )
                        {
                            hobbies.push_back( *hobbyStr );
                        }
                    }
                }
            }
        } // Validation if enabled
        if( serializer.options().validateOnDeserialize )
        {
            if( age < 0 || age > 150 )
            {
                throw std::runtime_error{ "Invalid age: must be between 0 and 150" };
            }
        }
    }
};

struct Company
{
    std::string name;
    std::vector<Person> employees;
    std::map<std::string, int> departments;
    DateTime founded;

    bool operator==( const Company& other ) const
    {
        return name == other.name && employees == other.employees && departments == other.departments &&
               founded == other.founded;
    }

    // Custom serialization method - no parameters (simple case)
    Document serialize() const
    {
        Document doc;
        doc.set<std::string>( "/name", name );

        // Serialize founded date
        std::string foundedStr = founded.toIso8601();
        doc.set<std::string>( "/founded", foundedStr );

        // Serialize employees array using default serialization
        if( !employees.empty() )
        {
            doc.set<Array>( "/employees" );
            for( const auto& employee : employees )
            {
                // Use the no-parameter serialize method for simplicity
                Document employeeDoc = employee.serialize();
                auto employeesArrayWrapper = doc.get<Array>( "/employees" );
                if( employeesArrayWrapper.has_value() )
                {
                    employeesArrayWrapper->push_back( employeeDoc );
                    doc.set<Array>( "/employees", employeesArrayWrapper.value() );
                }
            }
        }

        // Serialize departments map
        if( !departments.empty() )
        {
            Document departmentsObj;

            for( const auto& [deptName, count] : departments )
            {
                std::string fieldPath = "/" + deptName;
                departmentsObj.set<int64_t>( fieldPath, count );
            }
            doc.set<Document>( "/departments", departmentsObj );
        }

        return doc;
    }

    // Custom serialization method - with serializer (for options and cross-type serialization)
    Document serialize( Serializer<Company>& companySerializer ) const
    {
        Document doc;
        doc.set<std::string>( "/name", name );

        // Serialize founded date
        std::string foundedStr = founded.toIso8601();
        doc.set<std::string>( "/founded", foundedStr );

        // Serialize employees array
        if( !employees.empty() )
        {
            doc.set<Array>( "/employees" );
            for( const auto& employee : employees )
            {
                // Create Person serializer with same options as Company serializer
                auto personOptions = Serializer<Person>::Options::createFrom<Company>( companySerializer.options() );
                Serializer<Person> personSerializer( personOptions );
                Document employeeDoc = personSerializer.serialize( employee );
                auto employeesArrayWrapper = doc.get<Array>( "/employees" );
                if( employeesArrayWrapper.has_value() )
                {
                    employeesArrayWrapper->push_back( employeeDoc );
                    doc.set<Array>( "/employees", employeesArrayWrapper.value() );
                }
            }
        }

        // Serialize departments map
        if( !departments.empty() )
        {
            Document departmentsObj;

            for( const auto& [deptName, count] : departments )
            {
                std::string fieldPath = "/" + deptName;
                departmentsObj.set<int64_t>( fieldPath, count );
            }
            doc.set<Document>( "/departments", departmentsObj );
        }

        return doc;
    }

    // Alternative serialization method - void with document parameter (different API style)
    void serialize( Serializer<Company>& companySerializer, Document& doc ) const
    {
        doc = serialize( companySerializer ); // Delegate to the Document-returning version
    }

    // Custom deserialization method
    void deserialize( const Serializer<Company>& serializer, const Document& doc )
    {
        if( auto nameVal = doc.get<std::string>( "/name" ) )
        {
            name = *nameVal;
        }

        // Deserialize founded date
        if( auto foundedVal = doc.get<std::string>( "/founded" ) )
        {
            if( !DateTime::fromString( *foundedVal, founded ) )
            {
                throw std::runtime_error{ "Invalid DateTime format in Company::founded" };
            }
        }

        // Deserialize employees array
        employees.clear();
        if( auto employeesArray = doc.get<Document>( "/employees" ) )
        {
            if( employeesArray->is<Array>( "" ) )
            {
                auto arrOpt = employeesArray->get<Array>( "" );
                if( arrOpt.has_value() )
                {
                    for( const auto& employeeDoc : arrOpt.value() )
                    {
                        // Create Person serializer with same options as Company serializer
                        Serializer<Person>::Options personOptions;
                        personOptions.includeNullFields = serializer.options().includeNullFields;
                        personOptions.prettyPrint = serializer.options().prettyPrint;
                        personOptions.validateOnDeserialize = serializer.options().validateOnDeserialize;

                        Serializer<Person> personSerializer( personOptions );
                        Person employee = personSerializer.deserialize( employeeDoc );
                        employees.push_back( std::move( employee ) );
                    }
                }
            }
        }

        // Deserialize departments map
        departments.clear();
        if( auto departmentsObj = doc.get<Document>( "/departments" ) )
        {
            if( departmentsObj->is<Object>( "" ) )
            {
                auto objOpt = departmentsObj->get<Object>( "" );
                if( objOpt.has_value() )
                {
                    for( const auto& [deptName, valueDoc] : objOpt.value() )
                    {
                        if( auto countVal = valueDoc.get<int64_t>( "" ) )
                        {
                            departments[deptName] = static_cast<int>( *countVal );
                        }
                    }
                }
            }
        }
    }
};

//=====================================================================
// Demonstration functions
//=====================================================================

void demonstratePrimitiveTypes()
{
    std::cout << "=== Primitive Types Serialization ===" << std::endl;

    // Boolean
    bool flag = true;
    std::string json = Serializer<bool>::toString( flag );
    std::cout << "Boolean: " << json << std::endl;
    bool deserializedFlag = Serializer<bool>::fromString( json );
    std::cout << "Roundtrip successful: " << ( flag == deserializedFlag ? "YES" : "NO" ) << std::endl;

    // Integer
    int number = 42;
    json = Serializer<int>::toString( number );
    std::cout << "Integer: " << json << std::endl;

    // Floating point
    double pi = 3.14159;
    json = Serializer<double>::toString( pi );
    std::cout << "Double: " << json << std::endl;

    // String
    std::string text = "Hello, JSON Serializer!";
    json = Serializer<std::string>::toString( text );
    std::cout << "String: " << json << std::endl;

    std::cout << std::endl;
}

void demonstrateContainers()
{
    std::cout << "=== STL Containers Serialization ===" << std::endl;

    // Vector
    std::vector<int> numbers{ 1, 2, 3, 4, 5 };
    std::string json = Serializer<std::vector<int>>::toString( numbers );
    std::cout << "Vector<int>: " << json << std::endl;
    auto deserializedNumbers = Serializer<std::vector<int>>::fromString( json );
    std::cout << "Roundtrip successful: " << ( numbers == deserializedNumbers ? "YES" : "NO" ) << std::endl;

    // Map
    std::map<std::string, int> scores{ { "Alice", 95 }, { "Bob", 87 }, { "Charlie", 92 } };
    json = Serializer<std::map<std::string, int>>::toString( scores );
    std::cout << "Map<string,int>: " << json << std::endl;

    // Set
    std::set<std::string> tags{ "cpp", "json", "serialization" };
    json = Serializer<std::set<std::string>>::toString( tags );
    std::cout << "Set<string>: " << json << std::endl;

    // Array
    std::array<double, 3> coordinates{ 1.0, 2.5, 3.7 };
    json = Serializer<std::array<double, 3>>::toString( coordinates );
    std::cout << "Array<double,3>: " << json << std::endl;

    // List
    std::list<std::string> languages{ "C++", "Python", "JavaScript" };
    json = Serializer<std::list<std::string>>::toString( languages );
    std::cout << "List<string>: " << json << std::endl;

    // Deque
    std::deque<int> buffer{ 100, 200, 300 };
    json = Serializer<std::deque<int>>::toString( buffer );
    std::cout << "Deque<int>: " << json << std::endl;

    // Unordered map
    std::unordered_map<std::string, std::string> config{ { "theme", "dark" },
                                                         { "language", "en" },
                                                         { "timezone", "UTC" } };
    json = Serializer<std::unordered_map<std::string, std::string>>::toString( config );
    std::cout << "UnorderedMap<string,string>: " << json << std::endl;

    // Unordered set
    std::unordered_set<int> uniqueIds{ 42, 17, 99, 3 };
    json = Serializer<std::unordered_set<int>>::toString( uniqueIds );
    std::cout << "UnorderedSet<int>: " << json << std::endl;

    std::cout << std::endl;
}

void demonstrateOptionalAndSmartPointers()
{
    std::cout << "=== Optional and Smart Pointers ===" << std::endl;

    // Optional with value
    std::optional<int> optValue{ 42 };
    std::string json = Serializer<std::optional<int>>::toString( optValue );
    std::cout << "Optional<int> with value: " << json << std::endl;

    // Optional without value
    std::optional<int> optEmpty;
    json = Serializer<std::optional<int>>::toString( optEmpty );
    std::cout << "Optional<int> empty: " << json << std::endl;

    // Unique pointer
    auto uniquePtr = std::make_unique<std::string>( "Unique pointer content" );
    json = Serializer<std::unique_ptr<std::string>>::toString( uniquePtr );
    std::cout << "Unique_ptr<string>: " << json << std::endl;

    // Shared pointer
    auto sharedPtr = std::make_shared<int>( 123 );
    json = Serializer<std::shared_ptr<int>>::toString( sharedPtr );
    std::cout << "Shared_ptr<int>: " << json << std::endl;

    std::cout << std::endl;
}

void demonstrateCustomObjects()
{
    std::cout << "=== Custom Objects with Custom Serialization ===" << std::endl;

    // Create a person
    Person person;
    person.name = "John Doe";
    person.age = 30;
    person.isActive = true;
    person.email = "john.doe@example.com";
    person.hobbies = { "reading", "coding", "hiking" };

    // Serialize with default options
    std::string json = Serializer<Person>::toString( person );
    std::cout << "Person (default): " << json << std::endl;

    // Serialize with pretty printing
    Serializer<Person>::Options prettyOptions;
    prettyOptions.prettyPrint = true;
    json = Serializer<Person>::toString( person, prettyOptions );
    std::cout << "Person (pretty): " << json << std::endl;

    // Test roundtrip
    Person deserializedPerson = Serializer<Person>::fromString( json );
    std::cout << "Roundtrip successful: " << ( person == deserializedPerson ? "YES" : "NO" ) << std::endl;

    // Person without email
    Person personNoEmail;
    personNoEmail.name = "Jane Smith";
    personNoEmail.age = 25;
    personNoEmail.isActive = false;
    personNoEmail.hobbies = { "music", "art" };

    // Serialize with includeNullFields option
    Serializer<Person>::Options nullOptions;
    nullOptions.includeNullFields = true;
    nullOptions.prettyPrint = true;
    json = Serializer<Person>::toString( personNoEmail, nullOptions );
    std::cout << "Person without email (includeNullFields): " << json << std::endl;

    std::cout << std::endl;
}

void demonstrateComplexNesting()
{
    std::cout << "=== Complex Nested Structures ===" << std::endl;

    // Create a company with employees
    Company company;
    company.name = "Tech Innovations Inc.";
    company.founded = DateTime{ 2010, 3, 15 };

    // Add employees
    Person ceo;
    ceo.name = "Alice Johnson";
    ceo.age = 45;
    ceo.isActive = true;
    ceo.email = "alice@techinnovations.com";
    ceo.hobbies = { "leadership", "strategy" };

    Person developer;
    developer.name = "Bob Wilson";
    developer.age = 28;
    developer.isActive = true;
    developer.email = "bob@techinnovations.com";
    developer.hobbies = { "coding", "gaming", "coffee" };

    company.employees = { ceo, developer };
    company.departments = { { "Engineering", 50 }, { "Sales", 25 }, { "Marketing", 15 } };

    // Serialize the entire company
    Serializer<Company>::Options options;
    options.prettyPrint = true;
    std::string json = Serializer<Company>::toString( company, options );
    std::cout << "Complete Company Structure:" << std::endl << json << std::endl;

    // Test roundtrip
    Company deserializedCompany = Serializer<Company>::fromString( json );
    std::cout << "Complex roundtrip successful: " << ( company == deserializedCompany ? "YES" : "NO" ) << std::endl;

    std::cout << std::endl;
}

void demonstrateValidation()
{
    std::cout << "=== Validation and Error Handling ===" << std::endl;

    // Create JSON with invalid age
    std::string invalidJson = R"({
		"name": "Invalid Person",
		"age": 200,
		"isActive": true
	})";

    try
    {
        Serializer<Person>::Options validationOptions;
        validationOptions.validateOnDeserialize = true;

        Person person = Serializer<Person>::fromString( invalidJson, validationOptions );
        std::cout << "Validation failed - should not reach here!" << std::endl;
    }
    catch( const std::exception& e )
    {
        std::cout << "Validation caught error (as expected): " << e.what() << std::endl;
    }

    // Test with validation disabled
    try
    {
        Serializer<Person>::Options noValidationOptions;
        noValidationOptions.validateOnDeserialize = false;

        Person person = Serializer<Person>::fromString( invalidJson, noValidationOptions );
        std::cout << "Without validation - deserialized person with age: " << person.age << std::endl;
    }
    catch( const std::exception& e )
    {
        std::cout << "Unexpected error: " << e.what() << std::endl;
    }

    std::cout << std::endl;
}

void demonstrateSerializerClass()
{
    std::cout << "=== Serializer Class Usage ===" << std::endl;

    // Using Serializer class directly
    Serializer<std::vector<int>> vectorSerializer;

    std::vector<int> data{ 10, 20, 30, 40, 50 };

    // Serialize to document
    Document document = vectorSerializer.serialize( data );
    std::cout << "Document serialization: " << document.toString() << std::endl;

    // Serialize to string
    std::string jsonString = Serializer<std::vector<int>>::toString( data );
    std::cout << "String serialization: " << jsonString << std::endl;

    // Deserialize from document
    std::vector<int> fromDoc = vectorSerializer.deserialize( document );
    std::cout << "From document - equal: " << ( data == fromDoc ? "YES" : "NO" ) << std::endl;

    // Deserialize from string
    std::vector<int> fromString = Serializer<std::vector<int>>::fromString( jsonString );
    std::cout << "From string - equal: " << ( data == fromString ? "YES" : "NO" ) << std::endl;

    // Using serializer with options
    Serializer<std::map<std::string, int>>::Options options;
    options.prettyPrint = true;

    std::map<std::string, int> mapData{ { "first", 1 }, { "second", 2 }, { "third", 3 } };
    std::string prettyJson = Serializer<std::map<std::string, int>>::toString( mapData, options );
    std::cout << "Pretty printed map:" << std::endl << prettyJson << std::endl;

    std::cout << std::endl;
}

//=====================================================================
// Main function
//=====================================================================

int main()
{
    std::cout << "=== NFX JSON Serializer - Feature Demonstration ===\n\n";

    try
    {
        //=====================================================================
        // 1. Primitive Types Serialization
        //=====================================================================
        {
            std::cout << "1. Primitive Types Serialization\n";
            std::cout << "---------------------------------\n";

            bool flag{ true };
            std::string json{ Serializer<bool>::toString( flag ) };
            std::cout << "Boolean: " << json << "\n";

            int number{ 42 };
            json = Serializer<int>::toString( number );
            std::cout << "Integer: " << json << "\n";

            double pi{ 3.14159 };
            json = Serializer<double>::toString( pi );
            std::cout << "Double: " << json << "\n";

            std::string text{ "Hello, JSON!" };
            json = Serializer<std::string>::toString( text );
            std::cout << "String: " << json << "\n";
        }

        std::cout << "\n";

        //=====================================================================
        // 2. Container Serialization
        //=====================================================================
        {
            std::cout << "2. Container Serialization\n";
            std::cout << "--------------------------\n";

            std::vector<int> numbers{ 1, 2, 3, 4, 5 };
            std::string json{ Serializer<std::vector<int>>::toString( numbers ) };
            std::cout << "Vector: " << json << "\n";

            std::set<std::string> tags{ "cpp", "json", "serialization" };
            json = Serializer<std::set<std::string>>::toString( tags );
            std::cout << "Set: " << json << "\n";

            std::map<std::string, int> scores{ { "Alice", 95 }, { "Bob", 87 }, { "Charlie", 92 } };
            json = Serializer<std::map<std::string, int>>::toString( scores );
            std::cout << "Map: " << json << "\n";

            std::unordered_map<std::string, std::string> config{ { "host", "localhost" },
                                                                 { "port", "8080" },
                                                                 { "protocol", "https" } };
            json = Serializer<std::unordered_map<std::string, std::string>>::toString( config );
            std::cout << "Unordered Map: " << json << "\n";
        }

        std::cout << "\n";

        //=====================================================================
        // 3. Optional and Smart Pointers
        //=====================================================================
        {
            std::cout << "3. Optional and Smart Pointers\n";
            std::cout << "------------------------------\n";

            std::optional<int> hasValue{ 42 };
            std::string json{ Serializer<std::optional<int>>::toString( hasValue ) };
            std::cout << "Optional with value: " << json << "\n";

            std::optional<int> noValue{};
            json = Serializer<std::optional<int>>::toString( noValue );
            std::cout << "Optional without value: " << json << "\n";

            auto ptr{ std::make_shared<std::string>( "Shared pointer content" ) };
            json = Serializer<std::shared_ptr<std::string>>::toString( ptr );
            std::cout << "Shared pointer: " << json << "\n";

            auto uptr{ std::make_unique<int>( 999 ) };
            json = Serializer<std::unique_ptr<int>>::toString( uptr );
            std::cout << "Unique pointer: " << json << "\n";
        }

        std::cout << "\n";

        //=====================================================================
        // 4. Custom Objects
        //=====================================================================
        {
            std::cout << "4. Custom Objects\n";
            std::cout << "-----------------\n";

            Person person{};
            person.name = "Alice Johnson";
            person.age = 30;
            person.isActive = true;
            person.email = "alice@example.com";
            person.hobbies = { "reading", "coding", "hiking" };

            Serializer<Person>::Options options;
            options.prettyPrint = true;

            std::string json{ Serializer<Person>::toString( person, options ) };
            std::cout << "Person object:\n" << json << "\n\n";

            Person deserialized{ Serializer<Person>::fromString( json ) };
            std::cout << "Deserialized: " << deserialized.name << ", age " << deserialized.age << "\n";
        }

        std::cout << "\n";

        //=====================================================================
        // 5. Complex Nesting
        //=====================================================================
        {
            std::cout << "5. Complex Nesting\n";
            std::cout << "------------------\n";

            Company company{};
            company.name = "Tech Innovations Inc";
            company.founded = DateTime( 2020, 1, 15 );

            Person emp1{};
            emp1.name = "Alice";
            emp1.age = 30;
            emp1.isActive = true;
            emp1.email = "alice@techinnovations.com";

            Person emp2{};
            emp2.name = "Bob";
            emp2.age = 28;
            emp2.isActive = true;

            company.employees = { emp1, emp2 };
            company.departments = { { "Engineering", 10 }, { "Sales", 5 }, { "HR", 3 } };

            Serializer<Company>::Options options;
            options.prettyPrint = true;

            std::string json{ Serializer<Company>::toString( company, options ) };
            std::cout << "Company object:\n" << json << "\n\n";

            Company deserialized{ Serializer<Company>::fromString( json ) };
            std::cout << "Deserialized company: " << deserialized.name << " with " << deserialized.employees.size()
                      << " employees\n";
        }

        std::cout << "\n";

        //=====================================================================
        // 6. Validation
        //=====================================================================
        {
            std::cout << "6. Validation\n";
            std::cout << "-------------\n";

            Serializer<Person>::Options opts{};
            opts.validateOnDeserialize = true;

            Person invalidPerson{};
            invalidPerson.name = "Invalid";
            invalidPerson.age = 200;
            invalidPerson.isActive = true;

            std::string json{ Serializer<Person>::toString( invalidPerson ) };
            std::cout << "JSON with invalid age: " << json << "\n";

            try
            {
                Person deserialized{ Serializer<Person>::fromString( json, opts ) };
                std::cout << "Validation failed to catch error\n";
            }
            catch( const std::exception& e )
            {
                std::cout << "Validation caught error: " << e.what() << "\n";
            }
        }

        std::cout << "\n";

        //=====================================================================
        // 7. Serializer Class Instance
        //=====================================================================
        {
            std::cout << "7. Serializer Class Instance\n";
            std::cout << "----------------------------\n";

            Serializer<Person>::Options opts{};
            opts.prettyPrint = true;
            opts.includeNullFields = true;

            Person person{};
            person.name = "Charlie";
            person.age = 35;
            person.isActive = false;
            person.hobbies = { "photography", "traveling" };

            std::string json{ Serializer<Person>::toString( person, opts ) };
            std::cout << "Pretty printed Person:\n" << json << "\n\n";

            Person deserialized{ Serializer<Person>::fromString( json ) };
            std::cout << "Deserialized: " << deserialized.name
                      << ", active: " << ( deserialized.isActive ? "yes" : "no" ) << "\n";
        }

        std::cout << "\n";
    }
    catch( const std::exception& e )
    {
        std::cerr << "Error during demonstration: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
