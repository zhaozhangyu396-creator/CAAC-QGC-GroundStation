#include "FlightMapSettings.h"

#include "QGCMapEngineManager.h"

DECLARE_SETTINGGROUP(FlightMap, "FlightMap")
{
    auto* providerFact = mapProvider();
    auto* typeFact = mapType();
    const auto normalize = [providerFact, typeFact]() {
        const auto providers = QGCMapEngineManager::mapProviderList();
        if (providers.isEmpty()) {
            return;
        }
        if (!providers.contains(providerFact->rawValue().toString())) {
            providerFact->setRawValue(providers.contains(QStringLiteral("Bing")) ? QStringLiteral("Bing")
                                                                                 : providers.first());
        }
        const auto types = QGCMapEngineManager::mapTypeList(providerFact->rawValue().toString());
        if (!types.isEmpty() && !types.contains(typeFact->rawValue().toString())) {
            typeFact->setRawValue(types.contains(QStringLiteral("Hybrid")) ? QStringLiteral("Hybrid") : types.first());
        }
    };
    normalize();
    connect(providerFact, &Fact::rawValueChanged, this, normalize);
    connect(typeFact, &Fact::rawValueChanged, this, normalize);
}

DECLARE_SETTINGSFACT(FlightMapSettings, mapProvider)
DECLARE_SETTINGSFACT(FlightMapSettings, mapType)
DECLARE_SETTINGSFACT(FlightMapSettings, elevationMapProvider)
