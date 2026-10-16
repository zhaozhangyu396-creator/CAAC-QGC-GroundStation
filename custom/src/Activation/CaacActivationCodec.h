#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace CaacActivationCodec {
QByteArray machineHash(const QByteArray& machineIdentity);
QString machineCode(const QByteArray& hash);
QByteArray parseMachineCode(const QString& code);
QByteArray signedMessage(const QByteArray& hash);
QString encode(const QByteArray& hash, const QByteArray& signature);
QString verify(const QString& code, const QByteArray& publicKey, const QByteArray& hash);
QString normalize(const QString& code);
}  // namespace CaacActivationCodec
