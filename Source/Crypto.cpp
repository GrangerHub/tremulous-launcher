/*
  ==============================================================================

    Crypto.cpp
    Created: 18 Jul 2015 10:44:39am
    Author:  jkent

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PublicKey.h"
#include "Crypto.h"
#include "Main.h"

const char Crypto::KEY_REVOCATION_CHECK_URL[] = "http://trem.jkent.net/updater/revoked.php";
OwnedArray<PublicKey> Crypto::publicKeys;

void Crypto::init()
{
    loadKeys();
    checkKeys();
}

bool Crypto::hashFile(const File &file, uint8_t *digest)
{
    ScopedPointer<FileInputStream> stream(file.createInputStream());
    if (stream == nullptr || stream->failedToOpen()) {
        return false;
    }
    
    MemoryBlock buffer;
    buffer.setSize(0x8000);

    sha256_ctx hash;
    sha256_init(&hash);
    while (int len = stream->read(buffer.getData(), buffer.getSize())) {
        sha256_update(&hash, len, (uint8_t *)buffer.getData());
    }
    sha256_digest(&hash, SHA256_DIGEST_SIZE, digest);
    
    return true;
}

bool Crypto::verify(const File &file, const uint8_t *digest)
{
    uint8_t localDigest[SHA256_DIGEST_SIZE];
    if (digest == nullptr) {
        if (!hashFile(file, localDigest)) {
            return false;
        }
        digest = localDigest;
    }
    
    File signatureFile(file.getFullPathName() + ".sig");
    if (!signatureFile.existsAsFile()) {
        return false;
    }

    MemoryBlock buffer;
    if (!signatureFile.loadFileAsData(buffer)) {
        return false;
    }

    mpz_t signature;
    mpz_init(signature);
    nettle_mpz_set_str_256_u(signature, buffer.getSize(), (uint8_t *)buffer.getData());
    
    for (int i = 0; i < publicKeys.size(); i++) {
        PublicKey *key = publicKeys[i];
        if (key->isRevoked()) {
            continue;
        }
        if (rsa_sha256_verify_digest(&key->publicKey, digest, signature)) {
            mpz_clear(signature);
            return true;
        }
    }

    mpz_clear(signature);
    return false;
}

void Crypto::loadKeys()
{
    publicKeys.clear();
    for (int i = 0; i < BinaryData::namedResourceListSize; i++) {
        String resourceName(BinaryData::namedResourceList[i]);
        if (!resourceName.endsWith("_pub")) {
            continue;
        }

        int data_size;
        const char *data = BinaryData::getNamedResource(resourceName.toUTF8(), data_size);
        if (!data) {
            continue;
        }

        ScopedPointer<PublicKey> newKey(new PublicKey(data, data_size));
        if (!newKey->isLoaded()) {
            continue;
        }

        for (int j = 0; j < publicKeys.size(); j++) {
            PublicKey *key = publicKeys[j];
            if (key->getFingerprint() == newKey->getFingerprint()) {
                newKey = nullptr;
                break;
            }
        }
        if (newKey == nullptr) {
            continue;
        }

        String status(launcherApplication::getConfig()->getString("pubkeys", newKey->getFingerprint()));
        if (status == "revoked") {
            newKey = nullptr;
            continue;
        }

        publicKeys.add(newKey);
        newKey.release();
    }
}

void Crypto::checkKeys()
{
    if (publicKeys.size() == 0) {
        return;
    }
    
    String fingerprints;
    for (int i = 0; i < publicKeys.size(); i++) {
        PublicKey *key = publicKeys[i];
        fingerprints += key->getFingerprint();
        if (i + 1 < publicKeys.size()) {
            fingerprints += ',';
        }
    }

    URL url = URL(KEY_REVOCATION_CHECK_URL)
             .withParameter("fingerprints", fingerprints);

    Logger::writeToLog("Checking for public key revocation");
    String message;
    message = String("  URL: ") + url.toString(true);
    DBG(String::formatted("  URL: %s", url.toString(true).toRawUTF8()));

    InputStream *stream = url.createInputStream(false);
    if (stream == nullptr) {
        Logger::writeToLog("  Unable to create stream");
        return;
    }

    var fingerprintsObject = JSON::parse(*stream);
    delete stream;

    if (fingerprintsObject == var::null || !fingerprintsObject.isObject()) {
        Logger::writeToLog("  Key revocation service is unavailable");
        return;
    }

    for (int i = 0; i < publicKeys.size(); i++) {
        PublicKey *key = publicKeys[i];
        String status = fingerprintsObject[key->getFingerprint().toRawUTF8()];
        if (status == "valid") {
            key->revoked = REVOKED_NO;
        }
        else if (status == "revoked") {
            key->revoked = REVOKED_YES;
            launcherApplication::getConfig()->set("pubkeys", key->getFingerprint(), status);
            launcherApplication::getConfig()->save();
            publicKeys.remove(i--);
        }
        else {
            key->revoked = REVOKED_UNKNOWN;
        }
    }
}