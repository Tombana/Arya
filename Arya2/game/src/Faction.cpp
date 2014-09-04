#include "Faction.h"
#include "Unit.h"
#include "GameSession.h"
//#include "Packet.h"
//#include "FactionColors.h"

Faction::Faction(int _id, GameSession* _session) : session(_session), id(_id)
{
	clientId = -1;
    color = 0;
}

Faction::~Faction()
{
    for(auto it = units.begin(); it != units.end(); )
    {
        delete *it;
        it = units.erase(it);
    }
    session->destroyFaction(id);
}

void Faction::addUnit(Unit* unit)
{
    //unit->setTintColor(factionColors[color]);
    unit->setFactionId(id);
    units.push_back(unit);
}

//void Faction::serialize(Packet& pk)
//{
//    pk << color;
//}
//
//void Faction::deserialize(Packet& pk)
//{
//    pk >> color;
//}
