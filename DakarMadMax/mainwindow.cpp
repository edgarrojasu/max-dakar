#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), jugador(nullptr), escenario(nullptr), timer(nullptr)
{
    setFixedSize(800, 600);
    setWindowTitle("Dakar Mad Max");

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    // Pantalla de selección
    pantallaSeleccion = new SeleccionVehiculo();
    stack->addWidget(pantallaSeleccion);   // índice 0

    // Vista del juego (se llena al seleccionar)
    view = new QGraphicsView();
    view->setFixedSize(800, 600);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    stack->addWidget(view);               // índice 1

    connect(pantallaSeleccion, &SeleccionVehiculo::vehiculoSeleccionado,
            this, &MainWindow::iniciarJuego);

    // Mostrar pantalla de selección primero
    stack->setCurrentIndex(0);
}

void MainWindow::iniciarJuego(TipoVehiculo tipo) {
    scene = new QGraphicsScene(0, 0, 800, 600, this);
    view->setScene(scene);

    escenario = new Escenario(scene);

    jugador = new Vehiculo(270, 600, tipo);
    scene->addItem(jugador);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::gameLoop);
    view->setSceneRect(0, 0, 600, 800);
    view->centerOn(300, 400);
    timer->start(16);

    // Cambiar a la pantalla del juego
    stack->setCurrentIndex(1);
    jugador->setFlag(QGraphicsItem::ItemIsFocusable);
    jugador->setFocus();
}

void MainWindow::gameLoop() {
    escenario->desplazar();
    jugador->actualizar();

    // Generar terreno cada ~90 frames (~1.5 segundos)
    contadorFrames++;
    if (contadorFrames >= 90) {
        contadorFrames = 0;

        // Posición X aleatoria dentro de la pista (entre 20 y 480)
        float x = QRandomGenerator::global()->bounded(20, 480);
        float ancho = QRandomGenerator::global()->bounded(80, 200);
        float alto  = QRandomGenerator::global()->bounded(40, 80);

        Terreno *t = new Terreno(x, -alto, ancho, alto,
                                 QColor(139, 90, 43),   // marrón llamativo
                                 0.5f);                  // todos van al 50%
        scene->addItem(t);
        terrenos.push_back(t);
    }

    // Mover terrenos y detectar colisiones
    for (int i = terrenos.size() - 1; i >= 0; i--) {
        terrenos[i]->desplazar(3.0f);

        // Colisión con jugador
        if (terrenos[i]->collidesWithItem(jugador)) {
            jugador->setMultiplicador(0.5f);
        } else {
            jugador->setMultiplicador(1.0f);
        }

        // Eliminar si salió de pantalla
        if (terrenos[i]->fueraDePantalla()) {
            scene->removeItem(terrenos[i]);
            delete terrenos[i];
            terrenos.erase(terrenos.begin() + i);
        }
    }
}

MainWindow::~MainWindow() {}
