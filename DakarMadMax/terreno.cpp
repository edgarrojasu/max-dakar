#include "terreno.h"
#include "terreno.h"
#include <QBrush>
#include <QPen>

Terreno::Terreno(float x, float y, float ancho, float alto, TipoTerreno tipo)
    : QGraphicsRectItem(x, y, ancho, alto), tipo(tipo)
{
    setPen(Qt::NoPen);
    setZValue(0);

    switch (tipo) {
    case TipoTerreno::Fango:
        setBrush(QBrush(QColor(101, 67, 33)));   // marrón oscuro
        multiplicador = -1.5f;
        break;
    case TipoTerreno::Carretera:
        setBrush(QBrush(QColor(60, 60, 60)));    // gris oscuro
        multiplicador = 1.3f;
        break;
    }
}

void Terreno::desplazar(float velocidad) {
    setY(y() + velocidad);
}

bool Terreno::fueraDePantalla() const {
    return y() > 620;
}
