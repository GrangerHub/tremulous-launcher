/*
  ==============================================================================

    IniFile.cpp
    Created: 19 Jul 2015 11:12:38pm
    Author:  jkent

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "IniFile.h"

IniFile::IniFile(const File &file)
:file(file)
{
    load();
}

void IniFile::load()
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    StringArray lines;
    file.readLines(lines);
    lines.removeEmptyStrings();
    lines.trim();

    clear();

    String section("__empty");
    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].startsWithChar(';')) {
            continue;
        }
        if (lines[i].startsWithChar('[') && lines[i].endsWithChar(']')) {
            section = lines[i].substring(1, lines[i].length() - 1);
            continue;
        }
        if (!lines[i].containsChar('=')) {
            continue;
        }
        String key = lines[i].upToFirstOccurrenceOf("=", false, false).trim();
        String value = lines[i].fromFirstOccurrenceOf("=", false, false).trim();

        DynamicObject *sectionObject;
        if (data.contains(section)) {
            sectionObject = data[section].getDynamicObject();
        } else {
            sectionObject = new DynamicObject();
            data.set(section, var(sectionObject));
        }
        sectionObject->setProperty(key, var(value));
    }    
}

void IniFile::save()
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String config;
    for (int i = 0; i < data.size(); i++) {
        String section(data.getName(i));
        DynamicObject *sectionObject = data[section].getDynamicObject();
        NamedValueSet *sectionSet = &sectionObject->getProperties();

        if (sectionSet->size() == 0) {
            continue;
        }
        
        if (section != "__empty") {
            config += '[';
            config += section;
           #ifdef JUCE_WINDOWS
            config += "]\r\n";
           #else
            config += "]\n";
           #endif
        }

        for (int j = 0; j < sectionSet->size(); j++) {
            config += sectionSet->getName(j).toString();
            config += '=';
            config += sectionSet->getValueAt(j).toString();
           #ifdef JUCE_WINDOWS
            config += "\r\n";
           #else
            config += '\n';
           #endif
        }

       #ifdef JUCE_WINDOWS
        config += "\r\n";
       #else
        config += '\n';
       #endif
    }

    file.replaceWithText(config);
}

void IniFile::clear()
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    data.clear();
    data.set("__empty", var(new DynamicObject()));
}

int IniFile::getInt(const String& section, const String& key) const
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    if (!data.contains(realSection)) {
        return getDefaultInt(section, key);
    }
    
    DynamicObject *sectionObject = data[realSection].getDynamicObject();
    if (sectionObject->getProperty(key) == var::null) {
        return getDefaultInt(section, key);
    }
    return sectionObject->getProperty(key);
}

String IniFile::getString(const String& section, const String& key) const
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    if (!data.contains(realSection)) {
        return getDefaultString(section, key);
    }
    
    DynamicObject *sectionObject = data[realSection].getDynamicObject();
    if (sectionObject->getProperty(key) == var::null) {
        return getDefaultString(section, key);
    }
    return sectionObject->getProperty(key);
}

void IniFile::set(const String& section, const String& key, int value)
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    DynamicObject *sectionObject;
    if (data.contains(realSection)) {
        sectionObject = data[realSection].getDynamicObject();
    } else {
        sectionObject = new DynamicObject();
        data.set(section, var(sectionObject));
    }
    
    sectionObject->setProperty(key, var(value));
}

void IniFile::set(const String& section, const String& key, const String& value)
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    DynamicObject *sectionObject;
    if (data.contains(realSection)) {
        sectionObject = data[realSection].getDynamicObject();
    } else {
        sectionObject = new DynamicObject();
        data.set(section, var(sectionObject));
    }

    sectionObject->setProperty(key, var(value));
}

void IniFile::setDefault(const String& section, const String& key, int value)
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    DynamicObject *sectionObject;
    if (defaults.contains(realSection)) {
        sectionObject = defaults[realSection].getDynamicObject();
    }
    else {
        sectionObject = new DynamicObject();
        defaults.set(section, var(sectionObject));
    }

    sectionObject->setProperty(key, var(value));
}

void IniFile::setDefault(const String& section, const String& key, const String& value)
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    DynamicObject *sectionObject;
    if (defaults.contains(realSection)) {
        sectionObject = defaults[realSection].getDynamicObject();
    }
    else {
        sectionObject = new DynamicObject();
        defaults.set(section, var(sectionObject));
    }

    sectionObject->setProperty(key, var(value));
}

int IniFile::getDefaultInt(const String& section, const String& key) const
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    if (!defaults.contains(realSection)) {
        return 0;
    }

    DynamicObject *sectionObject = defaults[realSection].getDynamicObject();
    if (sectionObject->getProperty(key) == var::null) {
        return 0;
    }
    return sectionObject->getProperty(key);
}

String IniFile::getDefaultString(const String& section, const String& key) const
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    if (!defaults.contains(realSection)) {
        return String();
    }

    DynamicObject *sectionObject = defaults[realSection].getDynamicObject();
    if (sectionObject->getProperty(key) == var::null) {
        return String();
    }
    return sectionObject->getProperty(key);
}

bool IniFile::isset(const String& section, const String& key) const
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }

    if (!data.contains(realSection)) {
        return false;
    }

    DynamicObject *sectionObject = data[realSection].getDynamicObject();
    if (sectionObject->getProperty(key) == var::null) {
        return false;
    }

    return true;
}

void IniFile::unset(const String& section, const String& key)
{
    const GenericScopedLock<CriticalSection> scopedlock(lock);
    String realSection(section);
    if (realSection.isEmpty()) {
        realSection = "__empty";
    }
    
    if (!data.contains(realSection)) {
        return;
    }

    DynamicObject *sectionObject = data[realSection].getDynamicObject();
    sectionObject->removeProperty(key);
    
    if (realSection != "__empty" && sectionObject->getProperties().size() == 0) {
        data.remove(realSection);
    }
}