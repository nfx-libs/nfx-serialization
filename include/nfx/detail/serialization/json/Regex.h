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
 * @file Regex.h
 * @brief JSON Schema format validation regex patterns
 * @details Defines regex patterns for JSON Schema Draft 2020-12 format validation.
 *
 * Format validation patterns are based on the following RFCs:
 * - **RFC 3339** - Date and Time on the Internet: Timestamps
 *   @see https://datatracker.ietf.org/doc/html/rfc3339
 * - **RFC 4122** - Universally Unique IDentifier (UUID) URN Namespace
 *   @see https://datatracker.ietf.org/doc/html/rfc4122
 * - **RFC 2673** - Binary Labels in the Domain Name System
 *   @see https://datatracker.ietf.org/doc/html/rfc2673
 * - **RFC 4291** - IP Version 6 Addressing Architecture
 *   @see https://datatracker.ietf.org/doc/html/rfc4291
 * - **RFC 5321** - Simple Mail Transfer Protocol
 *   @see https://datatracker.ietf.org/doc/html/rfc5321
 * - **RFC 6531** - SMTP Extension for Internationalized Email
 *   @see https://datatracker.ietf.org/doc/html/rfc6531
 * - **RFC 1123** - Requirements for Internet Hosts - Application and Support
 *   @see https://datatracker.ietf.org/doc/html/rfc1123
 * - **RFC 5890** - Internationalized Domain Names for Applications (IDNA)
 *   @see https://datatracker.ietf.org/doc/html/rfc5890
 * - **RFC 3986** - Uniform Resource Identifier (URI): Generic Syntax
 *   @see https://datatracker.ietf.org/doc/html/rfc3986
 * - **RFC 3987** - Internationalized Resource Identifiers (IRIs)
 *   @see https://datatracker.ietf.org/doc/html/rfc3987
 * - **RFC 6570** - URI Template
 *   @see https://datatracker.ietf.org/doc/html/rfc6570
 * - **RFC 6901** - JavaScript Object Notation (JSON) Pointer
 *   @see https://datatracker.ietf.org/doc/html/rfc6901
 *
 * @see https://json-schema.org/draft/2020-12/json-schema-validation#section-7.3
 */

#pragma once

#include <regex>

namespace nfx::serialization::json::regex
{
	//=====================================================================
	// Format validation regex patterns (RFC-based)
	//=====================================================================

	//----------------------------------------------
	// Dates, Times, and Duration (RFC 3339)
	//----------------------------------------------

	/// @brief RFC 3339 date-time: YYYY-MM-DDTHH:MM:SS with optional fractional seconds and timezone.
	inline const std::regex DATE_TIME{ R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])[Tt]([01]\d|2[0-3]):[0-5]\d:[0-5]\d(\.\d+)?([Zz]|[+-]([01]\d|2[0-3]):[0-5]\d)$)" };

	/// @brief RFC 3339 full-date: YYYY-MM-DD.
	inline const std::regex DATE{ R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)" };

	/// @brief RFC 3339 full-time: HH:MM:SS with optional fractional seconds and timezone.
	inline const std::regex TIME{ R"(^([01]\d|2[0-3]):[0-5]\d:[0-5]\d(\.\d+)?([Zz]|[+-]([01]\d|2[0-3]):[0-5]\d)$)" };

	/// @brief ISO 8601 duration: P[n]Y[n]M[n]DT[n]H[n]M[n]S or P[n]W.
	inline const std::regex DURATION{ R"(^P((\d+Y)?(\d+M)?(\d+W)?(\d+D)?(T(\d+H)?(\d+M)?(\d+(\.\d+)?S)?)?|\d+W)$)" };

	//----------------------------------------------
	// Email Addresses (RFC 5321/6531)
	//----------------------------------------------

	/// @brief RFC 5321 email: simplified validation checking basic structure.
	inline const std::regex EMAIL{ R"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9]([a-zA-Z0-9-]*[a-zA-Z0-9])?(\.[a-zA-Z0-9]([a-zA-Z0-9-]*[a-zA-Z0-9])?)+$)" };

	/// @brief RFC 6531 internationalized email: same basic structure check.
	inline const std::regex IDN_EMAIL{ R"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9]([a-zA-Z0-9-]*[a-zA-Z0-9])?(\.[a-zA-Z0-9]([a-zA-Z0-9-]*[a-zA-Z0-9])?)+$)" };

	//----------------------------------------------
	// Hostnames (RFC 1123/5890)
	//----------------------------------------------

	/// @brief RFC 1123 hostname: labels separated by dots, alphanumeric and hyphens.
	inline const std::regex HOSTNAME{ R"(^[a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$)", std::regex::icase };

	//----------------------------------------------
	// IP Addresses (RFC 2673/4291)
	//----------------------------------------------

	/// @brief RFC 2673 IPv4: dotted-quad notation (0-255 for each octet).
	inline const std::regex IPV4{ R"(^(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])(\.(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]?[0-9])){3}$)" };

	/// @brief RFC 4291 IPv6: simplified validation supporting common formats.
	inline const std::regex IPV6{ R"(^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$|^([0-9a-fA-F]{1,4}:){1,7}:$|^([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}$|^([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}$|^([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}$|^([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}$|^([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}$|^[0-9a-fA-F]{1,4}:(:[0-9a-fA-F]{1,4}){1,6}$|^:(:[0-9a-fA-F]{1,4}){1,7}$|^::$)" };

	//----------------------------------------------
	// Resource Identifiers (RFC 3986/3987/4122)
	//----------------------------------------------

	/// @brief RFC 3986 URI: scheme://authority/path?query#fragment.
	inline const std::regex URI{ R"(^[a-zA-Z][a-zA-Z0-9+.-]*:[^\s]*$)" };

	/// @brief RFC 3986 URI-reference: URI or relative-reference.
	inline const std::regex URI_REFERENCE{ R"(^([a-zA-Z][a-zA-Z0-9+.-]*:)?[^\s]*$)" };

	/// @brief RFC 3987 IRI: internationalized URI.
	inline const std::regex IRI{ R"(^[a-zA-Z][a-zA-Z0-9+.-]*:.+$)" };

	/// @brief RFC 4122 UUID: 8-4-4-4-12 hexadecimal digits.
	inline const std::regex UUID{ R"(^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$)" };

	//----------------------------------------------
	// URI Template (RFC 6570)
	//----------------------------------------------

	/// @brief RFC 6570 URI Template: URI with {variable} expressions.
	inline const std::regex URI_TEMPLATE{ R"(^[^\s]*$)" };

	//----------------------------------------------
	// JSON Pointers (RFC 6901)
	//----------------------------------------------

	/// @brief RFC 6901 JSON Pointer: empty string or sequence of /reference-token.
	inline const std::regex JSON_POINTER{ R"(^(\/([^~/]|~[01])*)*$)" };

	/// @brief Relative JSON Pointer: non-negative integer followed by either # or JSON Pointer.
	inline const std::regex RELATIVE_JSON_POINTER{ R"(^(0|[1-9][0-9]*)(#|(\/([^~/]|~[01])*)*)$)" };
} // namespace nfx::serialization::json::regex
