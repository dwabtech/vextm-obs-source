;--------------------------------
;Include to detect if running on a 64-bit machine
  !include "x64.nsh"

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

  !addplugindir "Installer"

;--------------------------------
;General

  ;Name and file
  Name "VEX Tournament Manager OBS Plugin"
  BrandingText "VEX Tournament Manager OBS Plugin"

  ; Set requested execution level as Administrator.  This is needed on Vista
  ; to allow the installer to write to Program Files.
  RequestExecutionLevel admin

  ;Installer Icon
  ;Icon "wslogo16.ico"

;--------------------------------
;Installation Types

  InstType "Full"

;--------------------------------
;Don't show installation details
  ShowInstDetails nevershow

;--------------------------------
;Variables

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING
  !define MUI_COMPONENTSPAGE_SMALLDESC

;--------------------------------
;Theme Settings
  
  !define MUI_ICON   "Installer\installer-nopng.ico"
  !define MUI_UNICON "Installer\uninstaller-nopng.ico"
 
  ; MUI Settings / Header
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_RIGHT
  !define MUI_HEADERIMAGE_BITMAP   "Installer\header-r.bmp"
  !define MUI_HEADERIMAGE_UNBITMAP "Installer\header-r-un.bmp"
 
  ; MUI Settings / Wizard     
  !define MUI_WELCOMEFINISHPAGE_BITMAP   "Installer\wizard.bmp"
  !define MUI_UNWELCOMEFINISHPAGE_BITMAP "Installer\wizard-un.bmp" 

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
;--------------------------------
;Languages
  !insertmacro MUI_LANGUAGE "English"

!define PLUGIN_DIR "$APPDATA\obs-studio\plugins\vextm-source"

Function .onInit
    ; Check for a standard OBS Studio installation
    IfFileExists "$PROGRAMFILES64\obs-studio\bin\64bit\obs64.exe" continueInstall 0
        MessageBox MB_ICONSTOP|MB_OK "You must install OBS Studio before installing this plugin." /SD IDOK
        Abort

    continueInstall:
FunctionEnd

;--------------------------------
;Installer Sections

Section "-Base"
    SetShellVarContext all

    SetDetailsPrint textonly
    DetailPrint "Installing VEX Tournament Manager OBS Plugin..."

    ; Remove the previous legacy installation
    Delete "$PROGRAMFILES64\obs-studio\obs-plugins\64bit\vextm-source.dll"
    RMDir /r "$PROGRAMFILES64\obs-studio\data\obs-plugins\vextm-source"

    ; Install the DLL using the OBS 33 plugin layout
    SetOutPath "${PLUGIN_DIR}"
    File "vextm-source.dll"

    ; Install plugin data files
    SetOutPath "${PLUGIN_DIR}\data"
    File /r "data\*.*"

SectionEnd

