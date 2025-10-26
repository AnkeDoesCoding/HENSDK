#include "input/henInput.h"

#include "core/henTimer.h"
#include "tools/henConsole.h"

#include <map>

namespace hen::input
{
    bool Initialised = false;

    static SDL_Window* Window;

    static KeyboardState Keyboard;
    static MouseState Mouse;

    static std::vector<SDL_Event> Events;

    struct Input 
	{
		BUTTON button = BUTTON::NONE;

		bool operator<(const Input other) 
        {
			return (button != other.button);
		}

		struct LessComparer 
        {
			bool operator()(Input const& a, Input const& b) const 
            {
				return (a.button < b.button);
			}
		};

	};

    static std::map<Input, int, Input::LessComparer> Inputs;

    int ConvertScanCode(const SDL_Scancode& key, const SDL_Keycode& keyCode)
    {
        if (key >= 4 && key <= 29) // A to Z
        { 
            return (key - 4) + CHARACTER_RANGE_START;
        }

        if (key >= 30 && key <= 39) // 0 to 9
        { 
            return (key - 30) + DIGIT_RANGE_START;
        }

        if (key >= 58 && key <= 69) // F1 to F12
        { 
            return (key - 58) + KEYBOARD_BUTTON_F1;
        }

        if (key >= 79 && key <= 82) // Keyboard directional buttons
        { 
            return (82 - key) + KEYBOARD_BUTTON_UP;
        }
        
        switch (key) // Individual scancode key conversion
        { 
            case SDL_SCANCODE_SPACE:
                return KEYBOARD_BUTTON_SPACE;
                break;
            case SDL_SCANCODE_LSHIFT:
                return KEYBOARD_BUTTON_LSHIFT;
                break;
            case SDL_SCANCODE_RSHIFT:
                return KEYBOARD_BUTTON_RSHIFT;
                break;
            case SDL_SCANCODE_RETURN:
                return KEYBOARD_BUTTON_ENTER;
                break;
            case SDL_SCANCODE_ESCAPE:
                return KEYBOARD_BUTTON_ESCAPE;
                break;
            case SDL_SCANCODE_HOME:
                return KEYBOARD_BUTTON_HOME;
                break;
            case SDL_SCANCODE_RCTRL:
                return KEYBOARD_BUTTON_RCONTROL;
                break;
            case SDL_SCANCODE_LCTRL:
                return KEYBOARD_BUTTON_LCONTROL;
                break;
            case SDL_SCANCODE_DELETE:
                return KEYBOARD_BUTTON_DELETE;
                break;
            case SDL_SCANCODE_BACKSPACE:
                return KEYBOARD_BUTTON_BACKSPACE;
                break;
            case SDL_SCANCODE_PAGEDOWN:
                return KEYBOARD_BUTTON_PAGEDOWN;
                break;
            case SDL_SCANCODE_PAGEUP:
                return KEYBOARD_BUTTON_PAGEUP;
                break;
			case SDL_SCANCODE_KP_0:
				return KEYBOARD_BUTTON_NUMPAD0;
                break;
			case SDL_SCANCODE_KP_1:
				return KEYBOARD_BUTTON_NUMPAD1;
                break;
			case SDL_SCANCODE_KP_2:
				return KEYBOARD_BUTTON_NUMPAD2;
                break;
			case SDL_SCANCODE_KP_3:
				return KEYBOARD_BUTTON_NUMPAD3;
                break;
			case SDL_SCANCODE_KP_4:
				return KEYBOARD_BUTTON_NUMPAD4;
                break;
			case SDL_SCANCODE_KP_5:
				return KEYBOARD_BUTTON_NUMPAD5;
                break;
			case SDL_SCANCODE_KP_6:
				return KEYBOARD_BUTTON_NUMPAD6;
                break;
			case SDL_SCANCODE_KP_7:
				return KEYBOARD_BUTTON_NUMPAD7;
                break;
			case SDL_SCANCODE_KP_8:
				return KEYBOARD_BUTTON_NUMPAD8;
                break;
			case SDL_SCANCODE_KP_9:
				return KEYBOARD_BUTTON_NUMPAD9;
                break;
			case SDL_SCANCODE_KP_MULTIPLY:
				return KEYBOARD_BUTTON_MULTIPLY;
                break;
			case SDL_SCANCODE_KP_PLUS:
				return KEYBOARD_BUTTON_ADD;
                break;
			case SDL_SCANCODE_SEPARATOR:
				return KEYBOARD_BUTTON_SEPARATOR;
                break;
			case SDL_SCANCODE_KP_MINUS:
				return KEYBOARD_BUTTON_SUBTRACT;
                break;
			case SDL_SCANCODE_KP_DECIMAL:
				return KEYBOARD_BUTTON_DECIMAL;
                break;
			case SDL_SCANCODE_KP_DIVIDE:
				return KEYBOARD_BUTTON_DIVIDE;
                break;
			case SDL_SCANCODE_INSERT:
				return KEYBOARD_BUTTON_INSERT;
                break;
			case SDL_SCANCODE_TAB:
				return KEYBOARD_BUTTON_TAB;
                break;
			case SDL_SCANCODE_GRAVE: 
    			return KEYBOARD_BUTTON_TILDE;
                break;
			case SDL_SCANCODE_LALT:
				return KEYBOARD_BUTTON_ALT;
                break;
			case SDL_SCANCODE_RALT:
				return KEYBOARD_BUTTON_ALTGR;
                break;
        }

        // Keycode Conversion
        if (keyCode >= 91 && keyCode <= 126){
            return keyCode;
        }

        return -1;
    }
    
