#ifndef VEHICULO_H
#define VEHICULO_H

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <set>
#include <cmath>
#include "tiposvehiculo.h"
#include <QLabel>
#include <QGraphicsScene>

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
    void actualizarIA(const std::vector<class Terreno*> &terrenos);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    TipoVehiculo tipoVehiculo;
    float speed;
    float multiplicador = 0.0f;
    float limiteIzq, limiteDer, limiteArr, limiteAba;
    std::set<int> teclasPulsadas;

    // Derrape en fango
    bool  enFango       = false;
    bool  derrapando    = false;
    float derrapeDirX   = 0.0f;
    float derrapeFuerza = 0.0f;
    float derrapeTiempo = 0.0f;
    float derrapePhi    = 0.0f;

    // FIX: contador de bloqueo por llanta para la IA (miembro, no estático local)
    int framesBloqueadoIA = 0;

    // Punteros heredados del diseño original (no usados directamente aquí)
    Vehiculo          *rival         = nullptr;
    QGraphicsRectItem *lineaMeta     = nullptr;
    QLabel            *labelTiempo   = nullptr;
    int                tiempoRestante = 0;
    bool               juegoTerminado = false;
};

#endif
