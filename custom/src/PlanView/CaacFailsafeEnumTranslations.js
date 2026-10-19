.pragma library

// Match metadata text rather than enum values, whose meanings vary by firmware and version.
function displayString(parameterName, originalText) {
    if (parameterName !== "FS_THR_ENABLE" && parameterName !== "FS_GCS_ENABLE"
            && parameterName !== "THR_FAILSAFE" && parameterName !== "FS_GCS_ENABL"
            && parameterName !== "NAV_RCL_ACT" && parameterName !== "NAV_DLL_ACT") {
        return originalText
    }

    switch (originalText) {
    case "Disabled/NoAction":
        return qsTranslate("CaacSafetyFailsafeEnums", "Disabled/NoAction")
    case "RTL":
        return qsTranslate("CaacSafetyFailsafeEnums", "RTL")
    case "RTL or Continue with Mission in Auto Mode (Deprecated in 4.0+-see FS_OPTIONS)":
        return qsTranslate("CaacSafetyFailsafeEnums",
                           "RTL or Continue with Mission in Auto Mode (Deprecated in 4.0+-see FS_OPTIONS)")
    case "SmartRTL or RTL":
        return qsTranslate("CaacSafetyFailsafeEnums", "SmartRTL or RTL")
    case "SmartRTL or Land":
        return qsTranslate("CaacSafetyFailsafeEnums", "SmartRTL or Land")
    case "Land (4.0+ Only)":
        return qsTranslate("CaacSafetyFailsafeEnums", "Land (4.0+ Only)")
    case "Disabled":
        return qsTranslate("CaacSafetyFailsafeEnums", "Disabled")
    case "Enabled always RTL":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled always RTL")
    case "Enabled Continue with Mission in Auto Mode (Deprecated in 4.0+)":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled Continue with Mission in Auto Mode (Deprecated in 4.0+)")
    case "Enabled always Land":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled always Land")
    case "Enabled always SmartRTL or RTL":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled always SmartRTL or RTL")
    case "Enabled always SmartRTL or Land":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled always SmartRTL or Land")
    case "RTL or Continue with Mission in Auto Mode (Removed in 4.0+-see FS_OPTIONS)":
        return qsTranslate("CaacSafetyFailsafeEnums",
                           "RTL or Continue with Mission in Auto Mode (Removed in 4.0+-see FS_OPTIONS)")
    case "Enabled Continue with Mission in Auto Mode (Removed in 4.0+)":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled Continue with Mission in Auto Mode (Removed in 4.0+)")
    case "Land":
        return qsTranslate("CaacSafetyFailsafeEnums", "Land")
    case "Auto DO_LAND_START or RTL":
        return qsTranslate("CaacSafetyFailsafeEnums", "Auto DO_LAND_START or RTL")
    case "Enabled Auto DO_LAND_START or RTL":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled Auto DO_LAND_START or RTL")
    case "Enabled always Brake or Land":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled always Brake or Land")
    case "Brake or Land":
        return qsTranslate("CaacSafetyFailsafeEnums", "Brake or Land")
    case "Auto DO_LAND_START/DO_RETURN_PATH_START or RTL":
        return qsTranslate("CaacSafetyFailsafeEnums", "Auto DO_LAND_START/DO_RETURN_PATH_START or RTL")
    case "Enabled Auto DO_LAND_START/DO_RETURN_PATH_START or RTL":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled Auto DO_LAND_START/DO_RETURN_PATH_START or RTL")
    case "Enabled":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled")
    case "EnabledNoFailsafe":
        return qsTranslate("CaacSafetyFailsafeEnums", "EnabledNoFailsafe")
    case "Heartbeat":
        return qsTranslate("CaacSafetyFailsafeEnums", "Heartbeat")
    case "HeartbeatAndREMRSSI":
        return qsTranslate("CaacSafetyFailsafeEnums", "HeartbeatAndREMRSSI")
    case "HeartbeatAndAUTO":
        return qsTranslate("CaacSafetyFailsafeEnums", "HeartbeatAndAUTO")
    case "Enabled Continue with Mission in Auto":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enabled Continue with Mission in Auto")
    case "Warn only":
        return qsTranslate("CaacSafetyFailsafeEnums", "Warn only")
    case "Disarm":
        return qsTranslate("CaacSafetyFailsafeEnums", "Disarm")
    case "Enter depth hold mode":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enter depth hold mode")
    case "Enter surface mode":
        return qsTranslate("CaacSafetyFailsafeEnums", "Enter surface mode")
    case "Force effective control inputs to trim positions and prevent arming":
        return qsTranslate("CaacSafetyFailsafeEnums",
                           "Force effective control inputs to trim positions and prevent arming")
    case "Surface and hold on surface on failsafe":
        return qsTranslate("CaacSafetyFailsafeEnums", "Surface and hold on surface on failsafe")
    case "Hold mode":
        return qsTranslate("CaacSafetyFailsafeEnums", "Hold mode")
    case "Return mode":
        return qsTranslate("CaacSafetyFailsafeEnums", "Return mode")
    case "Land mode":
        return qsTranslate("CaacSafetyFailsafeEnums", "Land mode")
    case "Terminate":
        return qsTranslate("CaacSafetyFailsafeEnums", "Terminate")
    default:
        return originalText
    }
}

function displayStrings(parameterName, originalStrings) {
    const result = []
    for (let index = 0; index < originalStrings.length; ++index) {
        result.push(displayString(parameterName, originalStrings[index]))
    }
    return result
}
