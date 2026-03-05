#include "settings.h"
#include "logging.h"

void Settings::LoadSettings() 
{
    logger::info("Loading settings...");
    
    CSimpleIniA ini;
    ini.SetUnicode();

    constexpr auto iniPath = R"(.\Data\SKSE\Plugins\LevelLockedPicking.ini)";

    SI_Error rc = ini.LoadFile(iniPath);
    ini.LoadFile(iniPath);

    // If the ini file isn't found, it just uses the hardcoded values in settings.h
    if (rc < 0) {
       logger::warn("INI file not found in {}, using default values", iniPath);
    }
    
    debug_logging = ini.GetBoolValue("Logging", "bDebugLogging");

    novice_skill = ini.GetLongValue("SkillRequirements", "iNoviceLockRequirement", novice_skill);
    apprentice_skill = ini.GetLongValue("SkillRequirements", "iApprenticeLockRequirement",apprentice_skill);
    adept_skill = ini.GetLongValue("SkillRequirements", "iAdeptLockRequirement", adept_skill);
    expert_skill = ini.GetLongValue("SkillRequirements", "iExpertLockRequirement", expert_skill);
    master_skill = ini.GetLongValue("SkillRequirements", "iMasterLockRequirement", master_skill);


    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }
    
    if (RE::TESDataHandler::GetSingleton()->LookupLoadedModByName("HandtoHand.esp"sv)) {
        logger::info("Hand to Hand detected");
        h2h_present = true;
    }

    logger::info("Settings loaded");
}
