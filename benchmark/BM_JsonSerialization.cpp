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
 * @file BM_JsonSerialization.cpp
 * @brief Benchmarks for JSON serialization performance
 * @details Compares different serialization approaches:
 *          1. Document API - DOM manipulation (flexible navigation)
 *          2. Builder API - Streaming write (optimal performance, SAX-like)
 *          3. Serializer<T> - High-level API with SerializationTraits
 *          4. Legacy fallback - Member method toDocument() (compatibility)
 */

#include <benchmark/benchmark.h>

#include <nfx/Serialization.h>

#include <map>
#include <string>
#include <vector>

namespace nfx::serialization::json::benchmark
{
    using namespace nfx::json;
    using namespace nfx::serialization::json;

    //=====================================================================
    // Test Data Structures
    //=====================================================================

    struct Point2D
    {
        double x;
        double y;
    };

    struct Person
    {
        std::string name;
        int age;
        std::string email;
        bool active;
    };

    struct Company
    {
        std::string name;
        std::string industry;
        int employees;
        int founded;
        std::vector<Person> staff;
    };

    struct PersonLegacy
    {
        std::string name;
        int age;
        std::string email;
        bool active;

        // Legacy member method: toDocument()
        // Called by SerializationTraits default implementation via SFINAE
        // Path: Serializer → SerializationTraits → toDocument() → Document → JSON
        // Overhead: Creates intermediate Document object (DOM allocation + navigation)
        void toDocument( const Serializer<PersonLegacy>& serializer, Document& doc ) const
        {
            doc.set( "/name", name );
            doc.set( "/age", age );
            doc.set( "/email", email );
            doc.set( "/active", active );
        }
    };

    // Complex nested structure using legacy approach
    // Demonstrates compounding overhead with nested Document creation
    struct CompanyLegacy
    {
        std::string name;
        std::string industry;
        int employees;
        int founded;
        std::vector<PersonLegacy> staff;

        // Legacy member method: toDocument()
        // For each Person in staff vector:
        //   1. Creates new Document
        //   2. Calls person.toDocument() → fills Document
        //   3. Adds Document to Array
        // This is very inefficient compared to Builder streaming!
        void toDocument( const Serializer<CompanyLegacy>& serializer, Document& doc ) const
        {
            doc.set( "/name", name );
            doc.set( "/industry", industry );
            doc.set( "/employees", employees );
            doc.set( "/founded", founded );

            if( !staff.empty() )
            {
                Array staffArray;
                for( const auto& person : staff )
                {
                    // PERFORMANCE WARNING: Creates intermediate Document for each person!
                    // Modern approach would stream directly to Builder
                    Document personDoc;
                    Serializer<PersonLegacy> personSerializer;
                    person.toDocument( personSerializer, personDoc );
                    staffArray.push_back( personDoc );
                }
                doc.set<Array>( "/staff", staffArray );
            }
        }
    };

} // namespace nfx::serialization::json::benchmark

//=====================================================================
// SerializationTraits for Point2D (Modern Builder streaming approach)
//=====================================================================

namespace nfx::serialization::json
{
    template <>
    struct SerializationTraits<benchmark::Point2D>
    {
        static void serialize( const benchmark::Point2D& point, Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "x", point.x );
            builder.write( "y", point.y );
            builder.writeEndObject();
        }

        static void fromDocument( const Document& doc, benchmark::Point2D& point )
        {
            if( auto val = doc.get<double>( "x" ) )
                point.x = *val;
            if( auto val = doc.get<double>( "y" ) )
                point.y = *val;
        }
    };

    template <>
    struct SerializationTraits<benchmark::Person>
    {
        static void serialize( const benchmark::Person& person, Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "name", person.name );
            builder.write( "age", person.age );
            builder.write( "email", person.email );
            builder.write( "active", person.active );
            builder.writeEndObject();
        }

        static void fromDocument( const Document& doc, benchmark::Person& person )
        {
            if( auto val = doc.get<std::string>( "name" ) )
                person.name = *val;
            if( auto val = doc.get<int>( "age" ) )
                person.age = *val;
            if( auto val = doc.get<std::string>( "email" ) )
                person.email = *val;
            if( auto val = doc.get<bool>( "active" ) )
                person.active = *val;
        }
    };

    template <>
    struct SerializationTraits<benchmark::Company>
    {
        static void serialize( const benchmark::Company& company, Builder& builder )
        {
            builder.writeStartObject();
            builder.write( "name", company.name );
            builder.write( "industry", company.industry );
            builder.write( "employees", company.employees );
            builder.write( "founded", company.founded );
            builder.write( "staff" );
            builder.writeStartArray();
            for( const auto& person : company.staff )
            {
                SerializationTraits<benchmark::Person>::serialize( person, builder );
            }
            builder.writeEndArray();
            builder.writeEndObject();
        }

        static void fromDocument( const Document& doc, benchmark::Company& company )
        {
            if( auto val = doc.get<std::string>( "name" ) )
                company.name = *val;
            if( auto val = doc.get<std::string>( "industry" ) )
                company.industry = *val;
            if( auto val = doc.get<int>( "employees" ) )
                company.employees = *val;
            if( auto val = doc.get<int>( "founded" ) )
                company.founded = *val;
        }
    };
} // namespace nfx::serialization::json

