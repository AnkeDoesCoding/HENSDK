#ifndef _HENSLOTMAP_H_
#define _HENSLOTMAP_H_

#include <vector>
#include <cstdint>
#include <cassert>


namespace hen
{
    template <typename T>
    class SlotMap
    {
    public: 

        struct Handle
        {
            uint32_t Index = 0;
            uint32_t Generation 0;
             
            bool IsValid() const 
            { 
                return Index != 0; 
            }
        };

        struct Slot 
        {
            T Value;
            uint32_t Generation = 1;
            bool Alive = false;
        };

    public:

        template<typename... Args>
        Handle Emplace(Args&&... args) 
        {
            uint32_t index;
            if (!m_FreeList.empty()) 
            {
                index = m_FreeList.back();
                m_FreeList.pop_back();
                
                m_Slots[index].Value = T(std::forward<Args>(args)...);
                m_Slots[index].Alive = true;
            } 
            else 
            {
                index = static_cast<uint32_t>(m_Slots.size());
                Slot slot;
                slot.Value = T(std::forward<Args>(args)...);
                slot.Generation = 1;
                slot.Alive = true;
                m_Slots.push_back(std::move(slot));
            }

            return Handle{ index + 1u, m_Slots[index].Generation };
        }

        Handle Add(T&& value) 
        {
            uint32_t index;
            if (!m_FreeList.empty()) 
            {
                index = m_FreeList.back(); m_FreeList.pop_back();
                m_Slots[index].Value = std::move(value);
                m_Slots[index].Alive = true;
            } 
            else 
            {
                index = static_cast<uint32_t>(m_Slots.size());
                Slot slot;
                slot.Value = std::move(value);
                slot.Generation = 1;
                slot.Alive = true;
                m_Slots.push_back(std::move(slot));
            }

            return Handle{ index + 1u, m_Slots[index].Generation };
        }   

        T* Get(const Handle& handle) 
        {
            if (!handle.IsValid()) 
            {
                return nullptr;
            }

            uint32_t index0 = handle.Index - 1;

            if (index0 >= m_Slots.size())
            {
                return nullptr;
            }

            Slot& slot = m_Slots[index0];

            if (!slot.Alive)
            { 
                return nullptr;
            }

            if (slot.Generation != handle.Generation) 
            {
                return nullptr;
            }

            return &slot.Value;
        }

        T* Get(const Handle& handle) const 
        {
            if (!handle.IsValid()) 
            {
                return nullptr;
            }
            uint32_t index0 = handle.Index - 1;

            if (index0 >= m_Slots.size())
            {
                return nullptr;
            }

            Slot& slot = m_Slots[index0];

            if (!slot.Alive)
            { 
                return nullptr;
            }
            
            if (slot.Generation != handle.Generation) 
            {
                return nullptr;
            }

            return &slot.Value;
        }

        void Remove(const Handle& handle)
        {
            if (!handle.IsValid()) 
            {
                return false;
            }

            uint32_t index0 = handle.Index - 1;

            if (index0 >= m_Slots.size()) 
            {
                return false;
            }

            Slot& slot = m_Slots[index0];

            if (!slot.Alive) 
            {
                return false;
            }

            slot.Alive = false;
            ++slot.Generation;
            m_FreeList.push_back(index0);

            return true;
        }

        bool IsAlive(const Handle& handle) const 
        {
            if (!handle.IsValid()) 
            {
                return false;
            }

            uint32_t index0 = handle.Index - 1;

            if (index0 >= m_Slots.size()) 
            {
                return false;
            }

            const Slot& slot = m_Slots[index0];

            return slot.Alive && slot.Generation == handle.Generation;
        }
    
    private:

        std::vector<Slot> m_Slots;
        std::vector<uint32_t> m_FreeList;
    };
}


#endif // !_HENSLOTMAP_H_