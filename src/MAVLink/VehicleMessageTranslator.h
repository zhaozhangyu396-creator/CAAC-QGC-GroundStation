#pragma once

#include <QtCore/QString>

class QLocale;

/// Localizes vehicle-facing text without changing the received protocol message.
/// Unrecognized text, parameter identifiers, values, and HTML attributes are retained.
namespace VehicleMessageTranslator {
QString translate(const QString& text);
QString translateLocal(const QString& text);
void setLanguage(const QLocale& locale);
}  // namespace VehicleMessageTranslator
