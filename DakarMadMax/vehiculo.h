#ifndef VEHICULO_H
#define VEHICULO_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <set>
#include "tiposvehiculo.h"

class Vehiculo : public QGraphicsPixmapItem {
public:
    Vehiculo(float x, float y, TipoVehiculo tipo);

    void actualizar();
    void keyPress(int key);
    void keyRelease(int key);

    TipoVehiculo getTipo() const { return tipo; }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    TipoVehiculo tipo;
    float speed;
    float limiteIzq, limiteDer, limiteArr, limiteAba;  // reemplaza las constantes
    std::set<int> teclasPulsadas;
};

#endif
