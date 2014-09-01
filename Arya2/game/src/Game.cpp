#include "Game.h"
using namespace Arya;

Game::Game()
{
    pressedLeft = pressedRight = false;
}

Game::~Game()
{

}

bool Game::init()
{
    root = new Arya::Root();

    if(!root->init("Minimal Example", 1024, 768, false)) {
        return false;
    }

    //root->getInputSystem()->bind(INPUT_KEYDOWN, [](int key)
    //        {
    //            LogInfo << "Game got keydown: " << key << endLog;
    //        });

    root->getInputSystem()->bind('a',
            [](bool down) {
                LogInfo << "You " << (down ? "pressed" : "released") << " a !" << endLog;
            });

    root->getInputSystem()->bind('q',
            [this](bool down) {
                if(!down) root->stopGameLoop();
            });

    root->getInputSystem()->bind('a', [this](bool down) { pressedLeft = down; });
    root->getInputSystem()->bind('d', [this](bool down) { pressedRight = down; });

    Arya::Model* model = root->getModelManager()->getModel("ogros.aryamodel");
    if(model) model->getBoundingBoxVertex(2);

    Entity* ent = root->getWorld()->getEntitySystem()->createEntity();
    ent->setPosition(vec3(0,0,0));
    ent->setModel(model);

    return true;
}

void Game::run()
{
    // enter game loop
    // root returns false if engine closed
    root->gameLoop( std::bind(&Game::update, *this, std::placeholders::_1) );
}

void Game::update(float dt)
{
    Arya::Camera* cam = root->getGraphics()->getCamera();

    if (pressedLeft && !pressedRight) cam->rotate(0.1f*dt, 0);
    else if (pressedRight && !pressedLeft) cam->rotate(-0.1f*dt, 0);
}
