#pragma once
#include "logging.h"

class Settings
{ 
public:
    static void LoadSettings();

    inline static bool debug_logging{};
    
    inline static int novice_skill = 0;
    inline static int apprentice_skill = 25;
    inline static int adept_skill = 50;
    inline static int expert_skill = 75;
    inline static int master_skill = 100;

    inline static bool h2h_present{};

};

