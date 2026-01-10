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
 * @file BM_JSONObjectSerialization.cpp
 * @brief Benchmarks for typed object serialization using Serializer.
 */

#include <benchmark/benchmark.h>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/Serializer.h>

#include <string>
#include <vector>

namespace nfx::serialization::json::benchmark
{
    //-------------------------------------------------------------------------
    // Simple POD-like structures with serialization support
    //-------------------------------------------------------------------------
    struct Point
    {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;

        Document serialize() const
        {
            Document doc;
            doc.set<double>( "/x", x );
            doc.set<double>( "/y", y );
            doc.set<double>( "/z", z );
            return doc;
        }

        void deserialize( const Serializer<Point>&, const Document& doc )
        {
            x = doc.get<double>( "/x" ).value_or( 0.0 );
            y = doc.get<double>( "/y" ).value_or( 0.0 );
            z = doc.get<double>( "/z" ).value_or( 0.0 );
        }
    };

    struct Color
    {
        int r = 0;
        int g = 0;
        int b = 0;
        int a = 255;

        Document serialize() const
        {
            Document doc;
            doc.set<int64_t>( "/r", r );
            doc.set<int64_t>( "/g", g );
            doc.set<int64_t>( "/b", b );
            doc.set<int64_t>( "/a", a );
            return doc;
        }

        void deserialize( const Serializer<Color>&, const Document& doc )
        {
            r = static_cast<int>( doc.get<int64_t>( "/r" ).value_or( 0 ) );
            g = static_cast<int>( doc.get<int64_t>( "/g" ).value_or( 0 ) );
            b = static_cast<int>( doc.get<int64_t>( "/b" ).value_or( 0 ) );
            a = static_cast<int>( doc.get<int64_t>( "/a" ).value_or( 255 ) );
        }
    };

    //-------------------------------------------------------------------------
    // Medium complexity structures
    //-------------------------------------------------------------------------
    struct Person
    {
        std::string name;
        int age = 0;
        std::string email;
        bool active = true;

        Document serialize() const
        {
            Document doc;
            doc.set<std::string>( "/name", name );
            doc.set<int64_t>( "/age", age );
            doc.set<std::string>( "/email", email );
            doc.set<bool>( "/active", active );
            return doc;
        }

        void deserialize( const Serializer<Person>&, const Document& doc )
        {
            name = doc.get<std::string>( "/name" ).value_or( "" );
            age = static_cast<int>( doc.get<int64_t>( "/age" ).value_or( 0 ) );
            email = doc.get<std::string>( "/email" ).value_or( "" );
            active = doc.get<bool>( "/active" ).value_or( true );
        }
    };

    struct Address
    {
        std::string street;
        std::string city;
        std::string state;
        std::string zip;
        std::string country;

        Document serialize() const
        {
            Document doc;
            doc.set<std::string>( "/street", street );
            doc.set<std::string>( "/city", city );
            doc.set<std::string>( "/state", state );
            doc.set<std::string>( "/zip", zip );
            doc.set<std::string>( "/country", country );
            return doc;
        }

        void deserialize( const Serializer<Address>&, const Document& doc )
        {
            street = doc.get<std::string>( "/street" ).value_or( "" );
            city = doc.get<std::string>( "/city" ).value_or( "" );
            state = doc.get<std::string>( "/state" ).value_or( "" );
            zip = doc.get<std::string>( "/zip" ).value_or( "" );
            country = doc.get<std::string>( "/country" ).value_or( "" );
        }
    };

    //-------------------------------------------------------------------------
    // Complex nested structures
    //-------------------------------------------------------------------------
    struct Employee
    {
        int id = 0;
        Person person;
        Address address;
        std::string department;
        double salary = 0.0;
        std::vector<std::string> skills;

        Document serialize() const
        {
            Document doc;
            doc.set<int64_t>( "/id", id );
            doc.set<Document>( "/person", person.serialize() );
            doc.set<Document>( "/address", address.serialize() );
            doc.set<std::string>( "/department", department );
            doc.set<double>( "/salary", salary );

            Document skillsArray;
            skillsArray.set<Document::Array>( "" );
            auto arr = skillsArray.get<Document::Array>( "" ).value();
            for ( const auto& skill : skills )
            {
                arr.append( skill );
            }
            doc.set<Document>( "/skills", skillsArray );

            return doc;
        }

        void deserialize( const Serializer<Employee>&, const Document& doc )
        {
            id = static_cast<int>( doc.get<int64_t>( "/id" ).value_or( 0 ) );

            if ( auto personDoc = doc.get<Document>( "/person" ) )
            {
                Serializer<Person> s;
                person.deserialize( s, *personDoc );
            }
            if ( auto addressDoc = doc.get<Document>( "/address" ) )
            {
                Serializer<Address> s;
                address.deserialize( s, *addressDoc );
            }

            department = doc.get<std::string>( "/department" ).value_or( "" );
            salary = doc.get<double>( "/salary" ).value_or( 0.0 );

            skills.clear();
            if ( auto skillsArr = doc.get<Document::Array>( "/skills" ) )
            {
                for ( const auto& elem : *skillsArr )
                {
                    if ( auto skill = elem.get<std::string>( "" ) )
                    {
                        skills.push_back( *skill );
                    }
                }
            }
        }
    };

