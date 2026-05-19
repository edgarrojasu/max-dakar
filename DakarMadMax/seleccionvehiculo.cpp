#include "seleccionvehiculo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFont>

SeleccionVehiculo::SeleccionVehiculo(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(600, 800);
    setStyleSheet("background-color: #1a1a1a;");

    // Título
    QLabel *titulo = new QLabel("ELIGE TU VEHÍCULO", this);
    titulo->setGeometry(0, 80, 600, 50);
    titulo->setAlignment(Qt::AlignCenter);
    QFont font = titulo->font();
    font.setPointSize(22);
    font.setBold(true);
    titulo->setFont(font);
    titulo->setStyleSheet("color: #e8a020;");

    // Subtítulo
    QLabel *sub = new QLabel("Dakar Mad Max", this);
    sub->setGeometry(0, 40, 600, 35);
    sub->setAlignment(Qt::AlignCenter);
    QFont font2 = sub->font();
    font2.setPointSize(13);
    sub->setFont(font2);
    sub->setStyleSheet("color: #888888;");

    // Tres botones de selección
    crearBoton(TipoVehiculo::Moto,      ":/imagenes/motoCenital.png",   "MOTO",        60,  200);
    crearBoton(TipoVehiculo::CarroDakar,":/imagenes/carroCenital.png",  "CARRO DAKAR", 230, 200);
    crearBoton(TipoVehiculo::Camion,    ":/imagenes/camionCenital.png", "CAMIÓN",      400, 200);

    // Descripción de cada vehículo
    QLabel *descMoto = new QLabel("Rápida en\ncarretera\nVulnerable\nen fango", this);
    descMoto->setGeometry(40, 430, 120, 100);
    descMoto->setAlignment(Qt::AlignCenter);
    descMoto->setStyleSheet("color: #cccccc; font-size: 11px;");

    QLabel *descDakar = new QLabel("Equilibrado\nen todos\nlos terrenos", this);
    descDakar->setGeometry(210, 430, 120, 100);
    descDakar->setAlignment(Qt::AlignCenter);
    descDakar->setStyleSheet("color: #cccccc; font-size: 11px;");

    QLabel *descCamion = new QLabel("Lento pero\nresistente\na impactos", this);
    descCamion->setGeometry(380, 430, 120, 100);
    descCamion->setAlignment(Qt::AlignCenter);
    descCamion->setStyleSheet("color: #cccccc; font-size: 11px;");
}

void SeleccionVehiculo::crearBoton(TipoVehiculo tipo, const QString &imagen,
                                   const QString &nombre, int x, int y)
{
    // Contenedor del botón
    QLabel *imgLabel = new QLabel(this);
    imgLabel->setGeometry(x, y, 120, 180);
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setStyleSheet(
        "border: 2px solid #444444;"
        "border-radius: 8px;"
        "background-color: #2a2a2a;"
        );

    QPixmap px(imagen);
    if (!px.isNull())
        imgLabel->setPixmap(px.scaled(90, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        imgLabel->setText(nombre);  // fallback si no carga la imagen

    // Botón debajo de la imagen
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

    // Capturamos tipo por valor en el lambda
    connect(btn, &QPushButton::clicked, this, [this, tipo]() {
        emit vehiculoSeleccionado(tipo);
    });
}
