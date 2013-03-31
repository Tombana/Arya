#include "../include/common/GameLogger.h"
#include "../include/Server.h"
#include "../include/ServerGameSession.h"
#include "../include/ServerClient.h"
#include "../include/ServerClientHandler.h"
#include "../include/EventCodes.h"
#include "../include/Units.h"
#include "../include/Faction.h"
#include "../include/Map.h"
#include "../include/MapInfo.h"
#include "Arya.h"

ServerGameSession::ServerGameSession(Server* serv) : GameSession(serv->getScripting(), true), server(serv)
{
	gameStarted = false;
	idFactory = 1;

	gameInfo.playerCount = 0;
	for(int i = 0; i < MAX_PLAYER_COUNT; ++i)
	{
		gameInfo.players[i].accountId = 0;
		gameInfo.players[i].sessionHash = 0;
		gameInfo.players[i].slot = 0;
		gameInfo.players[i].color = 0;
		gameInfo.players[i].team = 0;
	}

	map = 0;
	initMap();
}

ServerGameSession::~ServerGameSession()
{
    //Deleting the factions will cause all units to be deleted
	for(factionIterator it = clientFactionList.begin(); it != clientFactionList.end(); ++it)
		delete *it;
	clientFactionList.clear();
}

void ServerGameSession::initialize()
{
	if(!clientFactionList.empty())
	{
		GAME_LOG_WARNING("ServerGameSession initialize called but factionlist was already populated. Removing old factions.");
		for(factionIterator it = clientFactionList.begin(); it != clientFactionList.end(); ++it)
			delete *it;
		clientFactionList.clear();
	}

    //TODO: change this theMap thing to something else, see MapInfo.h
    theMap->onLoad(this);

	//create faction
	for(int i = 0; i < gameInfo.playerCount; ++i)
	{
		//createFaction will register it to the list
		Faction* faction = createFaction();
		faction->setColor(gameInfo.players[i].color);
        clientFactionList.push_back(faction);
    }
    //call the scripts faction load function
    //we use seperate loops to ensure all factions have been created
    //in case the script needs this
    for(int i = 0; i < gameInfo.playerCount; ++i)
        theMap->onLoadFaction(this, clientFactionList[i]->getId(), gameInfo.players[i].slot);
}

void ServerGameSession::addClient(ServerClient* client, int index)
{
	if(index < 0 || index >= gameInfo.playerCount)
	{
		GAME_LOG_WARNING("ServerGameSession addClient called with invalid index!");
		return;
	}
    if(client->getSession())
    {
        GAME_LOG_WARNING("ServerClient was already in a session and then added to another gamesession");
    }
	if(clientFactionList.empty())
	{
		GAME_LOG_ERROR("ServerGameSession addClient called before session was initialized!");
		return;
	}
    client->setSession(this);
	client->setFaction(clientFactionList[index]);
	clientFactionList[index]->setClientId(client->getClientId());
    clientList.push_back(client);
    //Send the full game state (only to the new client)
	client->handler->sendPacket(createFullStatePacket());
}

void ServerGameSession::removeClient(ServerClient* client)
{
    //TODO:
    //Lots of stuff like sending a message
    //to everyone about the client being disconnected
	//
	//Let the faction remain intact in case the player reconnects
    int id = client->getClientId();
    Faction* faction = client->getFaction();
    if(faction) faction->setClientId(-1);
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        if(*iter == client)
        {
            clientList.erase(iter);

			//if there are remaining clients, let them know
            if(!clientList.empty())
            {
                Packet* pak = server->createPacket(EVENT_CLIENT_DISCONNECTED);
                *pak << id;
                sendToAllClients(pak);
            }

            break;
        }
    }
}

Packet* ServerGameSession::createFullStatePacket()
{
	Packet* pak = server->createPacket(EVENT_GAME_FULLSTATE);

	*pak << (int)clientFactionList.size();

	for(factionIterator iter = clientFactionList.begin(); iter != clientFactionList.end(); ++iter)
	{
		Faction* faction = *iter;

		*pak << faction->getClientId();
		*pak << faction->getId();

		faction->serialize(*pak);

		int unitCount = (int)faction->getUnits().size();
		*pak << unitCount;
		for(std::list<Unit*>::iterator uiter = faction->getUnits().begin(); uiter != faction->getUnits().end(); ++uiter)
		{
			*pak << (*uiter)->getId();
			(*uiter)->serialize(*pak);
		}
	}
	return pak;
}

Packet* ServerGameSession::createPacket(int id)
{
	return server->createPacket(id);
}

