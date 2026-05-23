#include "vehiculo.h"
#include <QPixmap>
#include <QKeyEvent>
#include <cmath>

Vehiculo::Vehiculo(float x, float y, TipoVehiculo tipo)
    : tipoVehiculo(tipo)
{
    // Velocidad base según vehículo — se ajustará con terreno después
    switch (tipoVehiculo) {
    case TipoVehiculo::Moto:       speed = 5.0f; break;
    case TipoVehiculo::CarroDakar: speed = 5.0f; break;
    case TipoVehiculo::Camion:     speed = 5.0f; break;
    }

    // Límites según tamaño del sprite de cada vehículo
    switch (tipoVehiculo) {
    case TipoVehiculo::Moto:
        limiteIzq = -10.0f;
        limiteDer = 550.0f;  // más angosta, puede ir más a la derecha
        limiteArr = 100.0f;
        limiteAba = 590.0f;
        break;
    case TipoVehiculo::CarroDakar:
        limiteIzq = -10.0f;
        limiteDer = 510.0f;
        limiteArr = 100.0f;
        limiteAba = 550.0f;
        break;
    case TipoVehiculo::Camion:
        limiteIzq = -10.0f;
        limiteDer = 530.0f;  // más ancho, límite más a la izquierda
        limiteArr = 100.0f;
        limiteAba = 530.0f;
        break;
    }

    // Cargar sprite según tipo
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
        // ── TAMAÑO por vehículo ──────────────────────────────
        int ancho, alto;
        switch (tipoVehiculo)
        {
            case TipoVehiculo::Moto:       ancho = 70;  alto = 120; break;
            case TipoVehiculo::CarroDakar: ancho = 120;  alto = 160; break;
            case TipoVehiculo::Camion:     ancho = 130;  alto = 180; break;
        }

        // ── ROTACIÓN ─────────────────────────────────────────
        // Cambiá el número hasta que quede orientado hacia arriba:
        // 0 = sin rotar | 90 = gira derecha | 180 = invertido | 270 = gira izquierda
        QTransform t;
        t.rotate(90);
        px = px.transformed(t, Qt::SmoothTransformation);

        setPixmap(px.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    setPos(x, y);
    setZValue(1);

}

void Vehiculo::actualizar() {
    if (multiplicador < 0) {
        static int framesBloqueado = 0;
        framesBloqueado++;
        if (framesBloqueado >= 60) {
            framesBloqueado = 0;
            multiplicador = 0.0f;
        }
        return;
    }

    float vx = 0, vy = 0;

    if (enFango) {
        // En fango: solo se mueve si el jugador presiona teclas
        // con el efecto de derrape ya implementado
        bool presA = teclasPulsadas.count(Qt::Key_A);
        bool presD = teclasPulsadas.count(Qt::Key_D);

        if (presA || presD) {
            derrapeTiempo += 0.05f;
            if (derrapeTiempo > 1.5f) derrapeTiempo = 1.5f;

            derrapeDirX  = presD ? 1.0f : -1.0f;
            derrapando   = true;
            derrapeFuerza = 2.0f + derrapeTiempo * 4.0f;
        } else {
            vy = +1.0f;
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
        // Sin teclas y sin derrape activo: vehículo quieto en fango

    } else {
        // Fuera del fango
        if (teclasPulsadas.count(Qt::Key_A)) vx = -speed;
        if (teclasPulsadas.count(Qt::Key_D)) vx =  speed;
        if (teclasPulsadas.count(Qt::Key_S)) vy =  speed;

        // Avanza solo si está sobre carretera (multiplicador > 1)
        // o sobre arena normal (multiplicador == 1)
        // No avanza si multiplicador == 0 (sin terreno)
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

void Vehiculo::keyPressEvent(QKeyEvent *event) {
    teclasPulsadas.insert(event->key());
}

void Vehiculo::keyReleaseEvent(QKeyEvent *event) {
    teclasPulsadas.erase(event->key());
}

void Vehiculo::setEnFango(bool fango) {
    enFango = fango;
    // Si salió del fango, cancela el derrape inmediatamente
    if (!fango) {
        derrapando = false;
        derrapeFuerza = 0.0f;
        derrapeTiempo = 0.0f;
        derrapePhi = 0.0f;
        derrapeDirX = 0.0f;
    }
}
