#ifndef TERRENO_H
#define TERRENO_H

#include <QGraphicsPixmapItem>

class Terreno : public QGraphicsPixmapItem {
public:
    explicit Terreno(float x, float y);

    virtual void actualizar(float velocidadMundo) = 0;  // cada subclase se mueve distinto
    virtual float getMultiplicador() const = 0;
    virtual bool fueraDePantalla() const;

protected:
    float velocidadY;
};

#endif
