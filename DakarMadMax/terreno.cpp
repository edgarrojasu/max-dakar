#include "terreno.h"

Terreno::Terreno(float x, float y)
    : velocidadY(0.0f)
{
    setPos(x, y);
    setZValue(0);
}

bool Terreno::fueraDePantalla() const {
    return y() > 800;
}
