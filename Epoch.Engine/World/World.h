#pragma once

#include "../Types.h"
#include "../Defines.h"
#include "../String/TString.h"
#include "../Math/Transform.h"
#include "../Containers/List.h"
#include "../Renderer/RenderData.h"
#include "../Resources/StaticMesh.h"

#include "WObject.h"

namespace Epoch {

    class StaticMeshEntityComponent;

    struct WorldRenderableObjectTable {
    public:
        U32 StaticMeshCount = 0;
        StaticMeshEntityComponent** StaticMeshes = nullptr;

        //List<StaticMeshRenderReferenceData> StaticMeshes;

    public:
        WorldRenderableObjectTable() {

            // TODO: Big, but should be more than enough in most cases.
            U64 staticMeshArraySize = sizeof( StaticMeshEntityComponent* ) * 16384;
            StaticMeshes = (StaticMeshEntityComponent**)TMemory::Allocate( staticMeshArraySize );
            TMemory::MemZero( StaticMeshes, staticMeshArraySize );
        }

        ~WorldRenderableObjectTable() {
            if( StaticMeshes ) {
                StaticMeshCount = 0;
                TMemory::Free( StaticMeshes );
                StaticMeshes = nullptr;
            }
        }
    };

    class Entity;

    class Level;

    class World {
    public:
        World();
        ~World();

        void Update( const F32 deltaTime );

        WorldRenderableObjectTable* GetRenderableObjects();
    private:
        Level* _rootLevel;
        WorldRenderableObjectTable* _objectTable = nullptr;
    };

}
