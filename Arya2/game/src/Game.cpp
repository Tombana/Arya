#include "Game.h"
#include "GameLogger.h"
using namespace Arya;

Game::Game()
{
    pressedLeft = pressedRight = false;
    totalTime = 0;
    entityCreated = false;
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

    Arya::InputSystem* input = root->getInputSystem();
    input->bind("Q", [this](bool down) { if(!down) root->stopGameLoop(); });
    input->bind("A", [this](bool down) { pressedLeft = down; });
    input->bind("D", [this](bool down) { pressedRight = down; });

    input->bind("ctrl+shift+f", [](bool down) { if(down) GameLogDebug << "ctrl+shift+f" << endLog; });

    return true;
}

void Game::run()
{
    root->gameLoop( std::bind(&Game::update, this, std::placeholders::_1) );
}

void Game::update(float dt)
{
    totalTime += dt;

    //We only create the entity after 0.5 seconds so that
    //the game first shows the window with black screen
    //instead of waiting to load all files
    //Step one towards a loading screen ;)
    if (!entityCreated && totalTime > 0.5f) {
        entityCreated = true;

        Arya::Model* model = root->getModelManager()->getModel("ogros.aryamodel");
        if(model) model->getBoundingBoxVertex(2);

        Entity* ent = root->getWorld()->getEntitySystem()->createEntity();
        ent->setPosition(vec3(0,0,0));
        ent->setModel(model);
        ent->getGraphics()->setAnimation("run");
    }

    Arya::Camera* cam = root->getGraphics()->getCamera();
    if (pressedLeft && !pressedRight) cam->rotate(1.5f*dt, 0);
    else if (pressedRight && !pressedLeft) cam->rotate(-1.5f*dt, 0);
}
