#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::mat4;

namespace Arya
{
    class RenderSpec;

    class GraphicsComponent
    {
        public:
            GraphicsComponent() {}
            virtual ~GraphicsComponent() {}
            virtual RenderSpec* getRenderSpec() { return 0; }
    };

    //TODO: Move this out of here
    class Model;
    class AnimationState;

    class ModelGraphicsComponent : public GraphicsComponent
    {
        public:
            ModelGraphicsComponent() {}
            ~ModelGraphicsComponent() {}

            Model* model;
            AnimationState* animState;

            //setModel also recreates a new AnimationState object
            //void setModel(Model* model); //see oldsrc/Objects.cpp for correct implementation
            //Model* getModel() const { return model; }

            //void setAnimation(const char* name);
            //void setAnimationTime(float time); //for the currently set animation

            AnimationState* getAnimationState() const { return animState; }
            void updateAnimation(float elapsedTime);
    };

    class Entity
    {
        private:
            //TODO: is this ok?
            //Only EntitySystem can create entities
            friend class EntitySystem;
            Entity();
            ~Entity();

        public:
            inline const vec3& getPosition() const { return position; }
            inline vec2 getPosition2() const { return vec2(position.x, position.y); }
            inline float getPitch() const { return pitch; }
            inline float getYaw() const { return yaw; }

            inline void setPosition(const vec3& pos) { position = pos; updateMatrix = true; }
            inline void setPitch(float p) { pitch = p; updateMatrix = true; }
            inline void setYaw(float y) { yaw = y; updateMatrix = true; }

            const mat4& getMoveMatrix();

            //! Creates a ModelGraphicsComponent with the specified model
            void setModel(Model* model);

            //Components
            GraphicsComponent* graphicsComponent;

        private:
            vec3 position;
            float pitch;
            float yaw;

            mat4 mMatrix; //cached version of position,pitch,yaw
            bool updateMatrix;
    };
}
