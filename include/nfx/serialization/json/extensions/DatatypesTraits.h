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
 * @file DatatypesTraits.h
 * @brief SerializationTraits specializations for nfx-datatypes types
 * @details This is an optional extension header that provides JSON serialization support
 *          for nfx::datatypes types (Int128, Decimal).
 *
 *          This header is safe to include even if nfx-datatypes is not available.
 *          Each datatype is independently supported - you can use any subset.
 *
 *          #include <nfx/serialization/json/SerializationTraits.h>
 *          #include <nfx/serialization/json/extensions/DatatypesTraits.h>
 *
 * @note Each specialization is only enabled if its corresponding header is available.
 */

#pragma once

#include "nfx/serialization/json/Serializer.h"

//=====================================================================
// Int128 support - enabled only if header is available
//=====================================================================

#if __has_include( "nfx/datatypes/Int128.h" )

#    include "nfx/datatypes/Int128.h"

// Register Int128 as nfx extension type
namespace nfx::serialization::json::detail
{
    template <>
    struct is_nfx_extension_type<nfx::datatypes::Int128> : std::true_type
    {
    };
} // namespace nfx::serialization::json::detail

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::datatypes::Int128
     */
    template <>
    struct SerializationTraits<nfx::datatypes::Int128>
    {
        /**
         * @brief Serialize Int128 to JSON document using platform-independent string representation
         * @param obj The Int128 object to serialize
         * @param doc The document to serialize into
         * @details Uses toString() method to ensure cross-platform compatibility.
         *          String representation works consistently across GCC/Clang (native __int128)
         *          and MSVC (manual implementation) platforms.
         */
        static void serialize( const nfx::datatypes::Int128& obj, Document& doc )
        {
            std::string value = obj.toString();
            doc.set<std::string>( "", value );
        }

        /**
         * @brief Deserialize Int128 from JSON document using platform-independent string parsing
         * @param obj The Int128 object to deserialize into
         * @param doc The document to deserialize from
         * @details Uses fromString() method to ensure cross-platform compatibility.
         *          Can deserialize values created on any platform (GCC/Clang/MSVC).
         */
        static void deserialize( nfx::datatypes::Int128& obj, const Document& doc )
        {
            if( doc.is<std::string>( "" ) )
            {
                auto val = doc.get<std::string>( "" );
                if( val.has_value() && !val.value().empty() )
                {
                    if( !nfx::datatypes::Int128::fromString( val.value(), obj ) )
                    {
                        throw std::runtime_error{ "Invalid Int128 format: unable to parse string representation" };
                    }
                }
            }
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include("nfx/datatypes/Int128.h")

//=====================================================================
// Decimal support - enabled only if header is available
//=====================================================================

#if __has_include( "nfx/datatypes/Decimal.h" )

#    include "nfx/datatypes/Decimal.h"

// Register Decimal as nfx extension type
namespace nfx::serialization::json::detail
{
    template <>
    struct is_nfx_extension_type<nfx::datatypes::Decimal> : std::true_type
    {
    };
} // namespace nfx::serialization::json::detail

namespace nfx::serialization::json
{
    /**
     * @brief Specialization for nfx::datatypes::Decimal
     */
    template <>
    struct SerializationTraits<nfx::datatypes::Decimal>
    {
        /**
         * @brief Serialize Decimal to JSON document using platform-independent string representation
         * @param obj The Decimal object to serialize
         * @param doc The document to serialize into
         * @details Uses toString() method to ensure cross-platform compatibility.
         *          String representation is platform-independent and preserves full precision.
         */
        static void serialize( const nfx::datatypes::Decimal& obj, Document& doc )
        {
            std::string value = obj.toString();
            doc.set<std::string>( "", value );
        }

        /**
         * @brief Deserialize Decimal from JSON document using platform-independent string parsing
         * @param obj The Decimal object to deserialize into
         * @param doc The document to deserialize from
         * @details Uses fromString() method to ensure cross-platform compatibility.
         *          Can deserialize values created on any platform (GCC/Clang/MSVC).
         */
        static void deserialize( nfx::datatypes::Decimal& obj, const Document& doc )
        {
            if( doc.is<std::string>( "" ) )
            {
                auto val = doc.get<std::string>( "" );
                if( val.has_value() && !val.value().empty() )
                {
                    if( !nfx::datatypes::Decimal::fromString( val.value(), obj ) )
                    {
                        throw std::runtime_error{ "Invalid Decimal format: unable to parse string representation" };
                    }
                }
            }
        }
    };
} // namespace nfx::serialization::json

#endif // __has_include("nfx/datatypes/Decimal.h")
