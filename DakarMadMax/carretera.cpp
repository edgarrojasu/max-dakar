#include "carretera.h"
#include <QPainter>

Carretera::Carretera(float x, float y, float ancho, float alto)
    : Terreno(x, y)
{
    int w = (int)ancho;
    int h = (int)alto;
    if (w <= 0) w = 10;
    if (h <= 0) h = 10;

    QPixmap px(":/imagenes/carretera.png");
    if (!px.isNull()) {
        setPixmap(px.scaled(w, h,
                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        QPixmap fallback(w, h);
        fallback.fill(QColor(60, 60, 60));
        setPixmap(fallback);
    }
    velocidadY = 3.0f;
}

void Carretera::actualizar(float velocidadMundo) {
    setY(y() + velocidadMundo);
}
