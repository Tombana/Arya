#include "Entity.h"

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

}
