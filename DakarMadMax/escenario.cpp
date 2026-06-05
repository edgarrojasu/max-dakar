#include "escenario.h"
#include <QPixmap>

Escenario::Escenario(QGraphicsScene *scene)
    : scene(scene), velocidadFondo(10.0f)
{
    QPixmap pxFondo1(":/imagenes/fondo1.png");
    QPixmap pxFondo2(":/imagenes/fonogem.png");

    fondo1 = new QGraphicsPixmapItem(
        pxFondo1.scaled(600, 700, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    fondo1->setPos(0, 0);
    fondo1->setZValue(-1);
    scene->addItem(fondo1);

    fondo2 = new QGraphicsPixmapItem(
        pxFondo2.scaled(600, 700, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    fondo2->setPos(0, -700);
    fondo2->setZValue(-1);
    scene->addItem(fondo2);

    QPixmap pxIzq(":/imagenes/fondoIzq.png");
    QPixmap pxDer(":/imagenes/fondoderchat.png");

    bordeIzq1 = new QGraphicsPixmapItem(
        pxIzq.scaled(100, 700, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    bordeIzq1->setPos(-100, 0);
    bordeIzq1->setZValue(0);
    scene->addItem(bordeIzq1);

    bordeIzq2 = new QGraphicsPixmapItem(
        pxIzq.scaled(100, 700, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    bordeIzq2->setPos(-100, -700);
    bordeIzq2->setZValue(0);
    scene->addItem(bordeIzq2);

    bordeDer1 = new QGraphicsPixmapItem(
        pxDer.scaled(100, 700, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    bordeDer1->setPos(600, 0);
    bordeDer1->setZValue(0);
    scene->addItem(bordeDer1);

    bordeDer2 = new QGraphicsPixmapItem(
        pxDer.scaled(100, 700, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );
    bordeDer2->setPos(600, -700);
    bordeDer2->setZValue(0);
    scene->addItem(bordeDer2);
}

void Escenario::desplazar()
{
    fondo1->setY(fondo1->y() + velocidadFondo);
    fondo2->setY(fondo2->y() + velocidadFondo);
    bordeIzq1->setY(bordeIzq1->y() + velocidadFondo);
    bordeIzq2->setY(bordeIzq2->y() + velocidadFondo);
    bordeDer1->setY(bordeDer1->y() + velocidadFondo);
    bordeDer2->setY(bordeDer2->y() + velocidadFondo);

    if (fondo1->y() >= 700) fondo1->setY(fondo2->y() - 700);
    if (fondo2->y() >= 700) fondo2->setY(fondo1->y() - 700);

    if (bordeIzq1->y() >= 700) bordeIzq1->setY(bordeIzq2->y() - 700);
    if (bordeIzq2->y() >= 700) bordeIzq2->setY(bordeIzq1->y() - 700);
    if (bordeDer1->y() >= 700) bordeDer1->setY(bordeDer2->y() - 700);
    if (bordeDer2->y() >= 700) bordeDer2->setY(bordeDer1->y() - 700);
}
