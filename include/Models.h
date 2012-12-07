#pragma once
#include "common/Singleton.h"
#include "Resources.h"
#include "Mesh.h"
#include "Materials.h"
#include <vector>
#include <GL/glew.h>

using std::vector;

namespace Arya
{
    enum ModelType
    {
        ModelTypeUnkown = 0,
        ModelTypeStatic = 1,
        VertexAnimated = 2,
        BoneAnimated = 3
    };

    //Base class for an animation state
    //Subclasses can be bone positions or keyframes
    //and so on
    class AnimationState
    {
        public:
            AnimationState(){}
            virtual ~AnimationState(){}

            virtual void setAnimation(const char* name);
            virtual void updateAnimation(float elapsedTime);
    };

    class Model
    {
        public:
            ModelType modelType;

            const vector<Mesh*>& getMeshes() const { return meshes; }

            //Called by Object
            AnimationState* createAnimationState();
            void addRef(){ refCount++; }
            void release(){ refCount--; }

        protected:
            //Private constructor because only
            //ModelManager is allowed to create these
            friend class ModelManager;
            friend class ResourceManager<Model>;
            Model();
            virtual ~Model();

            //Adds refcount to mesh as well
            void addMesh(Mesh* mesh);

            Mesh* createAndAddMesh();

            vector<Mesh*> meshes;
            vector<Material*> materials;

            int refCount;
            //TODO: keep a list of objects that
            //use this model. This way we can
            //use OpenGL instancing
    };

    class ModelManager : public Singleton<ModelManager>, public ResourceManager<Model> {
        public:
            ModelManager();
            ~ModelManager();

            int initialize();
            void cleanup();

            Model* getModel(const char* filename){ return getResource(filename); }
        private:
            Model* loadResource( const char* filename );
    };
}
