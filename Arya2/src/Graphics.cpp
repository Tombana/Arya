#include "common/Logger.h"
#include "Camera.h"
#include "Entity.h"
#include "EntitySystem.h"
#include "Geometry.h"
#include "Graphics.h"
#include "Materials.h"
#include "Models.h"
#include "Renderer.h"
#include "Textures.h"
#include "World.h"

#include <GL/glew.h>

namespace Arya
{
    Graphics::Graphics()
    {
        renderer = new Renderer;
        camera = new Camera;
        materialSystem = 0;
        textureSystem = 0;
    }

    Graphics::~Graphics()
    {
        delete camera;
        delete renderer;
    }

    bool Graphics::init()
    {
        glewExperimental = GL_TRUE; 
        glewInit();
        if (!GLEW_VERSION_3_1)
            LogWarning << "No OpenGL 3.1 support! Continuing" << endLog;
        return true;
    }

    void Graphics::clear(int width, int height)
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void Graphics::render(World* world)
    {
        const list<Entity*>& entities = world->getEntitySystem()->getEntities();
        for(auto ent : entities) {
            if(ent->graphicsComponent == 0) continue;

            //TODO: Everything done below should be done in a general
            // "RenderSpec" way
            Model* model = ((ModelGraphicsComponent*)ent->graphicsComponent)->model;

            mat4 totalMatrix = camera->getVPMatrix() * ent->getMoveMatrix();
            bool flag = false;
            for(int j = 0; j < 8; j++)
            {
                vec4 onScreen(model->getBoundingBoxVertex(j), 1.0);
                onScreen = totalMatrix * onScreen;
                onScreen /= onScreen.w;
                if(!(onScreen.x < -1.0 || onScreen.x > 1.0 || onScreen.y < -1.0 || onScreen.y > 1.0))
                {
                    flag = true;
                    break;
                }
            }
            //if(flag == false) continue;

            //basicProgram->setUniform3fv("tintColor", objects[i]->getTintColor());
            //basicProgram->setUniformMatrix4fv("mMatrix", objects[i]->getMoveMatrix());

            int frame = 0;
            //float interpolation = 0.0f;
            //AnimationState* animState = objects[i]->getAnimationState(); //can be zero
            //if(animState)
            //{
            //    frame = animState->getCurFrame();
            //    interpolation = animState->getInterpolation();
            //    basicProgram->setUniform1f("interpolation", interpolation);
            //}

            for(Mesh* mesh : model->getMeshes()) {
                if(mesh->getGeometry() && mesh->getGeometry()->frameCount > 0)
                {
                    Material* mat = mesh->getMaterial();
                    //basicProgram->setUniform1i("tex", 0);
                    glActiveTexture(GL_TEXTURE0);
                    if(mat) {
                        glBindTexture(GL_TEXTURE_2D, mat->texture->handle);
                        //basicProgram->setUniform4fv("parameters", vec4(mat->specAmp,mat->specPow,mat->ambient,mat->diffuse));
                    }
                    mesh->getGeometry()->draw(frame);
                }
            }

        }
        return;
    }

    void Graphics::update(float elapsed)
    {
        camera->update(elapsed);
    }

}
