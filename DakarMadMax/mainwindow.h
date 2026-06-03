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
#include "nivel2.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void iniciarJuego(TipoVehiculo tipo);
    void gameLoop();
    void iniciarNivel2();   // se llama cuando el jugador gana el nivel 1

private:
    // ── Navegación ────────────────────────────────────────────────────────
    QStackedWidget    *stack;
    SeleccionVehiculo *pantallaSeleccion;
    Nivel2            *pantallaNivel2;     // nivel 2 (vista lateral)

    // ── Nivel 1 ───────────────────────────────────────────────────────────
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

    TipoVehiculo       tipoElegido;   // guardamos el tipo para pasarlo al nivel 2

    std::vector<Terreno*> terrenos;
    int contadorFrames  = 0;
    int contadorFango   = 0;
    int contadorLlantas = 0;
};

#endif
