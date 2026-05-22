#ifndef FANGO_H
#define FANGO_H

#include "terreno.h"

class Fango : public Terreno {
public:
    Fango(float x, float y, float ancho, float alto);

    void actualizar(float velocidadMundo) override;
    float getMultiplicador() const override { return 0.5f; }
    bool esFango() const { return true; }
};

#endif
