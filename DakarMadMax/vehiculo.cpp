#include "vehiculo.h"
#include <QPixmap>
#include <QKeyEvent>
#include <cmath>
#include "terreno.h"
#include "fango.h"
#include "llanta.h"
#include "carretera.h"

Vehiculo::Vehiculo(float x, float y, TipoVehiculo tipo)
    : tipoVehiculo(tipo)
{
    switch (tipoVehiculo)
    {
        case TipoVehiculo::Moto:       speed = 5.0f; break;
        case TipoVehiculo::CarroDakar: speed = 5.0f; break;
        case TipoVehiculo::Camion:     speed = 5.0f; break;
    }

    switch (tipoVehiculo)
    {
        case TipoVehiculo::Moto:
            limiteIzq = -10.0f; limiteDer = 550.0f;
            limiteArr = 0.0f; limiteAba = 590.0f;
            break;
        case TipoVehiculo::CarroDakar:
            limiteIzq = -10.0f; limiteDer = 510.0f;
            limiteArr = 0.0f; limiteAba = 550.0f;
            break;
        case TipoVehiculo::Camion:
            limiteIzq = -10.0f; limiteDer = 530.0f;
            limiteArr = 0.0f; limiteAba = 530.0f;
            break;
    }

    QString rutaSprite;
    switch (tipoVehiculo)
    {
        case TipoVehiculo::Moto:       rutaSprite = ":/imagenes/motoCenital.png";   break;
        case TipoVehiculo::CarroDakar: rutaSprite = ":/imagenes/carroCenital.png";  break;
        case TipoVehiculo::Camion:     rutaSprite = ":/imagenes/camionCenital.png"; break;
    }

    QPixmap px(rutaSprite);

    if (px.isNull())
    {
        QPixmap fallback(40, 60);
        fallback.fill(QColor(200, 50, 50));
        setPixmap(fallback);
    }
    else
    {
        int ancho, alto;
        switch (tipoVehiculo)
        {
            case TipoVehiculo::Moto:       ancho = 70;  alto = 120; break;
            case TipoVehiculo::CarroDakar: ancho = 120; alto = 160; break;
            case TipoVehiculo::Camion:     ancho = 130; alto = 180; break;
        }
        QTransform t;
        t.rotate(90);
        px = px.transformed(t, Qt::SmoothTransformation);
        setPixmap(px.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    setPos(x, y);
    setZValue(1);
}

void Vehiculo::actualizar()
{
    if (multiplicador < 0)
    {
        static int framesBloqueado = 0;
        framesBloqueado++;
        if (framesBloqueado >= 60)
        {
            framesBloqueado = 0;
            multiplicador = 0.0f;
        }
        return;
    }

    float vx = 0, vy = 0;

    if (enFango)
    {
        bool presA = teclasPulsadas.count(Qt::Key_A);
        bool presD = teclasPulsadas.count(Qt::Key_D);

        if (presA || presD)
        {
            derrapeTiempo += 0.05f;
            if (derrapeTiempo > 1.5f) derrapeTiempo = 1.5f;
            derrapeDirX   = presD ? 1.0f : -1.0f;
            derrapando    = true;
            derrapeFuerza = 2.0f + derrapeTiempo * 4.0f;
        }
        else
        {
            vy = +3.0f;   // retrocede lentamente si no mueve
            derrapeTiempo *= 0.85f;
            if (derrapeTiempo < 0.05f)
            {
                derrapando    = false;
                derrapeFuerza = 0.0f;
                derrapeTiempo = 0.0f;
            }
        }

        if (derrapando)
        {
            derrapePhi += 0.08f;
            vx = derrapeDirX * derrapeFuerza * std::sin(derrapePhi);
            vy = derrapeFuerza * (1.0f - std::cos(derrapePhi)) * 0.6f;
        }

    }
    else
    {
        if (teclasPulsadas.count(Qt::Key_A)) vx = -speed;
        if (teclasPulsadas.count(Qt::Key_D)) vx =  speed;
        if (teclasPulsadas.count(Qt::Key_S)) vy =  speed;

        if (multiplicador > 0.0f)
            vy -= speed * multiplicador;
    }

    float nx = x() + vx;
    float ny = y() + vy;
    if (nx < limiteIzq) nx = limiteIzq;
    if (nx > limiteDer) nx = limiteDer;
    if (ny < limiteArr) ny = limiteArr;
    if (ny > limiteAba) ny = limiteAba;
    setPos(nx, ny);
}

void Vehiculo::keyPressEvent(QKeyEvent *event) { teclasPulsadas.insert(event->key()); }
void Vehiculo::keyReleaseEvent(QKeyEvent *event) { teclasPulsadas.erase(event->key()); }

void Vehiculo::setEnFango(bool fango)
{
    enFango = fango;
    if (!fango)
    {
        derrapando    = false;
        derrapeFuerza = 0.0f;
        derrapeTiempo = 0.0f;
        derrapePhi    = 0.0f;
        derrapeDirX   = 0.0f;
    }
}

void Vehiculo::actualizarIA(const std::vector<Terreno*> &terrenos)
{
    if (multiplicador < 0)
    {
        framesBloqueadoIA++;
        if (framesBloqueadoIA >= 60)
        {
            framesBloqueadoIA = 0;
            multiplicador = 0.0f;
        }
        return;
    }

    QRectF miRect = mapToScene(boundingRect()).boundingRect();
    float centroX = miRect.center().x();
    float miY     = miRect.top();

    //Escanear terrenos
    float visionAlto = 600.0f;
    QRectF vision(miRect.left() - 30, miY - visionAlto,
                  miRect.width() + 60, visionAlto);

    bool  sobreFangoActual  = false;
    bool  sobreCarretera    = false;
    bool  sobreLlanta       = false;
    float multCarretera     = 0.0f;

    bool  peligroAdelante   = false;
    bool  carreteraAdelante = false;
    float xPeligroAdelante  = -1.0f;
    float xCarretera        = -1.0f;

    for (Terreno *t : terrenos)
    {
        QRectF rT    = t->mapToScene(t->boundingRect()).boundingRect();
        bool toca    = miRect.intersects(rT);
        bool visible = vision.intersects(rT);

        bool esFangoT    = dynamic_cast<Fango*>(t)     != nullptr;
        bool esLlantaT   = dynamic_cast<Llanta*>(t)    != nullptr;
        bool esCarreteraT= dynamic_cast<Carretera*>(t) != nullptr;

        if (toca)
        {
            if (esFangoT)     sobreFangoActual = true;
            if (esLlantaT)    sobreLlanta      = true;
            if (esCarreteraT) { sobreCarretera = true; multCarretera = t->getMultiplicador(); }
        }

        if (visible && (esFangoT || esLlantaT) && !peligroAdelante)
        {
            peligroAdelante  = true;
            xPeligroAdelante = rT.center().x();
        }
        if (visible && esCarreteraT && !carreteraAdelante)
        {
            carreteraAdelante = true;
            xCarretera        = rT.center().x();
        }
    }

    if (sobreLlanta)
    {
        multiplicador = -1.0f;
    }
    else if (sobreFangoActual)
    {
        multiplicador = 0.5f;
    }
    else if (sobreCarretera)
    {
        multiplicador = multCarretera;
    }
    else
    {
        multiplicador = 0.0f;
    }

    float vx = 0.0f;
    float mitadPista = 300.0f;

    if (sobreFangoActual || sobreLlanta)
    {
        vx = (centroX < mitadPista) ? speed : -speed;
    }
    else if (peligroAdelante)
    {
        vx = (centroX < xPeligroAdelante) ? -speed : speed;
    }
    else if (carreteraAdelante && xCarretera >= 0)
    {
        if      (centroX < xCarretera - 10) vx =  speed;
        else if (centroX > xCarretera + 10) vx = -speed;
    }

    float vy = 0.0f;

    if (sobreFangoActual)
    {
        bool iaPresionaLateral = (vx != 0.0f);

        if (iaPresionaLateral)
        {
            derrapeTiempo += 0.05f;
            if (derrapeTiempo > 1.5f) derrapeTiempo = 1.5f;
            derrapeDirX   = (vx > 0) ? 1.0f : -1.0f;
            derrapando    = true;
            derrapeFuerza = 2.0f + derrapeTiempo * 4.0f;
        }
        else
        {
            vy = +3.0f;
            derrapeTiempo *= 0.85f;
            if (derrapeTiempo < 0.05f)
            {
                derrapando    = false;
                derrapeFuerza = 0.0f;
                derrapeTiempo = 0.0f;
            }
        }

        if (derrapando)
        {
            derrapePhi += 0.08f;
            vx = derrapeDirX * derrapeFuerza * std::sin(derrapePhi);
            vy = derrapeFuerza * (1.0f - std::cos(derrapePhi)) * 0.6f;
        }

        if (!enFango) setEnFango(true);

    }
    else
    {
        if (multiplicador > 0.0f)
            vy = -speed * multiplicador;

        if (enFango) setEnFango(false);
    }

    float nx = x() + vx;
    float ny = y() + vy;
    if (nx < limiteIzq) nx = limiteIzq;
    if (nx > limiteDer) nx = limiteDer;
    if (ny < limiteArr) ny = limiteArr;
    if (ny > limiteAba) ny = limiteAba;
    setPos(nx, ny);
}
