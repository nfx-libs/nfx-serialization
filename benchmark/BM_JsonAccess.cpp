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
 * @file BM_JsonAccess.cpp
 * @brief Benchmarks for JSON Document value access operations
 */

#include <benchmark/benchmark.h>

#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::benchmark
{
    //----------------------------------------------
    // Test data setup
    //----------------------------------------------

    static Document createTestDocument()
    {
        Document doc;
        doc.set<std::string>( "name", "John Doe" );
        doc.set<int>( "age", 30 );
        doc.set<bool>( "active", true );
        doc.set<double>( "balance", 1234.56 );
        doc.set<std::string>( "email", "john@example.com" );
        doc.set<std::string>( "address.street", "123 Main St" );
        doc.set<std::string>( "address.city", "Springfield" );
        doc.set<std::string>( "address.country", "USA" );
        doc.set<int>( "address.zip", 12345 );
        doc.set<std::string>( "company.name", "Acme Corp" );
        doc.set<std::string>( "company.department.name", "Engineering" );
        doc.set<int>( "company.department.floor", 5 );
        doc.set<std::string>( "company.department.manager.name", "Jane Smith" );
        doc.set<std::string>( "company.department.manager.email", "jane@acme.com" );
        return doc;
    }

    //----------------------------------------------
    // Get by Key benchmarks
    //----------------------------------------------

    static void BM_GetByKey_TopLevel( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            auto value = doc.get<std::string>( "name" );
            ::benchmark::DoNotOptimize( value );
        }
    }

    static void BM_GetByKey_Nested( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            auto value = doc.get<std::string>( "address.city" );
            ::benchmark::DoNotOptimize( value );
        }
    }

    //----------------------------------------------
    // Get by JSON Pointer benchmarks
    //----------------------------------------------

    static void BM_GetByPointer_TopLevel( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            auto value = doc.get<std::string>( "/name" );
            ::benchmark::DoNotOptimize( value );
        }
    }

    static void BM_GetByPointer_Nested( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            auto value = doc.get<std::string>( "/address/city" );
            ::benchmark::DoNotOptimize( value );
        }
    }

    //----------------------------------------------
    // Get Deep Nested Value benchmarks
    //----------------------------------------------

    static void BM_GetDeepNested_3Levels( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            auto value = doc.get<std::string>( "company.department.name" );
            ::benchmark::DoNotOptimize( value );
        }
    }

    static void BM_GetDeepNested_4Levels( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            auto value = doc.get<std::string>( "company.department.manager.name" );
            ::benchmark::DoNotOptimize( value );
        }
    }

    //----------------------------------------------
    // Check Field Existence benchmarks
    //----------------------------------------------

    static void BM_HasField_Exists( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            bool exists = doc.contains( "/name" );
            ::benchmark::DoNotOptimize( exists );
        }
    }

    static void BM_HasField_NotExists( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            bool exists = doc.contains( "/nonexistent" );
            ::benchmark::DoNotOptimize( exists );
        }
    }

    static void BM_HasField_NestedExists( ::benchmark::State& state )
    {
        Document doc = createTestDocument();

        for ( auto _ : state )
        {
            bool exists = doc.contains( "/company/department/manager/email" );
            ::benchmark::DoNotOptimize( exists );
        }
    }
} // namespace nfx::serialization::json::benchmark

//=====================================================================
// Benchmark Registration
//=====================================================================

BENCHMARK( nfx::serialization::json::benchmark::BM_GetByKey_TopLevel );
BENCHMARK( nfx::serialization::json::benchmark::BM_GetByKey_Nested );
BENCHMARK( nfx::serialization::json::benchmark::BM_GetByPointer_TopLevel );
BENCHMARK( nfx::serialization::json::benchmark::BM_GetByPointer_Nested );
BENCHMARK( nfx::serialization::json::benchmark::BM_GetDeepNested_3Levels );
BENCHMARK( nfx::serialization::json::benchmark::BM_GetDeepNested_4Levels );
BENCHMARK( nfx::serialization::json::benchmark::BM_HasField_Exists );
BENCHMARK( nfx::serialization::json::benchmark::BM_HasField_NotExists );
BENCHMARK( nfx::serialization::json::benchmark::BM_HasField_NestedExists );

BENCHMARK_MAIN();
