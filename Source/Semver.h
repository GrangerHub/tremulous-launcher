/*
  ==============================================================================

    Semver.h
    Created: 11 Jul 2015 8:07:10am
    Author:  jkent

  ==============================================================================
*/

#ifndef SEMVER_H_INCLUDED
#define SEMVER_H_INCLUDED

class Semver
{
public:
    Semver();
    Semver(const Semver &other);
    Semver(const String &s);
    Semver(const char *s);

    int getMajor() const;
    int getMinor() const;
    int getPatch() const;
    bool isPrerelease() const;
    bool hasMetadata() const;
    String getPrerelease() const;
    String getMetadata() const;

    void incrementPatch();
    void incrementMinor();
    void incrementMajor();
    void makeRelease();
    void setPrerelease(const String &s);
    void clearMetadata();
    void setMetadata(const String &s);
    
    String toString() const;
    void parse(const String &s);    
    
    friend bool operator == (const Semver &v1, const Semver &v2);
    friend bool operator != (const Semver &v1, const Semver &v2);
    friend bool operator < (const Semver &v1, const Semver &v2);
    friend bool operator <= (const Semver &v1, const Semver &v2);
    friend bool operator > (const Semver &v1, const Semver &v2);
    friend bool operator >= (const Semver &v1, const Semver &v2);

private:
    int m_major;
    int m_minor;
    int m_patch;
    String m_prerelease;
    String m_metadata;
    
    int compare(const Semver &other) const;
};

#endif  // SEMVER_H_INCLUDED
