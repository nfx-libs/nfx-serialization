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
 * @file BM_JSONIteration.cpp
 * @brief Benchmarks for JSON container iteration operations
 */

#include <benchmark/benchmark.h>

#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::benchmark
{
	//----------------------------------------------
	// Test data setup
	//----------------------------------------------

	static Document createObjectDocument()
	{
		Document doc;
		doc.set<std::string>( "/field1", "value1" );
		doc.set<std::string>( "/field2", "value2" );
		doc.set<std::string>( "/field3", "value3" );
		doc.set<std::string>( "/field4", "value4" );
		doc.set<std::string>( "/field5", "value5" );
		doc.set<int>( "/field6", 100 );
		doc.set<int>( "/field7", 200 );
		doc.set<int>( "/field8", 300 );
		doc.set<bool>( "/field9", true );
		doc.set<bool>( "/field10", false );
		return doc;
	}

	static Document createArrayDocument()
	{
		Document doc;
		doc.set<Document::Array>( "" ); // Create root as array
		auto arr = doc.get<Document::Array>( "" ).value();
		for ( int64_t i = 0; i < 100; ++i )
		{
			arr.append<int64_t>( i );
		}
		return doc;
	}

	static Document createNestedObjectDocument()
	{
		Document doc;
		doc.set<std::string>( "/user/name", "John" );
		doc.set<int>( "/user/age", 30 );
		doc.set<std::string>( "/user/address/city", "NYC" );
		doc.set<std::string>( "/user/address/country", "USA" );
		doc.set<std::string>( "/company/name", "Acme" );
		doc.set<int>( "/company/size", 500 );
		doc.set<std::string>( "/company/location/city", "LA" );
		doc.set<std::string>( "/company/location/country", "USA" );
		return doc;
	}

	//----------------------------------------------
	// Object Field Iteration benchmarks
	//----------------------------------------------

	static void BM_ObjectFieldIteration( ::benchmark::State& state )
	{
		Document doc = createObjectDocument();
		auto obj = doc.get<Document::Object>( "" ).value();

		for ( auto _ : state )
		{
			int count = 0;
			for ( const auto& [key, value] : obj )
			{
				auto k = key;
				auto v = value;
				::benchmark::DoNotOptimize( k );
				::benchmark::DoNotOptimize( v );
				++count;
			}
			::benchmark::DoNotOptimize( count );
		}
	}

	//----------------------------------------------
	// Array Element Iteration benchmarks
	//----------------------------------------------

	static void BM_ArrayElementIteration( ::benchmark::State& state )
	{
		Document doc = createArrayDocument();
		auto arr = doc.get<Document::Array>( "" ).value();

		for ( auto _ : state )
		{
			int sum = 0;
			for ( const auto& elem : arr )
			{
				sum += elem.get<int>( "" ).value_or( 0 );
			}
			::benchmark::DoNotOptimize( sum );
		}
	}

	//----------------------------------------------
	// Nested Iteration benchmarks
	//----------------------------------------------

	static void BM_NestedObjectIteration( ::benchmark::State& state )
	{
		Document doc = createNestedObjectDocument();

		for ( auto _ : state )
		{
			int count = 0;
			for ( const auto& entry : Document::PathView( doc ) )
			{
				auto path = entry.path;
				auto value = entry.value();
				::benchmark::DoNotOptimize( path );
				::benchmark::DoNotOptimize( value );
				++count;
			}
			::benchmark::DoNotOptimize( count );
		}
	}
} // namespace nfx::serialization::json::benchmark

//=====================================================================
// Benchmark Registration
//=====================================================================

BENCHMARK( nfx::serialization::json::benchmark::BM_ObjectFieldIteration );
BENCHMARK( nfx::serialization::json::benchmark::BM_ArrayElementIteration );
BENCHMARK( nfx::serialization::json::benchmark::BM_NestedObjectIteration );

BENCHMARK_MAIN();
