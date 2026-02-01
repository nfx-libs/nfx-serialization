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
 * @file Serializer.h
 * @brief Templated JSON serializer with compile-time type mapping
 * @details Provides object serialization to/from JSON with automatic
 *          type detection, trait-based customization, and extensible serialization strategies.
 *          Supports POD types, containers, custom objects, and nested structures.
 */

#pragma once

#include "Concepts.h"
#include "traits/SerializationTraits.h"

#include <nfx/json/Document.h>
#include <nfx/json/Builder.h>

namespace nfx::serialization::json
{
    //=====================================================================
    // Serializer class
    //=====================================================================

    /**
     * @brief Templated JSON serializer with compile-time type mapping
     * @tparam T The type to serialize/deserialize
     * @details Provides automatic serialization and deserialization of C++ objects
     *          to/from JSON using compile-time type detection and traits.
     */
    template <typename T>
    class Serializer final
    {
        template <typename U>
        friend struct SerializationTraits;

    public:
        //----------------------------------------------
        // Serialization options and context
        //----------------------------------------------

        /**
         * @brief Serialization options and context
         */
        struct Options
        {
            bool includeNullFields = false;    ///< Include fields with null values in output
            bool prettyPrint = false;          ///< Format output with indentation
            bool validateOnDeserialize = true; ///< Validate data during deserialization

            /**
             * @brief Default constructor
             */
            Options() = default;

            /**
             * @brief Copy values from another serializer's options
             * @tparam U The source serializer type
             * @param other Options from another serializer type
             */
            template <typename U>
            inline void copyFrom( const typename Serializer<U>::Options& other );

            /**
             * @brief Create Options with values copied from another serializer's options
             * @tparam U The source serializer type
             * @param other Options from another serializer type
             * @return New Options instance with copied values
             */
            template <typename U>
            inline static Options createFrom( const typename Serializer<U>::Options& other );
        };

        //----------------------------------------------
        // Type aliases
        //----------------------------------------------

        /** @brief The type being serialized/deserialized */
        using value_type = T;

        //----------------------------------------------
        // Construction
        //----------------------------------------------

        /**
         * @brief Default constructor
         */
        Serializer() = default;

        /**
         * @brief Constructor with options
         * @param options Serialization options to use
         */
        inline explicit Serializer( const Options& options ) noexcept;

        //----------------------------------------------
        // Options management
        //----------------------------------------------

        /**
         * @brief Get current serialization options
         * @return Current options
         */
        inline const Options& options() const noexcept;

        /**
         * @brief Set serialization options
         * @param options New options to use
         */
        inline void setOptions( const Options& options ) noexcept;

        //----------------------------------------------
        // Static convenience serialization methods
        //----------------------------------------------

        /**
         * @brief Serialize object to JSON string
         * @tparam T Type of object to serialize
         * @param obj Object to serialize
         * @param options Serialization options (optional, uses defaults if not provided)
         * @return JSON string representation
         */
        inline static std::string toString( const T& obj, const Options& options = {} );

        /**
         * @brief Deserialize object from JSON string
         * @tparam T Type of object to deserialize
         * @param jsonStr JSON string to deserialize from
         * @param options Serialization options (optional, uses defaults if not provided)
         * @return Deserialized object
         */
        inline static T fromString( std::string_view jsonStr, const Options& options = {} );

    private:
        //----------------------------------------------
        // Private methods
        //----------------------------------------------

        /**
         * @brief High-performance serialization directly to Builder
         * @tparam U The type to serialize (deduced from parameter)
         * @param obj Object to serialize
         * @param builder Builder to write JSON into
         */
        template <typename U>
        inline void serializeValue( const U& obj, nfx::json::Builder& builder ) const;

        /**
         * @brief Unified templated deserialization method
         * @tparam U The type to deserialize (deduced from parameter)
         * @param doc Document to deserialize from
         * @param obj Object to deserialize into
         */
        template <typename U>
        inline void deserializeValue( const nfx::json::Document& doc, U& obj ) const;

        //----------------------------------------------
        // Member variables
        //----------------------------------------------

        Options m_options{}; ///< Serialization options
    };
} // namespace nfx::serialization::json

#include "nfx/detail/serialization/json/Serializer.inl"