namespace nfx::serialization::json::benchmark
{
    //=====================================================================
    // Test Data Generators
    //=====================================================================

    static Document createSmallDocument()
    {
        Document doc;
        doc.set<std::string>( "name", "John" );
        doc.set<int>( "age", 30 );
        doc.set<bool>( "active", true );
        return doc;
    }

    static Document createLargeDocument()
    {
        Document doc;
        doc.set<std::string>( "id", "550e8400-e29b-41d4-a716-446655440000" );
        doc.set<std::string>( "name", "John Doe" );
        doc.set<std::string>( "email", "john.doe@example.com" );
        doc.set<int>( "age", 30 );
        doc.set<bool>( "active", true );
        doc.set<double>( "balance", 12345.67 );
        doc.set<std::string>( "created", "2025-01-15T10:30:00Z" );
        doc.set<std::string>( "address.street", "123 Main Street" );
        doc.set<std::string>( "address.city", "Springfield" );
        doc.set<std::string>( "address.state", "IL" );
        doc.set<std::string>( "address.country", "USA" );
        doc.set<std::string>( "address.zip", "62701" );
        doc.set<std::string>( "company.name", "Acme Corporation" );
        doc.set<std::string>( "company.industry", "Technology" );
        doc.set<int>( "company.employees", 5000 );
        doc.set<int>( "company.founded", 1985 );
        doc.set<std::string>( "metadata.source", "api" );
        doc.set<std::string>( "metadata.version", "2.0" );
        doc.set<std::string>( "metadata.checksum", "abc123def456" );
        return doc;
    }

    static std::vector<int> createIntVector( size_t count )
    {
        std::vector<int> data;
        data.reserve( count );
        for( size_t i = 0; i < count; ++i )
        {
            data.push_back( static_cast<int>( i ) );
        }
        return data;
    }

    static std::map<std::string, int> createStringIntMap( size_t count )
    {
        std::map<std::string, int> data;
        for( size_t i = 0; i < count; ++i )
        {
            data["key_" + std::to_string( i )] = static_cast<int>( i );
        }
        return data;
    }

    static std::vector<Person> createPersonVector( size_t count )
    {
        std::vector<Person> people;
        people.reserve( count );
        for( size_t i = 0; i < count; ++i )
        {
            people.push_back( { "Person " + std::to_string( i ),
                                static_cast<int>( 20 + ( i % 50 ) ),
                                "person" + std::to_string( i ) + "@example.com",
                                i % 2 == 0 } );
        }
        return people;
    }

    static std::vector<PersonLegacy> createPersonLegacyVector( size_t count )
    {
        std::vector<PersonLegacy> people;
        people.reserve( count );
        for( size_t i = 0; i < count; ++i )
        {
            people.push_back( { "Person " + std::to_string( i ),
                                static_cast<int>( 20 + ( i % 50 ) ),
                                "person" + std::to_string( i ) + "@example.com",
                                i % 2 == 0 } );
        }
        return people;
    }

    //=====================================================================
    // Simple Object (3 fields: name, age, active)
    //=====================================================================

