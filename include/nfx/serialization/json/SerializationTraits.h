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
 * @file SerializationTraits.h
 * @brief Serialization traits and type specializations for JSON serialization
 * @details Contains the SerializationTraits template that provides the extensible
 *          serialization framework for nfx-serialization library.
 *
 *          This file provides the base template that allows users to customize
 *          serialization behavior for their own types by specializing SerializationTraits.
 *
 *          For serialization support of nfx framework types (datetime, datatypes, containers),
 *          include the appropriate extension headers:
 *          - #include <nfx/serialization/json/extensions/DateTimeTraits.h>
 *          - #include <nfx/serialization/json/extensions/DatatypesTraits.h>
 *          - #include <nfx/serialization/json/extensions/ContainersTraits.h>
 */

#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "nfx/serialization/json/Document.h"

namespace nfx::serialization::json
{
    //=====================================================================
    // Forward declarations
    //=====================================================================

    template <typename T>
    class Serializer;

    namespace detail
    {
        template <typename T>
        struct has_serialize_method;

        template <typename T>
        struct has_serialize_method_returning_document;

        template <typename T>
        struct has_serialize_method_no_params;

        template <typename T>
        struct has_deserialize_method;
    } // namespace detail

    //=====================================================================
    // Serialization Traits (extensible by users)
    //=====================================================================

    /**
     * @brief Default serialization traits - users can specialize this
     * @tparam T The type to serialize/deserialize
     * @details This is the extension point for users to define custom serialization.
     *          Users can specialize this template for their types or even override
     *          library types with custom serialization logic.
     */
    template <typename T>
    struct SerializationTraits
    {
        /**
         * @brief Default serialize implementation - delegates to member method
         * @param obj Object to serialize
         * @param doc Document to serialize into
         */
        static void serialize( const T& obj, Document& doc )
        {
            // Look for serialize method with no parameters
            if constexpr ( detail::has_serialize_method_no_params<T>::value )
            {
                doc = obj.serialize();
            }
            // Look for serialize method returning Document with serializer parameter
            else if constexpr ( detail::has_serialize_method_returning_document<T>::value )
            {
                Serializer<T> serializer;
                doc = obj.serialize( serializer );
            }
            // Look for traditional serialize method with serializer and document parameters
            else if constexpr ( detail::has_serialize_method<T>::value )
            {
                Serializer<T> serializer;
                obj.serialize( serializer, doc );
            }
            else
            {
                static_assert( detail::has_serialize_method<T>::value ||
                                   detail::has_serialize_method_returning_document<T>::value ||
                                   detail::has_serialize_method_no_params<T>::value,
                    "Type must either specialize SerializationTraits or have a serialize() member method" );
            }
        }

        /**
         * @brief Default deserialize implementation - delegates to member method
         * @param obj Object to deserialize into
         * @param doc Document to deserialize from
         */
        static void deserialize( T& obj, const Document& doc )
        {
            // Look for member deserialize method
            if constexpr ( detail::has_deserialize_method<T>::value )
            {
                Serializer<T> serializer;
                obj.deserialize( serializer, doc );
            }
            else
            {
                static_assert( detail::has_deserialize_method<T>::value,
                    "Type must either specialize SerializationTraits or have a deserialize() member method" );
            }
        }
    };
} // namespace nfx::serialization::json
