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
 * @file SerializableDocument.inl
 * @brief Inline implementation file for SerializableDocument
 * @details Contains the inline and template method implementations for the SerializableDocument class.
 *          This file provides the actual implementation of all SerializableDocument methods,
 *          including delegation to base Document and serialization support for STL and nfx types.
 */

namespace nfx::serialization::json
{
    inline SerializableDocument::SerializableDocument( const nfx::json::Document& doc )
        : m_doc{ doc }
    {
    }

    inline SerializableDocument::SerializableDocument( nfx::json::Document&& doc )
        : m_doc{ std::move( doc ) }
    {
    }

    inline SerializableDocument::operator nfx::json::Document&() noexcept
    {
        return m_doc;
    }

    inline SerializableDocument::operator const nfx::json::Document&() const noexcept
    {
        return m_doc;
    }

    inline nfx::json::Document& SerializableDocument::document() noexcept
    {
        return m_doc;
    }

    inline const nfx::json::Document& SerializableDocument::document() const noexcept
    {
        return m_doc;
    }

    template <typename T>
        requires( nfx::json::Primitive<T> )
    inline void SerializableDocument::set( std::string_view path, T value )
    {
        m_doc.set<T>( path, value );
    }

    template <typename T>
        requires( nfx::json::Primitive<T> )
    inline std::optional<T> SerializableDocument::get( std::string_view path ) const
    {
        return m_doc.get<T>( path );
    }

    template <typename T>
        requires( nfx::json::Primitive<T> )
    bool SerializableDocument::get( std::string_view path, T& value ) const
    {
        return m_doc.get( path, value );
    }

    template <typename T>
        requires( nfx::json::Primitive<T> )
    bool SerializableDocument::is( std::string_view path ) const
    {
        return m_doc.is<T>( path );
    }

    template <typename T>
        requires( is_json_container_v<T> )
    inline void SerializableDocument::set( std::string_view path, T&& value )
    {
        m_doc.set( path, std::forward<T>( value ) );
    }

    template <typename T>
        requires( is_json_container_v<T> )
    inline void SerializableDocument::set( std::string_view path )
    {
        m_doc.set<T>( path );
    }

    template <typename T>
        requires( is_json_container_v<T> )
    inline std::optional<T> SerializableDocument::get( std::string_view path ) const
    {
        return m_doc.get<T>( path );
    }

    template <typename T>
        requires( is_json_container_v<T> )
    inline bool SerializableDocument::get( std::string_view path, T& value ) const
    {
        return m_doc.get( path, value );
    }

    template <typename T>
        requires( is_json_container_v<T> )
    inline bool SerializableDocument::is( std::string_view path ) const
    {
        return m_doc.is<T>( path );
    }

    inline std::string SerializableDocument::toString( int indent ) const
    {
        return m_doc.toString( indent );
    }

    inline std::vector<uint8_t> SerializableDocument::toBytes() const
    {
        return m_doc.toBytes();
    }

    inline void SerializableDocument::setNull( std::string_view path )
    {
        m_doc.setNull( path );
    }

    inline bool SerializableDocument::isNull( std::string_view path ) const
    {
        return m_doc.isNull( path );
    }

    inline nfx::json::Type SerializableDocument::type() const noexcept
    {
        return m_doc.type();
    }

    inline bool SerializableDocument::isEmpty() const noexcept
    {
        return m_doc.empty();
    }

    template <typename T>
        requires StlSerializable<T>
    inline void SerializableDocument::set( std::string_view path, const T& value )
    {
        Serializer<T> serializer;
        nfx::json::Document temp = serializer.serialize( value );
        m_doc.set<nfx::json::Document>( path, std::move( temp ) );
    }

    template <typename T>
        requires StlSerializable<T>
    inline void SerializableDocument::set( std::string_view path, T&& value )
    {
        Serializer<std::remove_cvref_t<T>> serializer;
        nfx::json::Document temp = serializer.serialize( std::forward<T>( value ) );
        m_doc.set<nfx::json::Document>( path, std::move( temp ) );
    }

    template <typename T>
        requires StlSerializable<T>
    inline std::optional<T> SerializableDocument::get( std::string_view path ) const
    {
        auto docOpt = m_doc.get<nfx::json::Document>( path );
        if( !docOpt.has_value() )
        {
            return std::nullopt;
        }

        Serializer<T> serializer;
        return serializer.deserialize( docOpt.value() );
    }

