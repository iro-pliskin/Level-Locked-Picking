#pragma once
#include <RE/B/BSTEvent.h>
#include <RE/M/MenuOpenCloseEvent.h>


namespace Events 
{
    class LockpickingMenuHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent> 
    {
    public:
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event,
                                                      RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_source) override;
    };
    void Register();
}
