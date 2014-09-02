#include "InputSystem.h"
#include "common/Logger.h"
#include <string>
#include <algorithm>
#include <SDL2/SDL.h>
using std::string;

namespace Arya
{
    InputSystem::InputSystem()
    {
    }

    InputSystem::~InputSystem()
    {
    }

    void InputSystem::bind(INPUTEVENT event, function<void(int)> f)
    {
        if( event == INPUT_KEYDOWN )
            bindingKeyDown = f;
        else if( event == INPUT_KEYUP )
            bindingKeyUp = f;
        else if( event == INPUT_MOUSEWHEEL )
            bindingMouseWheel = f;
        else
            LogWarning << "Trying to bind event with invalid function signature or invalid event type" << endLog;
    }

    void InputSystem::bind(INPUTEVENT event, function<void(MOUSEBUTTON,int,int)> f)
    {
        if( event == INPUT_MOUSEBUTTONDOWN )
            bindingMouseDown = f;
        else if( event == INPUT_MOUSEBUTTONUP )
            bindingMouseUp = f;
        else
            LogWarning << "Trying to bind event with invalid function signature or invalid event type" << endLog;
    }


    void InputSystem::bind(INPUTEVENT event, function<void(int,int,int,int)> f)
    {
        if( event == INPUT_MOUSEMOVEMENT )
            bindingMouseMovement = f;
        else
            LogWarning << "Trying to bind event with invalid function signature or invalid event type" << endLog;
    }

    void InputSystem::bind(const char* key, function<void(bool)> f)
    {
        InputKey k;
        if (!k.parseKey(key)) {
            LogWarning << "Unable to bind key. Could not parse: " << key << endLog;
            return;
        }
        bindingKey[k] = f;
        return;
    }

    void InputSystem::unbind(INPUTEVENT event)
    {
        switch(event) {
            case INPUT_KEYDOWN:
                bindingKeyDown = nullptr;
                break;
            case INPUT_KEYUP:
                bindingKeyUp = nullptr;
                break;
            case INPUT_MOUSEBUTTONDOWN:
                bindingMouseDown = nullptr;
                break;
            case INPUT_MOUSEBUTTONUP:
                bindingMouseUp = nullptr;
                break;
            case INPUT_MOUSEMOVEMENT:
                bindingMouseMovement = nullptr;
                break;
            case INPUT_MOUSEWHEEL:
                bindingMouseWheel = nullptr;
                break;
            default:
                break;
        }
    }

    void InputSystem::unbind(const char* key)
    {
        InputKey k;
        if (!k.parseKey(key)) {
            LogWarning << "Cannot unbind key. Unable to parse: " << key << endLog;
            return;
        }
        auto f = bindingKey.find(k);
        if( f == bindingKey.end() ) return;
        bindingKey.erase(f);
    }

    MOUSEBUTTON translateButton(Uint8 btn)
    {
        if( btn == SDL_BUTTON_LEFT )   return MOUSEBUTTON_LEFT;
        if( btn == SDL_BUTTON_MIDDLE ) return MOUSEBUTTON_MIDDLE;
        if( btn == SDL_BUTTON_RIGHT )  return MOUSEBUTTON_RIGHT;
        return MOUSEBUTTON_LEFT;
    }

    void InputSystem::handleInputEvent(const SDL_Event& event)
    {
        switch(event.type) {
            case SDL_KEYDOWN:
                {
                    //Do not count repeated keys, only for text-capture mode
                    if(event.key.repeat) break;
                    InputKey k(event.key.keysym);
                    if( bindingKeyDown ) bindingKeyDown(k.keysym);
                    auto f = bindingKey.find(k);
                    if( f != bindingKey.end() ) f->second(true);
                }
                break;
            case SDL_KEYUP:
                {
                    //Do not count repeated keys, only for text-capture mode
                    if(event.key.repeat) break;
                    InputKey k(event.key.keysym);
                    if( bindingKeyUp ) bindingKeyUp(k.keysym);
                    auto f = bindingKey.find(k);
                    if( f != bindingKey.end() ) f->second(false);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if( bindingMouseDown ) {
                    bindingMouseDown(translateButton(event.button.button),
                            event.button.x, event.button.y);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if( bindingMouseUp ) {
                    bindingMouseUp(translateButton(event.button.button),
                            event.button.x, event.button.y);
                }
                break;
            case SDL_MOUSEMOTION:
                if( bindingMouseMovement ) {
                    bindingMouseMovement(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                }
                break;
            case SDL_MOUSEWHEEL:
                if( bindingMouseWheel ) {
                    bindingMouseWheel(event.wheel.y);
                }
                break;
            default:
                LogWarning << "Unkown event received in InputSystem" << endLog;
                break;
        }
    }

    InputSystem::InputKey::InputKey(const SDL_Keysym& sdlkey)
    {
        keysym = (int)sdlkey.sym;
        mod = (int)sdlkey.mod & ~(KMOD_NUM | KMOD_CAPS);
    }

    bool InputSystem::InputKey::parseKey(const char* _key)
    {
        if (_key == 0 || _key[0] == 0) return false;
        string key(_key);
        //remove spaces and transform to lowercase
        key.erase(std::remove_if(key.begin(), key.end(), isspace), key.end());
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        keysym = 0;
        mod = 0;
        if (key.size() == 1) {
            keysym = (int)key[0];
            return true;
        } else {
            //If the string contains any of the substrings shown below
            //then remove it from the string. At the end there
            //should be only one character left which will be the letter
            string::size_type pos;
            pos = key.find("shift+");
            if (pos != string::npos) {
                mod |= KMOD_SHIFT;
                key.erase(pos,6);
            }
            pos = key.find("ctrl+");
            if (pos != string::npos) {
                mod |= KMOD_CTRL;
                key.erase(pos,5);
            }
            pos = key.find("s+");
            if( pos != string::npos) {
                mod |= KMOD_SHIFT;
                key.erase(pos,2);
            }
            pos = key.find("c+");
            if (pos != string::npos) {
                mod |= KMOD_CTRL;
                key.erase(pos,2);
            }
            if (key.size() == 1) {
                keysym = (int)key[0];
                return true;
            }
        }
        LogWarning << "Could not parse keybinding: " << _key << endLog;
        return false;
    }

}
