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
 * @file TESTS_JSONFormats.cpp
 * @brief Unit tests for JSON Schema format validation
 * @details Tests all JSON Schema Draft 2020-12 format validators
 */

#include <gtest/gtest.h>

#include "nfx/serialization/json/Document.h"
#include "nfx/serialization/json/SchemaValidator.h"

namespace nfx::serialization::json::test
{
	//=====================================================================
	// Format validation tests
	//=====================================================================

	class FormatValidationTest : public ::testing::Test
	{
	protected:
		SchemaValidator createValidator( std::string_view format )
		{
			std::string schemaJson = R"({"type": "string", "format": ")" + std::string{ format } + R"("})";
			auto schema = Document::fromString( schemaJson );
			EXPECT_TRUE( schema.has_value() );
			return SchemaValidator{ schema.value() };
		}

		bool validates( SchemaValidator& validator, std::string_view value )
		{
			std::string json = "\"" + std::string{ value } + "\"";
			auto doc = Document::fromString( json );
			EXPECT_TRUE( doc.has_value() );
			return validator.validate( doc.value() ).isValid();
		}
	};

	//----------------------------------------------
	// date-time (RFC 3339)
	//----------------------------------------------

	TEST_F( FormatValidationTest, DateTime_Valid )
	{
		auto validator = createValidator( "date-time" );

		EXPECT_TRUE( validates( validator, "2025-11-30T14:30:00Z" ) );
		EXPECT_TRUE( validates( validator, "2025-01-01T00:00:00Z" ) );
		EXPECT_TRUE( validates( validator, "2025-12-31T23:59:59Z" ) );
		EXPECT_TRUE( validates( validator, "2025-11-30T14:30:00+05:30" ) );
		EXPECT_TRUE( validates( validator, "2025-11-30T14:30:00-08:00" ) );
		EXPECT_TRUE( validates( validator, "2025-11-30T14:30:00.123Z" ) );
		EXPECT_TRUE( validates( validator, "2025-11-30t14:30:00z" ) ); // lowercase T and Z
		EXPECT_TRUE( validates( validator, "2025-06-30T23:59:60Z" ) ); // valid leap second (June 30)
		EXPECT_TRUE( validates( validator, "2025-12-31T23:59:60Z" ) ); // valid leap second (December 31)
	}

	TEST_F( FormatValidationTest, DateTime_Invalid )
	{
		auto validator = createValidator( "date-time" );

		EXPECT_FALSE( validates( validator, "2025-11-30" ) );			// date only
		EXPECT_FALSE( validates( validator, "14:30:00Z" ) );			// time only
		EXPECT_FALSE( validates( validator, "2025-13-30T14:30:00Z" ) ); // invalid month
		EXPECT_FALSE( validates( validator, "2025-11-32T14:30:00Z" ) ); // invalid day
		EXPECT_FALSE( validates( validator, "2025-11-30T25:30:00Z" ) ); // invalid hour
		EXPECT_FALSE( validates( validator, "2025-11-30T14:60:00Z" ) ); // invalid minute
		EXPECT_FALSE( validates( validator, "2025-11-30T14:30:60Z" ) ); // :60 seconds only valid for leap second
		EXPECT_FALSE( validates( validator, "2025-11-30 14:30:00Z" ) ); // space instead of T
		EXPECT_FALSE( validates( validator, "not-a-datetime" ) );
	}

	//----------------------------------------------
	// date (RFC 3339)
	//----------------------------------------------

	TEST_F( FormatValidationTest, Date_Valid )
	{
		auto validator = createValidator( "date" );

		EXPECT_TRUE( validates( validator, "2025-11-30" ) );
		EXPECT_TRUE( validates( validator, "2025-01-01" ) );
		EXPECT_TRUE( validates( validator, "2025-12-31" ) );
		EXPECT_TRUE( validates( validator, "2000-02-29" ) ); // leap year
	}

