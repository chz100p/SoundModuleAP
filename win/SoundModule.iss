;   Audacity: A Digital Audio Editor
;   Audacity(R) is copyright (c) 1999-2015 Audacity Team.
;   License: GPL v2.  See License.txt.
;
;   audacity.iss
;   Vaughan Johnson, Leland Lucius, Martyn Shaw, Richard Ash, & others
;

[UninstallRun]
; Uninstall prior installations.
Filename: "{app}\unins*.*"; 

[Setup]
; compiler-related directives
OutputBaseFilename=soundmodule-win-2.1.0

WizardImageFile=audacity_InnoWizardImage.bmp
WizardSmallImageFile=audacity_InnoWizardSmallImage.bmp

SolidCompression=yes

; installer-related directives
AppName=音源再生モジュール
AppVerName=SoundModule 2.1.0
; Specify AppVersion as well, so it appears in the Add/Remove Programs entry. 
AppVersion=2.1.0
AppPublisher=Bit Trade One.
AppPublisherURL=http://bit-trade-one.co.jp/
AppSupportURL=http://bit-trade-one.co.jp/
AppUpdatesURL=http://bit-trade-one.co.jp/
ChangesAssociations=yes

DefaultDirName={pf}\SoundModule

; Always warn if dir exists, because we'll overwrite previous Audacity.
DirExistsWarning=yes
DisableProgramGroupPage=yes
UninstallDisplayIcon="{app}\soundmodule.exe"

; No longer force them to accept the license, just display it.   LicenseFile=..\LICENSE.txt
InfoBeforeFile=audacity_InnoWizard_InfoBefore.rtf
InfoAfterFile=..\README.txt

; We no longer produce new ANSI builds. 
; As we use Inno Setup (u), the Unicode version, to build this script, 
; the MinVersion will automatically be set to what we need. 
; We no longer explicitly set it.
;   MinVersion=4.0,5.0

; cosmetic-related directives
SetupIconFile=audacity.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
; Name: "basque"; MessagesFile: "compiler:Languages\Basque.isl"
; Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
; Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
; Name: "Corsican"; MessagesFile: "compiler:Languages\Corsican.isl"
; Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
; Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
; Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
; Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
; Name: "french"; MessagesFile: "compiler:Languages\French.isl"
; Name: "german"; MessagesFile: "compiler:Languages\German.isl"
; Name: "Greek"; MessagesFile: "compiler:Languages\Greek.isl"
; Name: "hebrew"; MessagesFile: "compiler:Languages\Hebrew.isl"
; Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
; Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
; Name: "Nepali"; MessagesFile: "compiler:Languages\Nepali.islu"
; Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
; Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
; Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
; Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
; Name: "SerbianCyrillic"; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
; Name: "SerbianLatin"; MessagesFile: "compiler:Languages\SerbianLatin.isl"
; Name: "slovak"; MessagesFile: "compiler:Languages\Slovak.isl"
; Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
; Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
; Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
; Name: "ukrainian"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked
Name: resetPrefs; Description:  "Reset Preferences"; Flags: unchecked
; No longer allow user to choose whether to associate AUP file type with Audacity.
; Name: associate_aup; Description: "&Associate Audacity project files"; GroupDescription: "Other tasks:"; Flags: checkedonce

[Files]
; Don't display in separate window, rather as InfoAfterFile.   Source: "..\README.txt"; DestDir: "{app}"; Flags: ignoreversion isreadme
Source: "..\README.txt"; DestDir: "{app}"; Flags: ignoreversion

Source: "..\LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\win\release\soundmodule.exe"; DestDir: "{app}"; Flags: ignoreversion

; Manual, which should be got from the manual wiki using ..\scripts\mw2html_audacity\wiki2htm.bat
Source: "..\help\manual\*"; DestDir: "{app}\help\manual\"; Flags: ignoreversion recursesubdirs

Source: "..\presets\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

; wxWidgets DLLs. Be specific (not *.dll) so we don't accidentally distribute avformat.dll, for example.
; Don't use the WXWIN environment variable, because...
; 1) Can't get the documented {%WXWIN|default dir} parsing to work.
; 2) Need the DLL's in the release dir for testing, anyway.
Source: "..\win\release\wxbase28u_net_vc_custom.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\win\release\wxbase28u_vc_custom.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\win\release\wxmsw28u_adv_vc_custom.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\win\release\wxmsw28u_core_vc_custom.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\win\release\wxmsw28u_html_vc_custom.dll"; DestDir: "{app}"; Flags: ignoreversion

