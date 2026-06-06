#include "vehiculo.h"
#include <QPixmap>
#include <QKeyEvent>
#include <cmath>

Vehiculo::Vehiculo(float x, float y, TipoVehiculo tipo)
    : tipoVehiculo(tipo)
{
    switch (tipoVehiculo) {
    case TipoVehiculo::Moto:       speed = 5.0f; break;
    case TipoVehiculo::CarroDakar: speed = 5.0f; break;
    case TipoVehiculo::Camion:     speed = 5.0f; break;
    }

    switch (tipoVehiculo) {
    case TipoVehiculo::Moto:
        limiteIzq = -10.0f; limiteDer = 550.0f;
        limiteArr = 100.0f; limiteAba = 590.0f;
        break;
    case TipoVehiculo::CarroDakar:
        limiteIzq = -10.0f; limiteDer = 510.0f;
        limiteArr = 100.0f; limiteAba = 550.0f;
        break;
    case TipoVehiculo::Camion:
        limiteIzq = -10.0f; limiteDer = 530.0f;
        limiteArr = 100.0f; limiteAba = 530.0f;
        break;
    }

    QString rutaSprite;
    switch (tipoVehiculo) {
        case TipoVehiculo::Moto:       rutaSprite = ":/imagenes/motoCenital.png";   break;
        case TipoVehiculo::CarroDakar: rutaSprite = ":/imagenes/carroCenital.png";  break;
        case TipoVehiculo::Camion:     rutaSprite = ":/imagenes/camionCenital.png"; break;
    }

    QPixmap px(rutaSprite);
    if (px.isNull()) {
        QPixmap fallback(40, 60);
        fallback.fill(QColor(200, 50, 50));
        setPixmap(fallback);
    } else {
        int ancho, alto;
        switch (tipoVehiculo) {
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

// ── Helpers de actualizar() ──────────────────────────────────────────────────

// Aplica retroceso por llanta y cuenta el bloqueo.
// Devuelve true mientras el bloqueo sigue activo.
bool Vehiculo::manejarBloqueoLlantaJugador()
{
    if (multiplicador >= 0) return false;

    static int framesBloqueado = 0;
    framesBloqueado++;

    float ny = y() + 4.0f;
    if (ny > limiteAba) ny = limiteAba;
    setY(ny);

    if (framesBloqueado >= 60) {
        framesBloqueado = 0;
        multiplicador   = 0.0f;
    }
    return true;
}

// Calcula vx/vy cuando el vehículo está en fango (derrape sinusoidal).
void Vehiculo::calcularMovimientoEnFango(float &vx, float &vy)
{
    bool presA = teclasPulsadas.count(Qt::Key_A);
    bool presD = teclasPulsadas.count(Qt::Key_D);

    if (presA || presD) {
        derrapeTiempo += 0.05f;
        if (derrapeTiempo > 1.5f) derrapeTiempo = 1.5f;
        derrapeDirX   = presD ? 1.0f : -1.0f;
        derrapando    = true;
        derrapeFuerza = 2.0f + derrapeTiempo * 4.0f;
    } else {
        vy = +1.0f;   // sin input → retrocede lentamente
        derrapeTiempo *= 0.85f;
        if (derrapeTiempo < 0.05f) {
            derrapando    = false;
            derrapeFuerza = 0.0f;
            derrapeTiempo = 0.0f;
        }
    }

    if (derrapando) {
        derrapePhi += 0.08f;
        vx = derrapeDirX * derrapeFuerza * std::sin(derrapePhi);
        vy = derrapeFuerza * (1.0f - std::cos(derrapePhi)) * 0.6f;
    }
}

// Calcula vx/vy en terreno normal (carretera o arena).
void Vehiculo::calcularMovimientoNormal(float &vx, float &vy)
{
    if (teclasPulsadas.count(Qt::Key_A)) vx = -speed;
    if (teclasPulsadas.count(Qt::Key_D)) vx =  speed;
    if (teclasPulsadas.count(Qt::Key_S)) vy =  speed;

    if (multiplicador > 0.0f)
        vy -= speed * multiplicador;
}

// Aplica los límites de pantalla a la posición calculada.
void Vehiculo::aplicarLimites(float &nx, float &ny) const
{
    nx = qBound(limiteIzq, nx, limiteDer);
    ny = qBound(limiteArr, ny, limiteAba);
}

// ── JUGADOR ──────────────────────────────────────────────────────────────────
void Vehiculo::actualizar()
{
    if (manejarBloqueoLlantaJugador()) return;

    float vx = 0.0f, vy = 0.0f;

    if (enFango) calcularMovimientoEnFango(vx, vy);
    else         calcularMovimientoNormal(vx, vy);

    float nx = x() + vx;
    float ny = y() + vy;
    aplicarLimites(nx, ny);
    setPos(nx, ny);
}

void Vehiculo::keyPressEvent(QKeyEvent *event)  { teclasPulsadas.insert(event->key()); }
void Vehiculo::keyReleaseEvent(QKeyEvent *event){ teclasPulsadas.erase(event->key()); }

void Vehiculo::setEnFango(bool fango)
{
    enFango = fango;
    if (!fango) {
        derrapando    = false;
        derrapeFuerza = 0.0f;
        derrapeTiempo = 0.0f;
        derrapePhi    = 0.0f;
        derrapeDirX   = 0.0f;
    }
}