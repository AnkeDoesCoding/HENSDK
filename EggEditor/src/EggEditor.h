#ifndef _EGGEDITOR_H_
#define _EGGEDITOR_H_

#include "HENEngine.h"

class Editor : public hen::Application
{
public:
    void Initialise(SDL_Window* window) override;
    void Shutdown() override;

    void FixedUpdate() override;
    void Update(float deltaTime) override;
};

#endif // !_EGGEDITOR_H_