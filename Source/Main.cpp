#include "../JuceLibraryCode/JuceHeader.h"
#include "Main.h"
#include "SplashWindow.h"
#include "Updater.h"
#include "IniFile.h"

#if defined(JUCE_LINUX) || defined(JUCE_BSD)
 #include <unistd.h>
 #include <sys/types.h>

#elif defined(JUCE_WINDOWS)
 #include <Windows.h>
 #include <tchar.h>

#elif defined(JUCE_MAC)
 #include <stdlib.h>
 #include <Security/Authorization.h>
 #include <Security/AuthorizationTags.h>
#endif



launcherApplication::launcherApplication()
    :quitting(false)
{}

const String launcherApplication::getApplicationName()
{
    return ProjectInfo::projectName;
}

const String launcherApplication::getApplicationVersion()
{
    return ProjectInfo::versionString;
}

bool launcherApplication::moreThanOneInstanceAllowed()
{
    return true;
}

void launcherApplication::initialise(const String& commandLine)
{
    File appDataDirectory(File::getSpecialLocation(File::userApplicationDataDirectory));

#if defined(JUCE_LINUX) || defined(JUCE_BSD)
    settingsDirectory = appDataDirectory.getChildFile(".tremulous");
#elif defined(JUCE_WINDOWS)
    settingsDirectory = appDataDirectory.getChildFile("Tremulous");
#elif defined(JUCE_MAC)
    // FIXME/Verify
    settingsDirectory = appDataDirectory.getChildFile("Application Support").getChildFile("Tremulous");
#endif

    File logFile(settingsDirectory.getChildFile("launcher.log"));
    logger = new FileLogger(logFile, ProjectInfo::projectName, 0);
    Logger::setCurrentLogger(logger);

    // TODO: Parse arguments
    StringArray commandLineArray(getCommandLineParameterArray());
    for (int i = 0; i < commandLineArray.size(); i++) {
      Logger::writeToLog(String("arg") + String(i) + String(": ") + commandLineArray[i]);
    }

    config = new IniFile(settingsDirectory.getChildFile("launcher.ini"));

    File defaultBasepath;
    File defaultHomepath(settingsDirectory);

#if defined(JUCE_LINUX) || defined(JUCE_BSD)
    defaultBasepath = "/usr/local/games/tremulous";
#elif defined(JUCE_WINDOWS)
    defaultBasepath = File::getSpecialLocation(File::globalApplicationsDirectory).getChildFile("Tremulous");
#elif defined(JUCE_MAC)
    //  FIXME/Verify
    defaultBasepath = File::getSpecialLocation(File::globalApplicationsDirectory).getChildFile("Tremulous");
#endif

    config->setDefault("general", "overpath", defaultBasepath.getFullPathName());
    config->setDefault("general", "basepath", defaultBasepath.getFullPathName());
    config->setDefault("general", "homepath", defaultHomepath.getFullPathName());

    config->setDefault("updater", "downloads", defaultBasepath.getChildFile("updater").getChildFile("downloads").getFullPathName());

#if defined(JUCE_LINUX)
 #if __x86_64__
    String defaultPlatform("linux64");
 #else
    String defaultPlatform("linux32");
 #endif
#elif defined(JUCE_BSD) && defined(__FreeBSD__)
 #if __x86_64__
    String defaultPlatform("freebsd64");
 #else
    String defaultPlatform("freebsd32");
 #endif
#elif defined(JUCE_WINDOWS)
 #if _WIN64 || __x86_64__
    String defaultPlatform("win64");
 #else
    String defaultPlatform("win32");
 #endif
#elif defined(JUCE_MAC)
    String defaultPlatform("mac");
#endif

    config->setDefault("updater", "platform", defaultPlatform);
    config->setDefault("updater", "channel", "release");
    config->setDefault("updater", "checkForUpdates", 1);
    config->setDefault("updater", "verifySignature", 1);

    LookAndFeel *laf = &LookAndFeel::getDefaultLookAndFeel();
    laf->setColour(AlertWindow::backgroundColourId, Colour(0xFF111111));
    laf->setColour(AlertWindow::textColourId, Colour(0xFFFFFFFF));
    laf->setColour(AlertWindow::outlineColourId, Colour(0xFF333333));

    laf->setColour(TextButton::buttonColourId, Colour(0xFF333333));
    laf->setColour(TextButton::textColourOffId, Colour(0xFFFFFFFF));

    // Delete old launcher if it exists
    File launcherPath(File::getSpecialLocation(
            File::SpecialLocationType::currentApplicationFile));

    File oldLauncherPath(launcherPath.getFullPathName() + ".old");
    oldLauncherPath.deleteFile();

    initLauncher();
}

