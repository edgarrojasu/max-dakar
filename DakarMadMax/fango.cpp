#include "fango.h"

Fango::Fango(float x, float y, float ancho, float alto)
    : Terreno(x, y)
{
    int w = (int)ancho;
    int h = (int)alto;
    if (w <= 0) w = 10;
    if (h <= 0) h = 10;

    QPixmap px(":/imagenes/fango.png");
    if (!px.isNull())
    {
        setPixmap(px.scaled(w, h,
                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        QPixmap fallback(w, h);
        fallback.fill(QColor(101, 67, 33));
        setPixmap(fallback);
    }
    velocidadY = -0.2f;
}

void Fango::actualizar(float velocidadMundo)
{
    setY(y() + velocidadMundo);
}
