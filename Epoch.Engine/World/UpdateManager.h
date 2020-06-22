#pragma once

#include "../Types.h"

namespace Epoch {

    class IUpdatable {
    public:
        virtual void Update( const F32 deltaTime ) = 0;
    };

    class UpdateManager {
    public:
        static void Update( const F32 deltaTime );

        static void StartListening( IUpdatable* obj );
        static void StopListening( IUpdatable* obj );

    private:
        UpdateManager() {}
        ~UpdateManager() {}
    };
}