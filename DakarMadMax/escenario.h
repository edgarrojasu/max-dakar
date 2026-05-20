#ifndef ESCENARIO_H
#define ESCENARIO_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class Escenario {
public:
    explicit Escenario(QGraphicsScene *scene);
    void desplazar();

private:
    QGraphicsScene *scene;

    QGraphicsPixmapItem *fondo1;
    QGraphicsPixmapItem *fondo2;

    QGraphicsPixmapItem *bordeIzq1;
    QGraphicsPixmapItem *bordeIzq2;
    QGraphicsPixmapItem *bordeDer1;
    QGraphicsPixmapItem *bordeDer2;

    float velocidadFondo;
};

#endif
