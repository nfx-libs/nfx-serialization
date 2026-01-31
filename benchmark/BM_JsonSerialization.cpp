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
 * @brief Benchmarks comparing Document-based vs Builder-based serialization
 */

#include <benchmark/benchmark.h>

#include <nfx/Serialization.h>

#include <map>
#include <vector>

namespace nfx::serialization::json::benchmark
{
    //----------------------------------------------
    // Test data structures
    //----------------------------------------------

    struct Person
    {
        std::string name;
        int age;
        std::string email;
        bool active;

        void serialize( const Serializer<Person>& serializer, nfx::json::Document& doc ) const
        {
            doc.set( "/name", name );
            doc.set( "/age", age );
            doc.set( "/email", email );
            doc.set( "/active", active );
        }
    };

    struct Company
    {
        std::string name;
        std::string industry;
        int employees;
        int founded;
        std::vector<Person> staff;

        void serialize( const Serializer<Company>& serializer, nfx::json::Document& doc ) const
        {
            doc.set( "/name", name );
            doc.set( "/industry", industry );
            doc.set( "/employees", employees );
            doc.set( "/founded", founded );

            // Serialize staff using Serializer
            Serializer<std::vector<Person>> staffSerializer;
            nfx::json::Document staffDoc = staffSerializer.toDocument( staff ).document();
            doc.set( "/staff", staffDoc );
        }
    };

    //----------------------------------------------
    // Test data generation
    //----------------------------------------------

    static nfx::json::Document createSmallDocument()
    {
        nfx::json::Document doc;
        doc.set<std::string>( "name", "John" );
        doc.set<int>( "age", 30 );
        doc.set<bool>( "active", true );
        return doc;
    }

    static nfx::json::Document createLargeDocument()
    {
        nfx::json::Document doc;
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
            people.push_back( {
                "Person " + std::to_string( i ),
                static_cast<int>( 20 + ( i % 50 ) ),
                "person" + std::to_string( i ) + "@example.com",
                i % 2 == 0,
            } );
        }
        return people;
    }

    static Company createCompany()
    {
        return {
            "Acme Corporation", "Technology", 5000, 1985, createPersonVector( 10 ),
        };
    }

    //----------------------------------------------
    // Document serialization benchmarks
    //----------------------------------------------

    static void BM_SerializeSmallObject( ::benchmark::State& state )
    {
        nfx::json::Document doc = createSmallDocument();

        for( auto _ : state )
        {
            (void)_;

            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_SerializeLargeObject( ::benchmark::State& state )
    {
        nfx::json::Document doc = createLargeDocument();

        for( auto _ : state )
        {
            (void)_;

            std::string json = doc.toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_SerializePrettyPrint( ::benchmark::State& state )
    {
        nfx::json::Document doc = createLargeDocument();

        for( auto _ : state )
        {
            (void)_;

            std::string json = doc.toString( 2 ); // 2-space indent
            ::benchmark::DoNotOptimize( json );
        }
    }

    //----------------------------------------------
    // STL Containers: Builder vs Document
    //----------------------------------------------

    static void BM_IntVector_Small_Builder( ::benchmark::State& state )
    {
        auto data = createIntVector( 10 );
        Serializer<std::vector<int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            std::string json = serializer.toString( data );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_IntVector_Small_Document( ::benchmark::State& state )
    {
        auto data = createIntVector( 10 );
        Serializer<std::vector<int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.toDocument( data );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_IntVector_Large_Builder( ::benchmark::State& state )
    {
        auto data = createIntVector( 10000 );
        Serializer<std::vector<int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            std::string json = serializer.toString( data );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_IntVector_Large_Document( ::benchmark::State& state )
    {
        auto data = createIntVector( 10000 );
        Serializer<std::vector<int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.toDocument( data );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_StringIntMap_Builder( ::benchmark::State& state )
    {
        auto data = createStringIntMap( 100 );
        Serializer<std::map<std::string, int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            std::string json = serializer.toString( data );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_StringIntMap_Document( ::benchmark::State& state )
    {
        auto data = createStringIntMap( 100 );
        Serializer<std::map<std::string, int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.toDocument( data );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    //----------------------------------------------
    // Custom Types: Builder vs Document
    //----------------------------------------------

    static void BM_Person_Builder( ::benchmark::State& state )
    {
        Person person = { "John Doe", 30, "john@example.com", true };
        Serializer<Person> serializer;

        for( auto _ : state )
        {
            (void)_;
            std::string json = serializer.toString( person );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Person_Document( ::benchmark::State& state )
    {
        Person person = { "John Doe", 30, "john@example.com", true };
        Serializer<Person> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.toDocument( person );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_PersonVector_Builder( ::benchmark::State& state )
    {
        auto people = createPersonVector( 100 );
        Serializer<std::vector<Person>> serializer;

        for( auto _ : state )
        {
            (void)_;
            std::string json = serializer.toString( people );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_PersonVector_Document( ::benchmark::State& state )
    {
        auto people = createPersonVector( 100 );
        Serializer<std::vector<Person>> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.toDocument( people );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Company_Builder( ::benchmark::State& state )
    {
        auto company = createCompany();
        Serializer<Company> serializer;

        for( auto _ : state )
        {
            (void)_;
            std::string json = serializer.toString( company );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Company_Document( ::benchmark::State& state )
    {
        auto company = createCompany();
        Serializer<Company> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.toDocument( company );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Benchmark Registration
    //=====================================================================

    // Document serialization
    BENCHMARK( BM_SerializeSmallObject );
    BENCHMARK( BM_SerializeLargeObject );
    BENCHMARK( BM_SerializePrettyPrint );

    // STL Containers
    BENCHMARK( BM_IntVector_Small_Builder );
    BENCHMARK( BM_IntVector_Small_Document );
    BENCHMARK( BM_IntVector_Large_Builder );
    BENCHMARK( BM_IntVector_Large_Document );
    BENCHMARK( BM_StringIntMap_Builder );
    BENCHMARK( BM_StringIntMap_Document );

    // Custom Types
    BENCHMARK( BM_Person_Builder );
    BENCHMARK( BM_Person_Document );
    BENCHMARK( BM_PersonVector_Builder );
    BENCHMARK( BM_PersonVector_Document );
    BENCHMARK( BM_Company_Builder );
    BENCHMARK( BM_Company_Document );
} // namespace nfx::serialization::json::benchmark

BENCHMARK_MAIN();
