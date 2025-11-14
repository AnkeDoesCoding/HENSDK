#ifndef _MODELIMPORTER_H_
#define _MODELIMPORTER_H_

#include "HENEngine.h"

#include <string>

namespace importer
{
    void ImportModel(std::string path, hen::level::MeshComponent& meshComp, hen::level::MaterialComponent& materialComp);
}

#endif // !_MODELIMPORTER_H_