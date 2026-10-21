#pragma once

#include <QtCore/QTranslator>

#include "UnitTest.h"

class VehicleMessageTranslatorTest : public UnitTest
{
    Q_OBJECT

private slots:
    void init() override;
    void cleanup() override;
    void _translations_data();
    void _translations();
    void _rawMessages_data();
    void _rawMessages();
    void _catalogIntegrity();
    void _parameterCatalogIntegrity();
    void _languageSwitch();
    void _healthProblemAndDialog();

private:
    QTranslator _translator;
};
