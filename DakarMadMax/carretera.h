#ifndef CARRETERA_H
#define CARRETERA_H

#include "terreno.h"

class Carretera : public Terreno {
public:
    Carretera(float x, float y, float ancho, float alto);

    void actualizar(float velocidadMundo) override;
    float getMultiplicador() const override { return 1.3f; }
};

#endif
