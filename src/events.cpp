#include "PCH.h"
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
                logger::info("Lockpicking Menu opened...");

                const auto player = RE::PlayerCharacter::GetSingleton();

                const float lockpicking = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kLockpicking);
                const float security = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kPickpocket);
                
                // If hand to hand is active, it uses the player's pickpocketing/security skill instead
                const float lockpickingskill = Settings::h2h_present ? security : lockpicking;

                // Skill requirements for each lock level
                const float novice_skill = 0.0f;
                const float apprentice_skill = 15.0f;
                const float adept_skill = 30.0f;
                const float expert_skill = 45.0f;
                const float master_skill = 65.0f;

                // Gets the current crosshair target so i can get the locks level.
                // It also checks if the target is null to prevent assertion errors from spam activating while looking away from a container
                // and hides the menu to prevent the player from bypassing requirements by quickly looking away and spamming a bunch.
                auto crosshairtarget = RE::CrosshairPickData::GetSingleton();
                if (!crosshairtarget || !crosshairtarget->target) {
                    logger::error("Crosshair target is Null, stopping handler to prevent crash. Stop spamming the damn thing!");
                    RE::UIMessageQueue::GetSingleton()->AddMessage(RE::LockpickingMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr); 
                    return RE::BSEventNotifyControl::kContinue;
                }

                const auto lockedref = crosshairtarget->target.get();
                if (!lockedref) return RE::BSEventNotifyControl::kContinue; // Second check just in case, first one should always work though

                const auto locklevel = static_cast<RE::LOCK_LEVEL>(lockedref->GetLockLevel());

                logger::debug("Player is lockpicking {}", lockedref->GetName());
                logger::debug("Lock Level is: {}", static_cast<int>(locklevel));
                logger::debug("Player Lockpicking Skill is: {}", lockpickingskill);
                
                bool canLockpick = false;

                switch (locklevel) {
                    case novice:
                        canLockpick = lockpickingskill >= novice_skill;
                        break;
                    case apprentice:
                        canLockpick = lockpickingskill >= apprentice_skill;
                        break;
                    case adept:
                        canLockpick = lockpickingskill >= adept_skill;
                        break;
                    case expert:
                        canLockpick = lockpickingskill >= expert_skill;
                        break;
                    case master:
                        canLockpick = lockpickingskill >= master_skill;
                        break;
                    default:
                        canLockpick = true; // If for some reason the lock level is something other than 0-4, it defaults to letting the lockpick menu appear.
                        break;              // I have no clue why it would, or if it could be something other than 0-4 but if so, it lets the player pick the lock.
                }
                
                logger::debug("Can Lockpick: {}", canLockpick);

                
                if (canLockpick) {
                    logger::info("Lockpicking allowed.");
                    return RE::BSEventNotifyControl::kContinue;
                } 
                else {
                    logger::info("Lockpicking blocked.");
                    RE::PlaySound("UILockpickingCylinderStop");
                    RE::DebugNotification("You cannot pick this lock.");
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