; MSVC runtime DLLs. Some users can't put these in the system dir, so just put them in the EXE dir.
; It's legal, per http://www.fsf.org/licensing/licenses/gpl-faq.html#WindowsRuntimeAndGPL .
; This is not an ideal solution, but should need the least tech support.
; We'll know we have the right version, don't step on anybody else's older version, and
; it's easy to make the zip (and they match better).
; Copy the two required DLL's from 
; "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\"
; or "C:\Program Files\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\"
; according to your system 
;Source: "..\win\release\msvcp120.dll"; DestDir: "{app}"; Flags: ignoreversion
;Source: "..\win\release\msvcr120.dll"; DestDir: "{app}"; Flags: ignoreversion

Source: "..\win\release\languages\*"; DestDir: "{app}\Languages\"; Flags: ignoreversion recursesubdirs
; We don't currently ship any modules, so the next line is commented out
; Source: "..\win\release\modules\*"; DestDir: "{app}\Modules\"; Flags: ignoreversion recursesubdirs skipifsourcedoesntexist
Source: "..\win\release\nyquist\*"; DestDir: "{app}\Nyquist\"; Flags: ignoreversion recursesubdirs
Source: "..\win\release\plug-ins\*"; DestDir: "{app}\Plug-Ins\"; Flags: ignoreversion

; File that acts as a markers to reset prefs.
; Needs the 'Permissions' so that Audacity can delete it
Source: "resetPrefs.txt"; DestDir: "{app}"; Permissions: users-modify; Tasks: resetPrefs  

[Icons]
Name: "{commonprograms}\SoundModule"; Filename: "{app}\soundmodule.exe"
Name: "{commondesktop}\SoundModule"; Filename: "{app}\soundmodule.exe"; Tasks: desktopicon

[InstallDelete]
; Get rid of previous 'reset prefs' file, in case somebody want to reinstall without the reset option after they installed with it
Type: files; Name: "{app}\resetPrefs.txt" 

; Get rid of Audacity 1.0.0 stuff that's no longer used.
Type: files; Name: "{app}\audacity-help.htb"
Type: files; Name: "{app}\audacity-1.2-help.htb"

; Get rid of previous versions of MSVC runtimes.
Type: files; Name: "{app}\Microsoft.VC80.CRT.manifest"
Type: files; Name: "{app}\Microsoft.VC90.CRT.manifest"
Type: files; Name: "{app}\msvcp80.dll"
Type: files; Name: "{app}\msvcr80.dll"
Type: files; Name: "{app}\msvcp90.dll"
Type: files; Name: "{app}\msvcr90.dll"

; Get rid of previous help folder.
Type: filesandordirs; Name: "{app}\help"

; Don't want to do this because user may have stored their own.
;   Type: filesandordirs; Name: "{app}\vst"

; We've switched from a folder in the start menu to just the Audacity.exe at the top level.
; Get rid of 1.0.0 folder and its icons.
Type: files; Name: "{commonprograms}\SoundModule\soundmodule.exe"
Type: files; Name: "{commonprograms}\SoundModule\unins000.exe"
Type: dirifempty; Name: "{commonprograms}\SoundModule"

;Get rid of previous uninstall item
Type: files; Name: "{app}\unins*.*"

; Get rid of no longer used test.lsp.
Type: files; Name: "{app}\Nyquist\test.lsp"

; Get rid of specific LADSPA plug-ins that we now ship with different names.
Type: files; Name: "{app}\Plug-Ins\GVerb.dll"
Type: files; Name: "{app}\Plug-Ins\Hard Limiter.dll"
Type: files; Name: "{app}\Plug-Ins\sc4.dll"

;Get rid of any modules that we have ever installed
Type: files; Name: "{app}\Modules\mod-script-pipe.dll"
Type: files; Name: "{app}\Modules\mod-script-pipe.exp"
Type: files; Name: "{app}\Modules\mod-script-pipe.lib"

;get rid of the Modules dir, if it is empty
Type: dirifempty; Name: "{app}\Modules"

; Get rid of gverb that we no longer ship
Type: files; Name: "{app}\Plug-Ins\gverb_1216.dll"

; Get rid of old crossfade* plugins that we no longer ship
Type: files; Name: "{app}\Plug-Ins\crossfadein.ny"
Type: files; Name: "{app}\Plug-Ins\crossfadeout.ny"
                                            
