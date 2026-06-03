#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QStackedWidget>
#include <QLabel>
#include <vector>
#include <QRandomGenerator>
#include "terreno.h"
#include "vehiculo.h"
#include "escenario.h"
#include "seleccionvehiculo.h"
#include "tiposvehiculo.h"
#include "carretera.h"
#include "fango.h"
#include "llanta.h"
#include <QLabel>

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
    QGraphicsScene    *scene;
    QGraphicsView     *view;
    QTimer            *timer;
    Vehiculo          *jugador;
    Vehiculo          *rival;
    Escenario         *escenario;

    QGraphicsRectItem *lineaMeta;
    QLabel            *labelTiempo;
    int                tiempoRestante;
    bool               juegoTerminado;

    std::vector<Terreno*> terrenos;
    int contadorFrames  = 0;
    int contadorFango   = 0;
    int contadorLlantas = 0;
};

#endif