void launcherApplication::initLauncher()
{
    splashWindow = new SplashWindow(getApplicationName());
}

void launcherApplication::initApplication()
{
    //launcherWindow = new LauncherWindow(getApplicationName());
}

void launcherApplication::shutdown()
{
    // Add your application's shutdown code here..
    Logger::setCurrentLogger(nullptr);
    logger = nullptr;
    config = nullptr;
    splashWindow = nullptr; // (deletes our window)
}

void launcherApplication::systemRequestedQuit()
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.

    quitting = true;

    if (!splashWindow || !splashWindow->updaterRunning())
    {
        quit();
    }
}

void launcherApplication::anotherInstanceStarted(const String& commandLine)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}

bool launcherApplication::isQuitting()
{
    launcherApplication *app = (launcherApplication *)JUCEApplication::getInstance();
    return app->quitting;
}

bool launcherApplication::Elevate(const StringArray& commandLineArray)
{
#if defined(JUCE_LINUX) || defined(JUCE_BSD)
    if (geteuid() == 0) {
        return true;
    }

    String parameters("--user root /usr/bin/env ");

    const char *var = getenv("DISPLAY");
    if (var) {
        String s("DISPLAY=" + String(var).quoted());
        parameters += s + " ";
    }

    var = getenv("XAUTHORITY");
    if (var) {
        String s("XAUTHORITY=" + String(var).quoted());
        parameters += s + " ";
    }

    String launcher(File::getSpecialLocation(
                File::currentExecutableFile).getFullPathName());
    if (launcher.contains(" ")) {
        launcher = launcher.quoted();
    }

    parameters += String(" ") + launcher;
    for (int i = 0; i < commandLineArray.size(); i++)
    {
        parameters += " ";
        if (commandLineArray[i].contains(" "))
            parameters += commandLineArray[i].quoted();
        else
            parameters += commandLineArray[i];
    }

    File pkexec("/usr/bin/pkexec");
    if (pkexec.exists() && pkexec.startAsProcess(parameters)) {
        quit();
    }

    pkexec = "/usr/local/bin/pkexec";
    if (pkexec.exists() && pkexec.startAsProcess(parameters)) {
        quit();
    }

#elif defined(JUCE_WINDOWS)
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdministratorsGroup)) {
        dwError = GetLastError();
        goto cleanup;
    }

    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin)) {
        dwError = GetLastError();
        goto cleanup;
    }

cleanup:
    if (pAdministratorsGroup) {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    if (dwError != ERROR_SUCCESS) {
        throw dwError;
    }

    if (fIsRunAsAdmin) {
        return true;
    }

    TCHAR szPath[MAX_PATH];
    if (!GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath))) {
        dwError = GetLastError();
        throw dwError;
    }

    String commandLine;
    for (int i = 0; i < commandLineArray.size(); i++) {
        if (commandLineArray[i].contains(" ")) {
            commandLine += String("\"") + commandLineArray[i] + String("\"");
        } else {
            commandLine += commandLineArray[i];
        }
        if (i + 1 < commandLineArray.size()) {
            commandLine += " ";
        }
    }

    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.lpVerb = _T("runas");
    sei.lpFile = szPath;
    sei.lpParameters = commandLine.toUTF8();
    sei.nShow = SW_NORMAL;
    if (ShellExecuteEx(&sei)) {
        _exit(1);
    }

