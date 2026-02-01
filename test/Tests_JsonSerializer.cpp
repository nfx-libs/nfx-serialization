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
 * @file Tests_JsonSerializer.cpp
 * @brief Unit tests for JSON Serializer functionality
 * @details Tests covering all type specializations including primitive types,
 *          containers, smart pointers, optional types, nfx datatypes, and custom structs.
 *          Demonstrates how to specialize SerializationTraits for user-defined types.
 *          Validates serialization roundtrip integrity (serialize → deserialize == original).
 */

#include <gtest/gtest.h>

#include <nfx/Serialization.h>

#include <array>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace nfx::json;

namespace nfx::serialization::json::test
{
    //=====================================================================
    // Custom types for demonstrating SerializationTraits specialization
    //=====================================================================

    //----------------------------------------------
    // Custom HashMap-like container
    //----------------------------------------------

    /**
     * @brief Custom hash map container for demonstrating container trait specialization
     * @tparam K Key type
     * @tparam V Value type
     */
    template <typename K, typename V>
    struct CustomHashMap
    {
        using value_type = std::pair<const K, V>;
        using key_type = K;
        using mapped_type = V;

        std::unordered_map<K, V> data;

        bool operator==( const CustomHashMap& other ) const
        {
            return data == other.data;
        }

        void insert( const K& key, const V& value )
        {
            data[key] = value;
        }

        void insert( const value_type& pair )
        {
            data.insert( pair );
        }

        V& operator[]( const K& key )
        {
            return data[key];
        }

        const V& operator[]( const K& key ) const
        {
            return data.at( key );
        }

        auto begin() const
        {
            return data.begin();
        }

        auto end() const
        {
            return data.end();
        }

        auto begin()
        {
            return data.begin();
        }

        auto end()
        {
            return data.end();
        }

        size_t size() const
        {
            return data.size();
        }

        bool empty() const
        {
            return data.empty();
        }
    };

    //----------------------------------------------
    // Custom Set-like container
    //----------------------------------------------

    /**
     * @brief Custom set container for demonstrating container trait specialization
     * @tparam T Element type
     */
    template <typename T>
    struct CustomSet
    {
        using value_type = T;

        std::unordered_set<T> data;

        bool operator==( const CustomSet& other ) const
        {
            return data == other.data;
        }

        void insert( const T& value )
        {
            data.insert( value );
        }

        auto begin() const
        {
            return data.begin();
        }

        auto end() const
        {
            return data.end();
        }

        auto begin()
        {
            return data.begin();
        }

        auto end()
        {
            return data.end();
        }

        size_t size() const
        {
            return data.size();
        }

        bool empty() const
        {
            return data.empty();
        }
    };

    //----------------------------------------------
    // Person struct with custom serialize/deserialize methods
    //----------------------------------------------

    /**
     * @brief Person struct demonstrating custom serialization methods
     */
    struct Person
    {
        std::string name;
        int age;
        std::optional<std::string> email;
        std::vector<std::string> hobbies;

        bool operator==( const Person& other ) const
        {
            return name == other.name && age == other.age && email == other.email && hobbies == other.hobbies;
        }

        // Custom toDocument method
        void toDocument( const Serializer<Person>& serializer, Document& doc ) const
        {
            doc.set<std::string>( "/name", name );
            doc.set<int64_t>( "/age", age );

            if( email.has_value() )
            {
                doc.set<std::string>( "/email", *email );
            }
            else if( serializer.options().includeNullFields )
            {
                // Include null email field if requested
                doc.set<std::string>( "/email", "" );
            }

            // Serialize hobbies array
            if( !hobbies.empty() )
            {
                Array hobbiesArray;
                for( const auto& hobby : hobbies )
                {
                    hobbiesArray.push_back( Document( hobby ) );
                }
                doc.set<Array>( "/hobbies", hobbiesArray );
            }
        }

        // Custom fromDocument method
        void fromDocument( const Document& doc, const Serializer<Person>& serializer )
        {
            if( auto nameVal = doc.get<std::string>( "/name" ) )
            {
                name = *nameVal;
            }

            if( auto ageVal = doc.get<int64_t>( "/age" ) )
            {
                age = static_cast<int>( *ageVal );
            }

            if( auto emailVal = doc.get<std::string>( "/email" ) )
            {
                email = *emailVal;
            }
            else
            {
                email = std::nullopt;
            }

            // Deserialize hobbies array directly
            hobbies.clear();
            if( auto hobbiesDoc = doc.get<Document>( "/hobbies" ) )
            {
                if( auto arrRef = hobbiesDoc->rootRef<Array>() )
                {
                    for( const auto& hobbyDoc : arrRef->get() )
                    {
                        if( auto hobbyStr = hobbyDoc.root<std::string>() )
                        {
                            hobbies.push_back( *hobbyStr );
                        }
                    }
                }
            }

            if( serializer.options().validateOnDeserialize )
            {
                if( age < 0 || age > 150 )
                {
                    throw std::runtime_error{ "Invalid age value" };
                }
            }
        }
    };

