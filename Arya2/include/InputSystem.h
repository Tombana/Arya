#pragma once
#include <functional>
#include <map>
using std::function;
using std::map;

union SDL_Event;
struct SDL_Keysym;

namespace Arya
{
    enum INPUTEVENT
    {
        INPUT_KEYDOWN = 1,
        INPUT_KEYUP,
        INPUT_MOUSEBUTTONDOWN,
        INPUT_MOUSEBUTTONUP,
        INPUT_MOUSEMOVEMENT,
        INPUT_MOUSEWHEEL
    };

    enum MOUSEBUTTON
    {
        MOUSEBUTTON_LEFT = 1,
        MOUSEBUTTON_MIDDLE,
        MOUSEBUTTON_RIGHT
    };

    class InputSystem
    {
        public:
            InputSystem();
            ~InputSystem();

            //! Register for a callback
            //! Only one binding can be used for each event
            //! Different events require different callback prototypes
            //! KEYDOWN and KEYUP           void(int key)
            //! MOUSEDOWN and MOUSEUP       void(MOUSEBUTTON btn, int x, int y)
            //! MOUSEMOVEMENT               void(int x, int y, int relativex, int relativey)
            //! MOUSEWHEEL                  void(int delta)
            //! key                         void(bool down)
            void bind(INPUTEVENT event, function<void(int)> f);
            void bind(INPUTEVENT event, function<void(MOUSEBUTTON,int,int)> f);
            void bind(INPUTEVENT event, function<void(int,int,int,int)> f);
            void bind(const char* key, function<void(bool)> f);

            //! Unbind an event
            void unbind(INPUTEVENT event);
            void unbind(const char* key);

            //! Handle an input related event generated by SDL
            //! Called by Root
            void handleInputEvent(const SDL_Event& event);

        private:
            function<void(int)> bindingKeyDown;
            function<void(int)> bindingKeyUp;
            function<void(MOUSEBUTTON,int,int)> bindingMouseDown;
            function<void(MOUSEBUTTON,int,int)> bindingMouseUp;
            function<void(int,int,int,int)> bindingMouseMovement;
            function<void(int)> bindingMouseWheel;

            struct InputKey
            {
                int keysym;
                int mod;

                InputKey() : keysym(0), mod(0) {}
                InputKey(int k, int m) : keysym(k), mod(m) {}
                InputKey(const SDL_Keysym& sdlkey);
                ~InputKey(){}

                //! Parse and save into this struct
                bool parseKey(const char* key);

                bool operator<(const InputKey& rhs) const {
                    if( keysym != rhs.keysym ) return keysym < rhs.keysym;
                    if( mod != rhs.mod ) return mod < rhs.mod;
                    return false;
                }
            };

            map<InputKey,function<void(bool)> > bindingKey;
    };
}
