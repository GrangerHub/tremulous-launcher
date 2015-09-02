/*
==============================================================================

Component.cpp
Created: 24 Jul 2015 4:00:00pm
Author:  jkent

==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Component.h"
#include "Semver.h"
#include "Main.h"
#include "IniFile.h"
#include "Crypto.h"

#define DOWNLOAD_UPDATE_INTERVAL 100
#define DOWNLOAD_AVERAGE_TIME 10000
#define DOWNLOAD_SAMPLES (DOWNLOAD_AVERAGE_TIME / DOWNLOAD_UPDATE_INTERVAL)

namespace Launcher
{
    Component::Component()
        :Component("")
    {
    }

    Component::Component(const Component& other)
        : name(other.name), localVersion(other.localVersion), remoteVersion(other.remoteVersion)
    {
    }

    Component::Component(const String& name)
        : name(name)
    {
        if (name == "launcher") {
            localVersion = Semver(ProjectInfo::versionString);
        }
        else {
            File basepath(launcherApplication::getConfig()->getString("general", "basepath"));
            IniFile localComponents(basepath.getChildFile("components.ini"));
            localVersion = Semver(localComponents.getString("", name));
        }
    }

    String& Component::getName()
    {
        return name;
    }

    void Component::setRemoteVersion(const Semver& version)
    {
        remoteVersion = version;
    }

    bool Component::canUpdate()
    {
        return (remoteVersion > localVersion);
    }

    bool Component::getRemoteInfo()
    {
        URL url = URL(Updater::UPDATER_URL_PREFIX).getChildURL("download.php")
            .withParameter("version", remoteVersion.toString())
            .withParameter("platform", launcherApplication::getConfig()->getString("updater", "platform"))
            .withParameter("component", name);

        ScopedPointer<InputStream> stream(url.createInputStream(false, nullptr, nullptr, String(), 5000));
        if (stream == nullptr) {
            return false;
        }

        var componentObject = JSON::parse(*stream);
        if (componentObject == var::null || !componentObject.isObject()) {
            return false;
        }

        if (!componentObject["download"].isString()
            || !componentObject["size"].isInt()
            || !componentObject["sha256"].isString()) {
            return false;
        }

        remoteUrl = componentObject["download"].toString();
        remoteSize = componentObject["size"];
        remoteHash = componentObject["sha256"];

        return true;
    }

    bool Component::alreadyInstalled()
    {
        if (!name.endsWith(".pk3")) {
            return false;
        }

        File basepath(launcherApplication::getConfig()->getString("general", "basepath"));
        File targetPath(basepath.getChildFile(name));

        if (!targetPath.exists()) {
            return false;
        }

        uint8_t digest[SHA256_DIGEST_SIZE];
        if (!Crypto::hashFile(targetPath, digest)) {
            return false;
        }

        if (remoteHash != String::toHexString(digest, sizeof(digest), 0)) {
            return false;
        }

        IniFile components(basepath.getChildFile("components.ini"));
        components.set("", name, remoteVersion.toString());
        components.save();

        return true;
    }

    bool Component::download()
    {
        File downloadDir(launcherApplication::getConfig()->getString("updater", "downloads"));
        downloadPath = downloadDir.getChildFile(File::getCurrentWorkingDirectory().getChildFile(remoteUrl.getSubPath()).getFileName());
        if (downloadPath.getParentDirectory().createDirectory().failed()) {
            return false;
        }

        int64 start = 0;
        if (downloadPath.exists()) {
            start = downloadPath.getSize();
        }

        if (start < remoteSize) {
            if (!Updater::getUpdater()->downloadFile(remoteUrl, downloadPath, start)) {
                return false;
            }
        }

        URL signatureUrl(remoteUrl.toString(false) + ".sig");
        File signaturePath(downloadPath.getFullPathName() + ".sig");

        if (launcherApplication::getConfig()->getInt("updater", "verifySignature")) {
            if (!Updater::getUpdater()->downloadFile(signatureUrl, signaturePath)) {
                return false;
            }
        }

        return true;
    }

    bool Component::verify()
    {
        uint8_t digest[SHA256_DIGEST_SIZE];
        if (!Crypto::hashFile(downloadPath, digest)) {
            return false;
        }

        if (remoteHash != String::toHexString(digest, sizeof(digest), 0)) {
            return false;
        }

        if (launcherApplication::getConfig()->getInt("updater", "verifySignature")) {
            if (!Crypto::verify(downloadPath, digest)) {
                return false;
            }
        }

        return true;
    }

    bool Component::install()
    {
        if (name.endsWith(".pk3")) {
            if (!installPk3()) {
                return false;
            }
        }
        else if (name == "launcher") {
            if (!installLauncher()) {
                return false;
            }
        }
        else if (name == "tremulous") {
            if (!installTremulous()) {
                return false;
            }
        }
        else {
            return false;
        }

        downloadPath.deleteFile();

        File signaturePath(downloadPath.getFullPathName() + ".sig");
        signaturePath.deleteFile();

        if (name == "launcher") {
            return true; // don't store the launcher version in the ini file
        }

        File basepath(launcherApplication::getConfig()->getString("general", "basepath"));
        IniFile components(basepath.getChildFile("components.ini"));
        components.set("", name, remoteVersion.toString());
        components.save();

        return true;
    }

    bool Component::installPk3()
    {
        File basepath(launcherApplication::getConfig()->getString("general", "basepath"));
        File targetPath(basepath.getChildFile(name));

        if (!targetPath.getParentDirectory().createDirectory()) {
            return false;
        }

        if (!downloadPath.moveFileTo(targetPath)) {
            return false;
        }

        return true;
    }

    bool Component::installLauncher()
    {
        File launcherPath(File::getSpecialLocation(File::currentApplicationFile));
        File launcherDir(launcherPath.getParentDirectory());

        if (!launcherPath.moveFileTo(launcherPath.getFullPathName() + ".old")) {
            return false;
        }

        ZipFile launcherZip(downloadPath);
        if (launcherZip.uncompressTo(launcherDir).failed()) {
            return false;
        }

        return true;
    }

    bool Component::installTremulous()
    {
        return false;
    }
};