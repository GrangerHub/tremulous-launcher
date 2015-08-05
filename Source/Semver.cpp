/*
  ==============================================================================

    Version.cpp
    Created: 11 Jul 2015 8:07:10am
    Author:  jkent

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Semver.h"

Semver::Semver()
:m_major(0), m_minor(0), m_patch(0)
{
}

Semver::Semver(const Semver &other)
:m_major(other.m_major), m_minor(other.m_minor), m_patch(other.m_patch),
m_prerelease(other.m_prerelease), m_metadata(other.m_metadata)
{
}

Semver::Semver(const String &s)
{
    parse(s);
}

Semver::Semver(const char *s)
{
    parse(String(s));
}

int Semver::getMajor() const
{
    return m_major;
}

int Semver::getMinor() const
{
    return m_minor;
}

int Semver::getPatch() const
{
    return m_patch;
}

bool Semver::isPrerelease() const
{
    return m_prerelease.isNotEmpty();
}

bool Semver::hasMetadata() const
{
    return m_metadata.isNotEmpty();
}

String Semver::getPrerelease() const
{
    return m_prerelease;
}

String Semver::getMetadata() const
{
    return m_metadata;
}

void Semver::incrementPatch()
{
    m_patch++;
}

void Semver::incrementMinor()
{
    m_minor++;
    m_patch = 0;
}

void Semver::incrementMajor()
{
    m_major++;
    m_minor = 0;
    m_patch = 0;
}

void Semver::makeRelease()
{
    m_prerelease.clear();
}

void Semver::setPrerelease(const String &s)
{
    m_prerelease = s;
}

void Semver::clearMetadata()
{
    m_metadata.clear();
}

void Semver::setMetadata(const String &s)
{
    m_metadata = s;
}

String Semver::toString() const
{
    String s;
    
    s += m_major;
    s += ".";
    s += m_minor;
    s += ".";
    s += m_patch;
    
    if (m_prerelease.isNotEmpty()) {
        s += "-";
        s += m_prerelease;
    }
    
    if (m_metadata.isNotEmpty()) {
        s += "+";
        s += m_metadata;
    }
    
    return s;
}

enum parse_state_t {
    PARSE_MAJOR = 0,
    PARSE_MINOR,
    PARSE_PATCH,
    PARSE_PRERELEASE,
    PARSE_METADATA
};

void Semver::parse(const String &s)
{
    int start, end;
    enum parse_state_t parse_state;

    m_major = 0;
    m_minor = 0;
    m_patch = 0;
    m_prerelease.clear();
    m_metadata.clear();
    
    parse_state = PARSE_MAJOR;
    start = end = 0;
    
    while (++end < s.length()) {
        switch (parse_state) {
        case PARSE_MAJOR:
            if (s[end] == '.' || s[end] == '-' || s[end] == '+') {
                m_major = s.substring(start, end).getIntValue();
                if (s[end] == '.') {
                    parse_state = PARSE_MINOR;
                } else if (s[end] == '-') {
                    parse_state = PARSE_PRERELEASE;
                } else if (s[end] == '+') {
                    parse_state = PARSE_METADATA;
                }
                start = end + 1;
            }
            break;
        case PARSE_MINOR:
            if (s[end] == '.' || s[end] == '-' || s[end] == '+') {
                m_minor = s.substring(start, end).getIntValue();
                if (s[end] == '.') {
                    parse_state = PARSE_PATCH;
                } else if (s[end] == '-') {
                    parse_state = PARSE_PRERELEASE;
                } else if (s[end] == '+') {
                    parse_state = PARSE_METADATA;
                }
                start = end + 1;
            }
            break;
        case PARSE_PATCH:
            if (s[end] == '-' || s[end] == '+') {
                m_patch = s.substring(start, end).getIntValue();
                if (s[end] == '-') {
                    parse_state = PARSE_PRERELEASE;
                } else if (s[end] == '+') {
                    parse_state = PARSE_METADATA;
                }
                start = end + 1;
            }
            break;
        case PARSE_PRERELEASE:
            if (s[end] == '+') {
                m_prerelease = s.substring(start, end);
                parse_state = PARSE_METADATA;
                start = end + 1;
            }
            break;
        default:
            break;
        }
    }
    
    switch (parse_state) {
    case PARSE_MAJOR:
        m_major = s.substring(start, end).getIntValue();
        break;
    case PARSE_MINOR:
        m_minor = s.substring(start, end).getIntValue();
        break;
    case PARSE_PATCH:
        m_patch = s.substring(start, end).getIntValue();
        break;
    case PARSE_PRERELEASE:
        m_prerelease = s.substring(start, end);
        break;
    case PARSE_METADATA:
        m_metadata = s.substring(start, end);
        break;
    }
}

int Semver::compare(const Semver &other) const
{
    if (m_major < other.m_major) {
        return -1;
    } else if (m_major > other.m_major) {
        return 1;
    }
    
    if (m_minor < other.m_minor) {
        return -1;
    } else if (m_minor > other.m_minor) {
        return 1;
    }
    
    if (m_patch < other.m_patch) {
        return -1;
    } else if (m_patch > other.m_patch) {
        return 1;
    }
    
    if (m_prerelease != other.m_prerelease) {
        if (m_prerelease.isNotEmpty() && other.m_prerelease.isEmpty()) {
            return -1;
        } else if (m_prerelease.isEmpty() && other.m_prerelease.isNotEmpty()) {
            return 1;
        }

        int len1, len2, start1, start2, end1, end2;
        bool numeric1, numeric2;
        int int1, int2, result;
        String s1, s2;

        len1 = m_prerelease.length();
        len2 = other.m_prerelease.length();
        start1 = 0;
        start2 = 0;
        
        while (true) {
            end1 = start1;
            end2 = start2;
            numeric1 = true;
            numeric2 = true;

            while (end1 < len1 && m_prerelease[end1] != '.') {
                if (m_prerelease[end1] < '0' || m_prerelease[end1] > '9') {
                    numeric1 = false;
                }
                end1++;
            }
            while (end2 < len2 && other.m_prerelease[end2] != '.') {
                if (other.m_prerelease[end2] < '0' || other.m_prerelease[end2] > '9') {
                    numeric2 = false;
                }
                end2++;
            }
            
            if (numeric1 && !numeric2) {
                return -1;
            } else if (!numeric1 && numeric2) {
                return 1;
            } else {
                s1 = m_prerelease.substring(start1, end1);
                s2 = other.m_prerelease.substring(start2, end2);
                if (numeric1) {
                    int1 = s1.getIntValue();
                    int2 = s2.getIntValue();
                    if (int1 < int2) {
                        return -1;
                    } else if (int1 > int2) {
                        return 1;
                    }
                } else {
                    result = s1.compare(s2);
                    if (result != 0) {
                        return result;
                    }
                }
            }
            
            if (end1 >= len1 && end2 < len2) {
                return -1;
            } else if (end1 < len1 && end2 >= len2) {
                return 1;
            } else if (end1 >= len1 && end2 >= len2) {
                break;
            }

            start1 = end1 + 1;
            start2 = end2 + 1;
        }
    }
    
    return 0;
}

bool operator == (const Semver &v1, const Semver &v2)
{
    return v1.compare(v2) == 0;
}

bool operator != (const Semver &v1, const Semver &v2)
{
    return v1.compare(v2) != 0;
}

bool operator < (const Semver &v1, const Semver &v2)
{
    return v1.compare(v2) < 0;
}

bool operator <= (const Semver &v1, const Semver &v2)
{
    return v1.compare(v2) <= 0;
}

bool operator > (const Semver &v1, const Semver &v2)
{
    return v1.compare(v2) > 0;
}

bool operator >= (const Semver &v1, const Semver &v2)
{
    return v1.compare(v2) >= 0;
}