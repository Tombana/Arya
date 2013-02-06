#include "../include/common/GameLogger.h"
#include "../include/Game.h"
#include "../include/GameSession.h"
#include "../include/Network.h"
#include "../include/Events.h"

Game* Game::singleton = 0;

#define NETWORK_POLL 0.05f

Game::Game()
{
    root = 0;
    session = 0;
    eventManager = 0;
    network = 0;
    clientId = 0;

    timeSinceNetworkPoll = 1.0f;

    singleton = this;
}

Game::~Game()
{
    if(eventManager) delete eventManager;
    if(network) delete network;
    if(session) delete session;
    if(root) delete &Root::shared();
}

void Game::run()
{
    root = new Root;

    if(!(root->init(false, 800, 600))) {
        GAME_LOG_ERROR("Unable to init root");
    }
    else
    {
        root->addInputListener(this);

        if(network) delete network;
        network = new Network;

        //network->connectToSessionServer("balubu.com", 13337);
        //network->startServer();
        network->connectToSessionServer("localhost", 13337);

        if(eventManager) delete eventManager;
        eventManager = new EventManager(network);

        network->setPacketHandler(eventManager);

        Event& joinEvent = eventManager->createEvent(EVENT_JOIN_GAME);
        joinEvent << 0; // accountId
        joinEvent << 0; // roomId
        joinEvent.send();

        eventManager->addEventHandler(EVENT_GAME_READY, this);
        eventManager->addEventHandler(EVENT_CLIENT_ID, this);

        root->addFrameListener(this);
        root->startRendering();
    }
}

bool Game::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;
    switch(key) {
        case 'P':
            if(keyDown) {
                if(session) delete session;
                session = new GameSession;
                if(!session->init()) {
                    GAME_LOG_ERROR("Could not start a new session");
                    Root::shared().stopRendering();
                }
            }
            break;

        case 'L':
            if(keyDown)
            {
                if(session) delete session;
                session = 0;
            }
            break;

		case '8':
			if(keyDown)
			{
				Event& ev = getEventManager()->createEvent(EVENT_GAME_FULLSTATE_REQUEST);
				ev.send();
			}
			break;

        case 'O': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
        case 'I': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        case GLFW_KEY_F11: Root::shared().setFullscreen(!Root::shared().getFullscreen()); break;
        case GLFW_KEY_ESC: if(keyDown) Root::shared().stopRendering(); break;
        default: keyHandled = false; break;
    }
    return keyHandled;
}

bool Game::mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y)
{
    return false;
}

bool Game::mouseWheelMoved(int delta)
{
    return false;
}

bool Game::mouseMoved(int x, int y, int dx, int dy)
{
    return false; 
}

void Game::onFrame(float elapsedTime)
{
    timeSinceNetworkPoll += elapsedTime;
    if(timeSinceNetworkPoll > NETWORK_POLL)
    {
        network->update();
        timeSinceNetworkPoll = 0.0f;
    }
}

void Game::handleEvent(Packet& packet)
{
    int id = packet.getId();
    switch(id)
    {
        case EVENT_CLIENT_ID:
            packet >> clientId;
            break;

        case EVENT_GAME_READY:
            if(session) delete session;
            session = new GameSession;

            if(!session->init()) {
                GAME_LOG_ERROR("Could not start a new session");
                Root::shared().stopRendering();
            }

            GAME_LOG_INFO("Game is ready");
            break;

        default:
            GAME_LOG_INFO("Game: unknown event received! (" << id << ")");
            break;
    }
}
