#ifndef _HENCONFIG_H_
#define _HENCONFIG_H_

#include <string>
#include <vector>
#include <fstream>

namespace hen::config
{
    struct Key
    {
        std::string Name;
        std::string Value;
    };

    struct Section
    {
        std::string Name;
        std::vector<Key> Keys;
    };

    struct File
    {
        std::vector<Section> Sections;
        std::string Name; // needed for debug purposes

        bool HasKey(std::string sectionName, std::string keyName);
        
        int GetKeyAsInt(std::string sectionName, std::string keyName);
        bool GetKeyAsBool(std::string sectionName, std::string keyName);
        float GetKeyAsFloat(std::string sectionName, std::string keyName);
        std::string GetKeyAsString(std::string sectionName, std::string keyName);

        void SetKeyValue(std::string sectionName, std::string keyName, int value);
        void SetKeyValue(std::string sectionName, std::string keyName, bool value);
        void SetKeyValue(std::string sectionName, std::string keyName, float value);
        void SetKeyValue(std::string sectionName, std::string keyName, std::string value);
        
    };

    void Parse(File& configFile, std::string name);
    void Commit(File& configFile, std::string name);
}

#endif // !_HENCONFIG_H_