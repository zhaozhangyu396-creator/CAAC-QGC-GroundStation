#pragma once

#include "UnitTest.h"

class CompassRotationTest : public UnitTest
{
    Q_OBJECT

private slots:
    void _sixFaces_data();
    void _sixFaces();
    void _invalidMotion();
};
