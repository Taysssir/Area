#define MyAppName "Sentinel-Admin"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "eos-innovation"
#define MyAppURL "http://www.eos-innovation.eu/"
#define MyAppExeName "sentinel-admin.exe"
#define MyAppBuildDir "..\build-sentinel-admin-Desktop_Qt_5_5_1_MinGW_32bit3-Release\release\"
#define MyAppSetupBuildDir "..\build-sentinel-admin-Desktop_Qt_5_5_1_MinGW_32bit3-Release\"

[Setup]
AppId={{FDCECAB6-215E-4912-A144-ECC764193B3A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DisableDirPage=yes
DefaultGroupName={#MyAppName}
OutputDir={#MyAppSetupBuildDir}
OutputBaseFilename=Setup
Compression=lzma
SolidCompression=yes
AllowCancelDuringInstall=False
Uninstallable=yes
AppCopyright=eos-innovation
UninstallDisplayName=Sentinel-Admin
UninstallDisplayIcon=sentinel.ico
VersionInfoCompany=eos-innovation
VersionInfoCopyright=© eos-innovation. Tous droits réservés.
VersionInfoProductName=Sentinel-Admin
AlwaysRestart=False
AppComments=Sentinel-Admin
MinVersion=0,6.1
SetupIconFile=sentinel.ico
ShowLanguageDialog=no
UninstallRestartComputer=no
PrivilegesRequired=admin

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: french; MessagesFile: compiler:Languages\French.isl

[Files]
Source: {#MyAppBuildDir}config.ini; DestDir: {app}
Source: {#MyAppBuildDir}libgcc_s_dw2-1.dll; DestDir: {app}
Source: {#MyAppBuildDir}libstdc++-6.dll; DestDir: {app}
Source: {#MyAppBuildDir}libwinpthread-1.dll; DestDir: {app}
Source: {#MyAppBuildDir}libzip-2.dll; DestDir: {app}
Source: {#MyAppBuildDir}Qt5Core.dll; DestDir: {app}
Source: {#MyAppBuildDir}Qt5Gui.dll; DestDir: {app}
Source: {#MyAppBuildDir}Qt5Svg.dll; DestDir: {app}
Source: {#MyAppBuildDir}Qt5Widgets.dll; DestDir: {app}
Source: {#MyAppBuildDir}sentinel-admin.exe; DestDir: {app}
Source: {#MyAppBuildDir}zlib1.dll; DestDir: {app}
Source: {#MyAppBuildDir}iconengines\qsvgicon.dll; DestDir: {app}\iconengines
Source: {#MyAppBuildDir}imageformats\qjpeg.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qmng.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qsvg.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qtga.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qtiff.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qwbmp.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qwebp.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qdds.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qgif.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qicns.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qico.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}imageformats\qjp2.dll; DestDir: {app}\imageformats
Source: {#MyAppBuildDir}platforms\qwindows.dll; DestDir: {app}\platforms
Source: {#MyAppBuildDir}translations\qt_fi.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_fr.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_he.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_hu.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_it.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_ja.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_ko.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_lv.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_ru.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_sk.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_uk.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_ca.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_cs.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_de.qm; DestDir: {app}\translations
Source: {#MyAppBuildDir}translations\qt_en.qm; DestDir: {app}\translations

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}

[UninstallRun]
Filename: {sys}\taskkill.exe; Parameters: "/f /im ""sentinel-admin.exe"""; Flags: waituntilterminated runhidden

[UninstallDelete]
Type: filesandordirs; Name: {app}\*.*
Type: filesandordirs; Name: {app}\iconengines\*.*
Type: filesandordirs; Name: {app}\imageformats\*.*
Type: filesandordirs; Name: {app}\platforms\*.*
Type: filesandordirs; Name: {app}\translations\*.*
