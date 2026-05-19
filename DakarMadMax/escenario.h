#ifndef ESCENARIO_H
#define ESCENARIO_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <vector>

class Escenario {
public:
    explicit Escenario(QGraphicsScene *scene);
    void desplazar();

private:
    QGraphicsScene *scene;

    // Dos copias del fondo para el scroll infinito
    QGraphicsRectItem *fondo1;
    QGraphicsRectItem *fondo2;

    float velocidadFondo;
};

#endif
