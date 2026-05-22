#ifndef TERRENO_H
#define TERRENO_H

#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>

enum class TipoTerreno {
    Fango,
    Carretera
};

class Terreno : public QGraphicsRectItem {
public:
    Terreno(float x, float y, float ancho, float alto, TipoTerreno tipo);

    void desplazar(float velocidad);
    bool fueraDePantalla() const;
    float getMultiplicador() const { return multiplicador; }
    TipoTerreno getTipo() const { return tipo; }

private:
    float multiplicador;
    TipoTerreno tipo;
};

#endif
