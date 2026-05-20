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
}

MainWindow::~MainWindow() {}
