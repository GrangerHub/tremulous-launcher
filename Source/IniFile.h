/*
  ==============================================================================

    IniFile.h
    Created: 19 Jul 2015 11:12:38pm
    Author:  jkent

  ==============================================================================
*/

#ifndef INIFILE_H_INCLUDED
#define INIFILE_H_INCLUDED

class IniFile
{
public:
    IniFile(const File& file);

    void load();
    void save();
    void clear();
    
    int getInt(const String& section, const String& key) const;
    String getString(const String& section, const String& key) const;

    void set(const String& section, const String& key, int value);
    void set(const String& section, const String& key, const String& value);

    void setDefault(const String& section, const String& key, int value);
    void setDefault(const String& section, const String& key, const String& value);

    bool isset(const String& section, const String& key) const;
    void unset(const String& section, const String& key);
    
private:
    File file;
    NamedValueSet data;
    NamedValueSet defaults;
    CriticalSection lock;

    int getDefaultInt(const String& section, const String& key) const;
    String getDefaultString(const String& section, const String& key) const;
};

#endif  // INIFILE_H_INCLUDED