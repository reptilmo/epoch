#include "../Defines.h"

#include "TMath.h"
#include "Quaternion.h"

#include "Rotator.h"

namespace Epoch {

    Rotator::Rotator() {
        Pitch = Yaw = Roll = 0.0f;
    }

    Rotator::Rotator( const float pitch, const float yaw, const float roll ) {
        Pitch = pitch;
        Yaw = yaw;
        Roll = roll;
    }

    Quaternion Rotator::ToQuaternion() const {
        Quaternion q;


        const float RADS_DIVIDED_BY_2 = TMath::DEG2RAD_MULTIPLIER / 2.0f;
        float SP, SY, SR;
        float CP, CY, CR;

        const float PitchNoWinding = fmodf( Pitch, 360.0f );
        const float YawNoWinding = fmodf( Yaw, 360.0f );
        const float RollNoWinding = fmodf( Roll, 360.0f );

        TMath::SinCos( PitchNoWinding * RADS_DIVIDED_BY_2, SP, CP );
        TMath::SinCos( YawNoWinding * RADS_DIVIDED_BY_2, SY, CY );
        TMath::SinCos( RollNoWinding * RADS_DIVIDED_BY_2, SR, CR );

        q.X = CR * SP * SY - SR * CP * CY;
        q.Y = -CR * SP * CY - SR * CP * SY;
        q.Z = CR * CP * SY - SR * SP * CY;
        q.W = CR * CP * CY + SR * SP * SY;

        /*float pitchR = TMath::DegToRad( Pitch );
        float yawR = TMath::DegToRad( Yaw );
        float rollR = TMath::DegToRad( Roll );
        float c1 = cosf( yawR );
        float s1 = sinf( yawR );
        float c2 = cosf( pitchR );
        float s2 = sinf( pitchR );
        float c3 = cosf( rollR );
        float s3 = sinf( rollR );
        q[3] = sqrtf( 1.0f + c1 * c2 + c1 * c3 - s1 * s2 * s3 + c2 * c3 ) / 2.0f;
        float w4 = ( 4.0f * q[3] );
        q[0] = ( c2 * s3 + c1 * s3 + s1 * s2 * c3 ) / w4;
        q[1] = ( s1 * c2 + s1 * c3 + c1 * s2 * s3 ) / w4;
        q[2] = ( -s1 * s3 + c1 * s2 * c3 + s2 ) / w4;*/
        return q;
    }

    Vector3 Rotator::ToEuler() const {
        return Vector3( Pitch, Yaw, Roll );
    }

    float& Rotator::operator[]( const int index ) {
        ASSERT_MSG( index < 3 && index >= 0, "Rotator index out of bounds." );
        return ( &Pitch )[index];
    }

    const float& Rotator::operator[]( const int index ) const {
        ASSERT_MSG( index < 3 && index >= 0, "Rotator index out of bounds." );
        return ( &Pitch )[index];
    }

    Rotator Rotator::FromQuaternion( const Quaternion& q ) {
        return q.ToRotator();
    }

    Rotator Rotator::Zero() {
        return Rotator( 0, 0, 0 );
    }

    F32 Rotator::ClampAxis( F32 angle ) {
        F32 newAngle = fmodf( angle, 360.0f );
        if( angle < 0.0f ) {
            angle += 360.0f;
        }

        return angle;
    }

    F32 Rotator::NormalizeAxis( F32 angle ) {
        angle = ClampAxis( angle );

        if( angle > 180.0f ) {
            angle -= 360.0f;
        }

        return angle;
    }
}