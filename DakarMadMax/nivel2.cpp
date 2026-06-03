#include "nivel2.h"
#include <QPainter>
#include <QFont>
#include <cmath>

// ── Constantes de layout ──────────────────────────────────────────────────────
static const int   ANCHO_VISTA   = 800;
static const int   ALTO_VISTA    = 400;
static const float SUELO_Y       = 300.0f;   // Y donde descansa el vehículo (tope superior del suelo)
static const float ALTO_SUELO    = 100.0f;   // grosor del suelo
static const float VEH_X        = 120.0f;   // X fija del vehículo en pantalla
static const float GRAVEDAD      = 0.6f;
static const float FUERZA_SALTO  = -14.0f;  // velocidad inicial del salto (negativa = arriba)
static const int   DIST_TOTAL_FR = 1800;     // frames hasta la meta (~30 s a 60 fps)

Nivel2::Nivel2(TipoVehiculo tipo, QWidget *parent)
    : QWidget(parent),
    tipoVehiculo(tipo),
    velocidadMundo(4.0f),
    contadorAgujero(120),
    intervaloAgujero(260),
    letraRequerida('A'),
    esperandoTecla(false),
    tiempoLimiteLetra(180),   // 3 segundos a 60 fps
    contadorLetra(0),
    enSalto(false),
    agujeroObjetivo(0),
    anchoAgujeroActual(0),
    xMeta(0),
    distanciaTotal(DIST_TOTAL_FR),
    frameActual(0),
    estado(EstadoNivel2::Corriendo)
{
    setFixedSize(ANCHO_VISTA, ALTO_VISTA);
    setFocusPolicy(Qt::StrongFocus);

    // ── Escena y vista ────────────────────────────────────────────────────
    scene = new QGraphicsScene(0, 0, ANCHO_VISTA, ALTO_VISTA, this);
    view  = new QGraphicsView(scene, this);
    view->setFixedSize(ANCHO_VISTA, ALTO_VISTA);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setSceneRect(0, 0, ANCHO_VISTA, ALTO_VISTA);
    view->move(0, 0);

    // ── Fondo ────────────────────────────────────────────────────────────
    QPixmap pxFondo(":/imagenes/fondo1.png");
    if (pxFondo.isNull()) {
        // fallback: degradado celeste/arena pintado en un pixmap
        QPixmap fb(ANCHO_VISTA, ALTO_VISTA);
        QPainter p(&fb);
        p.fillRect(0, 0, ANCHO_VISTA, (int)SUELO_Y, QColor(135, 180, 220));
        p.fillRect(0, (int)SUELO_Y, ANCHO_VISTA, (int)ALTO_SUELO, QColor(180, 140, 80));
        p.end();
        pxFondo = fb;
    }
    QPixmap fondoEscalado = pxFondo.scaled(ANCHO_VISTA, (int)SUELO_Y,
                                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    fondo1 = new QGraphicsPixmapItem(fondoEscalado);
    fondo1->setPos(0, 0);
    fondo1->setZValue(-2);
    scene->addItem(fondo1);

    fondo2 = new QGraphicsPixmapItem(fondoEscalado);
    fondo2->setPos(ANCHO_VISTA, 0);
    fondo2->setZValue(-2);
    scene->addItem(fondo2);

    // ── Suelo (dos segmentos para el loop) ───────────────────────────────
    QColor colorSuelo(160, 120, 60);
    suelo1 = new QGraphicsRectItem(0, SUELO_Y, ANCHO_VISTA, ALTO_SUELO);
    suelo1->setBrush(colorSuelo);
    suelo1->setPen(Qt::NoPen);
    suelo1->setZValue(1);
    scene->addItem(suelo1);

    suelo2 = new QGraphicsRectItem(ANCHO_VISTA, SUELO_Y, ANCHO_VISTA, ALTO_SUELO);
    suelo2->setBrush(colorSuelo);
    suelo2->setPen(Qt::NoPen);
    suelo2->setZValue(1);
    scene->addItem(suelo2);

    // ── Vehículo ──────────────────────────────────────────────────────────
    QString rutaSprite;
    int ancho, alto;
    switch (tipoVehiculo) {
        case TipoVehiculo::Moto:
            rutaSprite = ":/imagenes/motoCenital.png"; ancho = 80; alto = 50; break;
        case TipoVehiculo::CarroDakar:
            rutaSprite = ":/imagenes/carroCenital.png"; ancho = 100; alto = 55; break;
        case TipoVehiculo::Camion:
            rutaSprite = ":/imagenes/camionCenital.png"; ancho = 110; alto = 65; break;
    }

    QPixmap pxVeh(rutaSprite);
    if (pxVeh.isNull()) {
        pxVeh = QPixmap(ancho, alto);
        pxVeh.fill(QColor(200, 60, 60));
    } else {
        // El sprite cenital mira hacia arriba; lo rotamos para vista lateral
        QTransform tr;
        tr.rotate(0);   // ajusta si el sprite queda al revés
        pxVeh = pxVeh.transformed(tr, Qt::SmoothTransformation);
        pxVeh = pxVeh.scaled(ancho, alto, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    vehiculo = new QGraphicsPixmapItem(pxVeh);
    vehYBase  = SUELO_Y - alto;
    vehY      = vehYBase;
    velY      = 0.0f;
    vehiculo->setPos(VEH_X, vehY);
    vehiculo->setZValue(3);
    scene->addItem(vehiculo);

    // ── Meta (rectángulo a cuadros) ───────────────────────────────────────
    // Se coloca lejos a la derecha y va llegando conforme avanza el mundo
    xMeta = VEH_X + ANCHO_VISTA * 2.5f;   // distancia inicial en coords de escena
    QPixmap pxMeta(30, (int)ALTO_SUELO);
    QPainter pm(&pxMeta);
    int cell = 10;
    for (int r = 0; r * cell < (int)ALTO_SUELO; r++)
        for (int c = 0; c * cell < 30; c++)
            pm.fillRect(c*cell, r*cell, cell, cell,
                        ((r+c)%2 == 0) ? Qt::white : Qt::black);
    pm.end();
    meta = new QGraphicsPixmapItem(pxMeta);
    meta->setPos(xMeta, SUELO_Y);
    meta->setZValue(2);
    scene->addItem(meta);

    // ── HUD: distancia ────────────────────────────────────────────────────
    labelDistancia = new QLabel("Distancia: 0%", this);
    labelDistancia->setGeometry(10, 8, 220, 28);
    labelDistancia->setStyleSheet(
        "color: white; font-size: 14px; font-weight: bold;"
        "background-color: rgba(0,0,0,140); border-radius: 5px; padding: 2px 8px;");
    labelDistancia->show();

    // ── HUD: letra requerida ──────────────────────────────────────────────
    labelLetra = new QLabel("", this);
    labelLetra->setGeometry(330, 120, 140, 130);
    labelLetra->setAlignment(Qt::AlignCenter);
    labelLetra->setStyleSheet(
        "color: #FFD700; font-size: 72px; font-weight: bold;"
        "background-color: rgba(0,0,0,180); border: 3px solid #FFD700;"
        "border-radius: 12px;");
    labelLetra->hide();

    // ── HUD: pista de texto ───────────────────────────────────────────────
    labelPista = new QLabel("", this);
    labelPista->setGeometry(240, 260, 320, 32);
    labelPista->setAlignment(Qt::AlignCenter);
    labelPista->setStyleSheet(
        "color: white; font-size: 13px; font-weight: bold;"
        "background-color: rgba(0,0,0,160); border-radius: 6px;");
    labelPista->hide();

    // ── HUD: mensaje final ────────────────────────────────────────────────
    labelMensaje = new QLabel("", this);
    labelMensaje->setGeometry(150, 140, 500, 80);
    labelMensaje->setAlignment(Qt::AlignCenter);
    labelMensaje->setStyleSheet(
        "color: #FFD700; font-size: 36px; font-weight: bold;"
        "background-color: rgba(0,0,0,200); border-radius: 14px;");
    labelMensaje->hide();

    // ── Timer ─────────────────────────────────────────────────────────────
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Nivel2::gameLoop);
    timer->start(16);   // ~60 fps
}

Nivel2::~Nivel2() {}

// ── Game loop principal ───────────────────────────────────────────────────────
void Nivel2::gameLoop() {
    if (estado == EstadoNivel2::Ganando || estado == EstadoNivel2::Perdiendo) return;

    frameActual++;

    // Progreso
    int pct = (int)(frameActual * 100.0f / distanciaTotal);
    if (pct > 100) pct = 100;
    labelDistancia->setText(QString("Meta: %1%").arg(pct));

    // ── Verificar llegada a la meta ───────────────────────────────────────
    float xMetaEnPantalla = xMeta - (frameActual * velocidadMundo) + VEH_X * 0;
    // La meta se desplaza igual que el mundo: cada frame avanza velocidadMundo hacia la izquierda
    meta->setX(meta->x() - velocidadMundo);
    if (meta->x() <= VEH_X + 40) {
        finalizarNivel(true);
        return;
    }

    // ── Fondo en loop ─────────────────────────────────────────────────────
    actualizarFondo();

    // ── Suelo ─────────────────────────────────────────────────────────────
    actualizarSuelo();

    // ── Agujeros ──────────────────────────────────────────────────────────
    actualizarAgujeros();

    // ── Generar agujeros periódicamente ───────────────────────────────────
    if (estado == EstadoNivel2::Corriendo) {
        contadorAgujero--;
        if (contadorAgujero <= 0) {
            generarAgujero();
            // Intervalo decrece con el tiempo para dificultar
            intervaloAgujero = qMax(160, intervaloAgujero - 8);
            contadorAgujero  = intervaloAgujero;
        }
    }

    // ── Física del vehículo ───────────────────────────────────────────────
    if (estado == EstadoNivel2::Saltando) {
        velY += GRAVEDAD;
        vehY += velY;
        vehiculo->setY(vehY);

        if (vehY >= vehYBase) {
            vehY = vehYBase;
            velY = 0.0f;
            vehiculo->setY(vehY);
            estado = EstadoNivel2::Corriendo;
            ocultarLetra();
        }
    } else if (estado == EstadoNivel2::Cayendo) {
        velY += GRAVEDAD;
        vehY += velY;
        vehiculo->setY(vehY);

        if (vehY > ALTO_VISTA + 50) {
            finalizarNivel(false);
        }
    }

    // ── Espera de tecla: tiempo límite ────────────────────────────────────
    if (esperandoTecla && estado == EstadoNivel2::MostrandoLetra) {
        contadorLetra++;
        // Cambiar color del borde a rojo cuando queda poco tiempo
        if (contadorLetra > tiempoLimiteLetra * 0.6f) {
            labelLetra->setStyleSheet(
                "color: #FF4444; font-size: 72px; font-weight: bold;"
                "background-color: rgba(0,0,0,180); border: 3px solid #FF4444;"
                "border-radius: 12px;");
        }
        if (contadorLetra >= tiempoLimiteLetra) {
            // Se acabó el tiempo → cae
            esperandoTecla = false;
            estado = EstadoNivel2::Cayendo;
            velY = 2.0f;
            ocultarLetra();
        }
    }
}

// ── Input ─────────────────────────────────────────────────────────────────────
void Nivel2::keyPressEvent(QKeyEvent *event) {
    if (estado != EstadoNivel2::MostrandoLetra || !esperandoTecla) return;

    int key = event->key();
    // Qt::Key_A = 65, Qt::Key_Z = 90
    if (key < Qt::Key_A || key > Qt::Key_Z) return;

    char presionada = (char)('A' + (key - Qt::Key_A));

    if (presionada == letraRequerida) {
        // ¡Correcto! → saltar
        esperandoTecla = false;
        iniciarSalto();
    } else {
        // Incorrecto → cae
        esperandoTecla = false;
        estado = EstadoNivel2::Cayendo;
        velY = 2.0f;
        ocultarLetra();
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────────
void Nivel2::iniciarSalto() {
    estado = EstadoNivel2::Saltando;
    velY   = FUERZA_SALTO;
    // Ocultar letra (la ocultamos al aterrizar para que el jugador vea que acertó)
    labelLetra->hide();
    labelPista->hide();
}

void Nivel2::mostrarLetra(float /*xAgujero*/, float /*anchoAgujero*/) {
    // Elegir letra aleatoria A-Z
    letraRequerida = (char)('A' + QRandomGenerator::global()->bounded(26));
    labelLetra->setText(QString(letraRequerida));
    labelLetra->setStyleSheet(
        "color: #FFD700; font-size: 72px; font-weight: bold;"
        "background-color: rgba(0,0,0,180); border: 3px solid #FFD700;"
        "border-radius: 12px;");
    labelLetra->show();
    labelPista->setText(QString("¡Presiona  \"%1\"  para saltar!").arg(letraRequerida));
    labelPista->show();

    esperandoTecla = true;
    contadorLetra  = 0;
    estado = EstadoNivel2::MostrandoLetra;
}

void Nivel2::ocultarLetra() {
    labelLetra->hide();
    labelPista->hide();
}

void Nivel2::generarAgujero() {
    // Ancho aleatorio entre 100 y 200 px
    float ancho = 100.0f + QRandomGenerator::global()->bounded(101);
    // Siempre aparece justo al borde derecho de la pantalla
    float xInicio = ANCHO_VISTA + 20.0f;

    // Rectángulo negro que tapa el suelo → simula el hueco
    QGraphicsRectItem *rect = new QGraphicsRectItem(xInicio, SUELO_Y, ancho, ALTO_SUELO + 10);
    rect->setBrush(Qt::black);
    rect->setPen(Qt::NoPen);
    rect->setZValue(2);   // encima del suelo
    scene->addItem(rect);

    Agujero ag;
    ag.rect         = rect;
    ag.xEscena      = xInicio;
    ag.ancho        = ancho;
    ag.letraMostrada = false;
    ag.superado     = false;
    agujeros.append(ag);
}

void Nivel2::actualizarSuelo() {
    // Los dos segmentos de suelo se mueven hacia la izquierda
    suelo1->setX(suelo1->x() - velocidadMundo);
    suelo2->setX(suelo2->x() - velocidadMundo);

    // Loop: cuando un segmento sale por la izquierda, se recoloca a la derecha
    if (suelo1->x() + ANCHO_VISTA < 0)
        suelo1->setX(suelo2->x() + ANCHO_VISTA);
    if (suelo2->x() + ANCHO_VISTA < 0)
        suelo2->setX(suelo1->x() + ANCHO_VISTA);
}

void Nivel2::actualizarAgujeros() {
    QVector<Agujero> vivos;

    for (Agujero &ag : agujeros) {
        // Mover agujero hacia la izquierda
        ag.xEscena -= velocidadMundo;
        ag.rect->setX(ag.xEscena);

        float xDerAgujero = ag.xEscena + ag.ancho;

        // ── Mostrar letra cuando el borde izquierdo del agujero
        //    esté a ~200 px del vehículo ─────────────────────────────────
        if (!ag.letraMostrada && !ag.superado) {
            float distancia = ag.xEscena - VEH_X;
            if (distancia < 200.0f && distancia > 0.0f
                && estado == EstadoNivel2::Corriendo) {
                ag.letraMostrada = true;
                mostrarLetra(ag.xEscena, ag.ancho);
            }
        }

        // ── Detectar si el vehículo está sobre el agujero ─────────────
        float vehXDer = VEH_X + vehiculo->boundingRect().width();
        bool sobreAgujero = (vehXDer > ag.xEscena) && (VEH_X < xDerAgujero);

        if (sobreAgujero && estado == EstadoNivel2::Corriendo) {
            // No saltó → cae
            estado = EstadoNivel2::Cayendo;
            velY   = 2.0f;
            ocultarLetra();
        }

        // Marcar superado cuando el agujero pasó completamente al jugador
        if (!ag.superado && xDerAgujero < VEH_X - 10.0f) {
            ag.superado = true;
        }

        // Eliminar agujeros que salieron por la izquierda
        if (ag.xEscena + ag.ancho < -50) {
            scene->removeItem(ag.rect);
            delete ag.rect;
        } else {
            vivos.append(ag);
        }
    }

    agujeros = vivos;
}

void Nivel2::actualizarFondo() {
    fondo1->setX(fondo1->x() - velocidadMundo * 0.4f);   // paralaje más lento
    fondo2->setX(fondo2->x() - velocidadMundo * 0.4f);

    if (fondo1->x() + ANCHO_VISTA < 0)
        fondo1->setX(fondo2->x() + ANCHO_VISTA);
    if (fondo2->x() + ANCHO_VISTA < 0)
        fondo2->setX(fondo1->x() + ANCHO_VISTA);
}

void Nivel2::finalizarNivel(bool exito) {
    timer->stop();
    estado = exito ? EstadoNivel2::Ganando : EstadoNivel2::Perdiendo;
    ocultarLetra();

    if (exito) {
        labelMensaje->setText("🏁  ¡LLEGASTE A LA META!");
        labelMensaje->setStyleSheet(
            "color: #FFD700; font-size: 30px; font-weight: bold;"
            "background-color: rgba(0,0,0,210); border-radius: 14px;");
    } else {
        labelMensaje->setText("💥  ¡Caíste en el agujero!");
        labelMensaje->setStyleSheet(
            "color: #FF4444; font-size: 30px; font-weight: bold;"
            "background-color: rgba(0,0,0,210); border-radius: 14px;");
    }
    labelMensaje->show();

    // Esperar 2.5 s y emitir señal
    QTimer::singleShot(2500, this, [this, exito]() {
        if (exito) emit nivelCompletado();
        else       emit nivelFallado();
    });
}
