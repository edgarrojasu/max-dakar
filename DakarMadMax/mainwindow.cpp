#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), jugador(nullptr), rival(nullptr),
    escenario(nullptr), timer(nullptr),
    lineaMeta(nullptr), tiempoRestante(7200), juegoTerminado(false)
{
    setFixedSize(800, 600);
    setWindowTitle("Dakar Mad Max");

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    pantallaSeleccion = new SeleccionVehiculo();
    stack->addWidget(pantallaSeleccion);

    view = new QGraphicsView();
    view->setFixedSize(800, 600);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    stack->addWidget(view);

    connect(pantallaSeleccion, &SeleccionVehiculo::vehiculoSeleccionado,
            this, &MainWindow::iniciarJuego);

    stack->setCurrentIndex(0);
}

void MainWindow::iniciarJuego(TipoVehiculo tipo) {
    scene = new QGraphicsScene(0, 0, 800, 600, this);
    view->setScene(scene);
    escenario = new Escenario(scene);

    // Jugador
    jugador = new Vehiculo(200, 500, tipo);
    jugador->setZValue(2);
    scene->addItem(jugador);

    // Rival — elige un tipo distinto al jugador
    TipoVehiculo tipoRival;
    if (tipo == TipoVehiculo::Moto)
        tipoRival = TipoVehiculo::CarroDakar;
    else if (tipo == TipoVehiculo::CarroDakar)
        tipoRival = TipoVehiculo::Camion;
    else
        tipoRival = TipoVehiculo::Moto;

    rival = new Vehiculo(400, 500, tipoRival);
    rival->setZValue(2);
    scene->addItem(rival);

    // Label del contador de tiempo — se agrega al view, no a la escena
    labelTiempo = new QLabel("2:00", view);
    labelTiempo->setGeometry(350, 10, 100, 30);
    labelTiempo->setAlignment(Qt::AlignCenter);
    labelTiempo->setStyleSheet(
        "color: white; font-size: 18px; font-weight: bold;"
        "background-color: rgba(0,0,0,150); border-radius: 6px;"
        );
    labelTiempo->show();

    tiempoRestante = 7200;
    juegoTerminado = false;
    lineaMeta      = nullptr;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::gameLoop);
    view->setSceneRect(0, 0, 600, 800);
    timer->start(16);

    stack->setCurrentIndex(1);
    jugador->setFlag(QGraphicsItem::ItemIsFocusable);
    jugador->setFocus();
}

void MainWindow::gameLoop() {
    if (juegoTerminado) return;

    escenario->desplazar();
    jugador->actualizar();
    // rival no se mueve por ahora

    // ── CONTADOR DE TIEMPO ────────────────────────────────────────
    tiempoRestante--;
    int minutos  = tiempoRestante / (60 * 60);
    int segundos = (tiempoRestante / 60) % 60;
    labelTiempo->setText(QString("%1:%2")
                             .arg(minutos)
                             .arg(segundos, 2, 10, QChar('0')));

    // ── APARECE LA META cuando llega a 0 ─────────────────────────
    if (tiempoRestante == 0 && lineaMeta == nullptr) {
        lineaMeta = new QGraphicsRectItem(0, 50, 600, 8);
        lineaMeta->setBrush(QBrush(Qt::white));
        lineaMeta->setPen(Qt::NoPen);
        lineaMeta->setZValue(3);
        scene->addItem(lineaMeta);
    }

    // ── LA META BAJA igual que los terrenos ───────────────────────
    if (lineaMeta != nullptr) {
        lineaMeta->setY(lineaMeta->y() + 10.0f);

        // Detectar quién cruza primero
        QRectF rMeta    = lineaMeta->mapToScene(lineaMeta->boundingRect()).boundingRect();
        QRectF rJugador = jugador->mapToScene(jugador->boundingRect()).boundingRect();
        QRectF rRival   = rival->mapToScene(rival->boundingRect()).boundingRect();

        QString ganador = "";
        if (rJugador.intersects(rMeta))
            ganador = "¡GANASTE!";
        else if (rRival.intersects(rMeta))
            ganador = "El rival ganó";

        if (!ganador.isEmpty()) {
            juegoTerminado = true;
            timer->stop();
            QMessageBox::information(this, "Fin de la carrera", ganador);
        }
    }

    // ── GENERACIÓN DE TERRENOS ────────────────────────────────────
    contadorFrames++;
    if (contadorFrames >= 200) {
        contadorFrames = 0;
        int x     = QRandomGenerator::global()->bounded(20, 400);
        int ancho = QRandomGenerator::global()->bounded(100, 140);
        int alto  = QRandomGenerator::global()->bounded(1000, 2000);
        Terreno *t = new Carretera((float)x, -(float)alto, (float)ancho, (float)alto);
        scene->addItem(t);
        terrenos.push_back(t);
    }

    contadorFango++;
    if (contadorFango >= 200) {
        contadorFango = 0;
        int x     = QRandomGenerator::global()->bounded(20, 400);
        int ancho = QRandomGenerator::global()->bounded(100, 200);
        int alto  = QRandomGenerator::global()->bounded(300, 500);
        Terreno *t = new Fango((float)x, -(float)alto, (float)ancho, (float)alto);
        scene->addItem(t);
        terrenos.push_back(t);
    }

    contadorLlantas++;
    if (contadorLlantas >= 150) {
        contadorLlantas = 0;
        float x   = (float)QRandomGenerator::global()->bounded(50, 500);
        float vel = 2.0f + QRandomGenerator::global()->bounded(0, 20) * 0.1f;
        Llanta *l = new Llanta(x, vel);
        QRectF rJugador = jugador->mapToScene(jugador->boundingRect()).boundingRect();
        if (!rJugador.intersects(QRectF(x, -60, 60, 60))) {
            scene->addItem(l);
            terrenos.push_back(l);
        } else {
            delete l;
        }
    }

    // ── MOVER TERRENOS ────────────────────────────────────────────
    for (int i = 0; i < (int)terrenos.size(); i++)
        terrenos[i]->actualizar(10.0f);

    // ── COLISIONES ────────────────────────────────────────────────
    bool sobreTerreno = false;
    bool sobreFango   = false;

    for (int i = 0; i < (int)terrenos.size(); i++) {
        if (!terrenos[i]->scene()) continue;
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

    // ── LIMPIAR TERRENOS FUERA DE PANTALLA ────────────────────────
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
}

MainWindow::~MainWindow() {}
