#include "CaacAccessController.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QRandomGenerator>
#include <QtCore/QSettings>
#include <QtNetwork/QPasswordDigestor>

namespace {
constexpr int PBKDF2_ITERATIONS = 600000;
constexpr int HASH_LENGTH = 32;
constexpr int SALT_LENGTH = 16;
const QByteArray DEFAULT_SALT = QByteArray::fromHex("fa7c7264f91736c90dd589cfe16c3f1e");
const QByteArray DEFAULT_HASH = QByteArray::fromHex("3f2d512655df17a10799dcb638f3d5dbd97a09ee152fb2c82bf4fa27bb3a869d");

QByteArray derive(const QString& password, const QByteArray& salt)
{
    return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, password.toUtf8(), salt, PBKDF2_ITERATIONS,
                                              HASH_LENGTH);
}
}  // namespace

CaacAccessController::CaacAccessController(QObject* parent, const QString& storageGroup)
    : QObject(parent)
    , _storageGroup(storageGroup)
{}

void CaacAccessController::ensureDefaultPassword()
{
    if (configured()) {
        return;
    }
    QSettings settings;
    settings.setValue(_storageGroup + "/salt", DEFAULT_SALT);
    settings.setValue(_storageGroup + "/hash", DEFAULT_HASH);
    settings.sync();
    emit configuredChanged();
}

bool CaacAccessController::configured() const
{
    const QSettings settings;
    return settings.value(_storageGroup + "/salt").toByteArray().size() == SALT_LENGTH &&
           settings.value(_storageGroup + "/hash").toByteArray().size() == HASH_LENGTH;
}

bool CaacAccessController::verify(const QString& password) const
{
    if (!configured() || password.isEmpty() || password.size() > 128) {
        return false;
    }
    const QSettings settings;
    const auto expected = settings.value(_storageGroup + "/hash").toByteArray();
    const auto actual = derive(password, settings.value(_storageGroup + "/salt").toByteArray());
    if (actual.size() != HASH_LENGTH) {
        return false;
    }
    unsigned char difference = 0;
    for (int i = 0; i < HASH_LENGTH; ++i) {
        difference |= static_cast<unsigned char>(expected[i]) ^ static_cast<unsigned char>(actual[i]);
    }
    return difference == 0;
}

bool CaacAccessController::setPassword(const QString& currentPassword, const QString& password)
{
    if (password.isEmpty() || password.size() > 128 || (configured() && !verify(currentPassword))) {
        return false;
    }
    QByteArray salt(SALT_LENGTH, Qt::Uninitialized);
    for (auto& byte : salt) {
        byte = static_cast<char>(QRandomGenerator::system()->generate());
    }
    const auto hash = derive(password, salt);
    if (hash.size() != HASH_LENGTH) {
        return false;
    }
    QSettings settings;
    settings.setValue(_storageGroup + "/salt", salt);
    settings.setValue(_storageGroup + "/hash", hash);
    settings.sync();
    if (settings.status() != QSettings::NoError) {
        return false;
    }
    emit configuredChanged();
    return true;
}
