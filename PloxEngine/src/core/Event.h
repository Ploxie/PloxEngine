#pragma once

#include <mutex>
#include "eastl/vector.h"

class IBaseListener
{
public:
    virtual ~IBaseListener(){};
};

template<typename T>
class IListener
{
public:
    virtual bool OnEvent(const T& event) = 0;
};

template<typename... Args>
class Listener : public IBaseListener, public IListener<Args>...
{
};

template<typename Container = eastl::vector<IBaseListener*>, typename Mutex = std::mutex>
class BaseEventManager
{
public:
    void Subscribe(IBaseListener* listener)
    {
        std::lock_guard<Mutex> lock(m_mutexLock);
        m_listeners.push_back(listener);
    }

    void Unsubscribe(IBaseListener* observer)
    {
        std::lock_guard<Mutex> lock(m_mutexLock);
        for (auto it = m_listeners.begin(); it != m_listeners.end(); ++it)
        {
            if ((*it) == observer)
            {
                m_listeners.erase(it);
                return;
            }
        }
    }

    template<typename T>
    void Dispatch(const T& event)
    {
        std::lock_guard<Mutex> lock(m_mutexLock);
        for (auto* l: m_listeners)
        {
            auto* listener = dynamic_cast<IListener<T>*>(l);
            if (listener) { listener->OnEvent(event); }
        }
    }

    template<typename T>
    void Dispatch(T&& event)
    {
        Dispatch((const T&) event);
    }

private:
    Container m_listeners;
    Mutex m_mutexLock;
};

class EventManager : public BaseEventManager<>
{
public:
    explicit EventManager() = default;
};