    //----------------------------------------------
    // Product struct with Document-returning serialize method
    //----------------------------------------------

    /**
     * @brief Product struct demonstrating Document-returning serialization
     */
    struct Product
    {
        std::string id;
        std::string name;
        double price;
        int quantity;

        bool operator==( const Product& other ) const
        {
            return id == other.id && name == other.name && std::abs( price - other.price ) < 0.0001 &&
                   quantity == other.quantity;
        }

        // Custom toDocument method
        void toDocument( const Serializer<Product>&, Document& doc ) const
        {
            doc.set<std::string>( "/id", id );
            doc.set<std::string>( "/name", name );
            doc.set<double>( "/price", price );
            doc.set<int64_t>( "/quantity", quantity );
        }

        // Custom fromDocument method
        void fromDocument( const Document& doc, const Serializer<Product>& )
        {
            if( auto idVal = doc.get<std::string>( "/id" ) )
                id = *idVal;
            if( auto nameVal = doc.get<std::string>( "/name" ) )
                name = *nameVal;
            if( auto priceVal = doc.get<double>( "/price" ) )
                price = *priceVal;
            if( auto quantityVal = doc.get<int64_t>( "/quantity" ) )
                quantity = static_cast<int>( *quantityVal );
        }
    };

    //=====================================================================
    // JSON Serializer tests
    //=====================================================================

    //----------------------------------------------
    // Test fixture
    //----------------------------------------------

    class JSONSerializerTest : public ::testing::Test
    {
    protected:
        template <typename T>
        void testRoundTrip( const T& original, const typename Serializer<T>::Options& options = {} )
        {
            // Serialize to JSON string
            std::string jsonStr = Serializer<T>::toString( original, options );

            // Verify JSON is not empty
            EXPECT_FALSE( jsonStr.empty() ) << "Serialized JSON should not be empty";

            // Deserialize back from JSON string
            T deserialized = Serializer<T>::fromString( jsonStr, options );

            // Verify roundtrip integrity
            EXPECT_EQ( original, deserialized ) << "Roundtrip must preserve original value";
        }
    };

    //----------------------------------------------
    // Basic primitive types
    //----------------------------------------------

    TEST_F( JSONSerializerTest, BooleanTypes )
    {
        testRoundTrip( true );
        testRoundTrip( false );
    }

    TEST_F( JSONSerializerTest, IntegerTypes )
    {
        testRoundTrip( int{ 42 } );
        testRoundTrip( int{ -42 } );
        testRoundTrip( int{ 0 } );
        testRoundTrip( std::int64_t{ 1234567890123LL } );
        testRoundTrip( std::int32_t{ -2147483648 } );
    }

    TEST_F( JSONSerializerTest, FloatingPointTypes )
    {
        testRoundTrip( double{ 3.14159 } );
        testRoundTrip( float{ -2.71828f } );
        testRoundTrip( double{ 0.0 } );
    }

    TEST_F( JSONSerializerTest, StringTypes )
    {
        testRoundTrip( std::string{ "Hello, World!" } );
        testRoundTrip( std::string{ "" } );
        testRoundTrip( std::string{ "Unicode: 你好 🌍" } );
    }

    //----------------------------------------------
    // STL containers
    //----------------------------------------------

    TEST_F( JSONSerializerTest, VectorTypes )
    {
        testRoundTrip( std::vector<int>{ 1, 2, 3, 4, 5 } );
        testRoundTrip( std::vector<std::string>{ "a", "b", "c" } );
        testRoundTrip( std::vector<int>{} );
        testRoundTrip( std::vector<bool>{ true, false, true } );
    }

    TEST_F( JSONSerializerTest, ArrayTypes )
    {
        testRoundTrip( std::array<int, 3>{ { 1, 2, 3 } } );
        testRoundTrip( std::array<std::string, 2>{ { "hello", "world" } } );
    }

    TEST_F( JSONSerializerTest, MapTypes )
    {
        testRoundTrip( std::map<std::string, int>{ { "one", 1 }, { "two", 2 }, { "three", 3 } } );
        testRoundTrip( std::map<std::string, std::string>{ { "key1", "value1" }, { "key2", "value2" } } );
        testRoundTrip( std::map<std::string, int>{} );
    }

