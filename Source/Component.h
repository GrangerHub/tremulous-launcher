/*
==============================================================================

Component.h
Created: 24 Jul 2015 4:00:00pm
Author:  jkent

==============================================================================
*/

#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include "Semver.h"

namespace Launcher
{
    class Component
    {
    public:
        Component();
        Component(const Component& other);
        Component(const String& name);

        void setRemoteVersion(const Semver& version);
        bool canUpdate();
        String& getName();

        bool getRemoteInfo();
        bool alreadyInstalled();
        bool download();
        bool verify();
        bool install();

    private:
        String name;
        Semver localVersion;
        Semver remoteVersion;
        URL remoteUrl;
        int64 remoteSize;
        String remoteHash;
        File downloadPath;

        bool installPk3();
        bool installLauncher();
        bool installTremulous();
    };
};

#endif  // COMPONENT_H_INCLUDED