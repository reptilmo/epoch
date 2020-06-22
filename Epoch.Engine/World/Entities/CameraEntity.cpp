
#include "CameraEntity.h"
#include "../../Math/TMath.h"

namespace Epoch {

    CameraEntity::CameraEntity() : Entity() {
        UpdateManager::StartListening( this );
    }

    CameraEntity::~CameraEntity() {
        UpdateManager::StopListening( this );
    }

    void CameraEntity::Update( const F32 deltaTime ) {
        Quaternion rotation = GetRotation();

        // Yaw
        if( _turningLeft ) {
            _yaw += ( -_rotateSpeed * deltaTime );
            rotation *= Quaternion::FromAxisAngle( Vector3::Up(), _yaw );
            _viewDirty = true;
        } else if( _turningRight ) {
            _yaw += ( _rotateSpeed * deltaTime );
            rotation *= Quaternion::FromAxisAngle( Vector3::Up(), _yaw );
            _viewDirty = true;
        }

        // Pitch
        if( _lookingDown ) {
            float pitch = _pitch + ( -_rotateSpeed * (float)deltaTime );

            // Ensure the rotation is clamped between -89 and 89 deg
            _pitch = TMath::ClampFloat32( pitch, -89.0f, 89.0f );
            rotation *= Quaternion::FromAxisAngle( Vector3::Left(), pitch );
            _viewDirty = true;
        } else if( _lookingUp ) {
            float pitch = _pitch + ( _rotateSpeed * (float)deltaTime );

            // Ensure the rotation is clamped between -89 and 89 deg
            _pitch = TMath::ClampFloat32( pitch, -89.0f, 89.0f );
            rotation *= Quaternion::FromAxisAngle( Vector3::Left(), pitch );
            _viewDirty = true;
        }

        SetRotation( rotation );

        // Recalculate vectors
        if( _viewDirty ) {
            _forward.X = TMath::Cos( _pitch ) * TMath::Cos( _yaw );
            _forward.Y = TMath::Sin( _pitch );
            _forward.Z = TMath::Cos( _pitch ) * TMath::Sin( _yaw );
            _forward.Normalize();

            Vector3 up = Vector3::Up();
            _right = Vector3::Normalized( Vector3::Cross( up, _forward ) );
            _up = Vector3::Cross( _forward, _right );
        }

        // Handle movement
        Vector3 up = Vector3::Up();
        F32 moveSpeed = 5.0f;
        F32 finalSpeed = moveSpeed * deltaTime;

        //Vector3 vSpeed( finalSpeed );
        Vector3 position = GetPosition();
        bool positionUpdated = false;
        if( _movingForward ) {
            position += ( _forward * finalSpeed );
            positionUpdated = true;
            _viewDirty = true;
        }
        if( _movingBackward ) {
            position -= ( _forward * finalSpeed );
            positionUpdated = true;
            _viewDirty = true;
        }

        if( _movingLeft ) {
            position += ( _right * finalSpeed );
            positionUpdated = true;
            _viewDirty = true;
        }
        if( _movingRight ) {
            position -= ( _right * finalSpeed );
            positionUpdated = true;
            _viewDirty = true;
        }

        if( _movingUp ) {
            position += ( up * finalSpeed );
            positionUpdated = true;
            _viewDirty = true;
        }
        if( _movingDown ) {
            position -= ( up * finalSpeed );
            positionUpdated = true;
            _viewDirty = true;
        }

        if( positionUpdated ) {
            SetPosition( position );
        }
    }

    Matrix4x4* CameraEntity::GetViewMatrix() {
        if( !_viewDirty ) {
            return &_view;
        } else {
            // Use the owning object's position. Combine with PYR of this camera. Scale can be ignored.
        }
    }
}