    TEST_F( JSONSerializerTest, UnorderedMapTypes )
    {
        testRoundTrip( std::unordered_map<std::string, int>{ { "alpha", 1 }, { "beta", 2 } } );
        testRoundTrip( std::unordered_map<std::string, double>{ { "pi", 3.14 }, { "e", 2.71 } } );
    }

    TEST_F( JSONSerializerTest, SetTypes )
    {
        testRoundTrip( std::set<int>{ 1, 2, 3, 4, 5 } );
        testRoundTrip( std::set<std::string>{ "apple", "banana", "cherry" } );
        testRoundTrip( std::set<int>{} );
    }

    TEST_F( JSONSerializerTest, UnorderedSetTypes )
    {
        testRoundTrip( std::unordered_set<int>{ 1, 2, 3, 4, 5 } );
        testRoundTrip( std::unordered_set<std::string>{ "apple", "banana", "cherry" } );
        testRoundTrip( std::unordered_set<int>{} );
    }

    TEST_F( JSONSerializerTest, ListTypes )
    {
        testRoundTrip( std::list<int>{ 10, 20, 30 } );
        testRoundTrip( std::list<std::string>{ "first", "second", "third" } );
    }

    TEST_F( JSONSerializerTest, DequeTypes )
    {
        testRoundTrip( std::deque<int>{ 100, 200, 300 } );
        testRoundTrip( std::deque<double>{ 1.1, 2.2, 3.3 } );
    }

    TEST_F( JSONSerializerTest, TupleTypes )
    {
        testRoundTrip( std::make_tuple( 42, std::string( "hello" ) ) );

        testRoundTrip( std::make_tuple( 1, 2.5, std::string( "test" ) ) );

        testRoundTrip( std::make_tuple( std::string( "data" ), 100, true, 3.14 ) );

        testRoundTrip( std::tuple<>{} );

        testRoundTrip( std::make_tuple( 42 ) );

        testRoundTrip( std::make_tuple( std::vector<int>{ 1, 2, 3 }, std::string( "vec" ) ) );
    }

    TEST_F( JSONSerializerTest, TupleNested )
    {
        auto nested =
            std::make_tuple( std::make_tuple( 1, 2 ), std::make_tuple( std::string( "a" ), std::string( "b" ) ) );
        testRoundTrip( nested );

        std::vector<std::tuple<int, std::string>> vecOfTuples = { std::make_tuple( 1, std::string( "one" ) ),
                                                                  std::make_tuple( 2, std::string( "two" ) ),
                                                                  std::make_tuple( 3, std::string( "three" ) ) };
        testRoundTrip( vecOfTuples );

        std::map<std::string, std::tuple<int, double>> mapWithTuples = { { "first", std::make_tuple( 1, 1.1 ) },
                                                                         { "second", std::make_tuple( 2, 2.2 ) } };
        testRoundTrip( mapWithTuples );
    }

    //----------------------------------------------
    // Custom containers with trait specialization
    //----------------------------------------------

    TEST_F( JSONSerializerTest, CustomHashMapTypes )
    {
        // Test CustomHashMap<string, int>
        {
            CustomHashMap<std::string, int> customMap;
            customMap.insert( "one", 1 );
            customMap.insert( "two", 2 );
            customMap.insert( "three", 3 );
            testRoundTrip( customMap );
        }

        // Test CustomHashMap<string, string>
        {
            CustomHashMap<std::string, std::string> customMap;
            customMap.insert( "greeting", "hello" );
            customMap.insert( "farewell", "goodbye" );
            testRoundTrip( customMap );
        }

        // Test empty CustomHashMap
        {
            CustomHashMap<std::string, int> emptyMap;
            testRoundTrip( emptyMap );
        }
    }

    TEST_F( JSONSerializerTest, CustomSetTypes )
    {
        // Test CustomSet<string>
        {
            CustomSet<std::string> customSet;
            customSet.insert( "apple" );
            customSet.insert( "banana" );
            customSet.insert( "cherry" );
            testRoundTrip( customSet );
        }

        // Test CustomSet<int>
        {
            CustomSet<int> customSet;
            customSet.insert( 1 );
            customSet.insert( 2 );
            customSet.insert( 3 );
            testRoundTrip( customSet );
        }

        // Test empty CustomSet
        {
            CustomSet<std::string> emptySet;
            testRoundTrip( emptySet );
        }
    }

    //----------------------------------------------
    // Optional types
    //----------------------------------------------

