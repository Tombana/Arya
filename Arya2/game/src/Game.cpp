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

    Arya::InputSystem* input = root->getInputSystem();
    input->bind('q', [this](bool down) { if(!down) root->stopGameLoop(); });
    input->bind('a', [this](bool down) { pressedLeft = down; });
    input->bind('d', [this](bool down) { pressedRight = down; });

    Arya::Model* model = root->getModelManager()->getModel("ogros.aryamodel");
    if(model) model->getBoundingBoxVertex(2);

    Entity* ent = root->getWorld()->getEntitySystem()->createEntity();
    ent->setPosition(vec3(0,0,0));
    ent->setModel(model);
    ent->getGraphics()->setAnimation("run");

    return true;
}

void Game::run()
{
    root->gameLoop( std::bind(&Game::update, this, std::placeholders::_1) );
}

void Game::update(float dt)
{
    Arya::Camera* cam = root->getGraphics()->getCamera();

    if (pressedLeft && !pressedRight) cam->rotate(1.5f*dt, 0);
    else if (pressedRight && !pressedLeft) cam->rotate(-1.5f*dt, 0);
}
