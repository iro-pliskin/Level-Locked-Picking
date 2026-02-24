#include "PCH.h"
#include "logging.h"
#include "events.h"

void OnMessage(SKSE:: MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        logger::trace("Game data loaded");
        RE::ConsoleLog::GetSingleton()->Print("LLP -- Data loaded"); // REMOVE LATER
        
        // The below is lowkey useless since player lockpicking is always at 15 but i'll keep it for reference for now.

        auto player = RE::PlayerCharacter::GetSingleton(); 
        float playerlockpickingskill = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kLockpicking);
        logger::info("Player Lockpicking Skill is {}", playerlockpickingskill); 
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

    Events::Register(); 

    return true;
}
