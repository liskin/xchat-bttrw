[Setup]
AppName=xchat-bttrw
AppVerName=xchat-bttrw r. 423
OutputBaseFilename=xchat-bttrw-423
AppPublisher=NOMI team
AppPublisherURL=http://www.nomi.cz/
AppSupportURL=http://nomi.cz/projects.shtml?id=xchat-bttrw
AppUpdatesURL=http://nomi.cz/projects.shtml?id=xchat-bttrw
DefaultDirName={pf}\xchat-bttrw
DefaultGroupName=xchat-bttrw
AllowNoIcons=yes
LicenseFile=data\COPYING.txt
OutputDir=output
Compression=lzma
SolidCompression=yes
MinVersion=4.1,4.0
PrivilegesRequired=admin
WizardImageFile=WizImage.bmp
WizardSmallImageFile=WizImageSmall.bmp

[Languages]
Name: "eng"; MessagesFile: "compiler:Default.isl"
Name: "cze"; MessagesFile: "compiler:Languages\Czech-5-5.1.0.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: ;
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "data\gate-win98.exe"; DestName: "gate.exe"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 4.1,4.0; OnlyBelowVersion: 0,5.0
Source: "data\gate-win2k.exe"; DestName: "gate.exe"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0,5.0; OnlyBelowVersion: 0,5.01
Source: "data\gate.exe"; DestName: "gate.exe"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0,5.01
Source: "data\COPYING.txt";    DestDir: "{app}"; Flags: ignoreversion
Source: "data\COPYRIGHT";  DestDir: "{app}"; Flags: ignoreversion
Source: "data\config.exe";  DestDir: "{app}"; Flags: ignoreversion
Source: "data\run.bat";  DestDir: "{app}"; Flags: ignoreversion
Source: "data\xchat\*";  DestDir: "{app}\xchat"; Flags: ignoreversion recursesubdirs
Source: "data\ico\all.ico";  DestName: "gate.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "data\xchat-appdata\*";  DestDir: "{userappdata}\X-Chat 2"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "data\libiconv-2.dll"; DestDir: "{sys}"

;Knihovna Visual C++
Source: "data\msvcr71.dll"; DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile

[INI]
Filename: "{app}\gate.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://www.nomi.cz/projects.shtml?id=xchat-bttrw"
Filename: "{app}\gate-help.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://wiki.nomi.cz/xchat-bttrw:start"
Filename: "{app}\xchat.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://www.xchat.cz/"

[Icons]
Name: "{group}\xchat-bttrw"; Filename: "{app}\run.bat"; WorkingDir: "{app}"; IconFilename: "{app}\gate.ico"
Name: "{group}\Officiální web xchat-bttrw"; Filename: "{app}\gate.url"
Name: "{group}\Nápovìda xchat-bttrw"; Filename: "{app}\gate-help.url"
Name: "{group}\Web www.xchat.cz"; Filename: "{app}\xchat.url"
Name: "{group}\{cm:UninstallProgram,xchat-bttrw}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\xchat-bttrw"; Filename: "{app}\run.bat"; Tasks: desktopicon; WorkingDir: "{app}"; IconFilename: "{app}\gate.ico"
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\xchat-bttrw"; Filename: "{app}\run.bat"; Tasks: quicklaunchicon;  WorkingDir: "{app}"; IconFilename: "{app}\gate.ico"

[Run]
Filename: "{app}\config.exe"; WorkingDir: "{userappdata}\X-Chat 2"; StatusMsg: "Nastavuji ..."; Flags: waituntilterminated
Filename: "{app}\run.bat"; Description: "{cm:LaunchProgram,xchat-bttrw}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\gate.url"
Type: files; Name: "{app}\gate-help.url"
Type: files; Name: "{app}\xchat.url"

