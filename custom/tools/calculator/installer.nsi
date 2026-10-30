Unicode true
!include "MUI2.nsh"
!include "x64.nsh"

Name "飞虎角度计算器"
OutFile "${OUTPUT}"
InstallDir "$LOCALAPPDATA\Programs\FeihuCalculator"
InstallDirRegKey HKCU "Software\FeihuCalculator" "InstallDir"
RequestExecutionLevel user
SetCompressor /SOLID lzma
VIProductVersion "1.0.0.0"
VIAddVersionKey /LANG=2052 "ProductName" "飞虎角度计算器"
VIAddVersionKey /LANG=2052 "FileDescription" "飞虎角度计算器安装程序"
VIAddVersionKey /LANG=2052 "FileVersion" "1.0.0"
VIAddVersionKey /LANG=2052 "LegalCopyright" "CAAC GroundStation Contributors"

!define MUI_ICON "calculator.ico"
!define MUI_UNICON "calculator.ico"
!define MUI_ABORTWARNING
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\..\LICENSE-GPL"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "SimpChinese"

Function .onInit
    ${IfNot} ${RunningX64}
        MessageBox MB_OK|MB_ICONSTOP "本工具需要 64 位 Windows。"
        Abort
    ${EndIf}
FunctionEnd

Section "角度计算器"
    SetShellVarContext current
    SetOutPath "$INSTDIR"
    File /r "${PAYLOAD}\*.*"
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    CreateDirectory "$SMPROGRAMS\飞虎角度计算器"
    CreateShortcut "$SMPROGRAMS\飞虎角度计算器\飞虎角度计算器.lnk" "$INSTDIR\bin\CaacCalculator.exe"
    CreateShortcut "$DESKTOP\飞虎角度计算器.lnk" "$INSTDIR\bin\CaacCalculator.exe"
    CreateShortcut "$SMPROGRAMS\飞虎角度计算器\卸载.lnk" "$INSTDIR\Uninstall.exe"
    WriteRegStr HKCU "Software\FeihuCalculator" "InstallDir" "$INSTDIR"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\FeihuCalculator" "DisplayName" "飞虎角度计算器"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\FeihuCalculator" "DisplayVersion" "1.0.0"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\FeihuCalculator" "DisplayIcon" "$INSTDIR\bin\CaacCalculator.exe,0"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\FeihuCalculator" "UninstallString" '$\"$INSTDIR\Uninstall.exe$\"'
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\FeihuCalculator" "QuietUninstallString" '$\"$INSTDIR\Uninstall.exe$\" /S'
    WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\FeihuCalculator" "NoModify" 1
    WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\FeihuCalculator" "NoRepair" 1
SectionEnd

Section "Uninstall"
    SetShellVarContext current
    !include "${DELETE_MANIFEST}"
    Delete "$INSTDIR\Uninstall.exe"
    RMDir "$INSTDIR"
    Delete "$DESKTOP\飞虎角度计算器.lnk"
    Delete "$SMPROGRAMS\飞虎角度计算器\飞虎角度计算器.lnk"
    Delete "$SMPROGRAMS\飞虎角度计算器\卸载.lnk"
    RMDir "$SMPROGRAMS\飞虎角度计算器"
    DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\FeihuCalculator"
    DeleteRegKey HKCU "Software\FeihuCalculator"
SectionEnd
