#include "nivel2.h"
#include <QPainter>
#include <QFont>
#include <QUrl>
#include <cmath>

// ── Constantes de layout ──────────────────────────────────────────────────────
static const int   ANCHO_VISTA     = 800;
static const int   ALTO_VISTA      = 400;
static const float SUELO_Y         = 300.0f;   // tope superior del suelo
static const float ALTO_SUELO      = 100.0f;   // grosor visible del suelo
static const float VEH_X           = 120.0f;   // X fija del vehículo
static const float GRAVEDAD        = 0.4f;
static const float FUERZA_SALTO    = -18.0f;
static const int   TOTAL_AGUJEROS  = 15;       // saltos para ganar

// Spritesheet motoN2: 3 cols × 1 fila = 3 frames
static const int   SPRITE_COLS     = 3;
static const int   SPRITE_FILAS    = 1;
static const int   SPRITE_FRAMES   = SPRITE_COLS * SPRITE_FILAS;

Nivel2::Nivel2(TipoVehiculo tipo, QWidget *parent)
    : QWidget(parent),
    tipoVehiculo(tipo),
    velocidadMundo(4.0f),
    contadorAgujero(180),       // frames hasta el primer agujero
    intervaloAgujero(300),      // separación inicial entre agujeros
    letraRequerida('A'),
    esperandoTecla(false),
    tiempoLimiteLetra(90),      // fijo — ver mostrarLetra() para cambiarlo
    contadorLetra(0),
    enSalto(false),
    agujeroObjetivo(0),
    anchoAgujeroActual(0),
    agujerosSuperados(0),
    frameActual(0),
    frameSprite(0),
    contadorFrameSprite(0),
    estado(EstadoNivel2::Corriendo),
    musicaNivel2(nullptr), audioNivel2(nullptr),
    sfxSalto(nullptr), audioSfxSalto(nullptr)
{
    setFixedSize(ANCHO_VISTA, ALTO_VISTA);
    setFocusPolicy(Qt::StrongFocus);

    // ── Escena y vista ─────────────────────────────────────────────────────
    scene = new QGraphicsScene(0, 0, ANCHO_VISTA, ALTO_VISTA, this);
    view  = new QGraphicsView(scene, this);
    view->setFixedSize(ANCHO_VISTA, ALTO_VISTA);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameStyle(0);
    view->setSceneRect(0, 0, ANCHO_VISTA, ALTO_VISTA);
    view->move(0, 0);

    // ── Fondo (fondoN2.png) ────────────────────────────────────────────────
    QPixmap pxFondo(":/imagenes/fondoN2.png");
    if (pxFondo.isNull()) {
        pxFondo = QPixmap(ANCHO_VISTA, (int)SUELO_Y);
        pxFondo.fill(QColor(135, 180, 220));
    }
    QPixmap fondoEsc = pxFondo.scaled(ANCHO_VISTA, (int)SUELO_Y,
                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    fondo1 = new QGraphicsPixmapItem(fondoEsc);
    fondo1->setPos(0, 0);
    fondo1->setZValue(-2);
    scene->addItem(fondo1);

    fondo2 = new QGraphicsPixmapItem(fondoEsc);
    fondo2->setPos(ANCHO_VISTA, 0);
    fondo2->setZValue(-2);
    scene->addItem(fondo2);

    // ── Suelo en loop (suelo2.png) ─────────────────────────────────────────
    QPixmap pxSuelo(":/imagenes/suelo2.png");
    if (pxSuelo.isNull()) {
        pxSuelo = QPixmap(ANCHO_VISTA, (int)ALTO_SUELO);
        pxSuelo.fill(QColor(160, 120, 60));
    }
    QPixmap sueloEsc = pxSuelo.scaled(ANCHO_VISTA, (int)ALTO_SUELO,
                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    suelo1 = new QGraphicsPixmapItem(sueloEsc);
    suelo1->setPos(0, SUELO_Y);
    suelo1->setZValue(1);
    scene->addItem(suelo1);

    suelo2 = new QGraphicsPixmapItem(sueloEsc);
    suelo2->setPos(ANCHO_VISTA, SUELO_Y);
    suelo2->setZValue(1);
    scene->addItem(suelo2);

    // ── Vehículo ──────────────────────────────────────────────────────────
    // Spritesheet motoN2.png — 3 cols × 1 fila = 3 frames
    QPixmap sheet(":/imagenes/motoN2.png");
    if (!sheet.isNull()) {
        struct FrameRect { int x, y, w, h; };
        QVector<FrameRect> frameRects = {
            { 48,   0, 490, 630 },
            { 638,  0, 490, 630 },
            { 1225, 0, 490, 630 },
        };
        for (const FrameRect &fr : frameRects) {
            QPixmap frame = sheet.copy(fr.x, fr.y, fr.w, fr.h);
            frame = frame.transformed(QTransform().scale(-1, 1));

            // ── TAMAÑO DE LA MOTO ─────────────────────────────────────────
            // ancho: qué tan larga se ve la moto
            // alto:  controla la posición sobre el suelo (vehYBase = SUELO_Y - alto + VEH_Y_OFFSET)
            frame = frame.scaled(180, 200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            // ─────────────────────────────────────────────────────────────

            motoFrames.append(frame);
        }
    } else {
        QPixmap fb(180, 200); fb.fill(Qt::red);
        for (int i = 0; i < SPRITE_FRAMES; i++) motoFrames.append(fb);
    }

    vehiculo = new QGraphicsPixmapItem(motoFrames[0]);

    // ── POSICIÓN VERTICAL DE LA MOTO ─────────────────────────────────────
    // VEH_Y_OFFSET desplaza la moto hacia abajo sin cambiar su tamaño.
    // Número positivo = baja, número negativo = sube.
    // Ajústalo hasta que las ruedas toquen visualmente el suelo.
    const float VEH_Y_OFFSET = 60.0f;
    // ─────────────────────────────────────────────────────────────────────

    vehYBase  = SUELO_Y - motoFrames[0].height() + VEH_Y_OFFSET;
    vehY      = vehYBase;
    velY      = 0.0f;
    vehiculo->setPos(VEH_X, vehY);
    vehiculo->setZValue(3);
    scene->addItem(vehiculo);

    // ── Meta (bandera a cuadros) ───────────────────────────────────────────
    // Se coloca lejos a la derecha; irá acercándose con el mundo
    QPixmap pxMeta(30, (int)ALTO_SUELO);
    QPainter pm(&pxMeta);
    int cell = 10;
    for (int r = 0; r*cell < (int)ALTO_SUELO; r++)
        for (int c = 0; c*cell < 30; c++)
            pm.fillRect(c*cell, r*cell, cell, cell,
                        ((r+c)%2==0) ? Qt::white : Qt::black);
    pm.end();
    meta = new QGraphicsPixmapItem(pxMeta);
    // La meta debe llegar DESPUÉS de que se superen los 15 agujeros.
    // Cada agujero tarda ~intervaloAgujero frames en generarse (empieza en 300,
    // baja hasta 100). Promedio ~200 frames × 15 agujeros × velocidadMundo(4) = 12000 px
    // Le sumamos margen extra para que no llegue antes de tiempo.
    meta->setPos(ANCHO_VISTA + 25000.0f, SUELO_Y - 60);
    meta->setZValue(4);
    scene->addItem(meta);

    // ── HUD: contador de agujeros ──────────────────────────────────────────
    labelDistancia = new QLabel("Agujeros: 0 / 15", this);
    labelDistancia->setGeometry(10, 8, 200, 28);
    labelDistancia->setStyleSheet(
        "color: white; font-size: 13px; font-weight: bold;"
        "background-color: rgba(0,0,0,150); border-radius: 5px; padding: 2px 8px;");
    labelDistancia->show();

    // ── HUD: letra requerida ───────────────────────────────────────────────
    labelLetra = new QLabel("", this);
    labelLetra->setGeometry(330, 100, 140, 130);
    labelLetra->setAlignment(Qt::AlignCenter);
    labelLetra->setStyleSheet(
        "color: #FFD700; font-size: 72px; font-weight: bold;"
        "background-color: rgba(0,0,0,185); border: 3px solid #FFD700;"
        "border-radius: 12px;");
    labelLetra->hide();

    // ── HUD: pista ─────────────────────────────────────────────────────────
    labelPista = new QLabel("", this);
    labelPista->setGeometry(220, 245, 360, 30);
    labelPista->setAlignment(Qt::AlignCenter);
    labelPista->setStyleSheet(
        "color: white; font-size: 13px; font-weight: bold;"
        "background-color: rgba(0,0,0,160); border-radius: 6px;");
    labelPista->hide();

    // ── HUD: barra de tiempo ───────────────────────────────────────────────
    labelTiempoBar = new QLabel("", this);
    labelTiempoBar->setGeometry(330, 240, 140, 10);
    labelTiempoBar->setStyleSheet("background-color: #FFD700; border-radius: 4px;");
    labelTiempoBar->hide();

    // ── HUD: mensaje final ─────────────────────────────────────────────────
    labelMensaje = new QLabel("", this);
    labelMensaje->setGeometry(100, 130, 600, 90);
    labelMensaje->setAlignment(Qt::AlignCenter);
    labelMensaje->setStyleSheet(
        "color: #FFD700; font-size: 32px; font-weight: bold;"
        "background-color: rgba(0,0,0,210); border-radius: 14px;");
    labelMensaje->hide();

    // ── Timer ──────────────────────────────────────────────────────────────
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Nivel2::gameLoop);
    timer->start(16);

    // ── Música de fondo nivel 2 ────────────────────────────────────────────
    musicaNivel2 = new QMediaPlayer(this);
    audioNivel2  = new QAudioOutput(this);
    musicaNivel2->setAudioOutput(audioNivel2);
    musicaNivel2->setSource(QUrl("qrc:/sonido/sonido/nivel2.m4a"));
    audioNivel2->setVolume(0.6f);   // 0.0 silencio — 1.0 máximo
    musicaNivel2->setLoops(QMediaPlayer::Infinite);
    musicaNivel2->play();

    // ── SFX: sonido al saltar correctamente ───────────────────────────────
    sfxSalto     = new QMediaPlayer(this);
    audioSfxSalto = new QAudioOutput(this);
    sfxSalto->setAudioOutput(audioSfxSalto);
    sfxSalto->setSource(QUrl("qrc:/sonido/sonido/wheelie_1.m4a"));
    audioSfxSalto->setVolume(0.8f);
}

Nivel2::~Nivel2() {}

// ── Helpers de gameLoop() ─────────────────────────────────────────────────────

// Avanza el frame del spritesheet solo cuando la moto se mueve
void Nivel2::animarSprite()
{
    if (estado != EstadoNivel2::Corriendo && estado != EstadoNivel2::Saltando) return;

    contadorFrameSprite++;
    if (contadorFrameSprite >= 5) {
        contadorFrameSprite = 0;
        frameSprite = (frameSprite + 1) % SPRITE_FRAMES;
        vehiculo->setPixmap(motoFrames[frameSprite]);
    }
}

// Mueve la meta hacia el vehículo y finaliza el nivel si llega
void Nivel2::avanzarMeta()
{
    meta->setX(meta->x() - velocidadMundo);
    if (meta->x() <= VEH_X + 20)
        finalizarNivel(true);
}

// Genera agujeros periódicamente mientras no se hayan superado todos
void Nivel2::gestionarAgujeros()
{
    if (estado != EstadoNivel2::Corriendo) return;
    if (agujerosSuperados >= TOTAL_AGUJEROS) return;

    contadorAgujero--;
    if (contadorAgujero <= 0) {
        generarAgujero();
        intervaloAgujero = qMax(80, intervaloAgujero - 12);
        contadorAgujero  = intervaloAgujero;
    }
}

// Aplica gravedad al salto o a la caída; aterriza o finaliza según el caso
void Nivel2::actualizarFisicaVehiculo()
{
    if (estado != EstadoNivel2::Saltando && estado != EstadoNivel2::Cayendo) return;

    velY += GRAVEDAD;
    vehY += velY;
    vehiculo->setY(vehY);

    if (estado == EstadoNivel2::Saltando && vehY >= vehYBase) {
        vehY  = vehYBase;
        velY  = 0.0f;
        vehiculo->setY(vehY);
        estado = EstadoNivel2::Corriendo;
        ocultarLetra();
    }

    if (estado == EstadoNivel2::Cayendo && vehY > ALTO_VISTA + 60)
        finalizarNivel(false);
}

// Actualiza la barra de tiempo y provoca caída si se agota el tiempo
void Nivel2::actualizarTemporizadorLetra()
{
    if (!esperandoTecla || estado != EstadoNivel2::MostrandoLetra) return;

    contadorLetra++;
    float pct = 1.0f - (float)contadorLetra / tiempoLimiteLetra;
    labelTiempoBar->setGeometry(330, 243, qMax((int)(140 * pct), 0), 8);

    if (pct < 0.4f) {
        labelLetra->setStyleSheet(
            "color: #FF4444; font-size: 72px; font-weight: bold;"
            "background-color: rgba(0,0,0,185); border: 3px solid #FF4444;"
            "border-radius: 12px;");
        labelTiempoBar->setStyleSheet("background-color: #FF4444; border-radius: 4px;");
    }

    if (contadorLetra >= tiempoLimiteLetra) {
        esperandoTecla = false;
        estado = EstadoNivel2::Cayendo;
        velY   = 2.0f;
        ocultarLetra();
    }
}

// ── Game loop ─────────────────────────────────────────────────────────────────
void Nivel2::gameLoop()
{
    if (estado == EstadoNivel2::Ganando || estado == EstadoNivel2::Perdiendo) return;

    frameActual++;
    animarSprite();
    actualizarFondo();
    actualizarSuelo();
    avanzarMeta();
    if (estado == EstadoNivel2::Ganando) return;   // avanzarMeta() puede haber finalizado
    actualizarAgujeros(true);
    gestionarAgujeros();
    actualizarFisicaVehiculo();
    if (estado == EstadoNivel2::Perdiendo) return; // fisicaVehiculo puede haber finalizado
    actualizarTemporizadorLetra();
}

// ── Input ─────────────────────────────────────────────────────────────────────
void Nivel2::keyPressEvent(QKeyEvent *event) {
    if (estado != EstadoNivel2::MostrandoLetra || !esperandoTecla) return;

    int key = event->key();
    if (key < Qt::Key_A || key > Qt::Key_Z) return;

    char presionada = (char)('A' + (key - Qt::Key_A));

    if (presionada == letraRequerida) {
        esperandoTecla = false;
        iniciarSalto();
    } else {
        // Tecla incorrecta → cae
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
    labelLetra->hide();
    labelPista->hide();
    labelTiempoBar->hide();

    // SFX tecla correcta
    if (sfxSalto) {
        sfxSalto->stop();
        sfxSalto->play();
    }
}

void Nivel2::mostrarLetra() {
    letraRequerida = (char)('A' + QRandomGenerator::global()->bounded(26));

    labelLetra->setText(QString(letraRequerida));
    labelLetra->setStyleSheet(
        "color: #FFD700; font-size: 72px; font-weight: bold;"
        "background-color: rgba(0,0,0,185); border: 3px solid #FFD700;"
        "border-radius: 12px;");
    labelLetra->show();

    labelPista->setText(QString("  ¡Presiona  \"%1\"  para saltar!  ").arg(letraRequerida));
    labelPista->show();

    // ── TIEMPO LÍMITE PARA PRESIONAR LA TECLA ────────────────────────────
    // Cambia este valor para ajustar cuánto tiempo tiene el jugador.
    // Está en frames (60 frames = 1 segundo).
    tiempoLimiteLetra = 120;   // 1.5 segundos
    // ─────────────────────────────────────────────────────────────────────
    labelTiempoBar->setGeometry(330, 243, 140, 8);
    labelTiempoBar->setStyleSheet("background-color: #FFD700; border-radius: 4px;");
    labelTiempoBar->show();

    esperandoTecla = true;
    contadorLetra  = 0;
    estado = EstadoNivel2::MostrandoLetra;
}

void Nivel2::ocultarLetra() {
    labelLetra->hide();
    labelPista->hide();
    labelTiempoBar->hide();
}

void Nivel2::generarAgujero() {
    // Ancho crece ligeramente con cada agujero (más difícil)
    float anchoBase = 90.0f + agujerosSuperados * 4.0f;
    float ancho = anchoBase + QRandomGenerator::global()->bounded(60);
    float xInicio = ANCHO_VISTA + 10.0f;
    int anchoI = (int)ancho;
    int altoI  = (int)ALTO_SUELO + 10;

    QGraphicsPixmapItem *imgHueco  = nullptr;
    QGraphicsRectItem   *tapaFondo = nullptr;

    QPixmap pxHueco(":/imagenes/hueco.png");
    if (!pxHueco.isNull()) {
        QPixmap huecoEsc = pxHueco.scaled(anchoI, altoI,
                                           Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        imgHueco = new QGraphicsPixmapItem(huecoEsc);
        imgHueco->setPos(xInicio, SUELO_Y);
        imgHueco->setZValue(2);
        scene->addItem(imgHueco);
    }

    Agujero ag;
    ag.imgItem       = imgHueco;
    ag.tapaFondo     = tapaFondo;
    ag.xEscena       = xInicio;
    ag.ancho         = ancho;
    ag.letraMostrada = false;
    ag.superado      = false;
    agujeros.append(ag);
}

void Nivel2::actualizarSuelo() {
    suelo1->setX(suelo1->x() - velocidadMundo);
    suelo2->setX(suelo2->x() - velocidadMundo);
    if (suelo1->x() + ANCHO_VISTA < 0) suelo1->setX(suelo2->x() + ANCHO_VISTA);
    if (suelo2->x() + ANCHO_VISTA < 0) suelo2->setX(suelo1->x() + ANCHO_VISTA);
}

void Nivel2::actualizarAgujeros(bool mover) {
    QVector<Agujero> vivos;

    for (Agujero &ag : agujeros) {
        if (mover) {
            ag.xEscena -= velocidadMundo;
            if (ag.imgItem)   ag.imgItem->setX(ag.xEscena);
            if (ag.tapaFondo) ag.tapaFondo->setX(ag.xEscena);
        }

        float xDer = ag.xEscena + ag.ancho;

        // Mostrar letra cuando el agujero está a ~350 px del borde derecho
        // de la moto — tiempo suficiente para reaccionar con el mundo corriendo.
        // La letra aparece una sola vez por agujero (letraMostrada lo garantiza).
        if (!ag.letraMostrada && !ag.superado) {
            float distAlVehiculo = ag.xEscena - (VEH_X + vehiculo->boundingRect().width());
            if (distAlVehiculo < 350.0f && distAlVehiculo > 0.0f
                && (estado == EstadoNivel2::Corriendo || estado == EstadoNivel2::MostrandoLetra)) {
                ag.letraMostrada = true;
                mostrarLetra();
            }
        }

        // Detectar caída: solo si el vehículo está corriendo en suelo
        // (nunca durante MostrandoLetra, Saltando ni Cayendo)
        float vehXDer = VEH_X + vehiculo->boundingRect().width();
        bool sobreAgujero = (vehXDer > ag.xEscena + 10) && (VEH_X < xDer - 10);

        if (sobreAgujero && estado == EstadoNivel2::Corriendo && vehY >= vehYBase - 2.0f) {
            estado = EstadoNivel2::Cayendo;
            velY   = 2.0f;
            ocultarLetra();
        }

        // Agujero superado
        if (!ag.superado && xDer < VEH_X - 5.0f) {
            ag.superado = true;
            agujerosSuperados++;
            labelDistancia->setText(QString("Agujeros: %1 / %2")
                                        .arg(agujerosSuperados).arg(TOTAL_AGUJEROS));
        }

        // Limpiar agujeros que ya salieron completamente
        if (ag.xEscena + ag.ancho < -60) {
            if (ag.imgItem)   { scene->removeItem(ag.imgItem);   delete ag.imgItem; }
            if (ag.tapaFondo) { scene->removeItem(ag.tapaFondo); delete ag.tapaFondo; }
        } else {
            vivos.append(ag);
        }
    }
    agujeros = vivos;
}

void Nivel2::actualizarFondo() {
    float velFondo = velocidadMundo * 0.35f;   // paralaje más lento
    fondo1->setX(fondo1->x() - velFondo);
    fondo2->setX(fondo2->x() - velFondo);
    if (fondo1->x() + ANCHO_VISTA < 0) fondo1->setX(fondo2->x() + ANCHO_VISTA);
    if (fondo2->x() + ANCHO_VISTA < 0) fondo2->setX(fondo1->x() + ANCHO_VISTA);
}

void Nivel2::finalizarNivel(bool exito) {
    timer->stop();
    if (musicaNivel2) musicaNivel2->stop();
    estado = exito ? EstadoNivel2::Ganando : EstadoNivel2::Perdiendo;
    ocultarLetra();

    if (exito) {
        labelMensaje->setText("🏁  ¡LLEGASTE A LA META!");
        labelMensaje->setStyleSheet(
            "color: #FFD700; font-size: 30px; font-weight: bold;"
            "background-color: rgba(0,0,0,215); border-radius: 14px;");
    } else {
        labelMensaje->setText("💥  ¡Caíste en el abismo!");
        labelMensaje->setStyleSheet(
            "color: #FF4444; font-size: 30px; font-weight: bold;"
            "background-color: rgba(0,0,0,215); border-radius: 14px;");
    }
    labelMensaje->show();

    QTimer::singleShot(2500, this, [this, exito]() {
        if (exito) emit nivelCompletado();
        else       emit nivelFallado();
    });
}
