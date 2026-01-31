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
 * @file DocumentTraits.h
 * @brief Document-based serialization traits for JSON DOM serialization/deserialization
 * @details Contains the DocumentTraits template that provides the extensible DOM-based
 *          serialization framework for nfx-serialization library.
 *
 *          This file provides the base template that allows users to customize
 *          serialization behavior for their own types by specializing DocumentTraits.
 *          DocumentTraits provides bidirectional serialization (read/write) through
 *          the Document API.
 *
 *          For high-performance write-only streaming serialization, see BuilderTraits.h
 *
 *          For serialization support of nfx framework types (datetime, datatypes, containers),
 *          include the appropriate extension headers:
 *          - #include <nfx/serialization/json/extensions/DateTimeTraits.h>
 *          - #include <nfx/serialization/json/extensions/DatatypesTraits.h>
 *          - #include <nfx/serialization/json/extensions/ContainersTraits.h>
 */

#pragma once

#include <nfx/json/Document.h>

#include <stdexcept>
#include <string>
#include <vector>

using namespace nfx::json;

namespace nfx::serialization::json
{
    //=====================================================================
    // Forward declarations
    //=====================================================================

    template <typename T>
    class Serializer;

    //=====================================================================
    // Document Traits (extensible by users)
    //=====================================================================

    /**
     * @brief Document-based serialization traits - users can specialize this
     * @tparam T The type to serialize/deserialize
     * @details This is the extension point for users to define custom DOM-based serialization.
     *          Users can specialize this template for their types or even override
     *          library types with custom serialization logic.
     *
     *          DocumentTraits provides bidirectional serialization through the Document API:
     *          - toDocument(obj, doc): Convert object → Document (serialization)
     *          - fromDocument(doc, obj): Convert Document → object (deserialization)
     *
     *          User types must provide member methods with these signatures:
     *          - void toDocument(const Serializer<T>&, Document&) const
     *          - void fromDocument(const Document&, const Serializer<T>&)
     *
     *          For write-only streaming serialization, see BuilderTraits.
     */
    template <typename T>
    struct DocumentTraits
    {
        /**
         * @brief Convert object to Document (serialization)
         * @param obj Object to convert (source)
         * @param doc Document to populate (destination)
         */
        static void toDocument( const T& obj, Document& doc )
        {
            Serializer<T> serializer;
            obj.toDocument( serializer, doc );
        }

        /**
         * @brief Convert Document to object (deserialization)
         * @param doc Document to read from (source)
         * @param obj Object to populate (destination)
         */
        static void fromDocument( const Document& doc, T& obj )
        {
            Serializer<T> serializer;
            obj.fromDocument( doc, serializer );
        }
    };
} // namespace nfx::serialization::json