void ServerGameSession::sendToAllClients(Packet* pak)
{
    if(clientList.empty())
    {
        GAME_LOG_WARNING("calling sendToAllClients but client list is empty. deleting packet.");
        server->deletePacket(pak);
        return;
    }
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        (*iter)->handler->sendPacket(pak);
    }
}

void ServerGameSession::startGame()
{

}

void ServerGameSession::update(float elapsedTime)
{
	for(factionIterator fac = clientFactionList.begin(); fac != clientFactionList.end(); ++fac)
	{
		Faction* faction = *fac;

        if(faction->getUnits().empty()) continue;

        bool lost = false;

        for(list<Unit*>::iterator it = faction->getUnits().begin();
                it != faction->getUnits().end(); )
        {
            Unit* unit = *it;

            if(unit->readyToDelete())
            {
                delete unit;
                it = faction->getUnits().erase(it);
            }
            else
            {
                //during this update, many events can occur like
                //units dieing, units being created, players losing, etc
                //TODO: check if this can result in unexpected behaviour
                // - creating units: get added to the end of list<Unit*>
                //   the std::list specification states that iterators are
                //   still valid after a push_back
                //   note that the new unit will get serverUpdate() in the
                //   same frame that it was created, so its timing will
                //   be different?
                // - removing units: this may never be done within serverUpdate
                //   the only way to remove units is by calling unit->markForDelete()
                //   so that this loop will remove it
                // - big events like a full faction that loses all its units
                //   ???????? solution needed ??????
                unit->update(elapsedTime);
                ++it;
            }
        }

        //all units of a faction have died during this loop
        //player loses
        if(lost)
        {
            if(clientList.empty()==false)
            {
                Packet *pak = server->createPacket(EVENT_PLAYER_DEFEAT);
                *pak << faction->getId();
                sendToAllClients(pak);
            }
            for(list<Unit*>::iterator it = faction->getUnits().begin(); it != faction->getUnits().end(); ++it)
            {
                (*it)->markForDelete();
            }
        }
    }
}

void ServerGameSession::handlePacket(ServerClient* client, Packet& packet)
{
    Faction* faction = client->getFaction();
    switch(packet.getId())
    {
		case EVENT_GAME_FULLSTATE_REQUEST:
			client->handler->sendPacket(createFullStatePacket());
			break;
        case EVENT_MOVE_UNIT_REQUEST:
            {
                if(faction)
                {
                    int count;
                    packet >> count;

                    vector<Unit*> validUnits;
                    validUnits.reserve(count);
                    for(int i = 0; i < count; ++i)
                    {
                        int unitId;
                        vec2 targetPos;
                        packet >> unitId >> targetPos;

                        Unit* unit = getUnitById(unitId);
                        if(unit)
                        {
                            //TODO: check if valid movement
                            unit->setTargetPosition(targetPos);
                            validUnits.push_back(unit);
                        }
                    }

                    if(!validUnits.empty())
                    {
                        Packet* outPak = server->createPacket(EVENT_MOVE_UNIT);

                        *outPak << (int)validUnits.size();
                        for(unsigned int i = 0; i < validUnits.size(); ++i)
                        {
                            *outPak << validUnits[i]->getId() << validUnits[i]->getTargetPosition();
                        }
                        sendToAllClients(outPak);
                    }
                }
            }
            break;
        case EVENT_ATTACK_MOVE_UNIT_REQUEST:
            {
                if(faction)
                {
                    int count;
                    packet >> count;

                    vector<Unit*> validUnits;
                    validUnits.reserve(count);
                    for(int i = 0; i < count; ++i)
                    {
                        int unitId, targetUnitId;
                        packet >> unitId >> targetUnitId;
                        Unit* unit = getUnitById(unitId);
                        Unit* target = getUnitById(targetUnitId);
                        if(unit && target)
                        {
                            //TODO: check if valid movement
                            unit->setTargetUnit(target);
                            validUnits.push_back(unit);
                        }
                    }

                    if(!validUnits.empty())
                    {
                        Packet* outPak = server->createPacket(EVENT_ATTACK_MOVE_UNIT);

                        *outPak << (int)validUnits.size();
                        for(unsigned int i = 0; i < validUnits.size(); ++i)
                        {
                            *outPak << validUnits[i]->getId() << validUnits[i]->getTargetUnit()->getId();
                        }
                        sendToAllClients(outPak);
                    }
                }
            }
            break;
        default:
            break;
    }
    return;
}

void ServerGameSession::initMap()
{
    //TODO: The map is now reloaded for every game session. Please.
    //Also, the server does not even need the map right now.
    return;
    if(!map)
    {
        map = new Map(theMap);

        if(!map->initHeightData())
        {
            delete map;
            map = 0;
            return;
        }
    }
}
