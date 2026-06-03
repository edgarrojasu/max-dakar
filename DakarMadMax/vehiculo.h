#ifndef VEHICULO_H
#define VEHICULO_H

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <set>
#include <cmath>
#include "tiposvehiculo.h"
#include <QLabel>

class Vehiculo : public QGraphicsPixmapItem {
public:
    void setMultiplicador(float m) { multiplicador = m; }
    float getMultiplicador() const { return multiplicador; }
    Vehiculo(float x, float y, TipoVehiculo tipo);
    void actualizar();
    void keyPress(int key);
    void keyRelease(int key);
    TipoVehiculo getTipo() const { return tipoVehiculo; }
    void setEnFango(bool fango);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    TipoVehiculo tipoVehiculo;  // era: tipo
    float speed;
    float multiplicador = 0.0f;
    float limiteIzq, limiteDer, limiteArr, limiteAba;
    std::set<int> teclasPulsadas;
    // Parábola de derrape
    bool enFango = false;
    bool derrapando = false;
    float derrapeDirX = 0.0f;    // dirección del derrape (-1 izq, +1 der)
    float derrapeFuerza = 0.0f;  // qué tan fuerte es el derrape
    float derrapeTiempo = 0.0f;  // tiempo acumulado presionando la tecla en fango
    float derrapePhi = 0.0f;     // fase actual de la parábola
    Vehiculo       *rival;
    QGraphicsRectItem *lineaMeta;
    QLabel         *labelTiempo;
    int            tiempoRestante;   // en frames (2min * 60fps * 60seg = 7200)
    bool           juegoTerminado;
};

#endif
