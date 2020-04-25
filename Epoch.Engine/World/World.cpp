
#include "Entity.h"
#include "Level.h"
#include "World.h"


namespace Epoch {


    World::World() {
        _rootLevel = new Level();
        _rootLevel->Name = "__ROOT__";
        _rootLevel->Load();
    }

    World::~World() {
        if( _rootLevel ) {
            _rootLevel->Unload();
            delete _rootLevel;
            _rootLevel = nullptr;
        }
    }

    void World::Update( const F32 deltaTime ) {
        if( _rootLevel ) {
            _rootLevel->Update( deltaTime );
        }
    }

    WorldRenderableObjectTable* World::GetRenderableObjects() {
        if( !_objectTable ) {
            _objectTable = new WorldRenderableObjectTable();
        }

        // Reset counts
        _objectTable->StaticMeshCount = 0;        

        if( _rootLevel ) {
            _rootLevel->AddRenderablesToTable( &_objectTable );
        } 

        return _objectTable;
    }
}