    TEST_F( JSONSerializerTest, OptionalTypes )
    {
        testRoundTrip( std::optional<int>{ 42 } );
        testRoundTrip( std::optional<std::string>{ "Hello" } );
        testRoundTrip( std::optional<double>{ 3.14 } );
        testRoundTrip( std::optional<int>{} );
        testRoundTrip( std::optional<std::string>{} );
    }

    //----------------------------------------------
    // Smart pointer types
    //----------------------------------------------

    TEST_F( JSONSerializerTest, UniquePointerTypes )
    {
        {
            auto original = std::make_unique<int>( 123 );
            std::string jsonStr = Serializer<std::unique_ptr<int>>::toString( original );
            auto deserialized = Serializer<std::unique_ptr<int>>::fromString( jsonStr );
            EXPECT_TRUE( original && deserialized );
            EXPECT_EQ( *original, *deserialized );
        }

        {
            std::unique_ptr<int> original = nullptr;
            std::string jsonStr = Serializer<std::unique_ptr<int>>::toString( original );
            auto deserialized = Serializer<std::unique_ptr<int>>::fromString( jsonStr );
            EXPECT_FALSE( original );
            EXPECT_FALSE( deserialized );
        }
    }

    TEST_F( JSONSerializerTest, SharedPointerTypes )
    {
        {
            auto original = std::make_shared<int>( 456 );
            std::string jsonStr = Serializer<std::shared_ptr<int>>::toString( original );
            auto deserialized = Serializer<std::shared_ptr<int>>::fromString( jsonStr );
            EXPECT_TRUE( original && deserialized );
            EXPECT_EQ( *original, *deserialized );
        }

        {
            std::shared_ptr<std::string> original = nullptr;
            std::string jsonStr = Serializer<std::shared_ptr<std::string>>::toString( original );
            auto deserialized = Serializer<std::shared_ptr<std::string>>::fromString( jsonStr );
            EXPECT_FALSE( original );
            EXPECT_FALSE( deserialized );
        }
    }

    //----------------------------------------------
    // Custom structs with serialize/deserialize methods
    //----------------------------------------------

    TEST_F( JSONSerializerTest, PersonStructWithCustomMethods )
    {
        {
            Person person;
            person.name = "Alice";
            person.age = 30;
            person.email = "alice@example.com";
            person.hobbies = { "reading", "hiking", "coding" };
            testRoundTrip( person );
        }

        {
            Person person;
            person.name = "Bob";
            person.age = 25;
            person.email = std::nullopt;
            person.hobbies = {};
            testRoundTrip( person );
        }
    }

    TEST_F( JSONSerializerTest, ProductStructWithDocumentReturn )
    {
        Product product;
        product.id = "PROD-001";
        product.name = "Widget";
        product.price = 19.99;
        product.quantity = 100;
        testRoundTrip( product );
    }

    //----------------------------------------------
    // Nested containers
    //----------------------------------------------

    TEST_F( JSONSerializerTest, NestedContainers )
    {
        // Vector of maps
        {
            std::vector<std::map<std::string, int>> vectorOfMaps{ { { "a", 1 }, { "b", 2 } },
                                                                  { { "x", 10 }, { "y", 20 } },
                                                                  {} };
            testRoundTrip( vectorOfMaps );
        }

        // Map of vectors
        {
            std::map<std::string, std::vector<int>> mapOfVectors{ { "numbers", { 1, 2, 3 } },
                                                                  { "primes", { 2, 3, 5, 7 } },
                                                                  { "empty", {} } };
            testRoundTrip( mapOfVectors );
        }

        // Map of maps
        {
            std::map<std::string, std::map<std::string, int>> mapOfMaps{
                { "group1", { { "item1", 1 }, { "item2", 2 } } }, { "group2", { { "itemA", 10 }, { "itemB", 20 } } }
            };
            testRoundTrip( mapOfMaps );
        }
    }

    //----------------------------------------------
    // Complex nested structures with custom types
    //----------------------------------------------

    TEST_F( JSONSerializerTest, ComplexNestedStructures )
    {
        // CustomHashMap with vector values
        {
            CustomHashMap<std::string, std::vector<int>> customMap;
            customMap.insert( "primes", { 2, 3, 5, 7, 11 } );
            customMap.insert( "fibonacci", { 1, 1, 2, 3, 5, 8 } );
            customMap.insert( "empty", {} );
            testRoundTrip( customMap );
        }

        // Vector of Persons
        {
            std::vector<Person> people;

            Person p1;
            p1.name = "Alice";
            p1.age = 30;
            p1.email = "alice@example.com";
            p1.hobbies = { "reading", "hiking" };
            people.push_back( p1 );

            Person p2;
            p2.name = "Bob";
            p2.age = 25;
            p2.email = std::nullopt;
            p2.hobbies = { "gaming" };
            people.push_back( p2 );

            testRoundTrip( people );
        }

        // Map of Products
        {
            std::map<std::string, Product> products;

            Product prod1;
            prod1.id = "PROD-001";
            prod1.name = "Widget";
            prod1.price = 19.99;
            prod1.quantity = 100;
            products["widget"] = prod1;

            Product prod2;
            prod2.id = "PROD-002";
            prod2.name = "Gadget";
            prod2.price = 29.99;
            prod2.quantity = 50;
            products["gadget"] = prod2;

            testRoundTrip( products );
        }
    }

