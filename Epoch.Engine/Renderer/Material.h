#pragma once

#include "../String/TString.h"

namespace Epoch {

    class ITexture;
    class IShader;

    enum class MaterialType {
        Unlit,
        Basic,
        Physical,
        Custom
    };

    /**
     * The base material for all materials.
     */
    class BaseMaterial {
    public:

        /**
         * The name of this material.
         */
        TString Name;

    public:
        BaseMaterial( const TString& name, const MaterialType type );
        virtual ~BaseMaterial();

        /**
         * Returns the type of this material.
         */
        const MaterialType GetType() const { return _type; }

        /**
         * Returns the shader used by this material.
         */
        IShader* GetShader() { return _shader; }

    protected:
        MaterialType _type;
        IShader* _shader;
    };

    /**
     * A flat, unlit material. This means that this material accepts no lighting information.
     * This is the quickest and simplest material to be rendered.
     */
    class UnlitMaterial : public BaseMaterial {
    public:

        /**
         * The diffuse texture map of this material.
         */
        ITexture* DiffuseMap = nullptr;
    public:
        UnlitMaterial( const TString& name );
        UnlitMaterial( const TString& name, const TString& diffusePath );
        virtual ~UnlitMaterial();
    };

    /**
     * Manages materials throughout the system and is responsible for keeping reference counts.
     * Automatically destroys materials when the reference count reaches 0.
     */
    class MaterialManager {
    public:
        static void Initialize();
        static void Shutdown();

        static const bool Exists( const TString& name );
        static BaseMaterial* Get( const TString& name );

        static void Release( const TString& name );

        static UnlitMaterial* CreateUnlit( const TString& name, const TString& diffusePath );

    };
}