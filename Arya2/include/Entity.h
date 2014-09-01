#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::mat4;

namespace Arya
{
    class RenderSpec;
    class Model;
    class AnimationState;

    class GraphicsComponent
    {
        public:
            GraphicsComponent() {}
            virtual ~GraphicsComponent() {}
            virtual RenderSpec* getRenderSpec() { return 0; }

            //TODO: Move into renderspec
            virtual Model* getModel() const { return 0; }
            virtual AnimationState* getAnimationState() const { return 0; }
            virtual void setAnimation(const char* /* name */) { return; }
            virtual void updateAnimation(float /* elapsedTime */) { return; }
    };

    class ModelGraphicsComponent : public GraphicsComponent
    {
        public:
            ModelGraphicsComponent();
            ~ModelGraphicsComponent();

            Model* getModel() const override { return model; }
            AnimationState* getAnimationState() const override { return animState; }
            void setAnimation(const char* name) override;
            void updateAnimation(float elapsedTime) override;

            //! setModel releases the old model and animationstate.
            //! If the new model is nonzero,
            //! it creates a new AnimationState object
            void setModel(Model* model);

            //! Set the animation time for the currently set animation
            void setAnimationTime(float time);
        private:
            Model* model;
            AnimationState* animState;
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

            //! Updates all components
            void update(float elapsedTime);

            //! Creates a ModelGraphicsComponent with the specified model
            void setModel(Model* model);

            //! Get the graphics component (can be zero)
            GraphicsComponent* getGraphics() const { return graphicsComponent; }

        private:
            vec3 position;
            float pitch;
            float yaw;

            mat4 mMatrix; //cached version of position,pitch,yaw
            bool updateMatrix;

            //Components
            GraphicsComponent* graphicsComponent;
    };
}
