#pragma once

#include <map>

#include "../Types.h"
#include "../String/TString.h"
#include "../String/StringUtilities.h"

namespace Epoch {

    class ITexture;

    struct TextureCacheEntry {
        U32 ReferenceCount;
        ITexture* Texture;
    };

    class TextureCache {
    public:
        TextureCache();
        ~TextureCache();

        void Initialize();
        const bool GetTextureReference( const TString& textureName, ITexture** texture );
        const bool Exists( const TString& textureName );
        void Add( const TString& textureName, ITexture* texture );
        void Release( const TString& textureName );

        ITexture* GetDefaultWhiteTexture() const { return _defaultWhiteTexture; }

    private:
        // TODO: optimize by using FNames instead.
        std::map<TString, TextureCacheEntry, TStringCompare> _textureCache;
        ITexture* _defaultWhiteTexture;
    };
}