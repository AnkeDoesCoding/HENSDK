#include "tools/henConfig.h"

#include "tools/henConsole.h"

namespace hen::config
{

    static std::string Trim(const std::string& string)
    {
        size_t start = string.find_first_not_of(" \t");
        size_t end = string.find_last_not_of(" \t");

        if (start == std::string::npos)
        {
            return "";
        }

        return string.substr(start, end - start + 1);
    }

    static Section& GetSection(File& file, const std::string& name)
    {
        for (Section& section : file.Sections)
        {
            if (section.Name == name)
            {
                return section;
            }
        }

        file.Sections.push_back({ name, {} });
        return file.Sections.back();
    }

    bool File::HasKey(std::string sectionName, std::string keyName)
    {
        if (Sections.size() == 0)
        {
            HEN_WARN("[hen::config] Config file: " + Name + " doesn't have any keys");
            return false;
        }

        Section& section = GetSection(*this, sectionName);

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                return true;
            }
        }

        return false;
    }

    int File::GetKeyAsInt(std::string sectionName, std::string keyName)
    {
        if (!HasKey(sectionName, keyName))
        {
            HEN_WARN("[hen::config] Config file: " + Name + " doesn't have key: " + keyName);
            return 0;
        }

        Section& section = GetSection(*this, sectionName);

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                return std::stoi(key.Value);
            }
        }

        return 0;
    }

    bool File::GetKeyAsBool(std::string sectionName, std::string keyName)
    {
        if (!HasKey(sectionName, keyName))
        {
            HEN_WARN("[hen::config] Config file: " + Name + " doesn't have key: " + keyName);
            return false;
        }

        Section& section = GetSection(*this, sectionName);

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                return key.Value == "true" || key.Value == "1";
            }
        }

        return false;
    }

    float File::GetKeyAsFloat(std::string sectionName, std::string keyName)
    {
        if (!HasKey(sectionName, keyName))
        {
            HEN_WARN("[hen::config] Config file: " + Name + " doesn't have key: " + keyName);
            return 0.0f;
        }

        Section& section = GetSection(*this, sectionName);

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                return std::stof(key.Value);
            }
        }

        return 0.0f;
    }

    std::string File::GetKeyAsString(std::string sectionName, std::string keyName)
    {
        if (!HasKey(sectionName, keyName))
        {
            HEN_WARN("[hen::config] Config file: " + Name + " doesn't have key:" + keyName);
            return "";
        }

        Section& section = GetSection(*this, sectionName);

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                return key.Value;
            }
        }

        return "";
    }

    void File::SetKeyValue(std::string sectionName, std::string keyName, int value)
    {
        Section& section = GetSection(*this, sectionName);

        if (!HasKey(sectionName, keyName))
        {
            Key newKey;
            newKey.Name = keyName;
            newKey.Value = std::to_string(value);
            
            section.Keys.push_back(newKey);
        }

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                key.Value = std::to_string(value);
            }
        }
    }

    void File::SetKeyValue(std::string sectionName, std::string keyName, bool value)
    {
        Section& section = GetSection(*this, sectionName);

        if (!HasKey(sectionName, keyName))
        {
            Key newKey;
            newKey.Name = keyName;
            newKey.Value = value ? "true" : "false";
            
            section.Keys.push_back(newKey);
        }

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                key.Value = value ? "true" : "false";
            }
        }
    }   

    void File::SetKeyValue(std::string sectionName, std::string keyName, float value)
    {
        Section& section = GetSection(*this, sectionName);

        if (!HasKey(sectionName, keyName))
        {
            Key newKey;
            newKey.Name = keyName;
            newKey.Value = std::to_string(value);
            
            section.Keys.push_back(newKey);
        }

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                key.Value = std::to_string(value);
            }
        }
    }

    void File::SetKeyValue(std::string sectionName, std::string keyName, std::string value)
    {
        Section& section = GetSection(*this, sectionName);

        if (!HasKey(sectionName, keyName))
        {
            Key newKey;
            newKey.Name = keyName;
            newKey.Value = value;
            
            section.Keys.push_back(newKey);
        }

        for (Key& key : section.Keys)
        {
            if (key.Name == keyName)
            {
                key.Value = value;
            }
        }
    }   


    void Parse(File& configFile, std::string name)
    {
        std::ifstream file("./" + name);
        Section* currentSection = nullptr;
        std::string line;

        if (!file.is_open())
        {
            HEN_ERROR("[hen::config] Failed to open config file: " + name);
            return;
        }

        configFile.Name = name;

        while (std::getline(file, line))
        {
            line = Trim(line);

            if (line.empty() || line[0] == ';' || line[0] == '#')
            {
                continue;
            }

            if (line.front() == '[' && line.back() == ']')
            {
                std::string sectionName = line.substr(1, line.size() - 2);
                currentSection = &GetSection(configFile, sectionName);
                continue;
            }

            size_t eqPos = line.find('=');
            if (eqPos == std::string::npos || !currentSection)
            {
                continue;
            }

            Key key;
            key.Name = Trim(line.substr(0, eqPos));
            key.Value = Trim(line.substr(eqPos + 1));

            currentSection->Keys.push_back(key);
        }
    }

    void Commit(File& configFile, std::string name)
    {
        std::ofstream file("./" + name, std::ios::out | std::ios::trunc);

        if (!file.is_open())
        {
            HEN_WARN("[hen::config] Failed to write to config file:" + name);
            return;
        }

        for (const Section& section : configFile.Sections)
        {
            file << "[" << section.Name << "]\n";

            for (const Key& key : section.Keys)
            {
                file << key.Name << " = " << key.Value << "\n";
            }

            file << "\n";
        }
    }
}