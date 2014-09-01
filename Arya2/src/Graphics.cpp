#include "common/Logger.h"
#include "Camera.h"
#include "Entity.h"
#include "EntitySystem.h"
#include "Geometry.h"
#include "Graphics.h"
#include "Materials.h"
#include "Models.h"
#include "Renderer.h"
#include "Shaders.h"
#include "Textures.h"
#include "World.h"

namespace Arya
{
    Graphics::Graphics()
    {
        renderer = new Renderer;
        camera = new Camera;
        defaultShader = 0;
    }

    Graphics::~Graphics()
    {
        delete camera;
        delete renderer;
        if (defaultShader) delete defaultShader;
    }

    bool Graphics::init(int width, int height)
    {
        if (!renderer->init()) return false;

        defaultShader = new ShaderProgram("../shaders/staticmodel.vert",
                "../shaders/staticmodel.frag");
        if (!defaultShader->isValid()) {
            delete defaultShader;
            defaultShader = 0;
            LogError << "Could not load default shader." << endLog;
            return false;
        }

        resize(width, height);

        return true;
    }

    void Graphics::resize(int width, int height)
    {
        if(height > 0)
            camera->setProjectionMatrix(45.0f, ((float)width)/((float)height), 0.1f, 2000.0f);
    }

    void Graphics::clear(int width, int height)
    {
        renderer->clear(width, height);
    }

    void Graphics::render(World* world)
    {
        defaultShader->use();
        defaultShader->setUniformMatrix4fv("vpMatrix", camera->getVPMatrix());
        defaultShader->setUniformMatrix4fv("viewMatrix", camera->getVMatrix());

        const list<Entity*>& entities = world->getEntitySystem()->getEntities();
        for(auto ent : entities) {
            if(ent->graphicsComponent == 0) continue;

            ModelGraphicsComponent* grComp = (ModelGraphicsComponent*)ent->graphicsComponent;

            //TODO: Everything done below should be done in a general
            // "RenderSpec" way
            Model* model = grComp->model;

            //TODO: Investigate the bounding box and also check onScreen.z ?
            //mat4 totalMatrix = camera->getVPMatrix() * ent->getMoveMatrix();
            //bool flag = false;
            //for(int j = 0; j < 8; j++)
            //{
            //    vec4 onScreen(model->getBoundingBoxVertex(j), 1.0);
            //    onScreen = totalMatrix * onScreen;
            //    onScreen /= onScreen.w;
            //    //LogDebug << "Onscreen = " << onScreen << endLog;
            //    if(!(onScreen.x < -1.0 || onScreen.x > 1.0 || onScreen.y < -1.0 || onScreen.y > 1.0))
            //    {
            //        flag = true;
            //        break;
            //    }
            //}
            //if(flag == false) continue;

            defaultShader->setUniform3fv("tintColor", vec3(0.5, 1.0, 0.5));
            defaultShader->setUniformMatrix4fv("mMatrix", ent->getMoveMatrix());

            int frame = 0;
            float interpolation = 0.0f;
            AnimationState* animState = grComp->getAnimationState();
            if(animState)
            {
            //    frame = animState->getCurFrame();
            //    interpolation = animState->getInterpolation();
            }
            defaultShader->setUniform1f("interpolation", interpolation);

            for(Mesh* mesh : model->getMeshes())
                renderer->renderMesh(mesh, frame, defaultShader);

        }
        return;
    }

    void Graphics::update(float elapsed)
    {
        camera->update(elapsed);
    }

}
