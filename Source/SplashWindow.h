/*
  ==============================================================================

    SplashWindow.h
    Created: 11 Jul 2015 5:00:59pm
    Author:  jkent

  ==============================================================================
*/

#ifndef SPLASHWINDOW_H_INCLUDED
#define SPLASHWINDOW_H_INCLUDED

#include "SplashComponent.h"
#include "Updater.h"
#include "IniFile.h"

class SplashWindow    : public DocumentWindow, private Timer
{
public:
    SplashWindow (String name);

    void closeButtonPressed() override;

    bool updaterRunning();

    /* Note: Be careful if you override any DocumentWindow methods - the base
       class uses a lot of them, so by overriding you might break its functionality.
       It's best to do all your work in your content component instead, but if
       you really have to override any DocumentWindow methods, make sure your
       subclass also calls the superclass's method.
    */

private:
    IniFile *config;
    ScopedPointer<SplashComponent> component;
    ScopedPointer<Updater> updater;
    void timerCallback() override;
    double overallPercent, taskPercent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SplashWindow)
};

#endif  // SPLASHWINDOW_H_INCLUDED