    static void BM_SimpleObject_Document( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            Document doc;
            doc.set<std::string>( "name", "John" );
            doc.set<int>( "age", 30 );
            doc.set<bool>( "active", true );
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_SimpleObject_Builder( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            Builder builder;
            builder.writeStartObject();
            builder.write( "name", "John" );
            builder.write( "age", 30 );
            builder.write( "active", true );
            builder.writeEndObject();
            std::string json = builder.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Int Array (10 elements)
    //=====================================================================

    static void BM_IntArray10_Document( ::benchmark::State& state )
    {
        auto data = createIntVector( 10 );

        for( auto _ : state )
        {
            Document doc;
            Array arr;
            for( int val : data )
                arr.push_back( val );
            doc.set<Array>( "", arr );
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_IntArray10_Builder( ::benchmark::State& state )
    {
        auto data = createIntVector( 10 );

        for( auto _ : state )
        {
            Builder builder;
            builder.writeStartArray();
            for( int value : data )
            {
                builder.write( value );
            }
            builder.writeEndArray();
            std::string json = builder.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_IntArray10_Serializer( ::benchmark::State& state )
    {
        auto data = createIntVector( 10 );
        Serializer<std::vector<int>> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( data );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Int Array Large (10,000 elements)
    //=====================================================================

    static void BM_IntArray10k_Document( ::benchmark::State& state )
    {
        auto data = createIntVector( 10000 );

        for( auto _ : state )
        {
            Document doc;
            Array arr;
            for( int val : data )
                arr.push_back( val );
            doc.set<Array>( "", arr );
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_IntArray10k_Builder( ::benchmark::State& state )
    {
        auto data = createIntVector( 10000 );

        for( auto _ : state )
        {
            Builder builder;
            builder.writeStartArray();
            for( int value : data )
            {
                builder.write( value );
            }
            builder.writeEndArray();
            std::string json = builder.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_IntArray10k_Serializer( ::benchmark::State& state )
    {
        auto data = createIntVector( 10000 );
        Serializer<std::vector<int>> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( data );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // String-Int Map (100 pairs)
    //=====================================================================

    static void BM_Map100_Document( ::benchmark::State& state )
    {
        auto data = createStringIntMap( 100 );

        for( auto _ : state )
        {
            Document doc;
            for( const auto& [key, val] : data )
                doc.set<int>( key, val );
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Map100_Builder( ::benchmark::State& state )
    {
        auto data = createStringIntMap( 100 );

        for( auto _ : state )
        {
            Builder builder;
            builder.writeStartObject();
            for( const auto& [key, value] : data )
            {
                builder.write( key, value );
            }
            builder.writeEndObject();
            std::string json = builder.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Map100_Serializer( ::benchmark::State& state )
    {
        auto data = createStringIntMap( 100 );
        Serializer<std::map<std::string, int>> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( data );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Point2D (2 doubles) - Modern Traits
    //=====================================================================

    static void BM_Point2D_Document( ::benchmark::State& state )
    {
        Point2D point{ 3.14, 2.71 };

        for( auto _ : state )
        {
            Document doc;
            doc.set<double>( "x", point.x );
            doc.set<double>( "y", point.y );
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Point2D_Builder( ::benchmark::State& state )
    {
        Point2D point{ 3.14, 2.71 };

        for( auto _ : state )
        {
            Builder builder;
            builder.writeStartObject();
            builder.write( "x", point.x );
            builder.write( "y", point.y );
            builder.writeEndObject();
            std::string json = builder.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Point2D_SerializerTraits( ::benchmark::State& state )
    {
        Point2D point{ 3.14, 2.71 };
        Serializer<Point2D> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( point );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Person (4 fields) - Legacy toDocument()
    //=====================================================================

    static void BM_Person_Document( ::benchmark::State& state )
    {
        Person person{ "John Doe", 30, "john@example.com", true };

        for( auto _ : state )
        {
            Document doc;
            doc.set<std::string>( "name", person.name );
            doc.set<int>( "age", person.age );
            doc.set<std::string>( "email", person.email );
            doc.set<bool>( "active", person.active );
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Person_Builder( ::benchmark::State& state )
    {
        Person person{ "John Doe", 30, "john@example.com", true };

        for( auto _ : state )
        {
            Builder builder;
            builder.writeStartObject();
            builder.write( "name", person.name );
            builder.write( "age", person.age );
            builder.write( "email", person.email );
            builder.write( "active", person.active );
            builder.writeEndObject();
            std::string json = builder.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Person_SerializerTraits( ::benchmark::State& state )
    {
        Person person{ "John Doe", 30, "john@example.com", true };
        Serializer<Person> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( person );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Person_SerializerLegacy( ::benchmark::State& state )
    {
        PersonLegacy person{ "John Doe", 30, "john@example.com", true };
        Serializer<PersonLegacy> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( person );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Person Vector (100 elements)
    //=====================================================================

    static void BM_PersonVector100_Document( ::benchmark::State& state )
    {
        auto people = createPersonVector( 100 );

        for( auto _ : state )
        {
            Document doc;
            Array arr;
            for( const auto& p : people )
            {
                Document personDoc;
                personDoc.set<std::string>( "name", p.name );
                personDoc.set<int>( "age", p.age );
                personDoc.set<std::string>( "email", p.email );
                personDoc.set<bool>( "active", p.active );
                arr.push_back( personDoc );
            }
            doc.set<Array>( "", arr );
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_PersonVector100_Builder( ::benchmark::State& state )
    {
        auto people = createPersonVector( 100 );

        for( auto _ : state )
        {
            Builder builder;
            builder.writeStartArray();
            for( const auto& p : people )
            {
                builder.writeStartObject();
                builder.write( "name", p.name );
                builder.write( "age", p.age );
                builder.write( "email", p.email );
                builder.write( "active", p.active );
                builder.writeEndObject();
            }
            builder.writeEndArray();
            std::string json = builder.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_PersonVector100_SerializerTraits( ::benchmark::State& state )
    {
        auto people = createPersonVector( 100 );
        Serializer<std::vector<Person>> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( people );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_PersonVector100_SerializerLegacy( ::benchmark::State& state )
    {
        auto people = createPersonLegacyVector( 100 );
        Serializer<std::vector<PersonLegacy>> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( people );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Company (nested with 10 staff)
    //=====================================================================

    static void BM_Company_Document( ::benchmark::State& state )
    {
        Company company{ "Acme Corporation", "Technology", 5000, 1985, createPersonVector( 10 ) };

        for( auto _ : state )
        {
            Document doc;
            doc.set<std::string>( "name", company.name );
            doc.set<std::string>( "industry", company.industry );
            doc.set<int>( "employees", company.employees );
            doc.set<int>( "founded", company.founded );
            Array staffArr;
            for( const auto& p : company.staff )
            {
                Document personDoc;
                personDoc.set<std::string>( "name", p.name );
                personDoc.set<int>( "age", p.age );
                personDoc.set<std::string>( "email", p.email );
                personDoc.set<bool>( "active", p.active );
                staffArr.push_back( personDoc );
            }
            doc.set<Array>( "staff", staffArr );
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Company_Builder( ::benchmark::State& state )
    {
        Company company{ "Acme Corporation", "Technology", 5000, 1985, createPersonVector( 10 ) };

        for( auto _ : state )
        {
            Builder builder;
            builder.writeStartObject();
            builder.write( "name", company.name );
            builder.write( "industry", company.industry );
            builder.write( "employees", company.employees );
            builder.write( "founded", company.founded );
            builder.write( "staff" );
            builder.writeStartArray();
            for( const auto& p : company.staff )
            {
                builder.writeStartObject();
                builder.write( "name", p.name );
                builder.write( "age", p.age );
                builder.write( "email", p.email );
                builder.write( "active", p.active );
                builder.writeEndObject();
            }
            builder.writeEndArray();
            builder.writeEndObject();
            std::string json = builder.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Company_SerializerTraits( ::benchmark::State& state )
    {
        Company company{ "Acme Corporation", "Technology", 5000, 1985, createPersonVector( 10 ) };
        Serializer<Company> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( company );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Company_SerializerLegacy( ::benchmark::State& state )
    {
        CompanyLegacy company{ "Acme Corporation", "Technology", 5000, 1985, createPersonLegacyVector( 10 ) };
        Serializer<CompanyLegacy> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( company );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Large Document (19 fields)
    //=====================================================================

    static void BM_LargeDocument_Document( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            Document doc = createLargeDocument();
            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_LargeDocument_PrettyPrint( ::benchmark::State& state )
    {
        for( auto _ : state )
        {
            Document doc = createLargeDocument();
            std::string json = doc.toString( 2 );
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Benchmark Registration
    //=====================================================================

    BENCHMARK( BM_SimpleObject_Document );
    BENCHMARK( BM_SimpleObject_Builder );

    BENCHMARK( BM_IntArray10_Document );
    BENCHMARK( BM_IntArray10_Builder );
    BENCHMARK( BM_IntArray10_Serializer );

    BENCHMARK( BM_IntArray10k_Document );
    BENCHMARK( BM_IntArray10k_Builder );
    BENCHMARK( BM_IntArray10k_Serializer );

    BENCHMARK( BM_Map100_Document );
    BENCHMARK( BM_Map100_Builder );
    BENCHMARK( BM_Map100_Serializer );

    BENCHMARK( BM_Point2D_Document );
    BENCHMARK( BM_Point2D_Builder );
    BENCHMARK( BM_Point2D_SerializerTraits );

    BENCHMARK( BM_Person_Document );
    BENCHMARK( BM_Person_Builder );
    BENCHMARK( BM_Person_SerializerTraits );
    BENCHMARK( BM_Person_SerializerLegacy );

    BENCHMARK( BM_PersonVector100_Document );
    BENCHMARK( BM_PersonVector100_Builder );
    BENCHMARK( BM_PersonVector100_SerializerTraits );
    BENCHMARK( BM_PersonVector100_SerializerLegacy );

    BENCHMARK( BM_Company_Document );
    BENCHMARK( BM_Company_Builder );
    BENCHMARK( BM_Company_SerializerTraits );
    BENCHMARK( BM_Company_SerializerLegacy );

    BENCHMARK( BM_LargeDocument_Document );
    BENCHMARK( BM_LargeDocument_PrettyPrint );
} // namespace nfx::serialization::json::benchmark

BENCHMARK_MAIN();
