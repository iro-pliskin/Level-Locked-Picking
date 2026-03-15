#include "events.h"

#include "logging.h"
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
            logger::debug("Lockpicking Menu opened...");

            const auto player = RE::PlayerCharacter::GetSingleton();
            
            // If hand to hand is active, it uses the player's pickpocketing/security skill instead
            const float lockpicking_skill = player->AsActorValueOwner()->GetActorValue(
                Settings::h2h_present ? RE::ActorValue::kPickpocket : RE::ActorValue::kLockpicking
            );

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

            logger::debug("Lock Target: {} | Lock Level: {} | Player Skill: {}", lockedref->GetName(), static_cast<int>(lock_level), lockpicking_skill);
            
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
                // If for some reason the lock level is something other than 0-4, it defaults to letting the lockpicking menu appear.
                default:
                    canLockpick = true;
                    break;
            }

            logger::debug("Can Lockpick: {} | Lockpicking skill check {}", canLockpick, canLockpick ? "succeeded" : "failed");

            if (canLockpick) {
                return RE::BSEventNotifyControl::kContinue;
            } else {
                RE::PlaySound("UILockpickingCylinderStop");
                RE::DebugNotification("You lack the skill required to pick this lock.");
                RE::UIMessageQueue::GetSingleton()->AddMessage(RE::LockpickingMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
                return RE::BSEventNotifyControl::kStop;
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
