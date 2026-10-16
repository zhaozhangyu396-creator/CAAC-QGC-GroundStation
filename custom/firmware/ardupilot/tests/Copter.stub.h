#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define AP_FENCE_ENABLED 1
#define AC_FENCE_TYPE_ALT_MAX 1
#define AC_FENCE_GIVE_UP_DISTANCE 100.0f
#define MAV_SEVERITY_NOTICE 5
#define MAV_SEVERITY_CRITICAL 2
#define LOGGER_WRITE_ERROR(...) ((void) 0)
#define GCS_SEND_TEXT(severity, text) messages.emplace_back(severity, text)

inline std::vector<std::pair<int, std::string>> messages;

namespace AP_HAL {
inline uint32_t millis()
{
    return 1000;
}

inline bool timeout_expired(uint32_t previous, uint32_t now, uint32_t interval)
{
    return now - previous >= interval;
}
}  // namespace AP_HAL

class AC_Fence
{
public:
    enum class Action
    {
        REPORT_ONLY,
        RTL_AND_LAND,
        ALWAYS_LAND,
        SMART_RTL,
        BRAKE,
        SMART_RTL_OR_LAND
    };

    struct Int8
    {
        int8_t value = 0;

        int8_t get() const { return value; }
    } _no_land;
    enum class OPTIONS
    {
        DISABLE_MODE_CHANGE
    };
    bool mode_lockout = true;

    bool enabled() const { return true; }

    bool option_enabled(OPTIONS) const { return mode_lockout; }

    Action action = Action::BRAKE;
    uint8_t breaches = 4;
    float distance = 1.0f;

    Action get_action() const { return action; }

    uint8_t get_breaches() const { return breaches; }

    uint8_t check(bool) const { return breaches; }

    float get_breach_distance(uint8_t) const { return distance; }

    void print_fence_message(const char*, uint8_t) const {}

    bool brake_no_land() const;
};

class Mode
{
public:
    enum class Number
    {
        STABILIZE,
        ALT_HOLD,
        AUTO,
        LAND,
        RTL,
        SMART_RTL,
        BRAKE
    };
    Number number = Number::STABILIZE;
    bool manual_throttle = true;

    bool is_landing() const { return number == Number::LAND; }

    bool has_manual_throttle() const { return manual_throttle; }
};

enum class ModeReason
{
    FENCE_BREACHED
};

namespace AP_Arming {
enum class Method
{
    FENCEBREACH
};
}  // namespace AP_Arming

class Copter
{
public:
    struct Motors
    {
        bool is_armed = true;

        bool armed() const { return is_armed; }
    } motor_state;

    Motors* motors = &motor_state;

    struct Arming
    {
        bool disarmed = false;

        void disarm(AP_Arming::Method) { disarmed = true; }
    } arming;

    struct
    {
        bool land_complete = false;
        bool throttle_zero = false;
    } ap;

    struct
    {
        bool radio = false;
    } failsafe;

    struct
    {
        uint32_t last_check_ms = 0;
        uint8_t new_breaches = 4;
        bool have_updates = true;
    } fence_breaches;

    AC_Fence fence;
    Mode mode;
    Mode* flightmode = &mode;
    bool brake_available = false;
    bool rtl_available = false;
    bool smart_rtl_available = false;
    std::vector<Mode::Number> requests;

    ModeReason get_control_mode_reason() const { return ModeReason::FENCE_BREACHED; }

    bool fence_mode_locked(bool ignore_checks) const;

    void fence_checks_async();
    void fence_check();

    // Mode entry rejection preserves flightmode, matching the pinned set_mode implementation.
    bool set_mode(Mode::Number requested, ModeReason)
    {
        requests.push_back(requested);
        if ((requested == Mode::Number::BRAKE && !brake_available) ||
            (requested == Mode::Number::RTL && !rtl_available) ||
            (requested == Mode::Number::SMART_RTL && !smart_rtl_available)) {
            return false;
        }
        mode.number = requested;
        return true;
    }
};

extern Copter copter;
