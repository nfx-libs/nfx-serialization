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
 * @file BM_JsonObjectSerialization.cpp
 * @brief Benchmarks for JSON Document modification operations
 */

#include <benchmark/benchmark.h>

#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::benchmark
{
    //----------------------------------------------
    // Set Primitive Value benchmarks
    //----------------------------------------------

    static void BM_SetPrimitiveValue_String( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<std::string>( "name", "John Doe" );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    static void BM_SetPrimitiveValue_Int( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<int>( "age", 30 );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    static void BM_SetPrimitiveValue_Bool( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<bool>( "active", true );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    static void BM_SetPrimitiveValue_Double( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<double>( "balance", 1234.56 );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    //----------------------------------------------
    // Set Nested Value benchmarks
    //----------------------------------------------

    static void BM_SetNestedValue_2Levels( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<std::string>( "address.city", "Springfield" );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    static void BM_SetNestedValue_3Levels( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<std::string>( "company.address.city", "Springfield" );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    static void BM_SetNestedValue_4Levels( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<std::string>( "company.department.manager.name", "Jane Smith" );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    //----------------------------------------------
    // Add Object Field benchmarks
    //----------------------------------------------

    static void BM_AddObjectField_Sequential( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<std::string>( "field1", "value1" );
            doc.set<std::string>( "field2", "value2" );
            doc.set<std::string>( "field3", "value3" );
            doc.set<std::string>( "field4", "value4" );
            doc.set<std::string>( "field5", "value5" );
            ::benchmark::DoNotOptimize( doc );
        }
    }

    //----------------------------------------------
    // Add Array Element benchmarks
    //----------------------------------------------

    static void BM_AddArrayElement_PushBack( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<Document::Array>( "" );
            auto arr = doc.get<Document::Array>( "" ).value();
            for ( int64_t i = 0; i < 10; ++i )
            {
                arr.append<int64_t>( i );
            }
            ::benchmark::DoNotOptimize( doc );
        }
    }

    static void BM_AddArrayElement_LargeArray( ::benchmark::State& state )
    {
        for ( auto _ : state )
        {
            Document doc;
            doc.set<Document::Array>( "" );
            auto arr = doc.get<Document::Array>( "" ).value();
            for ( int64_t i = 0; i < 100; ++i )
            {
                arr.append<int64_t>( i );
            }
            ::benchmark::DoNotOptimize( doc );
        }
    }
} // namespace nfx::serialization::json::benchmark

//=====================================================================
// Benchmark Registration
//=====================================================================

BENCHMARK( nfx::serialization::json::benchmark::BM_SetPrimitiveValue_String );
BENCHMARK( nfx::serialization::json::benchmark::BM_SetPrimitiveValue_Int );
BENCHMARK( nfx::serialization::json::benchmark::BM_SetPrimitiveValue_Bool );
BENCHMARK( nfx::serialization::json::benchmark::BM_SetPrimitiveValue_Double );
BENCHMARK( nfx::serialization::json::benchmark::BM_SetNestedValue_2Levels );
BENCHMARK( nfx::serialization::json::benchmark::BM_SetNestedValue_3Levels );
BENCHMARK( nfx::serialization::json::benchmark::BM_SetNestedValue_4Levels );
BENCHMARK( nfx::serialization::json::benchmark::BM_AddObjectField_Sequential );
BENCHMARK( nfx::serialization::json::benchmark::BM_AddArrayElement_PushBack );
BENCHMARK( nfx::serialization::json::benchmark::BM_AddArrayElement_LargeArray );

BENCHMARK_MAIN();
