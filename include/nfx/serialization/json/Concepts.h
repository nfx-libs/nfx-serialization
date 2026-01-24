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
 * @brief Type traits for JSON serialization
 * @details Defines type traits to detect SerializationTraits specializations
 *          and identify nfx extension types for proper template overload resolution.
 */

#pragma once

#include <nfx/json/Document.h>
#include <type_traits>

namespace nfx::serialization::json
{
    //=====================================================================
    // Forward declarations
    //=====================================================================

    class SerializableDocument; // Extended Document with serialization support

    //=====================================================================
    // Import types from nfx::json
    //=====================================================================

    using nfx::json::Array;
    using nfx::json::Object;
    using nfx::json::Type;

    // Import type trait from nfx::json
    using nfx::json::is_json_container_v;

    //=====================================================================
    // Forward declarations
    //=====================================================================

    template <typename T>
    struct SerializationTraits;

    //=====================================================================
    // Type trait to detect SerializationTraits specializations
    //=====================================================================

    namespace detail
    {
        using nfx::json::Document;

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
        struct has_serialization_traits<
            T,
            std::void_t<decltype( SerializationTraits<std::decay_t<T>>::serialize(
                std::declval<const std::decay_t<T>&>(), std::declval<Document&>() ) )>> : std::true_type
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
        /**
         * @brief Base template - defaults to false
         * @details Specialize this in extension headers (DatatypesTraits.h, etc.)
         *          AFTER including the actual type definitions
         */
        template <typename T>
        struct is_nfx_extension_type : std::false_type
        {
        };

        /**
         * @brief Helper variable template for is_nfx_extension_type
         */
        template <typename T>
        inline constexpr bool is_nfx_extension_type_v = is_nfx_extension_type<std::decay_t<T>>::value;
    } // namespace detail

    //=====================================================================
    // Concepts for template overload resolution
    //=====================================================================

    /**
     * @brief Concept for STL types that use Serializer<T>
     * @details Matches types that are NOT:
     *          - nfx extension types (use SerializationTraits)
     *          - Primitives (delegated to base Document)
     *          - JSON containers (delegated to base Document)
     *          - SerializableDocument itself (avoid recursion)
     *          - Document derivatives (avoid confusion)
     */
    template <typename T>
    concept StlSerializable =
        !detail::is_nfx_extension_type_v<T> && !nfx::json::Primitive<T> && !is_json_container_v<T> &&
        !std::is_same_v<std::remove_cvref_t<T>, SerializableDocument> &&
        !std::is_base_of_v<nfx::json::Document, std::remove_cvref_t<T>>;

    /**
     * @brief Concept for nfx extension types that use SerializationTraits<T>
     * @details Matches types that ARE nfx extensions but NOT:
     *          - Primitives (delegated to base Document)
     *          - JSON containers (delegated to base Document)
     *          - SerializableDocument itself (avoid recursion)
     *          - Document derivatives (avoid confusion)
     */
    template <typename T>
    concept NfxSerializable =
        detail::is_nfx_extension_type_v<T> && !nfx::json::Primitive<T> && !is_json_container_v<T> &&
        !std::is_same_v<std::remove_cvref_t<T>, SerializableDocument> &&
        !std::is_base_of_v<nfx::json::Document, std::remove_cvref_t<T>>;
} // namespace nfx::serialization::json
