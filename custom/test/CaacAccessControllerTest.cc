#include <QtCore/QScopeGuard>
#include <QtCore/QSettings>
#include <QtCore/QUuid>

#include "CaacAccessController.h"
#include "UnitTest.h"

class CaacAccessControllerTest : public UnitTest
{
    Q_OBJECT
private slots:
    void _defaultAndPasswordRotation();
};

void CaacAccessControllerTest::_defaultAndPasswordRotation()
{
    const QString group = QStringLiteral("CAAC/AccessTest/") + QUuid::createUuid().toString();
    const auto clean = qScopeGuard([&]() { QSettings().remove(group); });
    CaacAccessController access(nullptr, group);
    QVERIFY(!access.configured());
    access.ensureDefaultPassword();
    QVERIFY(access.configured());
    QVERIFY(!access.verify(QString()));
    QVERIFY(!access.verify(QStringLiteral("wrong")));
    QVERIFY(access.verify(QStringLiteral("333")));
    const auto before = QSettings().value(group + "/hash").toByteArray();
    QVERIFY(!access.setPassword(QStringLiteral("wrong"), QStringLiteral("new-test-password")));
    QCOMPARE(QSettings().value(group + "/hash").toByteArray(), before);
    QVERIFY(access.setPassword(QStringLiteral("333"), QStringLiteral("new-test-password")));
    QVERIFY(!access.verify(QStringLiteral("333")));
    QVERIFY(access.verify(QStringLiteral("new-test-password")));
    CaacAccessController reloaded(nullptr, group);
    reloaded.ensureDefaultPassword();
    QVERIFY(reloaded.verify(QStringLiteral("new-test-password")));
    QVERIFY(QSettings().value(group + "/hash").toByteArray() != QByteArray("new-test-password"));
}

UT_REGISTER_TEST(CaacAccessControllerTest, TestLabel::Unit)
#include "CaacAccessControllerTest.moc"
