#pragma once

#include "EntityComponent.h"

namespace Epoch {

    class StaticMesh;

    class StaticMeshEntityComponent : public RenderableEntityComponent {
    public:
        static StaticMeshEntityComponent* Create( const TString& name );
        static void Destroy( StaticMeshEntityComponent* component );

        const bool HasReferenceData();

        virtual RenderReferenceData* GetReferenceData();

        virtual const RenderableComponentType GetRenderableComponentType() const override { return RenderableComponentType::STATIC_MESH; }
    public:
        StaticMeshEntityComponent( const TString& name );
        virtual ~StaticMeshEntityComponent();
        void SetStaticMesh( StaticMesh* mesh );

    private:
        StaticMesh* _mesh;
    };
}