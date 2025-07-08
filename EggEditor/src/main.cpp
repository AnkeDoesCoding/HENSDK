
#include "HENEngine.h"

int main()
{
    hen::Application EggEditor;

    EggEditor.Initialise();

    if(EggEditor.Initialised)
    {
        EggEditor.Run();
    }

    EggEditor.Shutdown();

    return 0;
}