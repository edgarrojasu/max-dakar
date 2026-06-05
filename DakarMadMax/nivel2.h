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
#include <QMediaPlayer>
#include <QAudioOutput>

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
    QGraphicsScene *scene;
    QGraphicsView  *view;
    QTimer         *timer;

    QGraphicsPixmapItem *vehiculo;
    QVector<QPixmap>     motoFrames;
    int  frameSprite;
    int  contadorFrameSprite;
    float vehY;
    float vehYBase;
    float velY;
    TipoVehiculo tipoVehiculo;

    QGraphicsPixmapItem *fondo1;
    QGraphicsPixmapItem *fondo2;
    QGraphicsPixmapItem *suelo1;
    QGraphicsPixmapItem *suelo2;
    float velocidadMundo;

    struct Agujero {
        QGraphicsPixmapItem *imgItem;
        QGraphicsRectItem   *tapaFondo;
        float xEscena;
        float ancho;
        bool  letraMostrada;
        bool  superado;
    };
    QVector<Agujero> agujeros;
    int contadorAgujero;
    int intervaloAgujero;
    int agujerosSuperados;

    char   letraRequerida;
    QLabel *labelLetra;
    QLabel *labelPista;
    QLabel *labelTiempoBar;
    bool   esperandoTecla;
    int    tiempoLimiteLetra;
    int    contadorLetra;

    bool  enSalto;
    float agujeroObjetivo;
    float anchoAgujeroActual;

    QGraphicsPixmapItem *meta;

    QLabel *labelDistancia;
    QLabel *labelMensaje;

    EstadoNivel2 estado;
    int frameActual;

    void iniciarSalto();
    void mostrarLetra();
    void ocultarLetra();
    void generarAgujero();
    void actualizarSuelo();
    void actualizarAgujeros(bool mover);
    void actualizarFondo();
    void finalizarNivel(bool exito);

    QMediaPlayer  *musicaNivel2;
    QAudioOutput  *audioNivel2;
    QMediaPlayer  *sfxSalto;
    QAudioOutput  *audioSfxSalto;
};

#endif
