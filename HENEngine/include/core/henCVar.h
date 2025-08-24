#ifndef _HENCVAR_H_
#define _HENCVAR_H_

#include <string>
#include <functional>
#include <unordered_map>
#include <variant>

#define VALUE_TYPE std::variant<std::string, int, float, bool>

namespace hen::cvar
{
    enum FLAGS
    {
        FLAGS_NONE = 0,
        FLAGS_ARCHIVE = 1 << 0, // TODO: IMPLEMENT
        FLAGS_PROTECTED = 1 << 1
    };

    class CVar
    {
        friend class System;
    public:
        
        explicit CVar(const std::string& name, VALUE_TYPE defaultValue, FLAGS flag = FLAGS_NONE, std::function<void()> onChange = nullptr);

        void Set(VALUE_TYPE newValue);

        int GetInt() const;
        float GetFloat() const;
        const std::string& GetString() const;
        bool GetBool() const;

    public:

        std::string Name;
        VALUE_TYPE Value;
        VALUE_TYPE DefaultValue;
        
        FLAGS Flag;        
        std::function<void()> OnChange;

    private:

        static std::vector<CVar*>& Pending();

    };

    class System
    {
    public:
        System();

        void RegisterCVar(CVar* cvar);
        void RegisterPendingCVars();
        void Execute(const std::string& command);


        std::string ListCVars() const;
        std::vector<std::string> GetAllCVarNames() const;

        CVar* GetCVar(const std::string& name);


    private:
        std::unordered_map<std::string, CVar*> CVars;

    };

    inline System*& GetSystem()
	{
		static System* system = nullptr;
		return system;
	}

}

#endif // !_HENCVAR_H_