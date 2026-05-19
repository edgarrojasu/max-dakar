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

private:
    void crearBoton(TipoVehiculo tipo, const QString &imagen,
                    const QString &nombre, int x, int y);
};

#endif
