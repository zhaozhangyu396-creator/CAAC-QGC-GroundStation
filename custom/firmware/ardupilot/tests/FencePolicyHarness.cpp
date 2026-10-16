#include <algorithm>
#include <iostream>

#include "Copter.h"
#include "actual_accessor.h"
#include "actual_mode_lockout.h"
#include "fence.cpp"

Copter copter;

static void require(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

static void checkFailure(int8_t option, float distance, Mode::Number original)
{
    Copter vehicle;
    vehicle.fence._no_land.value = option;
    vehicle.fence.distance = distance;
    vehicle.mode.number = original;
    messages.clear();
    vehicle.fence_check();
    require(!vehicle.fence_breaches.have_updates, "event latch must be released");
    if (option == 1) {
        require(vehicle.mode.number == original, "Brake rejection must preserve current mode");
        require(vehicle.requests == std::vector{Mode::Number::BRAKE}, "no RTL or LAND request is allowed");
        require(
            messages == std::vector{std::pair{MAV_SEVERITY_CRITICAL, std::string("Fence brake failed: take control")}},
            "critical take-control warning required");
    } else {
        require(vehicle.mode.number == Mode::Number::LAND, "opt-out must preserve native LAND fallback");
    }
}

int main()
{
    size_t cases = 0;
    for (int8_t option : {int8_t(0), int8_t(1), int8_t(2)}) {
        Copter vehicle;
        vehicle.fence._no_land.value = option;
        require(vehicle.fence_mode_locked(false) == (option != 1),
                "opt-in policy must permit pilot mode changes despite fence lockout option");
        require(!vehicle.fence_mode_locked(true), "ignore-checks semantics must be preserved");
        cases++;
    }
    for (auto original : {Mode::Number::STABILIZE, Mode::Number::ALT_HOLD, Mode::Number::AUTO}) {
        for (float distance : {0.0f, 1.0f, 99.99f, 100.0f, 100.01f, 1000.0f}) {
            for (int8_t option : {int8_t(-1), int8_t(0), int8_t(1), int8_t(2)}) {
                checkFailure(option, distance, original);
                cases++;
            }
        }
    }
    for (int8_t option : {int8_t(0), int8_t(1), int8_t(2)}) {
        for (bool brake_available : {false, true}) {
            Copter vehicle;
            vehicle.fence._no_land.value = option;
            vehicle.ap.throttle_zero = true;
            vehicle.brake_available = brake_available;
            vehicle.fence_check();
            if (option == 1) {
                require(!vehicle.arming.disarmed, "airborne zero throttle must not disarm opt-in aircraft");
                require(vehicle.requests == std::vector{Mode::Number::BRAKE}, "zero throttle must attempt Brake");
                require(vehicle.mode.number == (brake_available ? Mode::Number::BRAKE : Mode::Number::STABILIZE),
                        "zero-throttle Brake rejection must preserve the original mode");
            } else {
                require(vehicle.arming.disarmed, "opt-out native zero-throttle disarm must remain unchanged");
            }
            cases++;
        }
    }
    for (float distance : {1.0f, 101.0f}) {
        Copter vehicle;
        vehicle.fence._no_land.value = 1;
        vehicle.fence.distance = distance;
        vehicle.brake_available = true;
        messages.clear();
        vehicle.fence_check();
        require(vehicle.mode.number == Mode::Number::BRAKE, "available Brake must be entered");
        require(vehicle.requests == std::vector{Mode::Number::BRAKE}, "severe breach must also try Brake");
        require(messages.empty(), "successful Brake should not report failure");
        cases++;
    }
    {
        Copter vehicle;
        vehicle.fence._no_land.value = 1;
        for (int breach = 0; breach < 500; breach++) {
            vehicle.fence.distance = float(breach * 20);
            vehicle.fence_breaches.have_updates = true;
            vehicle.fence_breaches.new_breaches = (breach % 2) ? 4 : 1;
            vehicle.fence_check();
        }
        require(vehicle.mode.number == Mode::Number::STABILIZE, "persistent failure must keep original mode");
        require(std::all_of(vehicle.requests.begin(), vehicle.requests.end(),
                            [](auto requested) { return requested == Mode::Number::BRAKE; }),
                "later or mixed fence breaches must never request LAND");
        cases++;
    }
    for (auto action : {AC_Fence::Action::REPORT_ONLY, AC_Fence::Action::ALWAYS_LAND, AC_Fence::Action::RTL_AND_LAND,
                        AC_Fence::Action::SMART_RTL, AC_Fence::Action::SMART_RTL_OR_LAND}) {
        Copter vehicle;
        vehicle.fence._no_land.value = 1;
        vehicle.fence.action = action;
        vehicle.fence_check();
        require(vehicle.mode.number ==
                    (action == AC_Fence::Action::REPORT_ONLY ? Mode::Number::STABILIZE : Mode::Number::LAND),
                "other actions must retain native semantics");
        cases++;
    }
    {
        Copter vehicle;
        vehicle.fence._no_land.value = 1;
        vehicle.ap.land_complete = true;
        vehicle.fence_check();
        require(vehicle.arming.disarmed, "ground disarm must remain native");
        require(vehicle.requests.empty(), "landed vehicle should not enter Brake");
        cases++;
    }
    {
        Copter vehicle;
        vehicle.motor_state.is_armed = false;
        vehicle.fence_check();
        require(vehicle.requests.empty(), "disarmed vehicle should not change mode");
        cases++;
    }
    {
        Copter vehicle;
        vehicle.fence_breaches.new_breaches = 0;
        vehicle.fence_check();
        require(vehicle.requests.empty(), "no new breach should not change mode");
        cases++;
    }
    std::cout << "PASS: " << cases << " actual patched fence_check policy scenarios\n";
}
