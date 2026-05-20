#include "vehiculo.h"
#include <QPixmap>
#include <QKeyEvent>

Vehiculo::Vehiculo(float x, float y, TipoVehiculo tipo)
    : tipo(tipo)
{
    // Velocidad base según vehículo — se ajustará con terreno después
    switch (tipo) {
    case TipoVehiculo::Moto:       speed = 5.0f; break;
    case TipoVehiculo::CarroDakar: speed = 4.0f; break;
    case TipoVehiculo::Camion:     speed = 3.0f; break;
    }

    // Cargar sprite según tipo
    QString rutaSprite;
    switch (tipo)
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
        switch (tipo)
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
}

void Vehiculo::actualizar()
{
    float vx = 0, vy = 0;

    if (teclasPulsadas.count(Qt::Key_A)) vx = -speed;
    if (teclasPulsadas.count(Qt::Key_D)) vx =  speed;
    if (teclasPulsadas.count(Qt::Key_W)) vy = -speed;
    if (teclasPulsadas.count(Qt::Key_S)) vy =  speed;

    float nx = x() + vx;
    float ny = y() + vy;

    if (nx < LIMITE_IZQ) nx = LIMITE_IZQ;
    if (nx > LIMITE_DER) nx = LIMITE_DER;
    if (ny < LIMITE_ARR) ny = LIMITE_ARR;
    if (ny > LIMITE_ABA) ny = LIMITE_ABA;

    setPos(nx, ny);
}

void Vehiculo::keyPressEvent(QKeyEvent *event) {
    teclasPulsadas.insert(event->key());
}

void Vehiculo::keyReleaseEvent(QKeyEvent *event) {
    teclasPulsadas.erase(event->key());
}