    //----------------------------------------------
    // Serialization options
    //----------------------------------------------

    TEST_F( JSONSerializerTest, SerializationOptions )
    {
        // Test includeNullFields option
        {
            Person person;
            person.name = "Charlie";
            person.age = 35;
            person.email = std::nullopt;
            person.hobbies = {};

            Serializer<Person>::Options options;
            options.includeNullFields = true;

            std::string jsonStr = Serializer<Person>::toString( person, options );

            EXPECT_NE( jsonStr.find( "email" ), std::string::npos ) << "Null email field should be included";
        }

        // Test prettyPrint option
        {
            std::map<std::string, int> data{ { "one", 1 }, { "two", 2 } };

            Serializer<std::map<std::string, int>>::Options options;
            options.prettyPrint = true;

            std::string jsonStr = Serializer<std::map<std::string, int>>::toString( data, options );

            EXPECT_NE( jsonStr.find( "\n" ), std::string::npos ) << "Pretty printed JSON should contain newlines";
        }

        // Test validateOnDeserialize option
        {
            Person person;
            person.name = "Invalid";
            person.age = 200; // Invalid age
            person.email = std::nullopt;
            person.hobbies = {};

            Serializer<Person>::Options options;
            options.validateOnDeserialize = true;

            std::string jsonStr = Serializer<Person>::toString( person, options );

            // Should throw during deserialization due to invalid age
            EXPECT_THROW( Serializer<Person>::fromString( jsonStr, options ), std::runtime_error );
        }
    }

    //----------------------------------------------
    // Convenience functions
    //----------------------------------------------

    TEST_F( JSONSerializerTest, ConvenienceFunctions )
    {
        // Test toJson/fromJson static methods
        {
            std::vector<int> original{ 1, 2, 3, 4, 5 };
            std::string jsonStr = Serializer<std::vector<int>>::toString( original );
            EXPECT_FALSE( jsonStr.empty() );

            std::vector<int> deserialized = Serializer<std::vector<int>>::fromString( jsonStr );
            EXPECT_EQ( original, deserialized );
        }

        {
            Person original;
            original.name = "Test";
            original.age = 30;
            original.email = "test@example.com";
            original.hobbies = { "testing" };

            std::string jsonStr = Serializer<Person>::toString( original );
            EXPECT_FALSE( jsonStr.empty() );

            Person deserialized = Serializer<Person>::fromString( jsonStr );
            EXPECT_EQ( original, deserialized );
        }
    }

    //----------------------------------------------
    // Stress test with large data structures
    //----------------------------------------------

    TEST_F( JSONSerializerTest, LargeDataStructures )
    {
        // Large vector
        {
            std::vector<int> largeVector;
            for( int i = 0; i < 1000; ++i )
            {
                largeVector.push_back( i );
            }
            testRoundTrip( largeVector );
        }

        // Large map
        {
            std::map<std::string, int> largeMap;
            for( int i = 0; i < 500; ++i )
            {
                std::string key = "key_" + std::to_string( i );
                largeMap[key] = i * i;
            }
            testRoundTrip( largeMap );
        }

        // Large custom container
        {
            CustomHashMap<std::string, int> largeCustomMap;
            for( int i = 0; i < 250; ++i )
            {
                std::string key = "custom_key_" + std::to_string( i );
                largeCustomMap.insert( key, i * 2 );
            }
            testRoundTrip( largeCustomMap );
        }
    }
} // namespace nfx::serialization::json::test

//=====================================================================
// SerializationTraits specializations for custom types
//=====================================================================

namespace nfx::serialization::json::detail
{
    // Specialize is_container for CustomHashMap
    template <typename K, typename V>
    struct is_container<::nfx::serialization::json::test::CustomHashMap<K, V>> : std::true_type
    {
    };

    // Specialize is_container for CustomSet
    template <typename T>
    struct is_container<::nfx::serialization::json::test::CustomSet<T>> : std::true_type
    {
    };
} // namespace nfx::serialization::json::detail
