#include "llanta.h"

Llanta::Llanta(float x, float velocidadBajada)
    : Terreno(x, -60),
    phi(0.0f),
    amplitud(60.0f),
    frecuencia(0.05f),
    xBase(x),
    frameActual(0),
    contadorFrame(0),
    velocidadAnimacion(6)
{
    velocidadY = velocidadBajada;

    QPixmap sheet(":/imagenes/llanta.png");
    if (!sheet.isNull()) {
        int sw = sheet.width()  / 3;
        int sh = sheet.height() / 2;
        for (int fila = 0; fila < 2; fila++)
            for (int col = 0; col < 3; col++)
                frames.append(sheet.copy(col*sw, fila*sh, sw, sh)
                                  .scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        setPixmap(frames[0]);
    } else {
        QPixmap fallback(60, 60);
        fallback.fill(QColor(20, 20, 20));
        setPixmap(fallback);
    }
}

void Llanta::actualizar(float velocidadMundo) {
    phi += frecuencia;
    setPos(xBase + amplitud * std::sin(phi), y() + velocidadY);

    // Solo animar si hay frames cargados
    if (frames.isEmpty()) return;

    contadorFrame++;
    if (contadorFrame >= velocidadAnimacion) {
        contadorFrame = 0;
        frameActual = (frameActual + 1) % frames.size();
        setPixmap(frames[frameActual]);
    }
}
