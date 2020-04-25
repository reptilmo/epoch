#pragma once

#include "../../Containers/List.h"
#include "../../String/TString.h"
#include "../WObject.h"

namespace Epoch {

    class Entity;
    class Matrix4x4;
    struct RenderReferenceData;

    class EntityComponent : public WObject {
    public:

        virtual ~EntityComponent() {}

        virtual const bool IsRenderable() const = 0;

        const TString& GetName() const { return _name; }

    protected:
        void setOwningEntity( Entity* entity );
        Entity* getOwningEntity();

    protected:
        TString _name;
    private:
        Entity* _owner;

        friend class Entity;
    };

    enum class RenderableComponentType {
        UNKNOWN,
        STATIC_MESH
    };

    class RenderableEntityComponent : public EntityComponent {
    public:

        virtual ~RenderableEntityComponent() {}

        virtual const Matrix4x4* GetWorldMatrix();

        virtual const bool IsRenderable() const override { return true; }

        virtual RenderReferenceData* GetReferenceData() = 0;

        virtual const RenderableComponentType GetRenderableComponentType() const = 0;

    };

    class EntityComponentFactory final {
    public:

        template<class T>
        static T* CreateComponent( const TString& name );
    };


    template<class T>
    static T* EntityComponentFactory::CreateComponent( const TString& name ) {
        return T::Create( name );
    }
}