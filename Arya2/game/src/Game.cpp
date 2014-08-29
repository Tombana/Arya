#include "Game.h"
using namespace Arya;

Game::Game()
{

}

Game::~Game()
{

}

bool Game::init()
{
    root = new Arya::Root();

    if(!root->init("Minimal Example",
                1024, 768, false)) {
        return false;
    }

    root->getInputSystem()->bind(INPUT_KEYDOWN, [](int key)
            {
            LogInfo << "Game got keydown: " << key << endLog;
            });

    root->getInputSystem()->bind('a', [](bool down)
            {
            LogInfo << "You " << (down ? "pressed" : "released") << " a !" << endLog;
            });

    root->getInputSystem()->bind('q', [this](bool down)
            {
            if(!down) root->stopGameLoop();
            });

    Arya::Model* model = root->getModelManager()->getModel("ogros.aryamodel");
    if(model) model->getBoundingBoxVertex(2);

    Entity* ent = root->getWorld()->getEntitySystem()->createEntity();
    ent->setModel(model);

    return true;
}

void Game::run()
{
    // enter game loop
    // root returns false if engine closed
    root->gameLoop( [] (float dt) { dt += 0.1; } );
}
