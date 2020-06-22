
#include "UpdateManager.h"

#include "../Containers/List.h"

namespace Epoch {

    List<IUpdatable*> _updatables;

    void UpdateManager::Update( const F32 deltaTime ) {
        U32 updatableCount = _updatables.Size();
        for( U32 i = 0; i < updatableCount; ++i ) {
            if( _updatables[i] ) {
                _updatables[i]->Update( deltaTime );
            }
        }
    }

    void UpdateManager::StartListening( IUpdatable* obj ) {
        _updatables.Add( obj );
    }

    void UpdateManager::StopListening( IUpdatable* obj ) {
        _updatables.Remove( obj );
    }
}