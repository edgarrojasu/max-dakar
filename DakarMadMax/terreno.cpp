#include "terreno.h"
#include <QPen>

Terreno::Terreno(float x, float y, float ancho, float alto,
                 QColor color, float multiplicador)
    : QGraphicsRectItem(x, y, ancho, alto),
    multiplicador(multiplicador)
{
    setBrush(QBrush(color));
    setPen(Qt::NoPen);
    setZValue(0);
}

void Terreno::desplazar(float velocidad) {
    setY(y() + velocidad);
}

bool Terreno::fueraDePantalla() const {
    return y() > 620;
}
