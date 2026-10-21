#pragma once

#include "UnitTest.h"

class OnlineMessageTranslatorTest : public UnitTest
{
    Q_OBJECT
private slots:
    void _roundTripAndCache();
    void _adjacentTokens();
    void _invalidResponse_data();
    void _invalidResponse();
    void _disabledAndBounded();
    void _timeout();
    void _longDescription();
    void _parameterMetadata();
    void _parameterSelectionSurvivesTranslation();
    void _refreshExistingMessages();
};
