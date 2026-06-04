#ifndef NIVEL2_H
#define NIVEL2_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QLabel>
#include <QKeyEvent>
#include <QVector>
#include <QRandomGenerator>
#include "tiposvehiculo.h"

enum class EstadoNivel2 {
    Corriendo,
    MostrandoLetra,
    Saltando,
    Cayendo,
    Ganando,
    Perdiendo
};

class Nivel2 : public QWidget {
    Q_OBJECT

public:
    explicit Nivel2(TipoVehiculo tipo, QWidget *parent = nullptr);
    ~Nivel2();

signals:
    void nivelCompletado();
    void nivelFallado();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    // ── Escena ────────────────────────────────────────────────────────────
    QGraphicsScene *scene;
    QGraphicsView  *view;
    QTimer         *timer;

    // ── Vehículo animado ──────────────────────────────────────────────────
    QGraphicsPixmapItem *vehiculo;
    QVector<QPixmap>     motoFrames;
    int  frameSprite;
    int  contadorFrameSprite;
    float vehY;
    float vehYBase;
    float velY;
    TipoVehiculo tipoVehiculo;

    // ── Fondo y suelo desplazables ────────────────────────────────────────
    QGraphicsPixmapItem *fondo1;
    QGraphicsPixmapItem *fondo2;
    QGraphicsPixmapItem *suelo1;
    QGraphicsPixmapItem *suelo2;
    float velocidadMundo;

    // ── Agujeros ──────────────────────────────────────────────────────────
    struct Agujero {
        QGraphicsPixmapItem *imgItem;    // imagen hueco.png (puede ser null)
        QGraphicsRectItem   *tapaFondo;  // rectángulo negro de respaldo
        float xEscena;
        float ancho;
        bool  letraMostrada;
        bool  superado;
    };
    QVector<Agujero> agujeros;
    int contadorAgujero;
    int intervaloAgujero;
    int agujerosSuperados;

    // ── Mecánica de letra ─────────────────────────────────────────────────
    char   letraRequerida;
    QLabel *labelLetra;
    QLabel *labelPista;
    QLabel *labelTiempoBar;   // barra visual de tiempo restante
    bool   esperandoTecla;
    int    tiempoLimiteLetra;
    int    contadorLetra;

    // ── Salto ─────────────────────────────────────────────────────────────
    bool  enSalto;
    float agujeroObjetivo;
    float anchoAgujeroActual;

    // ── Meta ──────────────────────────────────────────────────────────────
    QGraphicsPixmapItem *meta;

    // ── HUD ───────────────────────────────────────────────────────────────
    QLabel *labelDistancia;
    QLabel *labelMensaje;

    // ── Estado ────────────────────────────────────────────────────────────
    EstadoNivel2 estado;
    int frameActual;

    // ── Helpers ───────────────────────────────────────────────────────────
    void iniciarSalto();
    void mostrarLetra();
    void ocultarLetra();
    void generarAgujero();
    void actualizarSuelo();
    void actualizarAgujeros(bool mover);
    void actualizarFondo();
    void finalizarNivel(bool exito);
};

#endif
