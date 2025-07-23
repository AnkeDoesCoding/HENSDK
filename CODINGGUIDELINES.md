# Coding Guidelines

## Filenames and Folder Structure

Filenames should all be named using Pascal Case.

Filenames should be descriptive and contain the namespace that the file is part of:

```
henShader.cpp
henApplication.cpp
etc...
```

Private headers and source files should be put into `src/` and public headers should be organised into `include/`.

If a system has a lot of files for it, use an underscore to create categories:

```
henRHC_OpenGL.cpp
henRHC_OpenGL.h
...
henRHC_Vulkan.cpp
henRHC_Vulkan.h
...
henRHC_DX12.cpp
henRHC_DX12.h
```

## Braces and spacing

### Braces

HEN SDK uses Allman style braces.

You must use braces for all conditional statementsk, **DO NOT** omit braces:

```cpp
if(x)
{
    continue; // Correct
}

if(x)
    continue; // Wrong!
```

### Spacing

Pointers are aligned with the type and not the variable name:

```cpp
RenderHardwareContext* RHC; // Correct

RenderHardwareContext *RHC; // Wrong!
RenderHardwareContext * RHC; // Wrong!
```

here are always spaces between operators:

```cpp
int x = y + z; // Correct

int x = y+z; // Wrong
```

### Preprocessor Directives

All preprocessor directives need to be indented to match the surrounding code:

```cpp
namespace hen::platform
{
    void GetPlatformString(std::string* str)
    {
        std::string platformStr = "";

        #if PLATFORM_WINDOWS
            platformStr = "Windows";
        #elif defined(PLATFORM_LINUX)
            platformStr = "Linux";
        #endif

        str = platformStr;
    }
}
```

## Namespaces

The global HEN Engine namespace is `hen`, avoid putting things in the global namespace instead put things in domain specific namespaces:

```
hen::renderer
hen::input
hen::scene
etc...
```

**AVOID** `using namespace` directives

It it preferred to use C++17 namespace syntax over the old syntax, so do this:

```cpp
// This is correct
namespace hen::scene
{
    ...
}

// This is wrong
namespace hen
{
    namespace scene
    {
        ...
    }
}
```

It's okay to use the old syntax if you have multiple namespace in a single file:

```cpp
namespace hen
{
    namespace physics
    {
        ...
    }

    namespace jolt
    {
        ...
    }
}
```

## Naming Conventions

### Variables

Variables that dont go out of scope must use Pascal Case:

```cpp
int AnInteger;
```

Variables that are function parameters or go out of scope must use Camel Case:

```cpp
void Create(int someParam, float anotherParam);

void DoSomething()
{
    int aLocalInteger;
}
```

The following variable prefixes MUST be used:

* m_ for class/struct private members
* s_ for class/struct static members
* g_ any compile unit global variables

### Structure/Types

Struct/class names must use Pascal Case:

```cpp
class Application {};
struct KeyboardState {};
```

Structs/classes must be laid out as follows:

```cpp
struct/class Foo
{
    public:

    // All public functions

    public:

    // All public members

    protected:

    // All protected functions

    protected:

    // All protected members

    private:

    // All private functions

    private: 

    // All private members
}
```

### Enums

Enum names and members must be declared using ALL CAPS with Snake Case:

```cpp
enum BUTTON
{
    NONE = 0,

    DIGIT_RANGE_START = 48,

	CHARACTER_RANGE_START = 65,

    ...

    KEYBOARD_BUTTON_UP,
	KEYBOARD_BUTTON_DOWN,
	KEYBOARD_BUTTON_LEFT,
	KEYBOARD_BUTTON_RIGHT,	
	KEYBOARD_BUTTON_SPACE,
	KEYBOARD_BUTTON_RSHIFT,
	KEYBOARD_BUTTON_LSHIFT,

    ...
};
```

### Macros

Macros must be decalared in ALL CAPS with Snake Case 