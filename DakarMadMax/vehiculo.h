#ifndef VEHICULO_H
#define VEHICULO_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <set>
#include "tiposvehiculo.h"

class Vehiculo : public QGraphicsPixmapItem {
public:
    void setMultiplicador(float m) { multiplicador = m; }
    float getMultiplicador() const { return multiplicador; }

    Vehiculo(float x, float y, TipoVehiculo tipo);

    void actualizar();
    void keyPress(int key);
    void keyRelease(int key);

    TipoVehiculo getTipo() const { return tipoVehiculo; }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    TipoVehiculo tipoVehiculo;  // era: tipo
    float speed;
    float multiplicador = 0.0f;
    float limiteIzq, limiteDer, limiteArr, limiteAba;
    std::set<int> teclasPulsadas;
};

#endif
