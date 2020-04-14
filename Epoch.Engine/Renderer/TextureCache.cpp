
#include "../Logger.h"

#include "../Renderer/Frontend/RendererFrontend.h"
#include "../Resources/ITexture.h"
#include "../String/TString.h"

#include "TextureCache.h"

namespace Epoch {

    TextureCache::TextureCache() {
        Logger::Trace( "Created texture cache." );
    }

    TextureCache::~TextureCache() {
        if( _defaultWhiteTexture ) {
            delete _defaultWhiteTexture;
            _defaultWhiteTexture = nullptr;
        }
    }

    void TextureCache::Initialize() {
        _defaultWhiteTexture = RendererFrontEnd::GetTexture( "__DEFAULT_WHITE__", "assets/textures/defaultwhite.jpg", true );
        Logger::Trace( "Initialized texture cache." );
    }

    const bool TextureCache::GetTextureReference( const TString& textureName, ITexture** texture ) {
        auto entry = _textureCache.find( textureName );
        if( entry == _textureCache.end() ) {
            return false;
        } else {
            Logger::Trace( "Obtaining new reference to a texture named '%s'.", entry->first.CStr() );
            entry->second.ReferenceCount++;
            *texture = entry->second.Texture;
            return true;
        }
    }

    const bool TextureCache::Exists( const TString& textureName ) {
        return ( _textureCache.find( textureName ) != _textureCache.end() );
    }

    void TextureCache::Add( const TString& textureName, ITexture* texture ) {
        auto entry = _textureCache.find( textureName );
        if( entry == _textureCache.end() ) {

            Logger::Trace( "Adding a new texture named '%s' to the texture cache.", textureName.CStr() );

            // New entry, as expected
            TextureCacheEntry newEntry;
            newEntry.ReferenceCount = 1;
            newEntry.Texture = texture;
            _textureCache.emplace( TString( textureName ), newEntry );
        } else {
            // Already exists, just increase reference count and slap the user.

            Logger::Warn( "Attempted to add a texture named '%s' to the texture cache which already exists. Call GetTextureReference instead.", entry->first.CStr() );
            entry->second.ReferenceCount++;
        }
    }

    void TextureCache::Release( const TString& textureName ) {
        auto entry = _textureCache.find( textureName );
        if( entry == _textureCache.end() ) {
            Logger::Warn( "Attempted to release a reference to a texture which is not in the texture cache. Nothing was done." );
        } else {
            entry->second.ReferenceCount--;
            Logger::Trace( "Reducing reference count for texture '%s' to %u.", entry->first.CStr(), entry->second.ReferenceCount );

            if( entry->second.ReferenceCount <= 0 ) {
                Logger::Trace( "Reference count for texture '%s' has reached 0. Unloading.", entry->first.CStr() );
                delete entry->second.Texture;
                entry->second.Texture = nullptr;

                // Blow away the entry.
                _textureCache.erase( textureName.CStr() );
            }
        }
    }
}