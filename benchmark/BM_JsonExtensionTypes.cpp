/**
 * @file BM_JsonExtensionTypes.cpp
 * @brief Benchmark extension types BuilderTraits performance
 */

#include <benchmark/benchmark.h>

#include <nfx/Serialization.h>
#include <nfx/serialization/json/extensions/DatatypesTraits.h>
#include <nfx/serialization/json/extensions/DateTimeTraits.h>

using namespace nfx::serialization::json;

//=====================================================================
// Int128 Benchmarks
//=====================================================================

static void BM_Int128_Single( benchmark::State& state )
{
    nfx::datatypes::Int128 value( static_cast<int64_t>( 123456789012LL ) );

    for( auto _ : state )
    {
        Serializer<nfx::datatypes::Int128> ser;
        std::string result = ser.toDocument( value ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Int128_Single );

static void BM_Int128_VectorSmall( benchmark::State& state )
{
    std::vector<nfx::datatypes::Int128> values;
    for( int i = 0; i < 10; ++i )
    {
        values.push_back( nfx::datatypes::Int128( static_cast<int64_t>( 1234567890LL + i ) ) );
    }

    for( auto _ : state )
    {
        Serializer<std::vector<nfx::datatypes::Int128>> ser;
        std::string result = ser.toDocument( values ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Int128_VectorSmall );

static void BM_Int128_VectorLarge( benchmark::State& state )
{
    std::vector<nfx::datatypes::Int128> values;
    for( int i = 0; i < 1000; ++i )
    {
        values.push_back( nfx::datatypes::Int128( static_cast<int64_t>( 1234567890LL + i ) ) );
    }

    for( auto _ : state )
    {
        Serializer<std::vector<nfx::datatypes::Int128>> ser;
        std::string result = ser.toDocument( values ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Int128_VectorLarge );

//=====================================================================
// Decimal Benchmarks
//=====================================================================

static void BM_Decimal_Single( benchmark::State& state )
{
    nfx::datatypes::Decimal value( "123.456789" );

    for( auto _ : state )
    {
        Serializer<nfx::datatypes::Decimal> ser;
        std::string result = ser.toDocument( value ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Decimal_Single );

static void BM_Decimal_VectorSmall( benchmark::State& state )
{
    std::vector<nfx::datatypes::Decimal> values;
    for( int i = 0; i < 10; ++i )
    {
        values.push_back( nfx::datatypes::Decimal( 123.45 + i ) );
    }

    for( auto _ : state )
    {
        Serializer<std::vector<nfx::datatypes::Decimal>> ser;
        std::string result = ser.toDocument( values ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Decimal_VectorSmall );

//=====================================================================
// TimeSpan Benchmarks
//=====================================================================

static void BM_TimeSpan_Single( benchmark::State& state )
{
    nfx::time::TimeSpan value = nfx::time::TimeSpan::fromHours( 24 );

    for( auto _ : state )
    {
        Serializer<nfx::time::TimeSpan> ser;
        std::string result = ser.toDocument( value ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_TimeSpan_Single );

static void BM_TimeSpan_VectorSmall( benchmark::State& state )
{
    std::vector<nfx::time::TimeSpan> values;
    for( int i = 0; i < 10; ++i )
    {
        values.push_back( nfx::time::TimeSpan::fromHours( i ) );
    }

    for( auto _ : state )
    {
        Serializer<std::vector<nfx::time::TimeSpan>> ser;
        std::string result = ser.toDocument( values ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_TimeSpan_VectorSmall );

//=====================================================================
// DateTime Benchmarks
//=====================================================================

static void BM_DateTime_Single( benchmark::State& state )
{
    nfx::time::DateTime value = nfx::time::DateTime::now();

    for( auto _ : state )
    {
        Serializer<nfx::time::DateTime> ser;
        std::string result = ser.toDocument( value ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_DateTime_Single );

static void BM_DateTime_VectorSmall( benchmark::State& state )
{
    std::vector<nfx::time::DateTime> values;
    nfx::time::DateTime base = nfx::time::DateTime::now();
    for( int i = 0; i < 10; ++i )
    {
        values.push_back( base + nfx::time::TimeSpan::fromSeconds( i * 3600 ) );
    }

    for( auto _ : state )
    {
        Serializer<std::vector<nfx::time::DateTime>> ser;
        std::string result = ser.toDocument( values ).toString();
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_DateTime_VectorSmall );

//=====================================================================
// Comparison Benchmarks: BuilderTraits (toString) vs SerializationTraits (serialize)
//=====================================================================

// Int128 comparison
static void BM_Int128_BuilderTraits( benchmark::State& state )
{
    nfx::datatypes::Int128 value( static_cast<int64_t>( 123456789012LL ) );

    for( auto _ : state )
    {
        Serializer<nfx::datatypes::Int128> ser;
        std::string result = ser.toString( value ); // ← Uses BuilderTraits
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Int128_BuilderTraits );

static void BM_Int128_SerializationTraits( benchmark::State& state )
{
    nfx::datatypes::Int128 value( static_cast<int64_t>( 123456789012LL ) );

    for( auto _ : state )
    {
        Serializer<nfx::datatypes::Int128> ser;
        std::string result = ser.toDocument( value ).toString(); // ← Uses SerializationTraits
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Int128_SerializationTraits );

// Decimal comparison
static void BM_Decimal_BuilderTraits( benchmark::State& state )
{
    nfx::datatypes::Decimal value( "123.456789" );

    for( auto _ : state )
    {
        Serializer<nfx::datatypes::Decimal> ser;
        std::string result = ser.toString( value ); // ← Uses BuilderTraits
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Decimal_BuilderTraits );

static void BM_Decimal_SerializationTraits( benchmark::State& state )
{
    nfx::datatypes::Decimal value( "123.456789" );

    for( auto _ : state )
    {
        Serializer<nfx::datatypes::Decimal> ser;
        std::string result = ser.toDocument( value ).toString(); // ← Uses SerializationTraits
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_Decimal_SerializationTraits );

// DateTime comparison
static void BM_DateTime_BuilderTraits( benchmark::State& state )
{
    nfx::time::DateTime value = nfx::time::DateTime::now();

    for( auto _ : state )
    {
        Serializer<nfx::time::DateTime> ser;
        std::string result = ser.toString( value ); // ← Uses BuilderTraits
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_DateTime_BuilderTraits );

static void BM_DateTime_SerializationTraits( benchmark::State& state )
{
    nfx::time::DateTime value = nfx::time::DateTime::now();

    for( auto _ : state )
    {
        Serializer<nfx::time::DateTime> ser;
        std::string result = ser.toDocument( value ).toString(); // ← Uses SerializationTraits
        benchmark::DoNotOptimize( result );
    }
}
BENCHMARK( BM_DateTime_SerializationTraits );

BENCHMARK_MAIN();
