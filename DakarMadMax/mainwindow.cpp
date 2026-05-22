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

    // Generar terreno cada ~90 frames
    contadorFrames++;
    if (contadorFrames >= 90) {
        contadorFrames = 0;

        float x     = QRandomGenerator::global()->bounded(20, 400);
        float ancho = QRandomGenerator::global()->bounded(150, 350);
        float alto  = QRandomGenerator::global()->bounded(60, 120);

        // 50% probabilidad de fango o carretera
        TipoTerreno tipo = (QRandomGenerator::global()->bounded(2) == 0)
                               ? TipoTerreno::Fango
                               : TipoTerreno::Carretera;

        Terreno *t = new Terreno(x, -alto, ancho, alto, tipo);
        scene->addItem(t);
        terrenos.push_back(t);
    }

    // Mover y detectar colisiones
    bool sobreTerreno = false;
    for (int i = terrenos.size() - 1; i >= 0; i--) {
        terrenos[i]->desplazar(3.0f);

        if (terrenos[i]->collidesWithItem(jugador)) {
            jugador->setMultiplicador(terrenos[i]->getMultiplicador());
            sobreTerreno = true;
        }

        if (terrenos[i]->fueraDePantalla()) {
            scene->removeItem(terrenos[i]);
            delete terrenos[i];
            terrenos.erase(terrenos.begin() + i);
        }
    }

    // Si no está sobre ningún terreno, no avanza
    if (!sobreTerreno)
        jugador->setMultiplicador(0.0f);
}

MainWindow::~MainWindow() {}
