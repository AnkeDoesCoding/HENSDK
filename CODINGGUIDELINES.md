# Coding Guidelines

## Filenames and Folder Structure

Filenames should all be named using Pascal Case.

Filenames should be descriptive and contain the namespace that the file is part of:

```
henGraphics.cpp
henApplication.cpp
etc...
```

Source files should be organised into `src/` and public headers should be organised into `include/`.

If a system has a lot of files for it, use an underscore to create categories:

```
henRHC_OpenGL.cpp
henRHC_OpenGL.h
...
henScene_Actors.cpp
henScene_Actors.h
```

## Braces and spacing

### Braces

HEN SDK uses Allman style braces.

You must use braces for all conditional statements, **DO NOT** omit braces:

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
RHC* RHC; // Correct

RHC *RHC; // Wrong!
RHC * RHC; // Wrong!
```

There are always spaces between operators:

```cpp
int x = y + z; // Correct

int x = y+z; // Wrong!
```

## Namespaces

The global HEN Engine namespace is `hen`, avoid putting things in the global namespace, instead put things in domain specific namespaces:

```
hen::renderer
hen::input
hen::scene
etc...
```

**AVOID** `using namespace` directives:

```cpp
// Wrong!
using namespace hen; 
using namespace std;
```

It it preferred to use C++17 namespace syntax over the old syntax, so do this:

```cpp
// Correct
namespace hen::scene
{
    ...
}

// Wrong!
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

Namespaces should be laid out as follows:

```cpp

namespace Foo
{
    // Enums

    // Structs

    // Classes

    // Functions

    // extern
}

```

## Naming Conventions

### Variables

The following variable prefixes MUST be used:

* m_ for class/struct private members
* s_ for class/struct static members
* g_ for compile unit global variables
* cvar_ for CVars

Variables that dont go out of scope or are class/struct members must use Pascal Case:

```cpp
bool Initialised;
```

Variables that are created in '.cpp' files should be static unless they **NEED** to not be static.

```cpp
namespace hen::renderer
{   
    static std::unique_ptr<RHC> CurrentRHC;
}
```

Variables that are function parameters or go out of scope must use Camel Case:

```cpp
Camera(math::Vec3 pos, float yaw, float pitch);

void Camera::SetDirty()
{
    math::Vec3 front;
}
```

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

    ...
};
```

## Preprocessor

### Macros

Macros must be declared in ALL CAPS with Snake Case at the top of the file after includes

```cpp
#define HEN_DEBUG_BREAK() ((void)0)
```

###  Directives

All preprocessor directives must be indented to match the surrounding code:

```cpp
namespace hen::platform
{
    void GetPlatformString(std::string& str)
    {
        #if PLATFORM_WINDOWS
            str = "WINDOWS";
        #elif PLATFORM_LINUX
            str = "LINUX";
        #endif
    }
}
```