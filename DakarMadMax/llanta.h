#ifndef LLANTA_H
#define LLANTA_H

#include "terreno.h"
#include <QVector>
#include <cmath>

class Llanta : public Terreno {
public:
    Llanta(float x, float velocidadBajada);

    void actualizar(float velocidadMundo) override;
    float getMultiplicador() const override { return -1.0f; }

private:
    float phi;
    float amplitud;
    float frecuencia;
    float xBase;

    QVector<QPixmap> frames;
    int frameActual;
    int contadorFrame;
    int velocidadAnimacion;
};

#endif