	TEST_F( FormatValidationTest, Date_Invalid )
	{
		auto validator = createValidator( "date" );

		EXPECT_FALSE( validates( validator, "2025-13-30" ) ); // invalid month
		EXPECT_FALSE( validates( validator, "2025-00-30" ) ); // zero month
		EXPECT_FALSE( validates( validator, "2025-11-32" ) ); // invalid day
		EXPECT_FALSE( validates( validator, "2025-11-00" ) ); // zero day
		EXPECT_FALSE( validates( validator, "25-11-30" ) );	  // short year
		EXPECT_FALSE( validates( validator, "2025/11/30" ) ); // wrong separator
		EXPECT_FALSE( validates( validator, "not-a-date" ) );
	}

	//----------------------------------------------
	// time (RFC 3339)
	//----------------------------------------------

	TEST_F( FormatValidationTest, Time_Valid )
	{
		auto validator = createValidator( "time" );

		EXPECT_TRUE( validates( validator, "14:30:00Z" ) );
		EXPECT_TRUE( validates( validator, "00:00:00Z" ) );
		EXPECT_TRUE( validates( validator, "23:59:59Z" ) );
		EXPECT_TRUE( validates( validator, "14:30:00+05:30" ) );
		EXPECT_TRUE( validates( validator, "14:30:00-08:00" ) );
		EXPECT_TRUE( validates( validator, "14:30:00.123Z" ) );
		EXPECT_TRUE( validates( validator, "14:30:00.123456Z" ) );
		EXPECT_TRUE( validates( validator, "23:59:60Z" ) ); // leap second (allowed at 23:59:60)
	}

	TEST_F( FormatValidationTest, Time_Invalid )
	{
		auto validator = createValidator( "time" );

		EXPECT_FALSE( validates( validator, "25:30:00Z" ) ); // invalid hour
		EXPECT_FALSE( validates( validator, "14:60:00Z" ) ); // invalid minute
		EXPECT_FALSE( validates( validator, "14:30:00" ) );	 // missing timezone
		EXPECT_FALSE( validates( validator, "14:30" ) );	 // missing seconds
		EXPECT_FALSE( validates( validator, "not-a-time" ) );
		// Note: For time-only format, :60 is allowed at 23:59 but not at other times
		// However, without date context, we cannot fully validate leap second rules
	}

	//----------------------------------------------
	// duration (ISO 8601)
	//----------------------------------------------

	TEST_F( FormatValidationTest, Duration_Valid )
	{
		auto validator = createValidator( "duration" );

		EXPECT_TRUE( validates( validator, "P1Y" ) );
		EXPECT_TRUE( validates( validator, "P1M" ) );
		EXPECT_TRUE( validates( validator, "P1D" ) );
		EXPECT_TRUE( validates( validator, "PT1H" ) );
		EXPECT_TRUE( validates( validator, "PT1M" ) );
		EXPECT_TRUE( validates( validator, "PT1S" ) );
		EXPECT_TRUE( validates( validator, "P1Y2M3DT4H5M6S" ) );
		EXPECT_TRUE( validates( validator, "P1W" ) );
		EXPECT_TRUE( validates( validator, "PT0.5S" ) );
	}

	TEST_F( FormatValidationTest, Duration_Invalid )
	{
		auto validator = createValidator( "duration" );

		EXPECT_FALSE( validates( validator, "1Y" ) );  // missing P
		EXPECT_FALSE( validates( validator, "P" ) );   // P only
		EXPECT_FALSE( validates( validator, "PT" ) );  // PT only
		EXPECT_FALSE( validates( validator, "P1H" ) ); // H without T
		EXPECT_FALSE( validates( validator, "not-a-duration" ) );
	}

	//----------------------------------------------
	// email (RFC 5321)
	//----------------------------------------------

	TEST_F( FormatValidationTest, Email_Valid )
	{
		auto validator = createValidator( "email" );

		EXPECT_TRUE( validates( validator, "user@example.com" ) );
		EXPECT_TRUE( validates( validator, "user.name@example.com" ) );
		EXPECT_TRUE( validates( validator, "user+tag@example.com" ) );
		EXPECT_TRUE( validates( validator, "user@subdomain.example.com" ) );
	}

