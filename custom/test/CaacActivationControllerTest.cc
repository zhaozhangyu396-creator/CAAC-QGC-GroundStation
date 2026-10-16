#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>
#include <QtTest/QSignalSpy>

#include "CaacActivationController.h"
#include "CaacActivationTestSupport.h"
#include "UnitTest.h"

class CaacActivationControllerTest : public UnitTest
{
    Q_OBJECT
private slots:
    void _verification_data();
    void _verification();
    void _persistenceAndBinding();
    void _writeFailure();
    void _machineCode();
    void _rfc8032();
};

void CaacActivationControllerTest::_verification_data()
{
    QTest::addColumn<int>("variation");
    QTest::addColumn<bool>("valid");
    QTest::newRow("valid") << 0 << true;
    QTest::newRow("pasted-whitespace") << 1 << true;
    QTest::newRow("truncated") << 2 << false;
    QTest::newRow("another-machine") << 3 << false;
    QTest::newRow("modified-payload") << 4 << false;
    QTest::newRow("modified-signature") << 5 << false;
    QTest::newRow("another-issuer") << 6 << false;
    QTest::newRow("unconfigured-key") << 7 << false;
    QTest::newRow("missing-machine-identity") << 8 << false;
    QTest::newRow("oversized") << 9 << false;
    QTest::newRow("unknown-version") << 10 << false;
}

void CaacActivationControllerTest::_verification()
{
    QFETCH(int, variation);
    QFETCH(bool, valid);
    QByteArray identity("activation-test-machine-a");
    auto key = CaacActivationTestSupport::publicKey();
    auto code = CaacActivationTestSupport::activationCode(identity);
    switch (variation) {
        case 1:
            code.insert(50, QStringLiteral("\r\n "));
            code.prepend(QStringLiteral(" \t"));
            break;
        case 2:
            code.chop(1);
            break;
        case 3:
            identity = "activation-test-machine-b";
            break;
        case 4:
        case 5: {
            auto data = QByteArray::fromBase64(code.mid(6).toLatin1(), QByteArray::Base64UrlEncoding);
            data[variation == 4 ? 0 : 16] ^= 1;
            code = CaacActivationCodec::encode(data.first(16), data.last(64));
            break;
        }
        case 6:
            key[0] ^= 1;
            break;
        case 7:
            key.clear();
            break;
        case 8:
            identity.clear();
            break;
        case 9:
            code = QString(5000, QLatin1Char('A'));
            break;
        case 10:
            code.replace(4, 1, QStringLiteral("2"));
            break;
        default:
            break;
    }
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const auto path = dir.filePath(QStringLiteral("activation.license"));
    CaacActivationController controller(key, identity, path);
    QVERIFY(!controller.activated());
    QSignalSpy activated(&controller, &CaacActivationController::activatedChanged);
    QCOMPARE(controller.activate(code), valid);
    QCOMPARE(controller.activated(), valid);
    QCOMPARE(activated.count(), valid ? 1 : 0);
    QCOMPARE(QFile::exists(path), valid);
    QCOMPARE(controller.errorMessage().isEmpty(), valid);
}

void CaacActivationControllerTest::_persistenceAndBinding()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const auto path = dir.filePath(QStringLiteral("nested/activation.license"));
    const QByteArray identity("activation-persistence-machine");
    const auto code = CaacActivationTestSupport::activationCode(identity);
    const auto key = CaacActivationTestSupport::publicKey();
    CaacActivationController first(key, identity, path);
    QSignalSpy activated(&first, &CaacActivationController::activatedChanged);
    QVERIFY(first.activate(code));
    QVERIFY(first.activate(code));
    QCOMPARE(activated.count(), 1);
    QVERIFY(!first.activate(QStringLiteral("invalid")));
    QVERIFY(first.activated());
    CaacActivationController restart(key, identity, path);
    QVERIFY(restart.activated());
    QVERIFY(!restart.showWindow(nullptr, nullptr, [] {}));
    CaacActivationController differentMachine(key, QByteArrayLiteral("different-machine"), path);
    QVERIFY(!differentMachine.activated());
    QVERIFY(!differentMachine.errorMessage().isEmpty());
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
    QVERIFY(file.write("activated=true") > 0);
    file.close();
    CaacActivationController modified(key, identity, path);
    QVERIFY(!modified.activated());
}

void CaacActivationControllerTest::_writeFailure()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QByteArray identity("activation-write-test");
    CaacActivationController controller(CaacActivationTestSupport::publicKey(), identity, dir.path());
    QVERIFY(!controller.activate(CaacActivationTestSupport::activationCode(identity)));
    QVERIFY(!controller.activated());
    QVERIFY(!controller.errorMessage().isEmpty());
}

void CaacActivationControllerTest::_machineCode()
{
    const QByteArray identity("raw-system-machine-identity");
    const auto hash = CaacActivationCodec::machineHash(identity);
    const auto code = CaacActivationCodec::machineCode(hash);
    QVERIFY(!code.contains(QString::fromLatin1(identity)));
    QCOMPARE(CaacActivationCodec::parseMachineCode(code), hash);
    QCOMPARE(CaacActivationCodec::parseMachineCode(code.toLower()), hash);
    QVERIFY(CaacActivationCodec::parseMachineCode(QStringLiteral("CAAC-1234")).isEmpty());
    QVERIFY(CaacActivationCodec::machineHash({}).isEmpty());
    QVERIFY(CaacActivationCodec::machineCode({}).isEmpty());
}

void CaacActivationControllerTest::_rfc8032()
{
    const auto signature = QByteArray::fromHex(
        "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e06522490155"
        "5fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b");
    const auto key = CaacActivationTestSupport::publicKey();
    QCOMPARE(signature.size(), 64);
    QCOMPARE(crypto_ed25519_check(reinterpret_cast<const uint8_t*>(signature.constData()),
                                  reinterpret_cast<const uint8_t*>(key.constData()), nullptr, 0),
             0);
}

UT_REGISTER_TEST(CaacActivationControllerTest, TestLabel::Unit, TestLabel::Utilities)

#include "CaacActivationControllerTest.moc"
