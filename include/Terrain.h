#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

using glm::vec2;
using glm::vec3;
using glm::mat4;
using std::vector;

namespace Arya
{
    class Texture;
    class Shader;
    class ShaderProgram;
    class Scene;
    class Camera;
    class File;

    typedef struct
    {
        vec2 position;
        vec2 offset;
        int lod;
    } Patch;

    class Terrain
    {
        public:
            // Tileset needs to have 4 elements
            Terrain(const char* hm, vector<Texture*> ts, Texture* sm);
            ~Terrain(); 
			
            void render(Camera* cam);
            void update(float dt, Scene* curScene);

            float heightAtGroundPosition(float x, float z);

            bool init();

            GLuint getHeightMapHandle() const { return heightMapHandle; }
            mat4 getScaleMatrix() const { return scaleMatrix; }

        private:
            bool generate();
            bool generateIndices();
            bool generateVAO();

            const char* heightMapName;
            vector<Texture*> tileSet;
            Texture* splatMap;
            GLuint heightMapHandle;
            File* hFile;

            mat4 scaleMatrix;

            GLuint vertexBuffer;
            GLuint* indexBuffer;
            GLuint* indexCount;
            GLuint* vaoHandles;

            ShaderProgram* terrainProgram;

            vector<Patch> patches;
            int patchCount;
            int patchSizeMax;
            int levelMax;
			vec3 lightDirection;
    };
}
