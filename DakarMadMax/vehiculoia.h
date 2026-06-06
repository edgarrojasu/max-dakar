#ifndef VEHICULOIA_H
#define VEHICULOIA_H

#include "vehiculo.h"
#include "terreno.h"
#include <vector>

// Información que recoge el escáner de terrenos cada frame
struct InfoTerrenoIA {
    bool  sobreFango       = false;
    bool  sobreCarretera   = false;
    bool  sobreLlanta      = false;
    float multCarretera    = 0.0f;
    bool  peligroAdelante  = false;
    bool  carreteraAdelante= false;
    float xPeligro         = -1.0f;
    float xCarretera       = -1.0f;
};

class VehiculoIA : public Vehiculo {
public:
    VehiculoIA(float x, float y, TipoVehiculo tipo);

    // Llamar cada frame desde el game loop en lugar de actualizar()
    void actualizarIA(const std::vector<Terreno*> &terrenos);

private:
    // ── Estado propio de la IA ────────────────────────────────────────────
    int   framesBloqueado = 0;   // contador de bloqueo por llanta

    // ── Pasos del update ──────────────────────────────────────────────────
    bool          manejarBloqueoLlanta();
    InfoTerrenoIA escanearTerrenos(const std::vector<Terreno*> &terrenos) const;
    void          aplicarMultiplicador(const InfoTerrenoIA &info);
    float         calcularVx(const InfoTerrenoIA &info, float centroX) const;
    float         calcularVy(const InfoTerrenoIA &info, float &vx);
    void          moverConLimites(float vx, float vy);
};

#endif
