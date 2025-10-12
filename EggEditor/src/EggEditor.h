#pragma once

#include "HENEngine.h"

class Editor : public hen::Application
{
public:
    void Initialise(SDL_Window* window) override;
    void Shutdown() override;

    void FixedUpdate() override;
    void Update(float deltaTime) override;
};

