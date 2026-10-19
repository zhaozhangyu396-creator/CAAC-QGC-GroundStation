#include "CompassRotationTest.h"

#include <QtCore/QFile>
#include <QtQml/QJSEngine>

namespace {

bool loadRotationLibrary(QJSEngine& engine)
{
    QFile source(QStringLiteral(":/qml/QGroundControl/Controls/CompassRotation.js"));
    if (!source.open(QIODevice::ReadOnly)) {
        return false;
    }
    QString script = QString::fromUtf8(source.readAll());
    script.remove(QStringLiteral(".pragma library"));
    return !engine.evaluate(script).isError();
}

}  // namespace

void CompassRotationTest::_sixFaces_data()
{
    QTest::addColumn<int>("side");
    QTest::addColumn<double>("roll");
    QTest::addColumn<double>("pitch");
    QTest::addColumn<int>("direction");
    for (int direction : {-1, 1}) {
        const QByteArray suffix = direction > 0 ? "clockwise" : "counterclockwise";
        QTest::newRow(("level-" + suffix).constData()) << 0 << 0.0 << 0.0 << direction;
        QTest::newRow(("left-" + suffix).constData()) << 1 << -90.0 << 0.0 << direction;
        QTest::newRow(("right-" + suffix).constData()) << 2 << 90.0 << 0.0 << direction;
        QTest::newRow(("nose-down-" + suffix).constData()) << 3 << 0.0 << -90.0 << direction;
        QTest::newRow(("nose-up-" + suffix).constData()) << 4 << 0.0 << 90.0 << direction;
        QTest::newRow(("inverted-" + suffix).constData()) << 5 << 180.0 << 0.0 << direction;
    }
}

void CompassRotationTest::_sixFaces()
{
    QFETCH(int, side);
    QFETCH(double, roll);
    QFETCH(double, pitch);
    QFETCH(int, direction);
    QJSEngine engine;
    QVERIFY(loadRotationLibrary(engine));
    QJSValue tracker = engine.globalObject().property("createTracker").call();
    QJSValue sample = engine.globalObject().property("sample");
    for (int step = 0; step <= 12; ++step) {
        QVERIFY(!sample.call({tracker, roll, pitch, step * 30 * direction, step * 100}).isError());
        QCOMPARE(tracker.property("side").toInt(), side);
        const auto progress = tracker.property("progress");
        QVERIFY(qAbs(progress.property(side).toNumber() - step / 12.0) < 0.00001);
        for (int other = 0; other < 6; ++other) {
            if (other != side) {
                QCOMPARE(progress.property(other).toNumber(), 0.0);
            }
        }
    }
    QCOMPARE(tracker.property("progress").property(side).toNumber(), 1.0);
}

void CompassRotationTest::_invalidMotion()
{
    QJSEngine engine;
    QVERIFY(loadRotationLibrary(engine));
    const auto result = engine.evaluate(QStringLiteral(R"(
        const tracker = createTracker();
        let now = 0;
        for (let repeat = 0; repeat < 20; ++repeat) {
            for (const yaw of [0, 30, 0, -30, 0]) {
                sample(tracker, 0, 0, yaw, now += 100);
            }
        }
        const shakeProgress = tracker.progress[0];
        // A stale sample and an orientation jump cannot bridge separate turns.
        sample(tracker, 0, 0, 60, now += 2000);
        sample(tracker, 0, 0, 180, now += 100);
        const staleProgress = tracker.progress[0];
        sample(tracker, 45, 0, 0, now += 100);
        const tiltedSide = tracker.side;
        sample(tracker, NaN, 0, 0, now += 100);
        const invalidSide = tracker.side;
        interrupt(tracker);
        sample(tracker, 0, 0, 210, now += 100);
        [shakeProgress, staleProgress, tiltedSide, invalidSide, tracker.progress[0]];
    )"));
    QVERIFY2(!result.isError(), qPrintable(result.toString()));
    QVERIFY(result.property(0).toNumber() < 0.17);
    QCOMPARE(result.property(1).toNumber(), result.property(0).toNumber());
    QCOMPARE(result.property(2).toInt(), -1);
    QCOMPARE(result.property(3).toInt(), -1);
    QCOMPARE(result.property(4).toNumber(), result.property(0).toNumber());
}

UT_REGISTER_TEST(CompassRotationTest, TestLabel::Unit)
