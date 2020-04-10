#pragma once

#include <xhash>

#include "../Types.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"

namespace Epoch {

    struct Vertex3D {

        Vector3 Position;
        Vector3 Normal;
        Vector2 TexCoord;
        Vector3 Color;

        // Required for used in unordered_map
        bool operator==( const Vertex3D& other ) const {
            return other.Position == Position && other.Normal == Normal && other.Color == Color && other.TexCoord == TexCoord;
        }
    };
}

namespace std {

    template<class T>
    inline void hash_combine( std::size_t& seed, const T& v ) noexcept {
        std::hash<T> hasher;
        seed ^= hasher( v ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
    }

    // Required for used in unordered_map
    template<> struct hash<Epoch::Vertex3D> {
        size_t operator() ( Epoch::Vertex3D const& vertex ) const {
            U64 h = hash<Epoch::Vector3>()( vertex.Position );
            hash_combine( h, hash<Epoch::Vector3>()( vertex.Normal ) );
            hash_combine( h, hash<Epoch::Vector2>()( vertex.TexCoord ) );
            hash_combine( h, hash<Epoch::Vector3>()( vertex.Color ) );
            return h;
            /*U64 pos = hash<Epoch::Vector3>()( vertex.Position );
            U64 nrm = hash<Epoch::Vector3>()( vertex.Normal );
            U64 tex = hash<Epoch::Vector2>()( vertex.TexCoord );
            return ( ( pos ^ ( nrm << 1 ) ) >> 1 ) ^
                ( tex << 1 );*/
        }
    };
}