#include "../include/ServerClient.h"
#include "../include/ServerGameSession.h"
#include "../include/Units.h"

ServerClient::~ServerClient()
{
    if(gameSession) gameSession->removeClient(this);
    if(faction) delete faction;
}

void ServerClient::createFaction()
{
    if(!faction) faction = new Faction;
    faction->setId(clientId);
    faction->setColor(clientId%5);
}

void ServerClient::createStartUnits()
{
    if(!faction) return;

    for(int i = 0; i < 30; ++ i) 
    {
        Unit* unit = new Unit(0);
        unit->setPosition(vec3(20.0f * (i / 10), 0.0f, -50.0f + 20.0f * (i % 10)));
        faction->addUnit(unit);
    }
    return;
}