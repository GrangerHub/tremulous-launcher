/*
  ==============================================================================

    Updater.cpp
    Created: 10 Jul 2015 7:07:51pm
    Author:  jkent

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Updater.h"
#include "Crypto.h"
#include "Main.h"
#include "IniFile.h"

#define DOWNLOAD_UPDATE_INTERVAL 100
#define DOWNLOAD_AVERAGE_TIME 10000
#define DOWNLOAD_SAMPLES (DOWNLOAD_AVERAGE_TIME / DOWNLOAD_UPDATE_INTERVAL)

const char Updater::UPDATER_URL_PREFIX[] = "http://trem.jkent.net/updater/";

Updater *Updater::updater;

Updater::Updater()
    :Thread("Updater Thread"), status(UPDATER_NOTSTARTED), taskPercent(0), overallPercent(0)
{
    basepath = launcherApplication::getConfig()->getString("general", "basepath");

    if (launcherApplication::getConfig()->getInt("updater", "verifySignature")) {
        Crypto::init();
    }

    if (updater == nullptr) {
        updater = this;
    }
}

Updater::~Updater()
{
    updater = nullptr;
}

bool Updater::checkForUpdates()
{
    if (!getLatestComponents()) {
        Logger::writeToLog("Unable to fetch latest version information");
        return false;
    }

    for (int i = 0; i < components.size(); i++) {
        if (components[i].canUpdate()) {
            return true;
        }
    }

    return false;
}

void Updater::run()
{
    status = UPDATER_RUNNING;

    totalTasks = 0;
    for (int i = 0; i < components.size(); i++) {
        Launcher::Component *component = &components.getReference(i);
        if (component->canUpdate()) {
            totalTasks += 5;
            if (component->getName() == "launcher") {
                totalTasks = 5;
                startFirstTask();
                if (updateComponent(*component)) {
                    status = UPDATER_NEEDRESTART;
                }
                else {
                    status = UPDATER_FAILED;
                }
                return;
            }
        }
    }

    startFirstTask();
    for (int i = 0; i < components.size(); i++) {
        Launcher::Component *component = &components.getReference(i);
        if (component->canUpdate()) {
            if (!updateComponent(*component)) {
                status = UPDATER_FAILED;
                return;
            }
        }
    }

    status = UPDATER_FINISHED;
}

enum UpdaterStatus Updater::getStatus()
{
    return status;
}

bool Updater::getStatusUpdate(double& overallPercent, double& taskPercent)
{
    const GenericScopedLock<SpinLock> lock(statusLock);
    overallPercent = this->overallPercent;
    taskPercent = this->taskPercent;
    return statusStringUpdated;
}

String Updater::getStatusText()
{
    const GenericScopedLock<SpinLock> lock(statusLock);
    statusStringUpdated = false;
    return statusString;
}

void Updater::setStatusText(const String& s)
{
    const GenericScopedLock<SpinLock> lock(statusLock);
    statusString = s;
    statusStringUpdated = true;
}

void Updater::setTaskPercent(const double& percent)
{
    const GenericScopedLock<SpinLock> lock(statusLock);
    taskPercent = percent;
}

void Updater::startFirstTask()
{
    const GenericScopedLock<SpinLock> lock(statusLock);
    taskPercent = 0;
    completedTasks = 0;
    overallPercent = 0;
}

void Updater::startNextTask()
{
    const GenericScopedLock<SpinLock> lock(statusLock);
    taskPercent = 0;
    completedTasks++;
    if (totalTasks) {
        overallPercent = (double)completedTasks / totalTasks;
    }
    else {
        overallPercent = 0;
    }
}

bool Updater::getLatestComponents()
{
    URL url = URL(UPDATER_URL_PREFIX).getChildURL("latest.php")
        .withParameter("channel", launcherApplication::getConfig()->getString("updater", "channel"))
        .withParameter("platform", launcherApplication::getConfig()->getString("updater", "platform"))
        .withParameter("components", "launcher,tremulous");

    ScopedPointer<InputStream> stream(url.createInputStream(false, nullptr, nullptr, String(), 2000));
    if (stream == nullptr) {
        return false;
    }

    var versionsVar = JSON::parse(*stream);
    if (versionsVar == var::null || !versionsVar.isObject()) {
        return false;
    }

    DynamicObject *versionsObject = versionsVar.getDynamicObject();
    NamedValueSet *versionsSet = &versionsObject->getProperties();

    components.clear();
    for (int i = 0; i < versionsSet->size(); i++) {
        String componentName(versionsSet->getName(i));
        Launcher::Component component(componentName);
        component.setRemoteVersion(versionsSet->getValueAt(i).toString());
        components.add(component);
    }

    return true;
}

bool Updater::updateComponent(Launcher::Component& component)
{
	Logger::writeToLog(String("updating component ") + component.getName());

	setStatusText(String(TRANS("Fetching info for ")) + component.getName());
    if (!component.getRemoteInfo()) {
        return false;
    }

    startNextTask();
    setStatusText(String(TRANS("Checking if ")) + component.getName() + String(TRANS(" is already installed")));
    if (component.alreadyInstalled()) {
        startNextTask();
        startNextTask();
        startNextTask();
        startNextTask();
        return true;
    }

    startNextTask();
    setStatusText(String(TRANS("Downloading ")) + component.getName());
    if (!component.download()) {
        return false;
    }

    startNextTask();
    setStatusText(String(TRANS("Verifying ")) + component.getName());
    if (!component.verify()) {
        return false;
    }

    startNextTask();
    setStatusText(String(TRANS("Installing ")) + component.getName());
    if (!component.install()) {
        return false;
    }

    startNextTask();

    return true;
}

Updater *Updater::getUpdater()
{
    return updater;
}

bool Updater::downloadFile(const URL& url, const File& file, int64 resume)
{
    String headers;

    if (resume > 0) {
        headers = String("Range: bytes=") + String(resume) + String("-\r\n");
    }
    else if (file.exists()) {
        if (!file.deleteFile()) {
            return false;
        }
    }

    StringPairArray responseHeaders;
    int statusCode;
    ScopedPointer<InputStream> inputStream(url.createInputStream(false, nullptr, nullptr, headers, 2000, &responseHeaders, &statusCode));
    if (inputStream == nullptr) {
        return false;
    }

    if (resume <= 0 && statusCode != 200) {
        return false;
    }
    else if (resume > 0 && statusCode != 206) {
        return false;
    }

    ScopedPointer<FileOutputStream> outputStream(file.createOutputStream());
    if (outputStream == nullptr || outputStream->failedToOpen()) {
        return false;
    }

    int64 length = -1;
    if (responseHeaders.containsKey("Content-Length")) {
        length = responseHeaders["Content-Length"].getLargeIntValue();
    }

    int64 pos = 0;
    int64 last_pos = pos;
    double start_time = Time::getMillisecondCounterHiRes();
    double last_time = start_time;
    double now;

    int samples[DOWNLOAD_SAMPLES];
    int num_samples = 0;
    int next_sample = 0;
    int bytes_per_second;

    while (!inputStream->isExhausted() && !Thread::getCurrentThread()->threadShouldExit()) {
        char buf[0x8000];
        int size = inputStream->read(buf, sizeof(buf));
        outputStream->write(buf, size);
        pos += size;

        now = Time::getMillisecondCounterHiRes();
        if (now - last_time >= DOWNLOAD_UPDATE_INTERVAL) {
            last_time = now;
            if (length > 0) {
                updater->setTaskPercent((double)pos / length);
            }

            samples[next_sample++] = (int)(pos - last_pos);
            last_pos = pos;
            if (next_sample >= DOWNLOAD_SAMPLES) {
                next_sample = 0;
            }
            if (num_samples < DOWNLOAD_SAMPLES) {
                num_samples++;
            }
            int64 accumulator = 0;
            for (int i = 0; i < num_samples; i++) {
                accumulator += samples[i];
            }
            bytes_per_second = (int)(accumulator / (num_samples * ((double)DOWNLOAD_UPDATE_INTERVAL / 1000)));

            String message(String("Downloading ") + file.getFileName() + String(" at "));
            if (bytes_per_second >= 1024 * 1024) {
                message += String((double)bytes_per_second / 1024 / 1024, 2) + String(" MB/s");
            }
            else if (bytes_per_second >= 1024) {
                message += String((double)bytes_per_second / 1024, 2) + String(" KB/s");
            }
            else {
                message += String(bytes_per_second) + String(" bytes/s");
            }
            updater->setStatusText(message);
        }
    }

    if (pos < length) {
        return false;
    }

    now = Time::getMillisecondCounterHiRes();
    bytes_per_second = (int)(pos / ((now - start_time) / 1000));
    updater->setTaskPercent(1);

    String message(String("Completed download of ") + file.getFileName() + String(" at "));
    if (bytes_per_second >= 1024 * 1024) {
        message += String((double)bytes_per_second / 1024 / 1024, 2) + String(" MB/s");
    }
    else if (bytes_per_second >= 1024) {
        message += String((double)bytes_per_second / 1024, 2) + String(" KB/s");
    }
    else {
        message += String(bytes_per_second) + String(" bytes/s");
    }
    updater->setStatusText(message);

    return true;
}
