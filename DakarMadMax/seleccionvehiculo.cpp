#include "seleccionvehiculo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFont>

// ┌─────────────────────────────────────────────────────────────────────────┐
// │  MODO DEBUG — comenta esta línea para volver al juego normal            │
// └─────────────────────────────────────────────────────────────────────────┘
#define DEBUG_NIVEL2

SeleccionVehiculo::SeleccionVehiculo(QWidget *parent)
    : QWidget(parent)
{
    // Fondo del menú
    QLabel *fondo = new QLabel(this);
    fondo->setGeometry(0, 0, 800, 600);
    QPixmap fondoPx(":/imagenes/fondoMenu.png");
    fondo->setPixmap(fondoPx.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondo->lower();
    setFixedSize(800, 600);
    setStyleSheet("background-color: transparent;");

    // Título
    QLabel *titulo = new QLabel("ELIGE TU VEHÍCULO", this);
    titulo->setGeometry(0, 80, 800, 66.6);
    titulo->setAlignment(Qt::AlignCenter);
    QFont font = titulo->font();
    font.setPointSize(22);
    font.setBold(true);
    titulo->setFont(font);
    titulo->setStyleSheet("color: #e8a020;");

    // Subtítulo
    QLabel *sub = new QLabel("Dakar Mad Max", this);
    sub->setGeometry(100, 40, 600, 45);
    sub->setAlignment(Qt::AlignCenter);
    QFont font2 = sub->font();
    font2.setPointSize(30);
    sub->setFont(font2);
    sub->setStyleSheet("color: #cf2222;");

    // Tres botones de selección
    crearBoton(TipoVehiculo::Moto,      ":/imagenes/motoCenital.png",   "MOTO",        100,  200);
    crearBoton(TipoVehiculo::CarroDakar,":/imagenes/carroCenital.png",  "CARRO DAKAR", 330, 200);
    crearBoton(TipoVehiculo::Camion,    ":/imagenes/camionCenital.png", "CAMIÓN",      570, 200);

    // Descripción de cada vehículo
    QLabel *descMoto = new QLabel("Rápida en\ncarretera\nVulnerable\nen fango", this);
    descMoto->setGeometry(100, 430, 120, 100);
    descMoto->setAlignment(Qt::AlignCenter);
    descMoto->setStyleSheet("color: #cccccc; font-size: 11px; background-color: rgba(0,0,0,150);");

    QLabel *descDakar = new QLabel("Equilibrado\nen todos\nlos terrenos", this);
    descDakar->setGeometry(330, 430, 120, 100);
    descDakar->setAlignment(Qt::AlignCenter);
    descDakar->setStyleSheet("color: #cccccc; font-size: 11px; background-color: rgba(0,0,0,150);");

    QLabel *descCamion = new QLabel("Lento pero\nresistente\na impactos", this);
    descCamion->setGeometry(570, 430, 120, 100);
    descCamion->setAlignment(Qt::AlignCenter);
    descCamion->setStyleSheet("color: #cccccc; font-size: 11px; background-color: rgba(0,0,0,150);");

#ifdef DEBUG_NIVEL2
    // ── BOTÓN DEBUG: ir directo al nivel 2 ───────────────────────────────
    QPushButton *btnDebug = new QPushButton("⚙  DEBUG → Nivel 2", this);
    btnDebug->setGeometry(290, 545, 220, 34);
    btnDebug->setStyleSheet(
        "QPushButton {"
        "  background-color: #333333;"
        "  color: #00FF88;"
        "  font-weight: bold;"
        "  border: 1px solid #00FF88;"
        "  border-radius: 6px;"
        "  font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #444444; }"
        "QPushButton:pressed { background-color: #222222; }"
    );
    connect(btnDebug, &QPushButton::clicked, this, [this]() {
        emit irANivel2Debug();
    });
#endif
}

void SeleccionVehiculo::crearBoton(TipoVehiculo tipo, const QString &imagen,
                                   const QString &nombre, int x, int y)
{
    QLabel *imgLabel = new QLabel(this);
    imgLabel->setGeometry(x, y, 120, 180);
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setStyleSheet(
        "border: 2px solid #444444;"
        "border-radius: 8px;"
        "background-color: rgba(30, 30, 30, 180);"
        );

    QPixmap px(imagen);
    if (!px.isNull())
        imgLabel->setPixmap(px.scaled(90, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        imgLabel->setText(nombre);

    QPushButton *btn = new QPushButton(nombre, this);
    btn->setGeometry(x, y + 190, 120, 36);
    btn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e8a020;"
        "  color: #1a1a1a;"
        "  font-weight: bold;"
        "  border-radius: 6px;"
        "  font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #ffbb40;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #c07010;"
        "}"
        );

    connect(btn, &QPushButton::clicked, this, [this, tipo]() {
        emit vehiculoSeleccionado(tipo);
    });
}
