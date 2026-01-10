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
 * @file Concepts.h
 * @brief C++20 concepts for JSON serialization type constraints
 * @details Defines JsonPrimitive concept and type traits infrastructure for
 *          type-safe template constraints in the Document API.
 *
 * @note JsonValue, JsonContainer, and JsonCheckable concepts are defined at
 *       the end of Document.h after Document::Object and Document::Array are
 *       fully defined, since nested class forward declarations don't work with concepts.
 */

#pragma once

#include <concepts>
#include <type_traits>

namespace nfx::serialization::json
{
    //=====================================================================
    // Forward declarations
    //=====================================================================

    class Document;

    template <typename T>
    struct SerializationTraits;

    //=====================================================================
    // Type traits for JSON nested classes
    //=====================================================================

    /**
     * @brief Type trait to identify JSON container types (Object, Array)
     * @details Primary template returns false. Specialized in Document.h for
     *          Document::Object and Document::Array.
     */
    template <typename T>
    struct is_json_container : std::false_type
    {
    };

    /**
     * @brief Specialization for Document
     */
    template <>
    struct is_json_container<Document> : std::true_type
    {
    };

    /**
     * @brief Helper variable template
     */
    template <typename T>
    inline constexpr bool is_json_container_v = is_json_container<std::decay_t<T>>::value;

    //=====================================================================
    // Type trait to detect SerializationTraits specializations
    //=====================================================================

    namespace detail
    {
        /**
         * @brief Detect if a type has a custom SerializationTraits specialization
         * @details Uses SFINAE to detect if SerializationTraits<T>::serialize is available
         *          and is not the default implementation (which requires member methods).
         *          This allows conditional support for extension types only when their
         *          trait headers are included.
         */
        template <typename T, typename = void>
        struct has_serialization_traits : std::false_type
        {
        };

        template <typename T>
        struct has_serialization_traits<T,
            std::void_t<decltype( SerializationTraits<std::decay_t<T>>::serialize(
                std::declval<const std::decay_t<T>&>(),
                std::declval<Document&>() ) )>> : std::true_type
        {
        };

        /**
         * @brief Helper variable template for has_serialization_traits
         */
        template <typename T>
        inline constexpr bool has_serialization_traits_v = has_serialization_traits<T>::value;

        //=====================================================================
        // Type trait to identify nfx extension types
        //=====================================================================

        /**
         * @brief Identifies types from nfx:: namespaces that have SerializationTraits
         * @details This trait explicitly lists all nfx extension types to distinguish them
         *          from STL types. Used to route types to correct template overloads:
         *          - nfx types → SerializationTraits templates
         *          - STL types → Serializer templates
         */
        template <typename T>
        struct is_nfx_extension_type : std::false_type
        {
        };

        namespace nfx
        {
            namespace datatypes
            {
                class Int128;
                class Decimal;
            } // namespace datatypes
        } // namespace nfx

        namespace nfx
        {
            namespace time
            {
                class TimeSpan;
                class DateTime;
                class DateTimeOffset;
            } // namespace time
        } // namespace nfx

        namespace nfx
        {
            namespace containers
            {
                template <typename, typename, typename, auto, typename, typename>
                class PerfectHashMap;
                template <typename, typename, typename, auto, typename, typename>
                class FastHashMap;
                template <typename, typename, auto, typename, typename>
                class FastHashSet;
            } // namespace containers
        } // namespace nfx

        // Specializations for nfx::datatypes types
        template <>
        struct is_nfx_extension_type<nfx::datatypes::Int128> : std::true_type
        {
        };

        template <>
        struct is_nfx_extension_type<nfx::datatypes::Decimal> : std::true_type
        {
        };

        // Specializations for nfx::time types
        template <>
        struct is_nfx_extension_type<nfx::time::TimeSpan> : std::true_type
        {
        };

        template <>
        struct is_nfx_extension_type<nfx::time::DateTime> : std::true_type
        {
        };

        template <>
        struct is_nfx_extension_type<nfx::time::DateTimeOffset> : std::true_type
        {
        };

        // Specializations for nfx::containers types
        template <typename K, typename V, typename H, auto S, typename Hr, typename E>
        struct is_nfx_extension_type<nfx::containers::PerfectHashMap<K, V, H, S, Hr, E>> : std::true_type
        {
        };

        template <typename K, typename V, typename H, auto S, typename Hr, typename E>
        struct is_nfx_extension_type<nfx::containers::FastHashMap<K, V, H, S, Hr, E>> : std::true_type
        {
        };

        template <typename K, typename H, auto S, typename Hr, typename E>
        struct is_nfx_extension_type<nfx::containers::FastHashSet<K, H, S, Hr, E>> : std::true_type
        {
        };

        /**
         * @brief Helper variable template for is_nfx_extension_type
         */
        template <typename T>
        inline constexpr bool is_nfx_extension_type_v = is_nfx_extension_type<std::decay_t<T>>::value;
    } // namespace detail

    //=====================================================================
    // C++20 Concepts for JSON value types
    //=====================================================================

    /**
     * @brief Concept for JSON-compatible primitive types
     * @details Matches strings, characters, booleans, integers, and floating-point types
     */
    template <typename T>
    concept JsonPrimitive =
        std::is_same_v<std::decay_t<T>, std::string> ||
        std::is_same_v<std::decay_t<T>, char> ||
        std::is_same_v<std::decay_t<T>, bool> ||
        (std::is_integral_v<std::decay_t<T>> && !std::is_same_v<std::decay_t<T>, bool> && !std::is_same_v<std::decay_t<T>, char>) ||
        std::is_floating_point_v<std::decay_t<T>>;

    /**
     * @brief Concept for all JSON-compatible value types
     * @details Matches primitives plus Document, Document::Object, and Document::Array.
     *          Does NOT include types with SerializationTraits - those are handled separately
     *          via dedicated template overloads to avoid ambiguity.
     */
    template <typename T>
    concept JsonValue =
        JsonPrimitive<T> ||
        is_json_container_v<T>;

    /**
     * @brief Concept for JSON container types only
     * @details Matches Document, Document::Object, and Document::Array
     */
    template <typename T>
    concept JsonContainer = is_json_container_v<T>;

    /**
     * @brief Concept for types that can be checked with is<T>()
     * @details Same as JsonValue but excludes Document itself
     */
    template <typename T>
    concept JsonCheckable =
        JsonPrimitive<T> ||
        ( is_json_container_v<T> && !std::is_same_v<std::decay_t<T>, Document> );
} // namespace nfx::serialization::json
