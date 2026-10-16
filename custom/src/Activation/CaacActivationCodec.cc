#include "CaacActivationCodec.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QCryptographicHash>
#include <QtCore/QRegularExpression>

#include "monocypher-ed25519.h"

namespace CaacActivationCodec {
QByteArray machineHash(const QByteArray& machineIdentity)
{
    if (machineIdentity.isEmpty()) {
        return {};
    }
    return QCryptographicHash::hash(QByteArrayLiteral("CAAC-GroundStation/machine/v1:") + machineIdentity,
                                    QCryptographicHash::Sha256)
        .first(16);
}

QString machineCode(const QByteArray& hash)
{
    if (hash.size() != 16) {
        return {};
    }
    const auto hex = QString::fromLatin1(hash.toHex().toUpper());
    return QStringLiteral("CAAC-%1-%2-%3-%4").arg(hex.mid(0, 8), hex.mid(8, 8), hex.mid(16, 8), hex.mid(24, 8));
}

QByteArray parseMachineCode(const QString& code)
{
    auto text = code.trimmed().toUpper();
    text.remove(QLatin1Char('-'));
    static const QRegularExpression pattern(QStringLiteral("^CAAC[0-9A-F]{32}$"));
    return pattern.match(text).hasMatch() ? QByteArray::fromHex(text.mid(4).toLatin1()) : QByteArray();
}

QByteArray signedMessage(const QByteArray& hash)
{
    return QByteArrayLiteral("CAAC-GroundStation/permanent/v1:") + hash;
}

QString normalize(const QString& code)
{
    QString normalized;
    normalized.reserve(code.size());
    for (const auto character : code) {
        if (!character.isSpace()) {
            normalized += character;
        }
    }
    return normalized;
}

QString encode(const QByteArray& hash, const QByteArray& signature)
{
    if (hash.size() != 16 || signature.size() != 64) {
        return {};
    }
    return QStringLiteral("CAAC1.") +
           QString::fromLatin1(
               (hash + signature).toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

QString verify(const QString& code, const QByteArray& publicKey, const QByteArray& hash)
{
    if (publicKey.size() != 32) {
        return QCoreApplication::translate(
            "CaacActivationController",
            "The publisher has not configured an activation key. Contact the software provider.");
    }
    if (hash.size() != 16) {
        return QCoreApplication::translate(
            "CaacActivationController", "Unable to read this computer's machine identity. Activation is unavailable.");
    }
    const auto normalized = normalize(code);
    static const QRegularExpression pattern(QStringLiteral("^CAAC1\\.[A-Za-z0-9_-]{107}$"));
    if (!pattern.match(normalized).hasMatch()) {
        return QCoreApplication::translate("CaacActivationController",
                                           "Invalid activation code format. Paste the complete code.");
    }
    const auto data = QByteArray::fromBase64Encoding(
        normalized.mid(6).toLatin1(), QByteArray::Base64UrlEncoding | QByteArray::AbortOnBase64DecodingErrors);
    if (!data || data.decoded.size() != 80 || encode(data.decoded.first(16), data.decoded.last(64)) != normalized) {
        return QCoreApplication::translate("CaacActivationController",
                                           "Invalid activation code format. Paste the complete code.");
    }
    const auto message = signedMessage(data.decoded.first(16));
    if (crypto_ed25519_check(reinterpret_cast<const uint8_t*>(data.decoded.constData() + 16),
                             reinterpret_cast<const uint8_t*>(publicKey.constData()),
                             reinterpret_cast<const uint8_t*>(message.constData()), message.size()) != 0) {
        return QCoreApplication::translate("CaacActivationController",
                                           "The activation code is invalid or has been modified.");
    }
    if (data.decoded.first(16) != hash) {
        return QCoreApplication::translate(
            "CaacActivationController",
            "This activation code belongs to another computer. Send this machine code to the provider.");
    }
    return {};
}
}  // namespace CaacActivationCodec
