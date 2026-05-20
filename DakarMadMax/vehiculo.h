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

    TipoVehiculo getTipo() const { return tipo; }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    float multiplicador = 1.0f;
    TipoVehiculo tipo;
    float speed;
    float limiteIzq, limiteDer, limiteArr, limiteAba;  // reemplaza las constantes
    std::set<int> teclasPulsadas;
};

#endif
