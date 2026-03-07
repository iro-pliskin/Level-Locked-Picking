#include "logging.h"
#include "events.h"
#include "settings.h"

constexpr auto novice {RE::LOCK_LEVEL::kVeryEasy};
constexpr auto apprentice {RE::LOCK_LEVEL::kEasy};
constexpr auto adept {RE::LOCK_LEVEL::kAverage};
constexpr auto expert {RE::LOCK_LEVEL::kHard};
constexpr auto master {RE::LOCK_LEVEL::kVeryHard};

namespace Events 
{  
    RE::BSEventNotifyControl LockpickingMenuHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) 
    {
        if (a_event && a_event->menuName == RE::LockpickingMenu::MENU_NAME && a_event->opening) 
        {
            {
                logger::debug("Lockpicking Menu opened...");

                const auto player = RE::PlayerCharacter::GetSingleton();

                const float lockpicking = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kLockpicking);
                const float security = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kPickpocket);
                
                // If hand to hand is active, it uses the player's pickpocketing/security skill instead
                const float lockpicking_skill = Settings::h2h_present ? security : lockpicking;

                // If the crosshair moves away from an object just as code fires, theres a chance that the crosshair target can be null, causing a crash.
                // This checks if the target is null before getting the lock level, and exits the menu early to avoid a race condition.
                auto crosshairTarget = RE::CrosshairPickData::GetSingleton();
                if (!crosshairTarget || !crosshairTarget->target) {
                    logger::error("Crosshair target is Null, stopping handler to prevent crash. Stop spamming the damn thing!");
                    RE::UIMessageQueue::GetSingleton()->AddMessage(RE::LockpickingMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr); 
                    return RE::BSEventNotifyControl::kStop;
                }

                const auto lockedref = crosshairTarget->target.get();
                if (!lockedref) return RE::BSEventNotifyControl::kStop; // Second check just in case the first null check fails

                const auto lock_level = static_cast<RE::LOCK_LEVEL>(lockedref->GetLockLevel());

                logger::debug("Player is lockpicking {}", lockedref->GetName());
                logger::debug("Lock Level is: {}", static_cast<int>(lock_level));
                logger::debug("Player Lockpicking Skill is: {}", lockpicking_skill);
                
                bool canLockpick = false;

                switch (lock_level) {
                    case novice:
                        canLockpick = lockpicking_skill >= Settings::novice_skill;
                        break;
                    case apprentice:
                        canLockpick = lockpicking_skill >= Settings::apprentice_skill;
                        break;
                    case adept:
                        canLockpick = lockpicking_skill >= Settings::adept_skill;
                        break;
                    case expert:
                        canLockpick = lockpicking_skill >= Settings::expert_skill;
                        break;
                    case master:
                        canLockpick = lockpicking_skill >= Settings::master_skill;
                        break;
                    default:
                        canLockpick = true; // If for some reason the lock level is something other than 0-4, it defaults to letting the lockpick menu appear.
                        break;              // I have no clue why it would, or if it could be something other than 0-4 but if so, it lets the player pick the lock.
                }

                logger::debug("Can Lockpick: {}", canLockpick);
                
                if (canLockpick) {
                    logger::debug("Lockpicking skill check succeeded");
                    return RE::BSEventNotifyControl::kContinue;
                } else {
                    logger::debug("Lockpicking skill check failed");
                    RE::PlaySound("UILockpickingCylinderStop");
                    RE::DebugNotification("You lack the skill requirement needed to pick this lock.");
                    RE::UIMessageQueue::GetSingleton()->AddMessage(RE::LockpickingMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
                    return RE::BSEventNotifyControl::kStop;
                }
            }
        }
    return RE::BSEventNotifyControl::kContinue;
}
    
    void Register() {
        auto ui = RE::UI::GetSingleton();
            if (ui) {
            static LockpickingMenuHandler handler;
            ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(&handler);
        logger::info("Lockpick Menu Handler registered successfully");
        }
    }
}


