#pragma once

#include <vector>
#include <glm/glm.hpp>
using std::vector;

namespace Arya
{
    class Terrain;
    class Camera;
    class Texture;
    class Scene;

    class Map
    {
        public:
            Map();
            ~Map();

            bool init(const char* hm, vector<Texture*> ts, Texture* sm);

            bool setTerrain(const char* hm, vector<Texture*> ts, Texture* sm);
            void render(Camera* cam);
            void update(float elapsedTime, Scene* scene);

            Terrain* getTerrain() const { return terrain; };

        private:
            Terrain* terrain;
    };
}