	TEST_F( FormatValidationTest, Email_Invalid )
	{
		auto validator = createValidator( "email" );

		EXPECT_FALSE( validates( validator, "not-an-email" ) );
		EXPECT_FALSE( validates( validator, "@example.com" ) );
		EXPECT_FALSE( validates( validator, "user@" ) );
		EXPECT_FALSE( validates( validator, "user@.com" ) );
		EXPECT_FALSE( validates( validator, "" ) );
	}

	//----------------------------------------------
	// idn-email (RFC 6531)
	//----------------------------------------------

	TEST_F( FormatValidationTest, IdnEmail_Valid )
	{
		auto validator = createValidator( "idn-email" );

		EXPECT_TRUE( validates( validator, "user@example.com" ) );
		EXPECT_TRUE( validates( validator, "user.name@example.com" ) );
	}

	TEST_F( FormatValidationTest, IdnEmail_Invalid )
	{
		auto validator = createValidator( "idn-email" );

		EXPECT_FALSE( validates( validator, "not-an-email" ) );
		EXPECT_FALSE( validates( validator, "@example.com" ) );
		EXPECT_FALSE( validates( validator, "" ) );
		EXPECT_FALSE( validates( validator, "user@xn--.com" ) ); // malformed Punycode
	}

	//----------------------------------------------
	// hostname (RFC 1123)
	//----------------------------------------------

	TEST_F( FormatValidationTest, Hostname_Valid )
	{
		auto validator = createValidator( "hostname" );

		EXPECT_TRUE( validates( validator, "example.com" ) );
		EXPECT_TRUE( validates( validator, "localhost" ) );
		EXPECT_TRUE( validates( validator, "my-server.example.com" ) );
		EXPECT_TRUE( validates( validator, "server01.internal.example.com" ) );
	}

	TEST_F( FormatValidationTest, Hostname_Invalid )
	{
		auto validator = createValidator( "hostname" );

		EXPECT_FALSE( validates( validator, "-invalid.com" ) );	 // starts with hyphen
		EXPECT_FALSE( validates( validator, "invalid-.com" ) );	 // ends with hyphen
		EXPECT_FALSE( validates( validator, "inv..alid.com" ) ); // consecutive dots
		EXPECT_FALSE( validates( validator, "" ) );
	}

	//----------------------------------------------
	// idn-hostname (RFC 5890)
	//----------------------------------------------

	TEST_F( FormatValidationTest, IdnHostname_Valid )
	{
		auto validator = createValidator( "idn-hostname" );

		EXPECT_TRUE( validates( validator, "example.com" ) );
		EXPECT_TRUE( validates( validator, "localhost" ) );
	}

	TEST_F( FormatValidationTest, IdnHostname_Invalid )
	{
		auto validator = createValidator( "idn-hostname" );

		EXPECT_FALSE( validates( validator, "" ) );
		EXPECT_FALSE( validates( validator, "a]b.com" ) ); // invalid character
	}

	//----------------------------------------------
	// ipv4 (RFC 2673)
	//----------------------------------------------

	TEST_F( FormatValidationTest, IPv4_Valid )
	{
		auto validator = createValidator( "ipv4" );

		EXPECT_TRUE( validates( validator, "192.168.1.1" ) );
		EXPECT_TRUE( validates( validator, "0.0.0.0" ) );
		EXPECT_TRUE( validates( validator, "255.255.255.255" ) );
		EXPECT_TRUE( validates( validator, "10.0.0.1" ) );
		EXPECT_TRUE( validates( validator, "172.16.0.1" ) );
	}

	TEST_F( FormatValidationTest, IPv4_Invalid )
	{
		auto validator = createValidator( "ipv4" );

		EXPECT_FALSE( validates( validator, "256.1.1.1" ) );	 // octet > 255
		EXPECT_FALSE( validates( validator, "192.168.1" ) );	 // missing octet
		EXPECT_FALSE( validates( validator, "192.168.1.1.1" ) ); // extra octet
		EXPECT_FALSE( validates( validator, "192.168.1.a" ) );	 // non-numeric
		EXPECT_FALSE( validates( validator, "not-an-ip" ) );
	}