    template <typename T>
        requires StlSerializable<T>
    inline bool SerializableDocument::get( std::string_view path, T& value ) const
    {
        auto result = get<T>( path );
        if( result.has_value() )
        {
            value = std::move( result.value() );
            return true;
        }
        return false;
    }

    template <typename T>
        requires StlSerializable<T>
    inline bool SerializableDocument::is( std::string_view path ) const
    {
        auto docOpt = m_doc.get<nfx::json::Document>( path );
        if( !docOpt.has_value() )
            return false;
        try
        {
            Serializer<T> serializer;
            serializer.deserialize( docOpt.value() );
            return true;
        }
        catch( ... )
        {
            return false;
        }
    }

    template <typename T>
        requires NfxSerializable<T>
    inline void SerializableDocument::set( std::string_view path, const T& value )
    {
        SerializableDocument temp;
        SerializationTraits<T>::serialize( value, temp );
        m_doc.set<nfx::json::Document>( path, std::move( static_cast<nfx::json::Document&>( temp ) ) );
    }

    template <typename T>
        requires NfxSerializable<T>
    inline void SerializableDocument::set( std::string_view path, T&& value )
    {
        using CleanT = std::remove_cvref_t<T>;
        SerializableDocument temp;
        SerializationTraits<CleanT>::serialize( value, temp );
        m_doc.set<nfx::json::Document>( path, std::move( static_cast<nfx::json::Document&>( temp ) ) );
    }

    template <typename T>
        requires NfxSerializable<T>
    inline std::optional<T> SerializableDocument::get( std::string_view path ) const
    {
        auto docOpt = m_doc.get<nfx::json::Document>( path );
        if( !docOpt.has_value() )
        {
            return std::nullopt;
        }

        T result;
        SerializationTraits<T>::deserialize( result, docOpt.value() );
        return result;
    }

    template <typename T>
        requires NfxSerializable<T>
    inline bool SerializableDocument::get( std::string_view path, T& value ) const
    {
        auto result = get<T>( path );
        if( result.has_value() )
        {
            value = std::move( result.value() );
            return true;
        }
        return false;
    }

    template <typename T>
        requires NfxSerializable<T>
    inline bool SerializableDocument::is( std::string_view path ) const
    {
        auto docOpt = m_doc.get<nfx::json::Document>( path );
        if( !docOpt.has_value() )
            return false;
        try
        {
            T temp;
            SerializationTraits<T>::deserialize( temp, docOpt.value() );
            return true;
        }
        catch( ... )
        {
            return false;
        }
    }

    template <typename T>
        requires( NfxSerializable<T> && std::is_constructible_v<T, const char*> )
    inline void SerializableDocument::set( std::string_view path, const char* value )
    {
        set<T>( path, T( value ) );
    }

    template <typename T, typename U>
        requires( NfxSerializable<T> && std::is_arithmetic_v<U> && std::is_constructible_v<T, U> )
    inline void SerializableDocument::set( std::string_view path, U value )
    {
        set<T>( path, T( value ) );
    }

    inline std::optional<SerializableDocument> SerializableDocument::fromString( std::string_view jsonStr )
    {
        auto baseDoc = nfx::json::Document::fromString( jsonStr );
        if( !baseDoc )
        {
            return std::nullopt;
        }
        return SerializableDocument( std::move( *baseDoc ) );
    }

    inline bool SerializableDocument::fromString( std::string_view jsonStr, SerializableDocument& value )
    {
        return nfx::json::Document::fromString( jsonStr, static_cast<nfx::json::Document&>( value ) );
    }

    inline std::optional<SerializableDocument> SerializableDocument::fromBytes( const std::vector<uint8_t>& bytes )
    {
        auto baseDoc = nfx::json::Document::fromBytes( bytes );
        if( !baseDoc )
        {
            return std::nullopt;
        }
        return SerializableDocument( std::move( *baseDoc ) );
    }

    inline bool SerializableDocument::fromBytes( const std::vector<uint8_t>& bytes, SerializableDocument& value )
    {
        return nfx::json::Document::fromBytes( bytes, static_cast<nfx::json::Document&>( value ) );
    }
} // namespace nfx::serialization::json
