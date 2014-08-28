#pragma once

#include <glm/glm.hpp>

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

    class Entity
    {
        private:
            //Only EntitySystem can create entities
            friend class EntitySystem;
            Entity();
            ~Entity();

        public:
            inline const vec3& getPosition() const { return position; }
            inline float getPitch() const { return pitch; }
            inline float getYaw() const { return yaw; }

            inline void setPosition(const vec3& pos) { position = pos; updateMatrix = true; }
            inline void setPitch(float p) { pitch = p; updateMatrix = true; }
            inline void setYaw(float y) { yaw = y; updateMatrix = true; }

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