	//----------------------------------------------
	// ipv6 (RFC 4291)
	//----------------------------------------------

	TEST_F( FormatValidationTest, IPv6_Valid )
	{
		auto validator = createValidator( "ipv6" );

		EXPECT_TRUE( validates( validator, "2001:0db8:85a3:0000:0000:8a2e:0370:7334" ) );
		EXPECT_TRUE( validates( validator, "2001:db8:85a3::8a2e:370:7334" ) );
		EXPECT_TRUE( validates( validator, "::1" ) );
		EXPECT_TRUE( validates( validator, "::" ) );
		EXPECT_TRUE( validates( validator, "fe80::1" ) );
	}

	TEST_F( FormatValidationTest, IPv6_Invalid )
	{
		auto validator = createValidator( "ipv6" );

		EXPECT_FALSE( validates( validator, "192.168.1.1" ) ); // IPv4
		EXPECT_FALSE( validates( validator, "2001:db8:85a3::8a2e:370:7334:extra" ) );
		EXPECT_FALSE( validates( validator, "not-an-ipv6" ) );
	}

	//----------------------------------------------
	// uri (RFC 3986)
	//----------------------------------------------

	TEST_F( FormatValidationTest, URI_Valid )
	{
		auto validator = createValidator( "uri" );

		EXPECT_TRUE( validates( validator, "https://example.com" ) );
		EXPECT_TRUE( validates( validator, "http://example.com/path" ) );
		EXPECT_TRUE( validates( validator, "ftp://files.example.com/file.txt" ) );
		EXPECT_TRUE( validates( validator, "mailto:user@example.com" ) );
		EXPECT_TRUE( validates( validator, "urn:isbn:0451450523" ) );
	}

	TEST_F( FormatValidationTest, URI_Invalid )
	{
		auto validator = createValidator( "uri" );

		EXPECT_FALSE( validates( validator, "not-a-uri" ) );	  // no scheme
		EXPECT_FALSE( validates( validator, "://example.com" ) ); // empty scheme
		EXPECT_FALSE( validates( validator, "" ) );
	}

	//----------------------------------------------
	// uri-reference (RFC 3986)
	//----------------------------------------------

	TEST_F( FormatValidationTest, URIReference_Valid )
	{
		auto validator = createValidator( "uri-reference" );

		EXPECT_TRUE( validates( validator, "https://example.com" ) );
		EXPECT_TRUE( validates( validator, "/path/to/resource" ) );
		EXPECT_TRUE( validates( validator, "relative/path" ) );
		EXPECT_TRUE( validates( validator, "#fragment" ) );
		EXPECT_TRUE( validates( validator, "?query=value" ) );
	}

	TEST_F( FormatValidationTest, URIReference_Invalid )
	{
		auto validator = createValidator( "uri-reference" );

		// URI-reference is very permissive, most strings are valid
		EXPECT_FALSE( validates( validator, "has space" ) );
	}

	//----------------------------------------------
	// iri (RFC 3987)
	//----------------------------------------------

	TEST_F( FormatValidationTest, IRI_Valid )
	{
		auto validator = createValidator( "iri" );

		// Standard URIs (subset of IRI)
		EXPECT_TRUE( validates( validator, "https://example.com" ) );
		EXPECT_TRUE( validates( validator, "http://example.com/path" ) );
		EXPECT_TRUE( validates( validator, "mailto:user@example.com" ) );

		// IRIs with Unicode (the key difference from URI)
		EXPECT_TRUE( validates( validator, "https://example.com/\xE8\xB7\xAF\xE5\xBE\x84" ) ); // 路径 (path in Chinese)
		EXPECT_TRUE( validates( validator, "https://\xE4\xBE\x8B\xE3\x81\x88.jp" ) );		   // 例え.jp (example in Japanese)
		EXPECT_TRUE( validates( validator, "https://example.com/caf\xC3\xA9" ) );			   // café
	}

