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
 * @file Tests_JsonObjects.cpp
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

    //----------------------------------------------
    // Nested object/array access via Object::get<Object/Array>
    //----------------------------------------------

    TEST_F( JSONObjectTest, GetNestedObjectFromObject )
    {
        // Get the user object
        auto userObj = testDoc.get<Document::Object>( "user" );
        ASSERT_TRUE( userObj.has_value() );

        // a pointer to a temporary Document that would be destroyed
        auto prefsObj = userObj.value().get<Document::Object>( "preferences" );
        ASSERT_TRUE( prefsObj.has_value() );

        // Access fields from the nested object to ensure it's still valid
        auto theme = prefsObj.value().get<std::string>( "theme" );
        ASSERT_TRUE( theme.has_value() );
        EXPECT_EQ( theme.value(), "dark" );

        auto notifications = prefsObj.value().get<bool>( "notifications" );
        ASSERT_TRUE( notifications.has_value() );
        EXPECT_TRUE( notifications.value() );
    }

    TEST_F( JSONObjectTest, GetNestedArrayFromObject )
    {
        // Get the user object
        auto userObj = testDoc.get<Document::Object>( "user" );
        ASSERT_TRUE( userObj.has_value() );

        // a pointer to a temporary Document that would be destroyed
        auto hobbiesArray = userObj.value().get<Document::Array>( "hobbies" );
        ASSERT_TRUE( hobbiesArray.has_value() );

        // Access elements from the array to ensure it's still valid
        EXPECT_EQ( hobbiesArray.value().size(), 3 );

        auto hobby0 = hobbiesArray.value().get<std::string>( 0 );
        ASSERT_TRUE( hobby0.has_value() );
        EXPECT_EQ( hobby0.value(), "reading" );

        auto hobby1 = hobbiesArray.value().get<std::string>( 1 );
        ASSERT_TRUE( hobby1.has_value() );
        EXPECT_EQ( hobby1.value(), "gaming" );

        auto hobby2 = hobbiesArray.value().get<std::string>( 2 );
        ASSERT_TRUE( hobby2.has_value() );
        EXPECT_EQ( hobby2.value(), "cooking" );
    }

    TEST_F( JSONObjectTest, GetNestedObjectAndModifyParent )
    {
        // This test ensures the returned nested object remains valid
        // even if we continue working with the parent
        auto userObj = testDoc.get<Document::Object>( "user" );
        ASSERT_TRUE( userObj.has_value() );

        auto prefsObj = userObj.value().get<Document::Object>( "preferences" );
        ASSERT_TRUE( prefsObj.has_value() );

        // Access parent object after getting nested object
        auto userName = userObj.value().get<std::string>( "name" );
        ASSERT_TRUE( userName.has_value() );
        EXPECT_EQ( userName.value(), "Alice" );

        // Nested object should still be valid
        auto theme = prefsObj.value().get<std::string>( "theme" );
        ASSERT_TRUE( theme.has_value() );
        EXPECT_EQ( theme.value(), "dark" );
    }
} // namespace nfx::serialization::json::test
