#include "VehicleMessageTranslator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QHash>
#include <QtCore/QLocale>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QTranslator>

#include "OnlineMessageTranslator.h"
#include "VehicleMessageCatalog.h"

namespace {

constexpr int MAX_TRANSLATION_DEPTH = 8;

bool parameterCatalogEnabled = false;

const QList<QTranslator*>& parameterTranslators()
{
    static const QList<QTranslator*> translators = [] {
        QList<QTranslator*> result;
        for (int i = 0; i < 8; ++i) {
            result.append(new QTranslator(QCoreApplication::instance()));
        }
        return result;
    }();
    return translators;
}

QString parameterTranslation(const QString& text)
{
    if (!parameterCatalogEnabled) {
        return {};
    }
    const QByteArray source = text.toUtf8();
    for (const auto* translator : parameterTranslators()) {
        const QString translated = translator->translate("VehicleParameters", source.constData());
        if (!translated.isEmpty()) {
            return translated;
        }
    }
    return {};
}

struct CompiledRule
{
    QRegularExpression expression;
    const char* source;
};

const QHash<QString, const char*>& exactSources()
{
    static const QHash<QString, const char*> sources = [] {
        QHash<QString, const char*> result;
        for (const char* source : VehicleMessageCatalog::messages) {
            result.insert(QString::fromUtf8(source).simplified().toCaseFolded(), source);
        }
        return result;
    }();
    return sources;
}

const QList<CompiledRule>& rules()
{
    static const QList<CompiledRule> compiled = [] {
        QList<CompiledRule> result;
        for (const auto& rule : VehicleMessageCatalog::rules) {
            result.append(
                {QRegularExpression(QString::fromUtf8(rule.expression), QRegularExpression::CaseInsensitiveOption),
                 rule.source});
        }
        return result;
    }();
    return compiled;
}

QString translateText(const QString& text, int depth);

QString substitute(const QString& format, const QStringList& arguments)
{
    // Substitute only template placeholders: an argument may itself contain "%1" or a parameter link.
    static const QRegularExpression placeholder(QStringLiteral("%([1-9][0-9]?)"));
    QString result;
    qsizetype offset = 0;
    auto matches = placeholder.globalMatch(format);
    while (matches.hasNext()) {
        const auto match = matches.next();
        result += QStringView(format).mid(offset, match.capturedStart() - offset);
        const int index = match.captured(1).toInt() - 1;
        result += index < arguments.size() ? arguments[index] : match.captured();
        offset = match.capturedEnd();
    }
    result += QStringView(format).mid(offset);
    return result;
}

QString translateBody(const QString& text, int depth)
{
    if (text.startsWith(QLatin1String("- "))) {
        return text.left(2) + translateText(text.mid(2), depth + 1);
    }
    const auto source = exactSources().constFind(text.simplified().toCaseFolded());
    if (source != exactSources().cend()) {
        const QString translated = QCoreApplication::translate("VehicleMessages", source.value());
        if (translated != QString::fromUtf8(source.value())) {
            return translated;
        }
    }

    const QString translatedParameter = parameterTranslation(text);
    if (!translatedParameter.isEmpty()) {
        return translatedParameter;
    }

    for (const auto& rule : rules()) {
        const auto match = rule.expression.match(text);
        if (!match.hasMatch()) {
            continue;
        }
        const QString translated = QCoreApplication::translate("VehicleMessages", rule.source);
        if (translated == QString::fromUtf8(rule.source)) {
            continue;
        }
        QStringList arguments;
        for (int index = 1; index <= match.lastCapturedIndex(); ++index) {
            const QString argument = match.captured(index);
            arguments.append(argument.size() < text.size() ? translateText(argument, depth + 1) : argument);
        }
        return substitute(translated, arguments);
    }

    // Translate visible runs only. Never rewrite href/src, parameter IDs, or other HTML attributes.
    static const QRegularExpression tag(QStringLiteral(R"(<(?:[^<>"']|"[^"]*"|'[^']*')+>)"));
    auto tags = tag.globalMatch(text);
    if (!tags.hasNext()) {
        return text;
    }
    QString result;
    qsizetype offset = 0;
    while (tags.hasNext()) {
        const auto match = tags.next();
        result += translateText(text.mid(offset, match.capturedStart() - offset), depth + 1);
        result += match.captured();
        offset = match.capturedEnd();
    }
    result += translateText(text.mid(offset), depth + 1);
    return result;
}

QString translateText(const QString& text, int depth)
{
    if (text.isEmpty() || depth > MAX_TRANSLATION_DEPTH) {
        return text;
    }
    const QString translatedParameter = parameterTranslation(text);
    if (!translatedParameter.isEmpty()) {
        return translatedParameter;
    }
    if (text.contains('\n')) {
        QStringList lines = text.split('\n');
        for (QString& line : lines) {
            line = translateText(line, depth + 1);
        }
        return lines.join('\n');
    }
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        return text;
    }
    const qsizetype start = text.indexOf(trimmed);
    return text.left(start) + translateBody(trimmed, depth) + text.mid(start + trimmed.size());
}

}  // namespace

QString VehicleMessageTranslator::translate(const QString& text)
{
    const QString local = translateText(text, 0);
    const QString trimmed = text.trimmed();
    // Complete local entries can intentionally retain brands, protocol names, or units.
    // Do not spend public-service quota translating those names a second time.
    if (!parameterTranslation(text).isEmpty() || (trimmed != text && !parameterTranslation(trimmed).isEmpty()) ||
        (local != text && exactSources().contains(trimmed.simplified().toCaseFolded()))) {
        return local;
    }
    return OnlineMessageTranslator::instance()->translateUnknown(local);
}

QString VehicleMessageTranslator::translateLocal(const QString& text)
{
    return translateText(text, 0);
}

void VehicleMessageTranslator::setLanguage(const QLocale& locale)
{
    const auto& translators = parameterTranslators();
    parameterCatalogEnabled = locale.language() == QLocale::Chinese;
    for (int i = 0; i < translators.size(); ++i) {
        auto* translator = translators[i];
        QCoreApplication::removeTranslator(translator);
        if (locale.language() == QLocale::Chinese &&
            translator->load(QStringLiteral("qgc_parameters_%1_zh_CN").arg(i), QStringLiteral(":/i18n"))) {
            QCoreApplication::installTranslator(translator);
        }
    }
}
