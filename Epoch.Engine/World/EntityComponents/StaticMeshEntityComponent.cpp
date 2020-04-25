
#include "../../Resources/StaticMesh.h"
#include "../Entity.h"

#include "StaticMeshEntityComponent.h"

namespace Epoch {


    StaticMeshEntityComponent* StaticMeshEntityComponent::Create( const TString& name ) {
        StaticMeshEntityComponent* result = static_cast<StaticMeshEntityComponent*>( WObject::Allocate( sizeof( StaticMeshEntityComponent ) ) );

        // Use placement new to call constructor manually.
        new ( result )StaticMeshEntityComponent( name );
        return result;
    }

    void StaticMeshEntityComponent::Destroy( StaticMeshEntityComponent* component ) {

        // Manually call destructor.
        component->~StaticMeshEntityComponent();
        WObject::Free( component );
    }

    StaticMeshEntityComponent::StaticMeshEntityComponent( const TString& name ) {
        _name = name;
    }

    StaticMeshEntityComponent::~StaticMeshEntityComponent() {
        if( _mesh ) {
            delete _mesh;
            _mesh = nullptr;
        }
    }

    const bool StaticMeshEntityComponent::HasReferenceData() {
        return _mesh->IsLoaded();
    }

    RenderReferenceData* StaticMeshEntityComponent::GetReferenceData() {
        return _mesh->GetReferenceData();
    }

    void StaticMeshEntityComponent::SetStaticMesh( StaticMesh* mesh ) {
        _mesh = mesh;
    }
}