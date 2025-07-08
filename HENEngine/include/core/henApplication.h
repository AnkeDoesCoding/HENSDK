#ifndef _HENAPPLICATION_H_
#define _HENAPPLICATION_H_

namespace hen
{
    class Application
    {
    public:
        Application();
        ~Application();

        virtual void Initialise();
        virtual void Shutdown();
        
        void Run();

        virtual void FixedUpdate();
        virtual void Update(double dT);

    public:
        
        bool Initialised = false;
    
    };
}

#endif // !_HENAPPLICATION_H_