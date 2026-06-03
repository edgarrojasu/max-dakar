#include "seleccionvehiculo.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFont>

SeleccionVehiculo::SeleccionVehiculo(QWidget *parent)
    : QWidget(parent)
{
    // Fondo del menú
    QLabel *fondo = new QLabel(this);
    fondo->setGeometry(0, 0, 800, 600);
    QPixmap fondoPx(":/imagenes/fondoMenu.png");
    fondo->setPixmap(fondoPx.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondo->lower();  // manda el fondo atrás de todo
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
        "background-color: rgba(30, 30, 30, 180);"  // semitransparente
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
