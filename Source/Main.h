#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SplashWindow.h"
#include "Updater.h"
#include "IniFile.h"

class launcherApplication  : public JUCEApplication
{
public:
    launcherApplication();

    const String getApplicationName() override;
    const String getApplicationVersion() override;
    bool moreThanOneInstanceAllowed() override;
    void initialise (const String& commandLine) override;
    void shutdown() override;
    void systemRequestedQuit() override;
    void anotherInstanceStarted (const String& commandLine) override;

    void initLauncher();
    void initApplication();

    static void runTremulous();
    static void runLauncher();
    static bool Elevate(const StringArray& commandLine=StringArray());
    static IniFile *getConfig();
    static bool isQuitting();

private:
    bool quitting;
    File settingsDirectory;
    ScopedPointer<FileLogger> logger;
    ScopedPointer<IniFile> config;
    ScopedPointer<SplashWindow> splashWindow;
};

#endif  // MAIN_H_INCLUDED
