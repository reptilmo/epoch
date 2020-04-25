#pragma once

#include "../Containers/List.h"
#include "../Defines.h"
#include "../Types.h"
#include "../String/TString.h"


namespace Epoch {

    class World;
    class Entity;
    class RenderableEntityComponent;
    struct WorldRenderableObjectTable;

    /**
     * Represents a container which holds entities, which in turn hold many types of components.
     */
    class EPOCH_API Level {
    public:
        TString Name;
    public:
        Level( const TString& name );
        ~Level();

        void Load();
        void Unload();

        void Update( const F32 deltaTime );
        
        void OnEntityAdded( Entity* entity );
        void OnEntityRemoved( Entity* entity );
        void OnRenderableEntityComponentAdded( RenderableEntityComponent* component );
        void OnRenderableEntityComponentRemoved( RenderableEntityComponent* component );

        /**
         * Adds all renderable components to the provided render table. Recursive to child levels.
         * 
         * @param renderableObjectTable The render table to add to.
         */
        void AddRenderablesToTable( WorldRenderableObjectTable** renderableObjectTable );

        /**
         * Obtain the count of all renderable components. Recursive to child levels.
         */
        const U32 MaxRenderableComponentCount() const;

        const bool IsRoot() const { return _isRoot; }
        Entity* GetRootEntity() { return _root; }

    private:

        // Should only ever be called by the World.
        Level();

    private:
        bool _isRoot = false;
        Level* _parent;
        List<Level*> _children;

        Entity* _root;

        // A flat list of all entities.
        List<Entity*> _entities;
        List<RenderableEntityComponent*> _renderableEntityComponents;

        friend class World;
    };
}