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
 * @file DateTimeTraits.h
 * @brief SerializationTraits specializations for nfx-datetime types
 * @details This is an optional extension header that provides JSON serialization support
 *          for nfx::time types (DateTime, DateTimeOffset, TimeSpan).
 *
 *          This header is safe to include even if nfx-datetime is not available.
 *          Each datetime type is independently supported - you can use any subset.
 *
 *          #include <nfx/serialization/json/Serializer.h>
 *          #include <nfx/serialization/json/extensions/DateTimeTraits.h>
 *
 * @note Each specialization is only enabled if its corresponding header is available.
 */

#pragma once

#include "nfx/serialization/json/Serializer.h"

//=====================================================================
// TimeSpan support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/datetime/TimeSpan.h>)

#    include <nfx/datetime/TimeSpan.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::time::TimeSpan
     */
    template <>
    struct SerializationTraits<nfx::time::TimeSpan>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj The TimeSpan object to serialize
         * @param builder The builder to write to
         * @details Serializes as JSON number (ticks) for compact representation
         */
        static void serialize( const nfx::time::TimeSpan& obj, Builder& builder )
        {
            builder.write( obj.ticks() );
        }

        /**
         * @brief Deserialize TimeSpan from JSON document
         * @param doc The document to deserialize from
         * @param obj The TimeSpan object to deserialize into
         */
        static void fromDocument( const Document& doc, nfx::time::TimeSpan& obj )
        {
            if( doc.is<int>( "" ) )
            {
                auto ticksVal = doc.get<int64_t>( "" );
                if( ticksVal.has_value() )
                {
                    obj = nfx::time::TimeSpan( ticksVal.value() );
                }
            }
            else
            {
                throw std::runtime_error{ "Invalid TimeSpan format: expected integer ticks" };
            }
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/datetime/TimeSpan.h>)

//=====================================================================
// DateTime support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/datetime/DateTime.h>)

#    include <nfx/datetime/DateTime.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::time::DateTime
     */
    template <>
    struct SerializationTraits<nfx::time::DateTime>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj The DateTime object to serialize
         * @param builder The builder to write to
         * @details Serializes as ISO 8601 string with precise tick resolution
         */
        static void serialize( const nfx::time::DateTime& obj, Builder& builder )
        {
            builder.write( obj.toString( nfx::time::DateTime::Format::Iso8601Precise ) );
        }

        /**
         * @brief Deserialize DateTime from JSON document
         * @param doc The document to deserialize from
         * @param obj The DateTime object to deserialize into
         */
        static void fromDocument( const Document& doc, nfx::time::DateTime& obj )
        {
            if( doc.is<std::string>( "" ) )
            {
                auto val = doc.get<std::string>( "" );
                if( val.has_value() && !val.value().empty() )
                {
                    if( !nfx::time::DateTime::fromString( val.value(), obj ) )
                    {
                        throw std::runtime_error{ "Invalid DateTime format: expected ISO 8601 string" };
                    }
                }
            }
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/datetime/DateTime.h>)

//=====================================================================
// DateTimeOffset support - enabled only if header is available
//=====================================================================

#if __has_include( <nfx/datetime/DateTimeOffset.h>)

#    include <nfx/datetime/DateTimeOffset.h>

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::time::DateTimeOffset
     */
    template <>
    struct SerializationTraits<nfx::time::DateTimeOffset>
    {
        /**
         * @brief High-performance streaming serialization
         * @param obj The DateTimeOffset object to serialize
         * @param builder The builder to write to
         * @details Serializes as ISO 8601 string with timezone offset and precise tick resolution
         */
        static void serialize( const nfx::time::DateTimeOffset& obj, Builder& builder )
        {
            builder.write( obj.toString( nfx::time::DateTime::Format::Iso8601Precise ) );
        }

        /**
         * @brief Deserialize DateTimeOffset from JSON document
         * @param doc The document to deserialize from
         * @param obj The DateTimeOffset object to deserialize into
         */
        static void fromDocument( const Document& doc, nfx::time::DateTimeOffset& obj )
        {
            if( doc.is<std::string>( "" ) )
            {
                auto val = doc.get<std::string>( "" );
                if( val.has_value() && !val.value().empty() )
                {
                    if( !nfx::time::DateTimeOffset::fromString( val.value(), obj ) )
                    {
                        throw std::runtime_error{ "Invalid DateTimeOffset format: expected ISO 8601 string" };
                    }
                }
            }
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include(<nfx/datetime/DateTimeOffset.h>)