    void Initialise(SDL_Window* window)
    {
        Timer timer;

        HEN_ASSERT(window != nullptr, "[hen::input] Window is nullptr");

        Window = window;

        Initialised = true;

        console::Log("[hen::input] Initialised in " + std::to_string((int)std::round(timer.ElapsedMilliseconds())) + " ms");
    }

    void HandleSDLEvent(SDL_Event& event)
    {
        switch (event.type)
        {
            case SDL_EVENT_KEY_DOWN:
            {
                int converted = ConvertScanCode(event.key.scancode, event.key.key);
                if (converted >= 0)
                {
                    Keyboard.Buttons[converted] = true;
                }
            }
                break;
            case SDL_EVENT_KEY_UP:
            {
                int converted = ConvertScanCode(event.key.scancode, event.key.key);
                if (converted >= 0 )
                {
                    Keyboard.Buttons[converted] = false;
                    Input input;
                    input.button = static_cast<BUTTON>(converted);
                    Inputs.erase(input);
                }
            }
                break;
            case SDL_EVENT_TEXT_EDITING:     
            case SDL_EVENT_TEXT_INPUT:    
            case SDL_EVENT_KEYMAP_CHANGED:
                break;
            case SDL_EVENT_MOUSE_MOTION:
                Mouse.Pos.x = event.motion.x;
                Mouse.Pos.y = event.motion.y;
                Mouse.DeltaPos.x += event.motion.xrel;
                Mouse.DeltaPos.y += event.motion.yrel;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    Mouse.LMB = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    Mouse.RMB = true;
                    break;
                case SDL_BUTTON_MIDDLE:
                    Mouse.MMB = true;
                    break;
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:        
                switch (event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        Mouse.LMB = false;
                        break;
                    case SDL_BUTTON_RIGHT:
                        Mouse.RMB = false;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        Mouse.MMB = false;
                        break;
                }
                break;
            case SDL_EVENT_MOUSE_WHEEL:
            {
                float delta = static_cast<float>(event.wheel.y);
                if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                {
                    delta *= -1;
                }
                Mouse.DeltaWheel += delta;
                break;
            }   
            // TODO: ADD CONTROLLER SHIT
            default:
                break;
        }
    }

    void Update()
    {
        Keyboard = GetKeyboardState();
        Mouse = GetMouseState();

        // Dont accumulate this stuff
        Mouse.DeltaWheel = 0;
        Mouse.DeltaPos = glm::vec2(0.0f);

        SDL_GetMouseState(&Mouse.Pos.x, &Mouse.Pos.y);
        
        for(auto& event : Events)
        {
            HandleSDLEvent(event);
        }

        Events.clear();
    }
    
    void ProcessEvent(const SDL_Event& event)
    {
        Events.push_back(event);
    }

    void ClearDelta()
    {   
        Mouse.DeltaWheel = 0.0f;
        Mouse.DeltaPos = glm::vec2(0.0f);
    }

    const KeyboardState& GetKeyboardState() 
    {
        return Keyboard;
    }

    const MouseState& GetMouseState() 
    {
        return Mouse;
    }

    glm::vec2 GetPointerPos()
    {
        return glm::vec2(Mouse.Pos.x, Mouse.Pos.y);
    }

    void SetPointerPos(float newX, float newY)
    {
        SDL_WarpMouseInWindow(Window, newX, newY);
    }

    void HidePointer()
    {
        SDL_HideCursor();
    }

    void ShowPointer()
    {
        SDL_ShowCursor();
    }

    void LockMouse()
    {
        SDL_SetWindowRelativeMouseMode(Window, true);
    }

    void UnLockMouse()
    {
        SDL_SetWindowRelativeMouseMode(Window, false);
    }

    bool Down(BUTTON button)
    {
        uint16_t keycode = (uint16_t)button;

		switch (button)
		{
		case MOUSE_BUTTON_LEFT:
			if (Mouse.LMB) 
            {
				return true;
            }
			return false;
            break;
		case MOUSE_BUTTON_RIGHT:
			if (Mouse.RMB) 
            {
				return true;
            }
			return false;
            break;
		case MOUSE_BUTTON_MIDDLE:
			if (Mouse.MMB) 
            {
				return true;
            }
			return false;
            break;
        default: 
            break;
        }

        return Keyboard.Buttons[keycode] == 1;
    }

    bool Press(BUTTON button)
    {
        if (!Down(button))
        {
			return false;
        }

		Input input;
		input.button = button;
		auto iter = Inputs.find(input);

		if (iter == Inputs.end())
		{
			Inputs.insert(std::make_pair(input, 0));
			return true;
		}

		return false;
    }   

}