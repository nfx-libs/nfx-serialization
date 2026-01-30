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
 * @file BM_JsonSerializerBuilder.cpp
 * @brief Benchmarks comparing Document-based vs Builder-based serialization
 */

#include <benchmark/benchmark.h>

#include <nfx/Serialization.h>
#include <nfx/json/Builder.h>
#include <nfx/json/Document.h>

#include <map>
#include <vector>

using namespace nfx::json;
using namespace nfx::serialization::json;

namespace nfx::serialization::json::bm
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

        void serialize( const Serializer<Person>& serializer, Document& doc ) const
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

        void serialize( const Serializer<Company>& serializer, Document& doc ) const
        {
            doc.set( "/name", name );
            doc.set( "/industry", industry );
            doc.set( "/employees", employees );
            doc.set( "/founded", founded );

            // Serialize staff using Serializer
            Serializer<std::vector<Person>> staffSerializer;
            Document staffDoc = staffSerializer.serialize( staff ).document();
            doc.set( "/staff", staffDoc );
        }
    };

    //----------------------------------------------
    // Test data generation
    //----------------------------------------------

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
    // Primitive vector benchmarks
    //----------------------------------------------

    static void BM_Serializer_IntVector_Small_Builder( ::benchmark::State& state )
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

    static void BM_Serializer_IntVector_Small_Document( ::benchmark::State& state )
    {
        auto data = createIntVector( 10 );
        Serializer<std::vector<int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.serialize( data );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Serializer_IntVector_Large_Builder( ::benchmark::State& state )
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

    static void BM_Serializer_IntVector_Large_Document( ::benchmark::State& state )
    {
        auto data = createIntVector( 10000 );
        Serializer<std::vector<int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.serialize( data );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    //----------------------------------------------
    // Map benchmarks
    //----------------------------------------------

    static void BM_Serializer_StringIntMap_Builder( ::benchmark::State& state )
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

    static void BM_Serializer_StringIntMap_Document( ::benchmark::State& state )
    {
        auto data = createStringIntMap( 100 );
        Serializer<std::map<std::string, int>> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.serialize( data );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    //----------------------------------------------
    // Custom type benchmarks
    //----------------------------------------------

    static void BM_Serializer_Person_Builder( ::benchmark::State& state )
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

    static void BM_Serializer_Person_Document( ::benchmark::State& state )
    {
        Person person = { "John Doe", 30, "john@example.com", true };
        Serializer<Person> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.serialize( person );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Serializer_PersonVector_Builder( ::benchmark::State& state )
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

    static void BM_Serializer_PersonVector_Document( ::benchmark::State& state )
    {
        auto people = createPersonVector( 100 );
        Serializer<std::vector<Person>> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.serialize( people );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    //----------------------------------------------
    // Nested structure benchmarks
    //----------------------------------------------

    static void BM_Serializer_Company_Builder( ::benchmark::State& state )
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

    static void BM_Serializer_Company_Document( ::benchmark::State& state )
    {
        auto company = createCompany();
        Serializer<Company> serializer;

        for( auto _ : state )
        {
            (void)_;
            auto doc = serializer.serialize( company );
            std::string json = doc.document().toString();
            ::benchmark::DoNotOptimize( json );
        }
    }

    //=====================================================================
    // Benchmark Registration
    //=====================================================================

    // Primitive vectors
    BENCHMARK( BM_Serializer_IntVector_Small_Builder );
    BENCHMARK( BM_Serializer_IntVector_Small_Document );
    BENCHMARK( BM_Serializer_IntVector_Large_Builder );
    BENCHMARK( BM_Serializer_IntVector_Large_Document );

    // Maps
    BENCHMARK( BM_Serializer_StringIntMap_Builder );
    BENCHMARK( BM_Serializer_StringIntMap_Document );

    // Custom types
    BENCHMARK( BM_Serializer_Person_Builder );
    BENCHMARK( BM_Serializer_Person_Document );
    BENCHMARK( BM_Serializer_PersonVector_Builder );
    BENCHMARK( BM_Serializer_PersonVector_Document );

    // Nested structures
    BENCHMARK( BM_Serializer_Company_Builder );
    BENCHMARK( BM_Serializer_Company_Document );
} // namespace nfx::serialization::json::bm

// Use global benchmark namespace to avoid conflict
BENCHMARK_MAIN();
