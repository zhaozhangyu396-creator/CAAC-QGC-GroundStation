#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QScopeGuard>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "ActuatorComponent.h"
#include "Actuators/Actuators.h"
#include "Fact.h"
#include "MockLink.h"
#include "ParameterManager.h"
#include "Vehicle.h"
#include "VehicleConfigUITestBase.h"

#ifdef QGC_CUSTOM_BUILD
class CaacActuatorLayoutUITest : public VehicleConfigUITestBase
{
    Q_OBJECT
private slots:
    void _narrowPanels();
};

UT_REGISTER_TEST(CaacActuatorLayoutUITest, TestLabel::Integration)

void CaacActuatorLayoutUITest::_narrowPanels()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink>, Vehicle* vehicle) {
            QFile fixture(QStringLiteral(":/unittest/actuators.layout.json"));
            QVERIFY(fixture.open(QIODevice::ReadOnly));
            QJsonParseError error;
            const auto metadata = QJsonDocument::fromJson(fixture.readAll(), &error);
            QCOMPARE(error.error, QJsonParseError::NoError);
            vehicle->setActuatorsMetadata(vehicle->defaultComponentId(), fixture.fileName(), metadata);
            auto* actuators = vehicle->actuators();
            QVERIFY(actuators);
            actuators->init();
            QVERIFY2(actuators->isInitialized(), qPrintable(actuators->initializationError()));
            auto* rtl = vehicle->parameterManager()->getParameter(-1, QStringLiteral("RTL_TYPE"));
            QVERIFY(rtl);
            rtl->setRawValue(2);
            QSignalSpy rtlWrites(rtl, &Fact::containerRawValueChanged);
            auto* motor = vehicle->parameterManager()->getParameter(-1, QStringLiteral("CA_ROTOR0_PX"));
            QSignalSpy motorWrites(motor, &Fact::containerRawValueChanged);
            auto* output = vehicle->parameterManager()->getParameter(-1, QStringLiteral("PWM_MAIN_FUNC1"));
            QSignalSpy outputWrites(output, &Fact::containerRawValueChanged);

            navigateToConfigureView();
            if (QTest::currentTestFailed()) {
                return;
            }
            QVERIFY(clickButton(QStringLiteral("vehicleConfig_summary")));
            auto* loader = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_panelLoader"));
            QVERIFY(loader);
            auto* component = new ActuatorComponent(vehicle, vehicle->autopilotPlugin(), vehicle);
            QVERIFY(QMetaObject::invokeMethod(
                loader, "setSource", Q_ARG(QVariant, QVariant(component->setupSource().toString())),
                Q_ARG(QVariant, QVariant::fromValue(static_cast<VehicleComponent*>(component)))));
            auto* layout = findVisibleItem(_rootItem, QStringLiteral("actuatorSetup_layout"));
            QVERIFY(layout);
            auto* viewport = findVisibleItem(_rootItem, QStringLiteral("vehicleSetup_scroller"));
            QVERIFY(viewport);
            const auto originalSize = _window->size();
            const auto restore = qScopeGuard([this, originalSize] { _window->resize(originalSize); });
            const auto fits = [](QQuickItem* item, QQuickItem* container) {
                const auto bounds = item->mapRectToItem(container, item->boundingRect());
                return item->width() > 0 && bounds.left() >= -1 && bounds.right() <= container->width() + 1;
            };
            for (int width : {1000, 1400}) {
                _window->resize(width, qMax(720, _window->minimumHeight()));
                QTRY_COMPARE_WITH_TIMEOUT(_window->width(), width, TestTimeout::shortMs());
                for (int outputIndex : {0, 1}) {
                    actuators->selectActuatorOutput(outputIndex);
                    QTRY_COMPARE_WITH_TIMEOUT(static_cast<QObject*>(actuators->selectedActuatorOutput()),
                                              actuators->actuatorOutputs()->get(outputIndex), TestTimeout::shortMs());
                    QTRY_VERIFY_WITH_TIMEOUT(fits(layout, viewport), TestTimeout::shortMs());
                    for (auto* item : layout->findChildren<QQuickItem*>()) {
                        if (!item->isVisible()) {
                            continue;
                        }
                        if (item->objectName().endsWith(QStringLiteral("_fact"))) {
                            QTRY_VERIFY2_WITH_TIMEOUT(fits(item, viewport), qPrintable(item->objectName()),
                                                      TestTimeout::shortMs());
                        }
                    }
                    auto* geometry = findItem(layout, QStringLiteral("actuatorSetup_geometryGrid_0"));
                    QVERIFY(geometry);
                    const auto lastChannel =
                        QStringLiteral("actuatorSetup_outputGrid_") +
                        (outputIndex == 0 ? QStringLiteral("0_channel_3") : QStringLiteral("1_channel_7"));
                    QVERIFY(findVisibleItemScrolled(lastChannel, QStringLiteral("vehicleSetup_scroller")));
                    QVERIFY(findVisibleItemScrolled(QStringLiteral("actuatorSetup_geometryGrid_0_cell_3_2_fact"),
                                                    QStringLiteral("vehicleSetup_scroller")));
                }
            }
            QCOMPARE(motorWrites.count(), 0);
            QCOMPARE(outputWrites.count(), 0);
            QCOMPARE(rtlWrites.count(), 0);

            _window->resize(1000, qMax(720, _window->minimumHeight()));
            QVERIFY(clickButtonScrolled(QStringLiteral("toolbar_safety"),
                                        QStringLiteral("caacSettingsNavigationScroller")));
            auto* choice = findVisibleItemScrolled(QStringLiteral("caacSafety_RTL_TYPE_choice"),
                                                   QStringLiteral("caacSafetyScroller"));
            QVERIFY(choice);
            auto* text = choice->property("contentItem").value<QQuickItem*>();
            QVERIFY(text);
            QTRY_VERIFY_WITH_TIMEOUT(!text->property("text").toString().isEmpty() &&
                                         !text->property("text").toString().contains(QStringLiteral("Return to")),
                                     TestTimeout::shortMs());
            choice->setProperty("textTransform", QVariant());
            QTRY_VERIFY_WITH_TIMEOUT(text->property("contentHeight").toReal() <= text->height() + 1,
                                     TestTimeout::shortMs());
            auto* nextRow = findItem(_rootItem, QStringLiteral("caacSafety_RTL_LOIT_TIME"));
            QVERIFY(nextRow);
            QTRY_VERIFY_WITH_TIMEOUT(choice->mapRectToScene(choice->boundingRect()).bottom() <=
                                         nextRow->mapRectToScene(nextRow->boundingRect()).top(),
                                     TestTimeout::shortMs());
            QCOMPARE(rtlWrites.count(), 0);
        });
}

#include "CaacActuatorLayoutUITest.moc"
#endif
