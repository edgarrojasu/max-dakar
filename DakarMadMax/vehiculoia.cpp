#include "vehiculoia.h"
#include "fango.h"
#include "llanta.h"
#include "carretera.h"
#include <cmath>

VehiculoIA::VehiculoIA(float x, float y, TipoVehiculo tipo)
    : Vehiculo(x, y, tipo) {}

bool VehiculoIA::manejarBloqueoLlanta()
{
    if (getMultiplicador() >= 0) return false;

    framesBloqueado++;

    float ny = y() + 4.0f;
    if (ny > getLimiteAba()) ny = getLimiteAba();
    setY(ny);

    if (framesBloqueado >= 60)
    {
        framesBloqueado = 0;
        setMultiplicador(0.0f);
    }
    return true;
}

InfoTerrenoIA VehiculoIA::escanearTerrenos(const std::vector<Terreno*> &terrenos) const
{
    InfoTerrenoIA info;
    QRectF miRect = mapToScene(boundingRect()).boundingRect();
    QRectF vision(miRect.left() - 30, miRect.top() - 200.0f,
                  miRect.width() + 60, 200.0f);

    for (Terreno *t : terrenos)
    {
        QRectF rT         = t->mapToScene(t->boundingRect()).boundingRect();
        bool toca         = miRect.intersects(rT);
        bool visible      = vision.intersects(rT);
        bool esFangoT     = dynamic_cast<Fango*>(t)     != nullptr;
        bool esLlantaT    = dynamic_cast<Llanta*>(t)    != nullptr;
        bool esCarreteraT = dynamic_cast<Carretera*>(t) != nullptr;

        if (toca)
        {
            if (esFangoT)     info.sobreFango     = true;
            if (esLlantaT)    info.sobreLlanta    = true;
            if (esCarreteraT) { info.sobreCarretera = true;
                                info.multCarretera  = t->getMultiplicador(); }
        }
        if (visible && (esFangoT || esLlantaT) && !info.peligroAdelante)
        {
            info.peligroAdelante = true;
            info.xPeligro        = rT.center().x();
        }
        if (visible && esCarreteraT && !info.carreteraAdelante)
        {
            info.carreteraAdelante = true;
            info.xCarretera        = rT.center().x();
        }
    }
    return info;
}

void VehiculoIA::aplicarMultiplicador(const InfoTerrenoIA &info)
{
    if      (info.sobreLlanta)    setMultiplicador(-1.0f);
    else if (info.sobreFango)     setMultiplicador( 0.5f);
    else if (info.sobreCarretera) setMultiplicador( info.multCarretera);
    else                          setMultiplicador( 0.0f);
}

float VehiculoIA::calcularVx(const InfoTerrenoIA &info, float centroX) const
{
    const float mitadPista = 300.0f;
    const float spd        = getSpeed();

    if (info.sobreFango || info.sobreLlanta)
        return (centroX < mitadPista) ? spd : -spd;

    if (info.peligroAdelante)
        return (centroX < info.xPeligro) ? -spd : spd;

    if (info.carreteraAdelante && info.xCarretera >= 0)
    {
        if      (centroX < info.xCarretera - 10) return  spd;
        else if (centroX > info.xCarretera + 10) return -spd;
    }
    return 0.0f;
}

float VehiculoIA::calcularVy(const InfoTerrenoIA &info, float &vx)
{
    float vy = 0.0f;

    if (info.sobreFango)
    {
        bool presionaLateral = (vx != 0.0f);
        if (presionaLateral)
        {
            addDerrapeTiempo(0.05f);
            setDerrapeDirX((vx > 0) ? 1.0f : -1.0f);
            setDerrapando(true);
            setDerrapeFuerza(2.0f + getDerrapeTiempo() * 4.0f);
        }
        else
        {
            vy = +3.0f;
            scaleDerrapeTiempo(0.85f);
            if (getDerrapeTiempo() < 0.05f)
            {
                setDerrapando(false);
                setDerrapeFuerza(0.0f);
                setDerrapeTiempo(0.0f);
            }
        }
        if (isDerrapando())
        {
            advanceDerrapePhi(0.08f);
            vx = getDerrapeDirX() * getDerrapeFuerza() * std::sin(getDerrapePhi());
            vy = getDerrapeFuerza() * (1.0f - std::cos(getDerrapePhi())) * 0.6f;
        }
        if (!isEnFango()) setEnFango(true);
    }
    else
    {
        if (getMultiplicador() > 0.0f) vy = -getSpeed() * getMultiplicador();
        if (isEnFango()) setEnFango(false);
    }
    return vy;
}

void VehiculoIA::moverConLimites(float vx, float vy)
{
    float nx = qBound(getLimiteIzq(), x() + vx, getLimiteDer());
    float ny = qBound(getLimiteArr(), y() + vy, getLimiteAba());
    setPos(nx, ny);
}

void VehiculoIA::actualizarIA(const std::vector<Terreno*> &terrenos)
{
    if (manejarBloqueoLlanta()) return;

    InfoTerrenoIA info = escanearTerrenos(terrenos);
    aplicarMultiplicador(info);

    float centroX = mapToScene(boundingRect()).boundingRect().center().x();
    float vx      = calcularVx(info, centroX);
    float vy      = calcularVy(info, vx);

    moverConLimites(vx, vy);
}
