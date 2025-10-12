#pragma once

#include "HENEngine.h"

#include <string>

namespace importer
{
    void ImportModel(std::string path, hen::level::MeshComponent& meshComp);
}