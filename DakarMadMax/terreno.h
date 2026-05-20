#ifndef TERRENO_H
#define TERRENO_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>

class Terreno : public QGraphicsRectItem {
public:
    Terreno(float x, float y, float ancho, float alto, QColor color, float multiplicador);

    void desplazar(float velocidad);
    bool fueraDePantalla() const;
    float getMultiplicador() const { return multiplicador; }

private:
    float multiplicador;
};

#endif
