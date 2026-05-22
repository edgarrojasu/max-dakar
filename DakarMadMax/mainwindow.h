#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QStackedWidget>
#include "vehiculo.h"
#include "escenario.h"
#include "seleccionvehiculo.h"
#include "tiposvehiculo.h"
#include <vector>
#include <QRandomGenerator>
#include "terreno.h"
#include "carretera.h"
#include "fango.h"
#include "llanta.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void iniciarJuego(TipoVehiculo tipo);
    void gameLoop();

private:
    QStackedWidget    *stack;
    SeleccionVehiculo *pantallaSeleccion;

    QGraphicsScene *scene;
    QGraphicsView  *view;
    QTimer         *timer;
    Vehiculo       *jugador;
    Escenario      *escenario;
    std::vector<Terreno*> terrenos;
    int contadorFrames = 0;
    int contadorLlantas = 0;

};

#endif
