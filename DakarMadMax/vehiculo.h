#ifndef VEHICULO_H
#define VEHICULO_H

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QLabel>
#include <set>
#include <cmath>
#include "tiposvehiculo.h"

class Vehiculo : public QGraphicsPixmapItem {
public:
    Vehiculo(float x, float y, TipoVehiculo tipo);

    void  setMultiplicador(float m)  { multiplicador = m; }
    float getMultiplicador()  const  { return multiplicador; }
    float getSpeed()          const  { return speed; }
    TipoVehiculo getTipo()    const  { return tipoVehiculo; }

    void actualizar();
    void setEnFango(bool fango);

protected:
    // ── Accesibles por VehiculoIA ─────────────────────────────────────────
    bool  isEnFango()          const { return enFango; }
    bool  isDerrapando()       const { return derrapando; }
    float getDerrapeTiempo()   const { return derrapeTiempo; }
    float getDerrapeFuerza()   const { return derrapeFuerza; }
    float getDerrapePhi()      const { return derrapePhi; }
    float getDerrapeDirX()     const { return derrapeDirX; }

    void  setDerrapando(bool v)       { derrapando    = v; }
    void  setDerrapeFuerza(float v)   { derrapeFuerza = v; }
    void  setDerrapeTiempo(float v)   { derrapeTiempo = v; }
    void  setDerrapeDirX(float v)     { derrapeDirX   = v; }
    void  addDerrapeTiempo(float v)   { derrapeTiempo += v; if (derrapeTiempo > 1.5f) derrapeTiempo = 1.5f; }
    void  scaleDerrapeTiempo(float f) { derrapeTiempo *= f; }
    void  advanceDerrapePhi(float v)  { derrapePhi    += v; }

    float getLimiteIzq() const { return limiteIzq; }
    float getLimiteDer() const { return limiteDer; }
    float getLimiteArr() const { return limiteArr; }
    float getLimiteAba() const { return limiteAba; }

    void keyPressEvent(QKeyEvent *event)   override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    bool  manejarBloqueoLlantaJugador();
    void  calcularMovimientoEnFango(float &vx, float &vy);
    void  calcularMovimientoNormal(float &vx, float &vy);
    void  aplicarLimites(float &nx, float &ny) const;

    TipoVehiculo  tipoVehiculo;
    float         speed;
    float         multiplicador = 0.0f;
    float         limiteIzq, limiteDer, limiteArr, limiteAba;
    std::set<int> teclasPulsadas;

    bool  enFango       = false;
    bool  derrapando    = false;
    float derrapeDirX   = 0.0f;
    float derrapeFuerza = 0.0f;
    float derrapeTiempo = 0.0f;
    float derrapePhi    = 0.0f;
};

#endif
