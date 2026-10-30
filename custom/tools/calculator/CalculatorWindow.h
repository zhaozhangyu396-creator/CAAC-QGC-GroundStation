#pragma once

#include <QtWidgets/QWidget>

#include "CalculatorEngine.h"

class QTabWidget;

class CalculatorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalculatorWindow(QWidget* parent = nullptr);
    [[nodiscard]] bool isReady() const;

private:
    void _addWindPage();
    void _addTimePage();
    void _addTrigonometryPage();
    void _addTrianglePage();

    CalculatorEngine _engine;
    bool _fontReady = false;
    QTabWidget* _tabs = nullptr;
};
