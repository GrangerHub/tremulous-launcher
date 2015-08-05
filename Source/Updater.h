/*
  ==============================================================================

    Updater.h
    Created: 10 Jul 2015 7:07:51pm
    Author:  jkent

  ==============================================================================
*/

#ifndef UPDATER_H_INCLUDED
#define UPDATER_H_INCLUDED

#include "Component.h"
#include "Semver.h"

enum UpdaterStatus {
    UPDATER_NOTSTARTED = 0,
    UPDATER_RUNNING,
    UPDATER_FINISHED,
    UPDATER_NEEDRESTART,
    UPDATER_FAILED,
};

class Updater : public Thread
{
public:
    static const char UPDATER_URL_PREFIX[];

    Updater();
    ~Updater();

    bool checkForUpdates();
    void run() override;

    // Status information
    enum UpdaterStatus getStatus();
    bool getStatusUpdate(double& overallPercent, double& taskPercent);
    String getStatusText();

    static Updater *getUpdater();
    static bool downloadFile(const URL& url, const File& file, int64 resume=0);

private:
    File basepath;
    Array<Launcher::Component> components;

    bool getLatestComponents();
    bool updateComponent(Launcher::Component& component);

    static Updater *updater;

    // Status information
    enum UpdaterStatus status;
    SpinLock statusLock;
    String statusString;
    bool statusStringUpdated;
    double taskPercent;
    double overallPercent;

    int totalTasks;
    int completedTasks;

    void setStatusText(const String& s);
    void startFirstTask();
    void startNextTask();
    void setTaskPercent(const double& percent);
};

#endif  // UPDATER_H_INCLUDED