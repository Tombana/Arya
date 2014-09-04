//GameSessionClient is the main (root) class for all ingame related things
#pragma once
#include "GameSession.h"

#include <vector>
using std::vector;

class Faction;
struct CellList;

class GameSessionClient : public GameSession
{
    public:
        GameSessionClient();
        ~GameSessionClient();

        //! See GameSession.h
        bool isServer() const override { return false; }

        //! Initialize the session
        //! Registers keybindings and event handlers
        bool init();

        //! Completely rebuilds cell lists
        void rebuildCellList();

        //! Return the faction of the local player
        //! TODO: What if the player could control multiple factions
        Faction* getLocalFaction() const { return localFaction; } ;

        //! Returns a list of all factions
        const vector<Faction*>& getFactions() const { return factions; }

        //! Update all units and so on
        void update(int elapsedTime); //in ms

        //TODO
        //void handleEvent(Packet& packet);

     private:
        GameSessionInput* input;
        Faction* localFaction;
        vector<Faction*> factions;
        vector<int> clients;

        CellList* unitCells;
};
