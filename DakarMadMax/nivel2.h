#ifndef NIVEL2_H
#define NIVEL2_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QLabel>
#include <QKeyEvent>
#include <QVector>
#include <QRandomGenerator>
#include "tiposvehiculo.h"

// ── Estado del nivel ──────────────────────────────────────────────────────────
enum class EstadoNivel2 {
    Corriendo,      // todo normal, suelo desplazándose
    MostrandoLetra, // apareció un agujero, se muestra la letra al jugador
    Saltando,       // el jugador presionó bien y está en el arco del salto
    Cayendo,        // falló la tecla, el vehículo cae al agujero
    Ganando,        // cruzó la meta
    Perdiendo       // terminó de caer
};

class Nivel2 : public QWidget {
    Q_OBJECT

public:
    explicit Nivel2(TipoVehiculo tipo, QWidget *parent = nullptr);
    ~Nivel2();

signals:
    void nivelCompletado();   // el jugador llegó a la meta
    void nivelFallado();      // cayó en un agujero

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    // ── Escena ────────────────────────────────────────────────────────────
    QGraphicsScene *scene;
    QGraphicsView  *view;
    QTimer         *timer;

    // ── Vehículo ──────────────────────────────────────────────────────────
    QGraphicsPixmapItem *vehiculo;
    float vehY;          // posición Y actual del vehículo
    float vehYBase;      // Y de reposo sobre el suelo
    float velY;          // velocidad vertical durante salto/caída
    TipoVehiculo tipoVehiculo;

    // ── Suelo desplazable ─────────────────────────────────────────────────
    // Dos segmentos de suelo que hacen loop
    QGraphicsRectItem *suelo1;
    QGraphicsRectItem *suelo2;
    float velocidadMundo;   // píxeles/frame que se desplaza el escenario

    // ── Agujeros ──────────────────────────────────────────────────────────
    struct Agujero {
        QGraphicsRectItem *rect;  // rectángulo negro del hueco
        float xEscena;            // posición X en la escena (se mueve)
        float ancho;
        bool  letraMostrada;      // ¿ya se mostró la letra para este agujero?
        bool  superado;           // ¿ya lo cruzó con éxito?
    };
    QVector<Agujero> agujeros;

    int contadorAgujero;      // frames hasta generar el próximo agujero
    int intervaloAgujero;     // separación entre agujeros (decrece con el tiempo)

    // ── Mecánica de letra ─────────────────────────────────────────────────
    char       letraRequerida;    // 'A'..'Z'
    QLabel    *labelLetra;        // widget que muestra la letra al jugador
    QLabel    *labelPista;        // "¡Presiona X para saltar!"
    bool       esperandoTecla;    // true mientras se muestra la letra
    int        tiempoLimiteLetra; // frames que tiene el jugador para responder
    int        contadorLetra;     // frames transcurridos esperando

    // ── Salto ─────────────────────────────────────────────────────────────
    bool  enSalto;
    float agujeroObjetivo;   // X final del agujero que está cruzando
    float anchoAgujeroActual;

    // ── Meta ──────────────────────────────────────────────────────────────
    QGraphicsPixmapItem *meta;
    float xMeta;            // posición X de la meta en escena
    int   distanciaTotal;   // frames de recorrido hasta la meta
    int   frameActual;

    // ── HUD ───────────────────────────────────────────────────────────────
    QLabel *labelDistancia;  // barra de progreso textual
    QLabel *labelMensaje;    // "¡GANASTE!" / "Caíste..."

    // ── Estado ────────────────────────────────────────────────────────────
    EstadoNivel2 estado;

    // ── Fondo desplazable ─────────────────────────────────────────────────
    QGraphicsPixmapItem *fondo1;
    QGraphicsPixmapItem *fondo2;

    // ── Helpers ───────────────────────────────────────────────────────────
    void iniciarSalto();
    void mostrarLetra(float xAgujero, float anchoAgujero);
    void ocultarLetra();
    void generarAgujero();
    void actualizarSuelo();
    void actualizarAgujeros();
    void verificarColisiones();
    void actualizarFondo();
    void finalizarNivel(bool exito);
};

#endif // NIVEL2_H
