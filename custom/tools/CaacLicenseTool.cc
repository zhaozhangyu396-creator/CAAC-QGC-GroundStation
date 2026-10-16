#include <array>

#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRandomGenerator>
#include <QtCore/QTextStream>

#include "CaacActivationCodec.h"
#include "monocypher-ed25519.h"

namespace {
bool writeNewFile(const QString& path, const QByteArray& data, bool secret = false)
{
    QFile file(path);
    const auto permissions =
        secret ? QFileDevice::ReadOwner | QFileDevice::WriteOwner
               : QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther;
    if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly, permissions)) {
        return false;
    }
    if (file.write(data) != data.size() || !file.flush()) {
        file.remove();
        return false;
    }
    return true;
}
}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("CaacLicenseTool"));
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Offline permanent activation issuer. Never distribute the private key."));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("command"), QStringLiteral("keygen or issue"));
    parser.addOption(
        {QStringLiteral("private-key"), QStringLiteral("Private key file (issuer only)."), QStringLiteral("file")});
    parser.addOption(
        {QStringLiteral("public-key"), QStringLiteral("Public key output for keygen."), QStringLiteral("file")});
    parser.addOption({QStringLiteral("machine"), QStringLiteral("Machine code displayed by the ground station."),
                      QStringLiteral("code")});
    parser.addOption({QStringLiteral("output"), QStringLiteral("Activation code output file; must not exist."),
                      QStringLiteral("file")});
    parser.process(app);
    const auto arguments = parser.positionalArguments();
    const auto privatePath = parser.value(QStringLiteral("private-key"));
    const auto fail = [](const QString& message) {
        QTextStream(stderr) << message << Qt::endl;
        return 1;
    };
    if (arguments.size() != 1 || privatePath.isEmpty()) {
        parser.showHelp(1);
    }
    if (arguments.first() == QStringLiteral("keygen")) {
        const auto publicPath = parser.value(QStringLiteral("public-key"));
        if (publicPath.isEmpty() || QFileInfo::exists(privatePath) || QFileInfo::exists(publicPath) ||
            QFileInfo(privatePath).absoluteFilePath() == QFileInfo(publicPath).absoluteFilePath()) {
            return fail(QStringLiteral("Choose two new, different files for the private and public keys."));
        }
        std::array<quint32, 8> seed{};
        QRandomGenerator::system()->fillRange(seed.data(), seed.size());
        std::array<uint8_t, 64> secret{};
        std::array<uint8_t, 32> publicKey{};
        crypto_ed25519_key_pair(secret.data(), publicKey.data(), reinterpret_cast<uint8_t*>(seed.data()));
        auto secretHex = QByteArray(reinterpret_cast<const char*>(secret.data()), secret.size()).toHex();
        const auto publicHex = QByteArray(reinterpret_cast<const char*>(publicKey.data()), publicKey.size()).toHex();
        const bool privateSaved = writeNewFile(privatePath, secretHex + '\n', true);
        crypto_wipe(secret.data(), secret.size());
        crypto_wipe(secretHex.data(), secretHex.size());
        if (!privateSaved) {
            return fail(QStringLiteral("Unable to create the private key file."));
        }
        if (!writeNewFile(publicPath, publicHex + '\n')) {
            QFile::remove(privatePath);
            return fail(QStringLiteral("Unable to create the public key file."));
        }
        QTextStream(stdout) << "Public key: " << publicHex << Qt::endl
                            << "Keep the private key offline. Back it up separately from the source and installers."
                            << Qt::endl;
        return 0;
    }
    if (arguments.first() != QStringLiteral("issue")) {
        return fail(QStringLiteral("Unknown command. Use keygen or issue."));
    }
    const auto hash = CaacActivationCodec::parseMachineCode(parser.value(QStringLiteral("machine")));
    if (hash.size() != 16) {
        return fail(QStringLiteral("Invalid machine code."));
    }
    QFile file(privatePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return fail(QStringLiteral("Unable to open the private key."));
    }
    auto hex = file.read(257).trimmed();
    auto secret = QByteArray::fromHex(hex);
    crypto_wipe(hex.data(), hex.size());
    if (secret.size() != 64) {
        crypto_wipe(secret.data(), secret.size());
        return fail(QStringLiteral("Invalid private key file."));
    }
    const auto message = CaacActivationCodec::signedMessage(hash);
    std::array<uint8_t, 64> signature{};
    crypto_ed25519_sign(signature.data(), reinterpret_cast<const uint8_t*>(secret.constData()),
                        reinterpret_cast<const uint8_t*>(message.constData()), message.size());
    const auto publicKey = secret.last(32);
    crypto_wipe(secret.data(), secret.size());
    const auto code = CaacActivationCodec::encode(
        hash, QByteArray(reinterpret_cast<const char*>(signature.data()), signature.size()));
    if (!CaacActivationCodec::verify(code, publicKey, hash).isEmpty()) {
        return fail(QStringLiteral("Invalid private key. The embedded public key does not match."));
    }
    const auto output = parser.value(QStringLiteral("output"));
    if (!output.isEmpty() && !writeNewFile(output, code.toUtf8() + '\n')) {
        return fail(QStringLiteral("Unable to create the activation code file."));
    }
    QTextStream(stdout) << code << Qt::endl;
    return 0;
}
