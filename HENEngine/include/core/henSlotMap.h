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
            uint32_t Index;
            uint32_t Generation;
        };

    public:
        Handle Add(const T& value) 
        {
            uint32_t index;

            if (!m_FreeList.empty()) 
            {
                index = m_FreeList.back();
                m_FreeList.pop_back();
                m_Slots[index].Value = value;
                m_Slots[index].Generation++;
                m_Slots[index].Alive = true;
            } 
            else 
            {
                index = (uint32_t)m_Slots.size();
                m_Slots.push_back({ value, 1, true });
            }

            return { index, m_Slots[index].Generation };
        }

        T* Get(const Handle& handle) 
        {
            if (handle.index >= m_Slots.size())
            { 
                return nullptr;
            }

            m_Slot& slot = m_Slots[handle.index];

            if (!slot.Alive || slot.Generation != handle.generation)
            { 
                return nullptr;
            }
            return &slot.Value;
        }

        void Remove(const Handle& handle)
        {
            if (handle.Index >= m_Slots.size())
            {
                return;
            }
            m_Slot& slot = m_Slots[handle.Index];

            if (!slot.Alive || slot.Generation != handle.Generation) 
            {
                return;
            }
            slot.Alive = false;
            m_FreeList.push_back(handle.Index);
        }

        bool IsAlive(const Handle& handle) const 
        {
            if (handle.Index >= m_Slots.size())
            {
                return false;
            }

            const m_Slot& slot = m_Slots[handle.Index];

            return { slot.Alive && slot.Generation == handle.Generation };
        }
    
    private:
        struct m_Slot 
        {
            T Value;
            uint32_t Generation = 1;
            bool Alive;
        };

        std::vector<m_Slot> m_Slots;
        std::vector<uint32_t> m_FreeList;
    };
}


#endif // !_HENSLOTMAP_H_