#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Arya
{
    Entity::Entity()
    {
        position = vec3(0);
        pitch = 0;
        yaw = 0;
        updateMatrix = true;

        graphicsComponent = 0;
    }

    Entity::~Entity()
    {
        if( graphicsComponent ) delete graphicsComponent;
    }

    const mat4& Entity::getMoveMatrix()
    {
        if(updateMatrix) {
            updateMatrix = false;
            mMatrix = glm::translate(mat4(1.0f), position);
            mMatrix = glm::rotate(mMatrix, yaw,   vec3(0.0, 0.0, 1.0)); //z-axis is up
            mMatrix = glm::rotate(mMatrix, pitch, vec3(1.0, 0.0, 0.0));
        }
        return mMatrix;
    }

    void Entity::setModel(Model* model)
    {
        if( graphicsComponent ) delete graphicsComponent;
        graphicsComponent = new ModelGraphicsComponent();
        ((ModelGraphicsComponent*)graphicsComponent)->model = model;
    }

}