#elif defined(JUCE_MAC)
    if (geteuid() == 0) {
        return true;
    }

    String launcher(File::getSpecialLocation(
                File::currentExecutableFile).getFullPathName());

    const char * execpath = launcher.toRawUTF8();
    char * args[] = { NULL };

    OSStatus           err;
    AuthorizationRef   ref;
    AuthorizationFlags flags;

    flags = kAuthorizationFlagDefaults;
    err   = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, flags, &ref);
    if ( err != errAuthorizationSuccess ) {
        quit();
    }

    AuthorizationItem    _temp = { kAuthorizationRightExecute, 0, NULL, 0 };
    AuthorizationRights rights = { 1, &_temp };

    flags = kAuthorizationFlagDefaults
          | kAuthorizationFlagInteractionAllowed
          | kAuthorizationFlagPreAuthorize
          | kAuthorizationFlagExtendRights;

    err  = AuthorizationCopyRights(ref, &rights, NULL, flags, NULL);
    if ( err != errAuthorizationSuccess ) {
        AuthorizationFree(ref, kAuthorizationFlagDefaults);
        quit();
    }

    flags = kAuthorizationFlagDefaults;
    err  = AuthorizationExecuteWithPrivileges(ref, execpath, flags, args, NULL);
    AuthorizationFree(ref, kAuthorizationFlagDefaults);

    // Probably overkill.
    if ( err != errAuthorizationSuccess ) {
        quit();
    }
#endif // JUCE_MAC

    return false;
}

IniFile *launcherApplication::getConfig()
{
    launcherApplication *app = (launcherApplication *)JUCEApplication::getInstance();
    return app->config;
}

void launcherApplication::runTremulous()
{
    Logger::writeToLog("Running Tremulous...");

    return;

    launcherApplication *app = (launcherApplication *)JUCEApplication::getInstance();

    StringArray commandLineArray;
    commandLineArray.add("+set");
    commandLineArray.add("fs_overpath");
    commandLineArray.add(app->config->getString("general", "overpath"));

    commandLineArray.add("+set");
    commandLineArray.add("fs_basepath");
    commandLineArray.add(app->config->getString("general", "basepath"));

    commandLineArray.add("+set");
    commandLineArray.add("fs_homepath");
    commandLineArray.add(app->config->getString("general", "homepath"));

    File tremulousDir(app->config->getString("general", "overpath"));
#if defined(JUCE_LINUX) || defined(JUCE_BSD)
    File tremulousApp(tremulousDir.getChildFile("tremulous"));
#elif defined(JUCE_WINDOWS)
    File tremulousApp(tremulousDir.getChildFile("tremulous.exe"));
#elif defined(JUCE_MAC)
    // FIXME
    File tremulousApp(tremulousDir.getChildFile("Tremulous.app"));
#endif

#if defined(JUCE_WINDOWS)
    String commandLine;

    for (int i = 0; i < commandLineArray.size(); i++) {
        commandLine += commandLineArray[i].quoted();
        if (i + 1 < commandLineArray.size())
            commandLine += " ";
    }

    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.lpVerb = _T("runas");
    sei.lpFile = tremulousApp.getFullPathName().toUTF8();
    sei.lpParameters = commandLine.toUTF8();
    sei.nShow = SW_NORMAL;
    if (ShellExecuteEx(&sei))
        _exit(1);
#endif
}

void launcherApplication::runLauncher()
{
    Logger::writeToLog("Running launcher...");

    File launcherDir(File::getSpecialLocation(File::currentApplicationFile).getParentDirectory());
#if defined(JUCE_LINUX) || defined(JUCE_BSD)
    File launcherApp(launcherDir.getChildFile("tremlauncher"));
#elif defined(JUCE_WINDOWS)
    File launcherApp(launcherDir.getChildFile("Tremulous Launcher.exe"));
#elif defined(JUCE_MAC)
    File launcherApp(launcherDir.getChildFile("Tremulous Launcher.app"));
#endif

#if defined(JUCE_WINDOWS)
    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.lpFile = launcherApp.getFullPathName().toUTF8();
    sei.lpParameters = GetCommandLine();
    sei.nShow = SW_NORMAL;
    if (ShellExecuteEx(&sei))
        _exit(1);
#endif
    return;
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (launcherApplication)
