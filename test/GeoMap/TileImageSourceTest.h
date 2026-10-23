#pragma once

#include "UnitTest.h"

class TileImageSourceTest : public UnitTest
{
    Q_OBJECT

private slots:
    void _tileDelivered();
    void _cancelledRequestSilent();
    void _multipleRequestsIndependent();
    void _unknownProviderFails();
    void _networkFallbackDeliversAndCaches();
    void _networkErrorFails();
    void _networkEmptyBodyFails();
    void _networkGarbageBodyFailsAndNotCached();
    void _bingPlaceholderNotDelivered();
    void _cachedUnusableTileFallsBackToNetwork();
    void _cancelAbortsNetworkFetch();
    void _gaodeCorrectionDeliveredAndCached_data();
    void _gaodeCorrectionDeliveredAndCached();
    void _gaodeNonChinaPreservesBytes();
    void _gaodeFailedSourceNotCached_data();
    void _gaodeFailedSourceNotCached();
    void _gaodeCancelAbortsAllSources();
    void _gaodeCancelDuringPlanning();
};
