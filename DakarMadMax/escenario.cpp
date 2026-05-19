#include "escenario.h"
#include <QBrush>
#include <QColor>

Escenario::Escenario(QGraphicsScene *scene)
    : scene(scene), velocidadFondo(3.0f)
{
    // Dos rectángulos que simulan el desierto desplazándose
    // Cuando el primero sale de pantalla, salta arriba del segundo
    fondo1 = new QGraphicsRectItem(0, 0, 600, 800);
    fondo1->setBrush(QBrush(QColor(194, 154, 94)));   // arena
    fondo1->setPen(Qt::NoPen);
    fondo1->setZValue(-1);
    scene->addItem(fondo1);

    fondo2 = new QGraphicsRectItem(0, -800, 600, 800);
    fondo2->setBrush(QBrush(QColor(180, 140, 80)));   // arena tono alternado
    fondo2->setPen(Qt::NoPen);
    fondo2->setZValue(-1);
    scene->addItem(fondo2);
}

void Escenario::desplazar() {
    fondo1->setY(fondo1->y() + velocidadFondo);
    fondo2->setY(fondo2->y() + velocidadFondo);

    // Si un fondo salió por abajo, lo reubica arriba del otro
    if (fondo1->y() >= 800)
        fondo1->setY(fondo2->y() - 800);

    if (fondo2->y() >= 800)
        fondo2->setY(fondo1->y() - 800);
}
