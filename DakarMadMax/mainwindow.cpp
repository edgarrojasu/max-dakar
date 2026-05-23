#include "mainwindow.h"
#include <qdebug>


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
    //qdebug() << "--- inicio gameLoop, terrenos:" << terrenos.size();

    escenario->desplazar();
    //qdebug() << "escenario ok";

    jugador->actualizar();
    //qdebug() << "jugador ok";

    // Generar carretera cada ~200 frames
    contadorFrames++;
    if (contadorFrames >= 200) {
        contadorFrames = 0;
        int x     = QRandomGenerator::global()->bounded(20, 400);
        int ancho = QRandomGenerator::global()->bounded(100, 140);
        int alto  = QRandomGenerator::global()->bounded(5000,7000);
        Terreno *t = new Carretera((float)x, -(float)alto, (float)ancho, (float)alto);
        scene->addItem(t);
        terrenos.push_back(t);
    }

    // Generar fango cada ~100 frames
    contadorFango++;
    if (contadorFango >= 200) {
        contadorFango = 0;
        int x     = QRandomGenerator::global()->bounded(20, 400);
        int ancho = QRandomGenerator::global()->bounded(200, 300);
        int alto  = QRandomGenerator::global()->bounded(1000,1500);
        Terreno *t = new Fango((float)x, -(float)alto, (float)ancho, (float)alto);
        scene->addItem(t);
        terrenos.push_back(t);
    }

    // Generar llanta cada ~150 frames
    contadorLlantas++;
    if (contadorLlantas >= 150) {
        contadorLlantas = 0;
        float x   = (float)QRandomGenerator::global()->bounded(50, 500);
        float vel = 2.0f + QRandomGenerator::global()->bounded(0, 20) * 0.1f;

        Llanta *l = new Llanta(x, vel);

        // Solo agregar si no está encima del jugador al generarse
        QRectF rJugador = jugador->mapToScene(jugador->boundingRect()).boundingRect();
        QRectF rLlanta  = QRectF(x, -60, 60, 60);

        if (!rJugador.intersects(rLlanta)) {
            scene->addItem(l);
            terrenos.push_back(l);
        } else {
            delete l;  // descartarla y esperar el próximo ciclo
        }
    }

    // Primero mover todos
    //qdebug() << "moviendo terrenos...";
    for (int i = 0; i < (int)terrenos.size(); i++) {
        //qdebug() << "  moviendo" << i << "tipo:" << typeid(*terrenos[i]).name();
        terrenos[i]->actualizar(50.0f);
        //qdebug() << "  ok" << i;
    }
    //qdebug() << "movimiento ok";


    // Detectar colisiones por separado
    ////qdebug() << "colisiones...";

    bool sobreTerreno = false;
    bool sobreFango   = false;

    for (int i = 0; i < (int)terrenos.size(); i++) {
        if (!terrenos[i]->scene()) continue;  // ya fue removido

        QRectF rJugador = jugador->mapToScene(jugador->boundingRect()).boundingRect();
        QRectF rTerreno = terrenos[i]->mapToScene(terrenos[i]->boundingRect()).boundingRect();

        if (rJugador.intersects(rTerreno)) {
            jugador->setMultiplicador(terrenos[i]->getMultiplicador());
            sobreTerreno = true;
            if (dynamic_cast<Fango*>(terrenos[i]))
                sobreFango = true;
        }
    }

    jugador->setEnFango(sobreFango);
    if (!sobreTerreno)
        jugador->setMultiplicador(0.0f);
    //qdebug() << "colisiones ok";


    // Eliminar los que salieron de pantalla — loop separado al final
    //qdebug() << "eliminando...";

    std::vector<Terreno*> sobrevivientes;
    for (int i = 0; i < (int)terrenos.size(); i++) {
        if (terrenos[i]->fueraDePantalla()) {
            scene->removeItem(terrenos[i]);
            delete terrenos[i];
        } else {
            sobrevivientes.push_back(terrenos[i]);
        }
    }
    terrenos = sobrevivientes;
    //qdebug() << "eliminacion ok";

}

MainWindow::~MainWindow() {}
