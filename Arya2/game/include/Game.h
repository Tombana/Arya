#include "Arya.h"

class Game
{
    public:
        Game();
        ~Game();

        bool init();
        void run();

    private:
        Arya::Root* root;
        
        bool pressedLeft;
        bool pressedRight;

        void update(float dt);
};