[Registry]
; No longer allow user to choose whether to associate AUP file type with Audacity.
; Leaving this one commented out example of the old way.
; Root: HKCR; Subkey: ".AUP"; ValueType: string; ValueData: "Audacity.Project"; Flags: createvalueifdoesntexist uninsdeletekey; Tasks: associate_aup
Root: HKCR; Subkey: ".ORP"; ValueType: string; ValueData: "SoundModule.Project"; Flags: createvalueifdoesntexist uninsdeletekey;
Root: HKCR; Subkey: "SoundModule.Project\OpenWithList\soundmodule.exe"; Flags: createvalueifdoesntexist uninsdeletekey;
Root: HKCR; Subkey: "SoundModule.Project"; ValueType: string; ValueData: "SoundModule Project File"; Flags: createvalueifdoesntexist uninsdeletekey;
Root: HKCR; Subkey: "SoundModule.Project\shell"; ValueType: string; ValueData: ""; Flags: createvalueifdoesntexist uninsdeletekey;
Root: HKCR; Subkey: "SoundModule.Project\shell\open"; Flags: createvalueifdoesntexist uninsdeletekey;
Root: HKCR; Subkey: "SoundModule.Project\shell\open\command"; ValueType: string; ValueData: """{app}\soundmodule.exe"" ""%1"""; Flags: createvalueifdoesntexist uninsdeletekey;

;The following would allow a following 'help' installer to know where to put the 'help' files.
;Root: HKCR; Subkey: "Audacity.Project\Path";  ValueType: string; ValueData: {app}; Flags: createvalueifdoesntexist uninsdeletekey;

[Run]
Filename: "{app}\soundmodule.exe"; Description: "Launch SoundModule"; Flags: nowait postinstall skipifsilent

;
;vc_redist 2022
#define VCmsg32 "Installing Microsoft Visual C++ Redistributable x86...."
#define VCmsg64 "Installing Microsoft Visual C++ Redistributable x64...."

[Files]
Source: C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\v143\vc_redist.x86.exe; DestDir: {tmp}; Flags: deleteafterinstall ignoreversion
;Source: C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\v143\vc_redist.x64.exe; DestDir: {tmp}; Flags: deleteafterinstall ignoreversion; Check: IsWin64

[Run]
Filename: {tmp}\vc_redist.x86.exe; Parameters: /quiet /norestart; StatusMsg: "{#VCmsg32}"; Check: not VCRuntime32Installed
;Filename: {tmp}\vc_redist.x64.exe; Parameters: /quiet /norestart; StatusMsg: "{#VCmsg64}"; Check: IsWin64 and not VCRuntime64Installed

[Code]
function VCRuntime32Installed: Boolean;
 var
  required_major: Cardinal;
  required_minor: Cardinal;
  required_bld: Cardinal;
  required_rbld: Cardinal;
  major: Cardinal;
  minor: Cardinal;
  bld: Cardinal;
  rbld: Cardinal;
  key: String;
 begin
  required_major := 14;
  required_minor := 34;
  required_bld := 31931;
  required_rbld := 0;
  Result := False;
  key := 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X86';
  if RegQueryDWordValue(HKLM32, key, 'Major', major) then begin
    if RegQueryDWordValue(HKLM32, key, 'Minor', minor) then begin
      if RegQueryDWordValue(HKLM32, key, 'Bld', bld) then begin
        if RegQueryDWordValue(HKLM32, key, 'Rbld', rbld) then begin
            Log('vcruntime (x86) version: ' + IntToStr(major) + '.' + IntToStr(minor) + '.' + IntToStr(bld) + '.' + IntToStr(rbld));
            Result := (major > required_major) or ((major = required_major) and ((minor > required_minor) or ((minor = required_minor) and ((bld > required_bld) or ((bld = required_bld) and (rbld >= required_rbld))))))
        end;
      end;
    end;
  end;
 end;
function VCRuntime64Installed: Boolean;
 var
  required_major: Cardinal;
  required_minor: Cardinal;
  required_bld: Cardinal;
  required_rbld: Cardinal;
  major: Cardinal;
  minor: Cardinal;
  bld: Cardinal;
  rbld: Cardinal;
  key: String;
 begin
  required_major := 14;
  required_minor := 34;
  required_bld := 31931;
  required_rbld := 0;
  Result := False;
  key := 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64';
  if RegQueryDWordValue(HKLM64, key, 'Major', major) then begin
    if RegQueryDWordValue(HKLM64, key, 'Minor', minor) then begin
      if RegQueryDWordValue(HKLM64, key, 'Bld', bld) then begin
        if RegQueryDWordValue(HKLM64, key, 'Rbld', rbld) then begin
            Log('vcruntime (x64) version: ' + IntToStr(major) + '.' + IntToStr(minor) + '.' + IntToStr(bld) + '.' + IntToStr(rbld));
            Result := (major > required_major) or ((major = required_major) and ((minor > required_minor) or ((minor = required_minor) and ((bld > required_bld) or ((bld = required_bld) and (rbld >= required_rbld))))))
        end;
      end;
    end;
  end;
 end;

