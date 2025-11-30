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
 * @brief Benchmarks for JSON Document serialization to string
 */

#include <benchmark/benchmark.h>

#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::benchmark
{
	//----------------------------------------------
	// Test data setup
	//----------------------------------------------

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

	//----------------------------------------------
	// Serialize to String benchmarks
	//----------------------------------------------

	static void BM_SerializeSmallObject( ::benchmark::State& state )
	{
		Document doc = createSmallDocument();

		for ( auto _ : state )
		{
			std::string json = doc.toString();
			::benchmark::DoNotOptimize( json );
		}
	}

	static void BM_SerializeLargeObject( ::benchmark::State& state )
	{
		Document doc = createLargeDocument();

		for ( auto _ : state )
		{
			std::string json = doc.toString();
			::benchmark::DoNotOptimize( json );
		}
	}

	static void BM_SerializePrettyPrint( ::benchmark::State& state )
	{
		Document doc = createLargeDocument();

		for ( auto _ : state )
		{
			std::string json = doc.toString( 2 ); // 2-space indent
			::benchmark::DoNotOptimize( json );
		}
	}
} // namespace nfx::serialization::json::benchmark

//=====================================================================
// Benchmark Registration
//=====================================================================

BENCHMARK( nfx::serialization::json::benchmark::BM_SerializeSmallObject );
BENCHMARK( nfx::serialization::json::benchmark::BM_SerializeLargeObject );
BENCHMARK( nfx::serialization::json::benchmark::BM_SerializePrettyPrint );

BENCHMARK_MAIN();
