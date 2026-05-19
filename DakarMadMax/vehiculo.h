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
    std::set<int> teclasPulsadas;

    static constexpr float ANCHO     = 40.0f;
    static constexpr float ALTO      = 60.0f;
    static constexpr float LIMITE_IZQ = 20.0f;
    static constexpr float LIMITE_DER = 540.0f;
    static constexpr float LIMITE_ARR = 100.0f;
    static constexpr float LIMITE_ABA = 720.0f;
};

#endif
