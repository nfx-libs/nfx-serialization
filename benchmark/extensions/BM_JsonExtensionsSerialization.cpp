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
 * @file BM_JsonExtensionsSerialization.cpp
 * @brief Benchmarks for JSON serialization of extension types (DateTime, Datatypes, Containers)
 * @details Measures the performance of serializing nfx extension types to JSON,
 *          focusing on temporary allocations and string conversion overhead.
 */

#include <benchmark/benchmark.h>

#include <nfx/Serialization.h>

#include <nfx/serialization/json/extensions/ContainersTraits.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>
#include <nfx/serialization/json/extensions/DateTimeTraits.h>

#if __has_include( <nfx/containers/FastHashMap.h>)
#    define NFX_CONTAINERS_AVAILABLE
#endif

#if __has_include( <nfx/datatypes/Int128.h>)
#    define NFX_DATATYPES_AVAILABLE
#endif

#if __has_include( <nfx/datetime/DateTime.h>)
#    define NFX_DATETIME_AVAILABLE
#endif

namespace nfx::serialization::json::benchmark
{
    using namespace nfx::json;
    using namespace nfx::serialization::json;

    //=====================================================================
    // Datatypes Serialization Benchmarks
    //=====================================================================

#ifdef NFX_DATATYPES_AVAILABLE
    static void BM_Int128_Serializer_Small( ::benchmark::State& state )
    {
        datatypes::Int128 value{ 42 };
        Serializer<datatypes::Int128> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( value );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Int128_Serializer_Medium( ::benchmark::State& state )
    {
        datatypes::Int128 value{ static_cast<std::int64_t>( 1234567890123456789LL ) };
        Serializer<datatypes::Int128> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( value );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Int128_Serializer_Large( ::benchmark::State& state )
    {
        datatypes::Int128 value{ "123456789012345678901234567890" };
        Serializer<datatypes::Int128> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( value );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Decimal_Serializer_Small( ::benchmark::State& state )
    {
        datatypes::Decimal value{ "123.45" };
        Serializer<datatypes::Decimal> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( value );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Decimal_Serializer_Large( ::benchmark::State& state )
    {
        datatypes::Decimal value{ "123456789012345678.901234567" };
        Serializer<datatypes::Decimal> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( value );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_Int128Array10_Serializer( ::benchmark::State& state )
    {
        std::vector<datatypes::Int128> values;
        for( int i = 0; i < 10; ++i )
        {
            values.push_back( datatypes::Int128{ i * 1000000 } );
        }
        Serializer<std::vector<datatypes::Int128>> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( values );
            ::benchmark::DoNotOptimize( json );
        }
    }
#endif

    //=====================================================================
    // DateTime Serialization Benchmarks
    //=====================================================================

#ifdef NFX_DATETIME_AVAILABLE
    static void BM_DateTime_Serializer( ::benchmark::State& state )
    {
        time::DateTime dt{ 2024, 6, 15, 14, 30, 45, 123 };
        Serializer<time::DateTime> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( dt );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_DateTimeOffset_Serializer( ::benchmark::State& state )
    {
        time::DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, 123, time::TimeSpan::fromHours( 2.0 ) };
        Serializer<time::DateTimeOffset> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( dto );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_TimeSpan_Serializer( ::benchmark::State& state )
    {
        time::TimeSpan ts = time::TimeSpan::fromHours( 25.5 );
        Serializer<time::TimeSpan> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( ts );
            ::benchmark::DoNotOptimize( json );
        }
    }

    static void BM_DateTimeArray10_Serializer( ::benchmark::State& state )
    {
        std::vector<time::DateTime> values;
        for( int i = 0; i < 10; ++i )
        {
            values.push_back( time::DateTime{ 2024, 1, i + 1, 12, 0, 0, 0 } );
        }
        Serializer<std::vector<time::DateTime>> serializer;

        for( auto _ : state )
        {
            std::string json = serializer.toString( values );
            ::benchmark::DoNotOptimize( json );
        }
    }
#endif

    //=====================================================================
    // Benchmark Registration
    //=====================================================================

#ifdef NFX_DATATYPES_AVAILABLE
    BENCHMARK( BM_Int128_Serializer_Small );
    BENCHMARK( BM_Int128_Serializer_Medium );
    BENCHMARK( BM_Int128_Serializer_Large );
    BENCHMARK( BM_Decimal_Serializer_Small );
    BENCHMARK( BM_Decimal_Serializer_Large );
    BENCHMARK( BM_Int128Array10_Serializer );
#endif

#ifdef NFX_DATETIME_AVAILABLE
    BENCHMARK( BM_DateTime_Serializer );
    BENCHMARK( BM_DateTimeOffset_Serializer );
    BENCHMARK( BM_TimeSpan_Serializer );
    BENCHMARK( BM_DateTimeArray10_Serializer );
#endif
} // namespace nfx::serialization::json::benchmark

BENCHMARK_MAIN();
