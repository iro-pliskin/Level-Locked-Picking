#include "settings.h"
#include "logging.h"

void Settings::LoadSettings() 
{
    if (RE::TESDataHandler::GetSingleton()->LookupLoadedModByName("HandtoHand.esp"sv)) {
        logger::info("Hand to Hand detected.");
        h2h_present = true;        
    }
}
