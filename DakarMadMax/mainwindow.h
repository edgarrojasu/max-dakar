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
#include <QMediaPlayer>
#include <QAudioOutput>
#include "terreno.h"
#include "vehiculo.h"
#include "vehiculoia.h"
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
    void iniciarNivel2();

private:
    // ── Navegación ────────────────────────────────────────────────────────
    QStackedWidget    *stack;
    SeleccionVehiculo *pantallaSeleccion;
    Nivel2            *pantallaNivel2;

    // ── Nivel 1 ───────────────────────────────────────────────────────────
    QGraphicsScene    *scene;
    QGraphicsView     *view;
    QTimer            *timer;
    Vehiculo          *jugador;
    VehiculoIA        *rival;
    Escenario         *escenario;

    QGraphicsRectItem *lineaMeta;
    QLabel            *labelTiempo;
    int                tiempoRestante;
    bool               juegoTerminado;

    TipoVehiculo       tipoElegido;

    std::vector<Terreno*> terrenos;
    int contadorFrames  = 0;
    int contadorFango   = 0;
    int contadorLlantas = 0;

    // ── Audio ─────────────────────────────────────────────────────────────
    QMediaPlayer  *musicaNivel1;
    QAudioOutput  *audioNivel1;
};

#endif