    //-------------------------------------------------------------------------
    // BM_SerializePOD
    //-------------------------------------------------------------------------
    static void BM_SerializePOD( ::benchmark::State& state )
    {
        Point p{ 1.5, 2.5, 3.5 };
        Serializer<Point> serializer;

        for ( auto _ : state )
        {
            Document doc = serializer.serialize( p );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    //-------------------------------------------------------------------------
    // BM_DeserializePOD
    //-------------------------------------------------------------------------
    static void BM_DeserializePOD( ::benchmark::State& state )
    {
        Document doc;
        doc.set<double>( "/x", 1.5 );
        doc.set<double>( "/y", 2.5 );
        doc.set<double>( "/z", 3.5 );
        Serializer<Point> serializer;

        for ( auto _ : state )
        {
            Point p = serializer.deserialize( doc );
            ::benchmark::DoNotOptimize( p );
        }
    }

    //-------------------------------------------------------------------------
    // BM_SerializeMediumObject
    //-------------------------------------------------------------------------
    static void BM_SerializeMediumObject( ::benchmark::State& state )
    {
        Person person{ "John Doe", 30, "john.doe@example.com", true };
        Serializer<Person> serializer;

        for ( auto _ : state )
        {
            Document doc = serializer.serialize( person );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    //-------------------------------------------------------------------------
    // BM_DeserializeMediumObject
    //-------------------------------------------------------------------------
    static void BM_DeserializeMediumObject( ::benchmark::State& state )
    {
        Document doc;
        doc.set<std::string>( "/name", "John Doe" );
        doc.set<int64_t>( "/age", 30 );
        doc.set<std::string>( "/email", "john.doe@example.com" );
        doc.set<bool>( "/active", true );
        Serializer<Person> serializer;

        for ( auto _ : state )
        {
            Person person = serializer.deserialize( doc );
            ::benchmark::DoNotOptimize( person );
        }
    }

    //-------------------------------------------------------------------------
    // BM_SerializeNestedObject
    //-------------------------------------------------------------------------
    static void BM_SerializeNestedObject( ::benchmark::State& state )
    {
        Employee emp;
        emp.id = 1001;
        emp.person = { "Jane Smith", 28, "jane.smith@company.com", true };
        emp.address = { "123 Main St", "Springfield", "IL", "62701", "USA" };
        emp.department = "Engineering";
        emp.salary = 85000.0;
        emp.skills = { "C++", "Python", "JavaScript", "SQL" };
        Serializer<Employee> serializer;

        for ( auto _ : state )
        {
            Document doc = serializer.serialize( emp );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    //-------------------------------------------------------------------------
    // BM_DeserializeNestedObject
    //-------------------------------------------------------------------------
    static void BM_DeserializeNestedObject( ::benchmark::State& state )
    {
        Employee emp;
        emp.id = 1001;
        emp.person = { "Jane Smith", 28, "jane.smith@company.com", true };
        emp.address = { "123 Main St", "Springfield", "IL", "62701", "USA" };
        emp.department = "Engineering";
        emp.salary = 85000.0;
        emp.skills = { "C++", "Python", "JavaScript", "SQL" };
        Serializer<Employee> serializer;
        Document doc = serializer.serialize( emp );

        for ( auto _ : state )
        {
            Employee result = serializer.deserialize( doc );
            ::benchmark::DoNotOptimize( result );
        }
    }

    //-------------------------------------------------------------------------
    // BM_RoundtripSerialization
    //-------------------------------------------------------------------------
    static void BM_RoundtripSerialization( ::benchmark::State& state )
    {
        Employee emp;
        emp.id = 1001;
        emp.person = { "Jane Smith", 28, "jane.smith@company.com", true };
        emp.address = { "123 Main St", "Springfield", "IL", "62701", "USA" };
        emp.department = "Engineering";
        emp.salary = 85000.0;
        emp.skills = { "C++", "Python", "JavaScript", "SQL" };
        Serializer<Employee> serializer;

        for ( auto _ : state )
        {
            Document doc = serializer.serialize( emp );
            Employee result = serializer.deserialize( doc );
            ::benchmark::DoNotOptimize( result );
        }
    }

    //-------------------------------------------------------------------------
    // BM_SerializeToJson - string output
    //-------------------------------------------------------------------------
    static void BM_SerializetoString( ::benchmark::State& state )
    {
        Person person{ "John Doe", 30, "john.doe@example.com", true };

        for ( auto _ : state )
        {
            std::string json = Serializer<Person>::toString( person );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //-------------------------------------------------------------------------
    // BM_DeserializeFromJson - string input
    //-------------------------------------------------------------------------
    static void BM_DeserializefromString( ::benchmark::State& state )
    {
        std::string json = R"({"name":"John Doe","age":30,"email":"john.doe@example.com","active":true})";

        for ( auto _ : state )
        {
            Person person = Serializer<Person>::fromString( json );
            ::benchmark::DoNotOptimize( person );
        }
    }
} // namespace nfx::serialization::json::benchmark

//=====================================================================
// Benchmark Registration
//=====================================================================

BENCHMARK( nfx::serialization::json::benchmark::BM_SerializePOD );
BENCHMARK( nfx::serialization::json::benchmark::BM_DeserializePOD );
BENCHMARK( nfx::serialization::json::benchmark::BM_SerializeMediumObject );
BENCHMARK( nfx::serialization::json::benchmark::BM_DeserializeMediumObject );
BENCHMARK( nfx::serialization::json::benchmark::BM_SerializeNestedObject );
BENCHMARK( nfx::serialization::json::benchmark::BM_DeserializeNestedObject );
BENCHMARK( nfx::serialization::json::benchmark::BM_RoundtripSerialization );
BENCHMARK( nfx::serialization::json::benchmark::BM_SerializetoString );
BENCHMARK( nfx::serialization::json::benchmark::BM_DeserializefromString );

BENCHMARK_MAIN();
