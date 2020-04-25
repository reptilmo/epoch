
#include "EntityComponents/StaticMeshEntityComponent.h"
#include "EntityComponents/EntityComponent.h"
#include "World.h"
#include "../Math/TMath.h"
#include "../Resources/StaticMesh.h"

#include "Entity.h"
#include "Level.h"

namespace Epoch {

    // This constructor is only available to the World via friend and prvately.
    // It is used to construct the root level.
    Level::Level() {
        _isRoot = true;
    }

    Level::Level( const TString& name ) {
        Name = name;
    }

    Level::~Level() {
        Name.Clear();
    }

    void Level::Load() {
        _root = new Entity();
        _root->Name = "__ROOT__";
        _root->setLevel( this );

        // TODO: Need to read in from a file (deserialize?) and stand up object heirarchy appropriately.
        // TODO: This is a test, remove this in favour of actual loading later.

        StaticMesh* testMesh = new StaticMesh( "test", "assets/models/Teapot001.esm" );
        testMesh->Load();

        //srand( 43456 );

        for( U32 i = 0; i < 100; ++i ) {
            TString name = "testObj";
            name.Append( i );
            Entity* entity = Entity::Create( name );
            F32 min = -15.0f;
            F32 max = 15.0f;
            F32 x = TMath::FloatRandomRange( min, max );
            F32 y = TMath::FloatRandomRange( min, max );
            F32 z = TMath::FloatRandomRange( min, max );

            F32 smin = 0.2f;
            F32 smax = 2.0f;
            F32 scale = TMath::FloatRandomRange( smin, smax );

            entity->SetPosition( Vector3( x, y, z ) );
            entity->SetScale( Vector3( scale, scale, scale ) );
            _root->AddChild( entity );

            // Add a test entity component.
            StaticMeshEntityComponent* staticMeshComponent2 = EntityComponentFactory::CreateComponent<StaticMeshEntityComponent>( "TestComponent2" );
            staticMeshComponent2->SetStaticMesh( testMesh );
            entity->AddComponent( staticMeshComponent2 );
        }
    }

    void Level::Unload() {

    }

    void Level::Update( const F32 deltaTime ) {

        // Randomly rotate the objects in the scene. TODO: Remove this temporary test logic.
        U32 count = _entities.Size();
        for( I32 i = 0; i < count; ++i ) {
            F32 amount = ( i % 2 == 0 ) ? 1.0f : -1.0f;
            if( _entities[i] ) {
                Quaternion q = Quaternion::FromAxisAngle( Vector3::Up(), deltaTime * amount );
                Quaternion rotation = _entities[i]->GetRotation() * q;
                _entities[i]->SetRotation( rotation );
            }
        }
    }

    void Level::OnEntityAdded( Entity* entity ) {
        _entities.Add( entity );
    }

    void Level::OnEntityRemoved( Entity* entity ) {
        _entities.Remove( entity );
    }

    void Level::OnRenderableEntityComponentAdded( RenderableEntityComponent* component ) {
        _renderableEntityComponents.Add( component );
    }

    void Level::OnRenderableEntityComponentRemoved( RenderableEntityComponent* component ) {
        _renderableEntityComponents.Remove( component );
    }

    void Level::AddRenderablesToTable( WorldRenderableObjectTable** renderableObjectTable ) {
        U32 entityCount = _renderableEntityComponents.Size();
        for( U32 i = 0; i < entityCount; ++i ) {
            RenderableEntityComponent* entity = _renderableEntityComponents[i];

            // Add static meshes types
            if( entity->GetRenderableComponentType() == RenderableComponentType::STATIC_MESH ) {
                ( *renderableObjectTable )->StaticMeshes[( *renderableObjectTable )->StaticMeshCount] = static_cast<StaticMeshEntityComponent*>( entity );
                ( *renderableObjectTable )->StaticMeshCount++;
                /* StaticMeshEntityComponent* castedComponent = static_cast<StaticMeshEntityComponent*>( entity );
                 if( castedComponent->HasReferenceData() ) {
                     List<StaticMeshRenderReferenceData> referenceData = castedComponent->GetReferenceData();
                     U32 refDataCount = referenceData.Size();
                     for( U32 r = 0; r < refDataCount; ++r ) {
                         renderableObjectTable->StaticMeshes[renderableObjectTable->StaticMeshCount] = referenceData[r];
                         renderableObjectTable->StaticMeshCount++;
                         renderableObjectTable->StaticMeshes.Add( referenceData[r] );
                     }
                 }*/
            }
            // TODO: other types
        }
    }

    const U32 Level::MaxRenderableComponentCount() const {
        U32 totalCount = _renderableEntityComponents.Size();
        U32 childCount = _children.Size();
        for( U32 i = 0; i < childCount; ++i ) {
            totalCount += _children[i]->MaxRenderableComponentCount();
        }
        return totalCount;
    }
}
