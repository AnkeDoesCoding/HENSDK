#include "core/henCVar.h"

#include "tools/henConsole.h"

#include <sstream>
#include <algorithm>

namespace hen::cvar
{

    CVar cvar_ProtectionEnabled("protection_enabled", true);

    CVar::CVar(const std::string& name, VALUE_TYPE defaultValue, FLAGS flag, std::function<void()> onChange)
        : Name(name), Value(defaultValue), DefaultValue(defaultValue), Flag(flag), OnChange(onChange)
    {
        if (GetSystem())
        {
            GetSystem()->RegisterCVar(this);
        }
        else
        {
            Pending().push_back(this);
        }   

    }

    void CVar::Set(VALUE_TYPE newValue)
    {
        if ((Flag & FLAGS_PROTECTED) && cvar_ProtectionEnabled.GetBool())
        {
            HEN_WARN("[hen::cvar] CVar " + Name + " is a protected CVar");
            return;
        }

        Value = newValue;

        if (OnChange)
        {
            OnChange();
        }  
    }

    int CVar::GetInt() const
    {
        return std::get<int>(Value);
    }

    float CVar::GetFloat() const
    {
        return std::get<float>(Value);
    }

    const std::string& CVar::GetString() const
    {
        return std::get<std::string>(Value);
    }

    bool CVar::GetBool() const
    {
        return std::get<bool>(Value);
    }

    std::vector<CVar*>& CVar::Pending()
    {
        static std::vector<CVar*> list;
        return list;
    }

    System::System()
    {
        RegisterPendingCVars();
    }

    void System::RegisterCVar(CVar* cvar)
    {
        CVars[cvar->Name] = cvar;
    }

    void System::RegisterPendingCVars()
    {
        for (CVar* cvar : CVar::Pending())
        {
            RegisterCVar(cvar);
        }
        CVar::Pending().clear();
    }

    std::string System::ListCVars() const
    {
        // sort cvars alphabetically
        std::vector<std::string> keys;
        keys.reserve(CVars.size());
        for (auto &pair : CVars)
        {
            keys.push_back(pair.first);
        }
        std::sort(keys.begin(), keys.end());

        auto getPrefix = [](const std::string& key) -> std::string 
        {
            size_t pos = key.find('_');
            if (pos == std::string::npos)
            {
                return key;
            }
            return key.substr(0, pos);
        };

        std::stringstream sString;
        std::string lastPrefix;

        for (size_t i = 0; i < keys.size(); i++)
        {
            const std::string& key = keys[i];
            const auto& cvar = CVars.at(key);

            std::string prefix = getPrefix(key);

            // add dividers after each prefix (r_, a_, etc.)
            if (prefix != lastPrefix)
            {
                if (!lastPrefix.empty())
                {
                    sString << "\n";
                }

                sString << "---- " << prefix << " ----\n\n";
                lastPrefix = prefix;
            }

            sString << key << " = ";

            std::visit([&](auto&& val)
            {
                using T = std::decay_t<decltype(val)>;

                if constexpr (std::is_same_v<T, bool>)
                {
                    sString << (val ? "true" : "false");
                }
                else if constexpr (std::is_same_v<T, std::string>)
                {
                    sString << val;
                }
                else
                {
                    sString << std::to_string(val);
                }
            }, cvar->Value);

            if (i + 1 < keys.size())
            {
                sString << "\n";
            }
        }

        return sString.str();
    }

    std::vector<std::string> System::GetAllCVarNames() const
    {
        std::vector<std::string> names;
        for (auto& [name, cvar] : CVars)
        {
            names.push_back(name);
        }
        return names;
    }

    CVar* System::GetCVar(const std::string& name)
    {
        auto cvar = CVars.find(name);
        return (cvar != CVars.end() ? cvar->second : nullptr);
    }
}