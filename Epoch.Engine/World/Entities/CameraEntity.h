#pragma once

#include "../Entity.h"
#include "../../Math/Matrix4x4.h"

namespace Epoch {

    /**
     * A special entity which is used to capture scenes from a given perspective.
     */
    class CameraEntity : public Entity {
    public:
        CameraEntity();
        ~CameraEntity();

        /**
         * Called if this object is opted-into updates via UpdateManager::OptIn()
         */
        virtual void Update( const F32 deltaTime ) override;

        Matrix4x4* GetViewMatrix();

    private:
        bool _viewDirty = true;
        Matrix4x4 _view;

        // Degrees/second
        float _rotateSpeed = 45.0f;
        Vector3 _forward, _right, _up;
        float _yaw = 0;
        float _pitch = 0;

        // Hack - putting camera controls here for now until a proper control system is built.
        bool _turningLeft, _turningRight, _lookingUp, _lookingDown;
        bool _movingForward, _movingBackward, _movingLeft, _movingRight, _movingUp, _movingDown;
    };
}