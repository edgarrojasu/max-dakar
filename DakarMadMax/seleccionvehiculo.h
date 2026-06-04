#ifndef SELECCIONVEHICULO_H
#define SELECCIONVEHICULO_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "tiposvehiculo.h"

class SeleccionVehiculo : public QWidget {
    Q_OBJECT

public:
    explicit SeleccionVehiculo(QWidget *parent = nullptr);

signals:
    void vehiculoSeleccionado(TipoVehiculo tipo);
    void irANivel2Debug();   // solo activa cuando DEBUG_NIVEL2 está definido

private:
    void crearBoton(TipoVehiculo tipo, const QString &imagen,
                    const QString &nombre, int x, int y);
};

#endif