	TEST_F( FormatValidationTest, IRI_Invalid )
	{
		auto validator = createValidator( "iri" );

		EXPECT_FALSE( validates( validator, "not-an-iri" ) );	  // no scheme
		EXPECT_FALSE( validates( validator, "://example.com" ) ); // empty scheme
		EXPECT_FALSE( validates( validator, "" ) );
		EXPECT_FALSE( validates( validator, "https://example .com" ) ); // space in host
	}

	//----------------------------------------------
	// iri-reference (RFC 3987)
	//----------------------------------------------

	TEST_F( FormatValidationTest, IRIReference_Valid )
	{
		auto validator = createValidator( "iri-reference" );

		// Absolute IRIs
		EXPECT_TRUE( validates( validator, "https://example.com" ) );
		EXPECT_TRUE( validates( validator, "https://example.com/\xE8\xB7\xAF\xE5\xBE\x84" ) ); // Unicode path

		// Relative references
		EXPECT_TRUE( validates( validator, "/path/to/resource" ) );
		EXPECT_TRUE( validates( validator, "relative" ) );
		EXPECT_TRUE( validates( validator, "/\xE8\xB7\xAF\xE5\xBE\x84" ) ); // Unicode relative path
		EXPECT_TRUE( validates( validator, "" ) );							// empty is valid relative-reference
	}

	TEST_F( FormatValidationTest, IRIReference_Invalid )
	{
		auto validator = createValidator( "iri-reference" );

		EXPECT_FALSE( validates( validator, "https://example .com" ) ); // space in authority
	}

	//----------------------------------------------
	// uuid (RFC 4122)
	//----------------------------------------------

	TEST_F( FormatValidationTest, UUID_Valid )
	{
		auto validator = createValidator( "uuid" );

		EXPECT_TRUE( validates( validator, "550e8400-e29b-41d4-a716-446655440000" ) );
		EXPECT_TRUE( validates( validator, "6ba7b810-9dad-11d1-80b4-00c04fd430c8" ) );
		EXPECT_TRUE( validates( validator, "00000000-0000-0000-0000-000000000000" ) );
		EXPECT_TRUE( validates( validator, "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF" ) );
	}

	TEST_F( FormatValidationTest, UUID_Invalid )
	{
		auto validator = createValidator( "uuid" );

		EXPECT_FALSE( validates( validator, "550e8400-e29b-41d4-a716-44665544000" ) );	 // too short
		EXPECT_FALSE( validates( validator, "550e8400-e29b-41d4-a716-4466554400000" ) ); // too long
		EXPECT_FALSE( validates( validator, "550e8400e29b41d4a716446655440000" ) );		 // no hyphens
		EXPECT_FALSE( validates( validator, "550g8400-e29b-41d4-a716-446655440000" ) );	 // invalid hex
		EXPECT_FALSE( validates( validator, "not-a-uuid" ) );
	}

	//----------------------------------------------
	// uri-template (RFC 6570)
	//----------------------------------------------

	TEST_F( FormatValidationTest, URITemplate_Valid )
	{
		auto validator = createValidator( "uri-template" );

		// Basic templates
		EXPECT_TRUE( validates( validator, "https://example.com/{id}" ) );
		EXPECT_TRUE( validates( validator, "/users/{userId}/posts/{postId}" ) );
		EXPECT_TRUE( validates( validator, "{scheme}://{host}/{path}" ) );
		EXPECT_TRUE( validates( validator, "https://example.com" ) ); // plain URI is valid template

		// RFC 6570 operators
		EXPECT_TRUE( validates( validator, "{+path}" ) );	   // reserved expansion
		EXPECT_TRUE( validates( validator, "{#fragment}" ) );  // fragment expansion
		EXPECT_TRUE( validates( validator, "{.ext}" ) );	   // label expansion
		EXPECT_TRUE( validates( validator, "{/segments*}" ) ); // path segments
		EXPECT_TRUE( validates( validator, "{;params}" ) );	   // path-style parameters
		EXPECT_TRUE( validates( validator, "{?query}" ) );	   // query string
		EXPECT_TRUE( validates( validator, "{&more}" ) );	   // query continuation

		// Empty template is valid
		EXPECT_TRUE( validates( validator, "" ) );
	}

