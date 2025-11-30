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
 * @file TESTS_JSONObjects.cpp
 * @brief Comprehensive tests for JSON Object class functionality
 * @details Tests covering object construction, serialization methods, factory methods,
 *          validation, and integration with Document system
 */

#include <gtest/gtest.h>

#include <nfx/serialization/json/Document.h>

namespace nfx::serialization::json::test
{
	//=====================================================================
	// Object serialization and factory methods tests
	//=====================================================================

	class JSONObjectTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			// Create test document with object structure
			std::string jsonStr = R"({
				"user": {
					"name": "Alice",
					"age": 30,
					"active": true,
					"height": 1.65,
					"spouse": null,
					"preferences": {
						"theme": "dark",
						"notifications": true
					},
					"hobbies": ["reading", "gaming", "cooking"]
				},
				"settings": {
					"volume": 0.8,
					"language": "en",
					"debug": false
				}
			})";

			auto doc = Document::fromString( jsonStr );
			ASSERT_TRUE( doc.has_value() );
			testDoc = std::move( doc.value() );
		}

		Document testDoc;
	};

	//----------------------------------------------
	// Object serialization methods (toJsonString/toJsonBytes)
	//----------------------------------------------

	TEST_F( JSONObjectTest, ToJsonStringEmpty )
	{
		Document emptyDoc;
		auto emptyObj = emptyDoc.get<Document::Object>( "" );
		ASSERT_TRUE( emptyObj.has_value() );

		std::string jsonStr = emptyObj.value().toString();
		EXPECT_EQ( jsonStr, "{}" );
	}

	TEST_F( JSONObjectTest, ToJsonBytesEmpty )
	{
		Document emptyDoc;
		auto emptyObj = emptyDoc.get<Document::Object>( "" );
		ASSERT_TRUE( emptyObj.has_value() );

		std::vector<uint8_t> jsonBytes = emptyObj.value().toBytes();
		std::string jsonStr( jsonBytes.begin(), jsonBytes.end() );
		EXPECT_EQ( jsonStr, "{}" );
	}

	//----------------------------------------------
	// Object validation methods (isValid/lastError)
	//----------------------------------------------

	TEST_F( JSONObjectTest, IsValidForValidObject )
	{
		auto userObj = testDoc.get<Document::Object>( "user" );
		ASSERT_TRUE( userObj.has_value() );

		EXPECT_TRUE( userObj.value().isValid() );
		EXPECT_TRUE( userObj.value().lastError().empty() );
	}

	TEST_F( JSONObjectTest, IsValidForEmptyObject )
	{
		Document emptyDoc;
		auto emptyObj = emptyDoc.get<Document::Object>( "" );
		ASSERT_TRUE( emptyObj.has_value() );

		EXPECT_TRUE( emptyObj.value().isValid() );
		EXPECT_TRUE( emptyObj.value().lastError().empty() );
	}
} // namespace nfx::serialization::json::test
