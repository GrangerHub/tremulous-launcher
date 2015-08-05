/*
  ==============================================================================

    SplashWindow.cpp
    Created: 11 Jul 2015 5:00:59pm
    Author:  jkent

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SplashWindow.h"
#include "SplashComponent.h"
#include "Main.h"
#include "Updater.h"

SplashWindow::SplashWindow(String name)
    : DocumentWindow(name, Colours::lightgrey, DocumentWindow::minimiseButton | DocumentWindow::closeButton),
    overallPercent(0), taskPercent(0)
{
    setUsingNativeTitleBar (false);
    setTitleBarHeight(0);
    component = new SplashComponent(overallPercent, taskPercent);
    setContentNonOwned(component, true);
    centreWithSize (getWidth(), getHeight());
    setVisible (true);

    config = launcherApplication::getConfig();
    updater = new Updater();
    if (config->getInt("updater", "checkForUpdates") && updater->checkForUpdates()) {
        if (config->getInt("updater", "autoUpdate") || config->getInt("updater", "updateConfirmed")) {
            if (launcherApplication::Elevate()) {
                updater->startThread();
            }
        }
        else {
            AlertWindow alertWindow(ProjectInfo::projectName,
                TRANS("An update is available, do you want to download and apply it now?"),
                AlertWindow::QuestionIcon);
            alertWindow.addButton(TRANS("Yes"), 1);
            alertWindow.addButton(TRANS("No"), 0, KeyPress(KeyPress::escapeKey, ModifierKeys::noModifiers, 0));
            if (alertWindow.runModalLoop()) {
                config->set("updater", "updateConfirmed", 1);
                config->save();
                if (launcherApplication::Elevate()) {
                    updater->startThread();
                }
            }
        }
    }

    startTimerHz(60);
}

void SplashWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

void SplashWindow::timerCallback()
{
    if (updater == nullptr) {
        stopTimer();
        return;
    }

    if (launcherApplication::isQuitting()) {
        if (updater->isThreadRunning()) {
            updater->stopThread(3000);
        }
        else {
            launcherApplication::quit();
        }
        return;
    }

    if (updater->getStatusUpdate(overallPercent, taskPercent)) {
        component->StatusLabel->setText(updater->getStatusText(), dontSendNotification);
    }

    enum UpdaterStatus status = updater->getStatus();
    if ((!updater->isThreadRunning() && status == UPDATER_NOTSTARTED) || status == UPDATER_FINISHED) {
        stopTimer();
        config->set("updater", "updateConfirmed", 0);
        config->save();
        component->StatusLabel->setText(TRANS("Starting Tremulous..."), dontSendNotification);
        launcherApplication::runTremulous();
    }
    else if (status == UPDATER_NEEDRESTART) {
        stopTimer();
        component->StatusLabel->setText(TRANS("Restarting Tremulous Launcher..."), dontSendNotification);
        launcherApplication::runLauncher();
    }
    else if (status == UPDATER_FAILED) {
        stopTimer();
        config->set("updater", "updateConfirmed", 0);
        config->save();
        component->StatusLabel->setText(TRANS("Update failed..."), dontSendNotification);
    }
}

bool SplashWindow::updaterRunning()
{
    if (updater != nullptr && updater->isThreadRunning()) {
        return true;
    }

    return false;
}