	TEST_F( FormatValidationTest, URITemplate_Invalid )
	{
		auto validator = createValidator( "uri-template" );

		// Whitespace not allowed
		EXPECT_FALSE( validates( validator, "has space" ) );

		// Unmatched braces
		EXPECT_FALSE( validates( validator, "{unclosed" ) );
		EXPECT_FALSE( validates( validator, "extra}" ) );
		EXPECT_FALSE( validates( validator, "{{nested}}" ) );
	}

	//----------------------------------------------
	// json-pointer (RFC 6901)
	//----------------------------------------------

	TEST_F( FormatValidationTest, JSONPointer_Valid )
	{
		auto validator = createValidator( "json-pointer" );

		EXPECT_TRUE( validates( validator, "" ) ); // empty is valid
		EXPECT_TRUE( validates( validator, "/foo" ) );
		EXPECT_TRUE( validates( validator, "/foo/bar" ) );
		EXPECT_TRUE( validates( validator, "/foo/0" ) );
		EXPECT_TRUE( validates( validator, "/a~1b" ) ); // escaped /
		EXPECT_TRUE( validates( validator, "/c~0d" ) ); // escaped ~
	}

	TEST_F( FormatValidationTest, JSONPointer_Invalid )
	{
		auto validator = createValidator( "json-pointer" );

		EXPECT_FALSE( validates( validator, "foo" ) );	  // must start with /
		EXPECT_FALSE( validates( validator, "/foo~" ) );  // incomplete escape
		EXPECT_FALSE( validates( validator, "/foo~2" ) ); // invalid escape
	}

	//----------------------------------------------
	// relative-json-pointer
	//----------------------------------------------

	TEST_F( FormatValidationTest, RelativeJSONPointer_Valid )
	{
		auto validator = createValidator( "relative-json-pointer" );

		EXPECT_TRUE( validates( validator, "0" ) );
		EXPECT_TRUE( validates( validator, "1" ) );
		EXPECT_TRUE( validates( validator, "0/foo" ) );
		EXPECT_TRUE( validates( validator, "1/foo/bar" ) );
		EXPECT_TRUE( validates( validator, "0#" ) );
		EXPECT_TRUE( validates( validator, "1#" ) );
	}

	TEST_F( FormatValidationTest, RelativeJSONPointer_Invalid )
	{
		auto validator = createValidator( "relative-json-pointer" );

		EXPECT_FALSE( validates( validator, "/foo" ) ); // absolute pointer
		EXPECT_FALSE( validates( validator, "-1" ) );	// negative
		EXPECT_FALSE( validates( validator, "01" ) );	// leading zero
		EXPECT_FALSE( validates( validator, "abc" ) );	// not a number
	}

	//----------------------------------------------
	// regex (ECMA-262)
	//----------------------------------------------

	TEST_F( FormatValidationTest, Regex_Valid )
	{
		auto validator = createValidator( "regex" );

		EXPECT_TRUE( validates( validator, "^[a-z]+$" ) );
		EXPECT_TRUE( validates( validator, ".*" ) );
		EXPECT_TRUE( validates( validator, "(foo|bar)" ) );
		EXPECT_TRUE( validates( validator, "[0-9]{3}-[0-9]{4}" ) ); // phone pattern
	}

	TEST_F( FormatValidationTest, Regex_Invalid )
	{
		auto validator = createValidator( "regex" );

		EXPECT_FALSE( validates( validator, "[invalid" ) );	 // unclosed bracket
		EXPECT_FALSE( validates( validator, "(unclosed" ) ); // unclosed paren
		EXPECT_FALSE( validates( validator, "*invalid" ) );	 // quantifier without target
	}

} // namespace nfx::serialization::json::test
