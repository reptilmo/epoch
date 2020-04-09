
#include <map>


#include "../Logger.h"
#include "../Resources/ITexture.h"
#include "Frontend/RendererFrontend.h"

#include "Material.h"

namespace Epoch {

    BaseMaterial::BaseMaterial( const TString& name, const MaterialType type ) {
        Name = name;
        _type = type;
    }

    BaseMaterial::~BaseMaterial() {

    }



    UnlitMaterial::UnlitMaterial( const TString& name ) : BaseMaterial( name, MaterialType::Unlit ) {

    }

    UnlitMaterial::UnlitMaterial( const TString& name, const TString& diffusePath ) : BaseMaterial( name, MaterialType::Unlit ) {
        if( !diffusePath.IsEmpty() ) {
            DiffuseMap = RendererFrontEnd::GetTexture( diffusePath, diffusePath );
        } else {
            DiffuseMap = RendererFrontEnd::GetDefaultWhiteTexture();
        }

        _shader = RendererFrontEnd::GetBuiltinMaterialShader( MaterialType::Unlit );
    }

    UnlitMaterial::~UnlitMaterial() {
        if( DiffuseMap ) {
            RendererFrontEnd::ReleaseTexture( DiffuseMap->GetName() );
            DiffuseMap = nullptr;
        }

        _shader = nullptr;
    }


    // Material manager
    struct MaterialEntry {
        U64 ReferenceCount = 0;
        BaseMaterial* Material;
    };

    std::map<TString, MaterialEntry> _materials;
    UnlitMaterial* _defaultMaterial;
    U64 _defaultMaterialReferences = 0;

    void MaterialManager::Initialize() {
        _defaultMaterial = new UnlitMaterial( "__DEFAULT__", "assets/textures/default.png" );
    }

    void MaterialManager::Shutdown() {

        // Clean up all material instances.
        auto it = _materials.begin();
        while( it != _materials.end() ) {
            delete it->second.Material;
            ++it;
        }

        _materials.clear();

        // Also release the default texture.
        delete _defaultMaterial;
        _defaultMaterial = nullptr;
    }

    const bool MaterialManager::Exists( const TString& name ) {
        return _materials.find( name ) != _materials.end();
    }

    BaseMaterial* MaterialManager::Get( const TString& name ) {
        auto entry = _materials.find( name );
        if( entry == _materials.end() ) {
            // Return default "warning" texture
            Logger::Warn( "Unable to find a material named '%s'. Assigning default material.", name.CStr() );
            _defaultMaterialReferences++;
            return _defaultMaterial;
        } else {
            entry->second.ReferenceCount++;
            return entry->second.Material;
        }
    }

    void MaterialManager::Release( const TString& name ) {
        auto entry = _materials.find( name );
        if( entry == _materials.end() ) {
            if( name == _defaultMaterial->Name ) {
                _defaultMaterialReferences--;
                Logger::Trace( "Released reference to default material" );
            } else {
                Logger::Warn( "Unable to release reference to unknown material '%s'.", name.CStr() );
            }
        } else {
            entry->second.ReferenceCount--;
            if( entry->second.ReferenceCount <= 0 ) {
                Logger::Trace( "All known references to material '%s' have been released. Unloading material." );
                delete entry->second.Material;
                _materials.erase( name );
            }
        }
    }

    UnlitMaterial* MaterialManager::CreateUnlit( const TString& name, const TString& diffusePath ) {
        auto entry = _materials.find( name );
        if( entry == _materials.end() ) {
            Logger::Trace( "Creating new material named '%s', diffuse: '%s'.", name.CStr(), diffusePath.CStr() );
            MaterialEntry newEntry;
            newEntry.ReferenceCount = 1;
            newEntry.Material = new UnlitMaterial( name, diffusePath );
            _materials.emplace( name, newEntry );
            return static_cast<UnlitMaterial*>( newEntry.Material );
        } else {
            Logger::Warn( "A material named '%s' already exists. Returning a reference to existing material.", name.CStr() );
            entry->second.ReferenceCount++;
            return static_cast<UnlitMaterial*>( entry->second.Material );
        }
    }
}