#include "logging.h"
#include "events.h"
#include "settings.h"

void OnMessage(SKSE:: MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        logger::trace("Game data loaded");
        RE::ConsoleLog::GetSingleton()->Print("LLP -- Data loaded"); // REMOVE LATER
        Settings::LoadSettings();
        Events::Register();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) 
{
    SetupLog();
    
    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto name{ plugin->GetName() };
    const auto version{ plugin->GetVersion() };
    
    logger::info("Game version is: {}", skse->RuntimeVersion().string());
    logger::info("{} v{}.{}.{} is loading...", name, version.major(), version.minor(), version.patch());

    SKSE::Init(skse);

    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

    logger::info("{} has finished loading.", name);

    return true;
}
