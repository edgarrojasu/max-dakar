QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += core gui widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    escenario.cpp \
    vehiculo.cpp \
    seleccionvehiculo.cpp

HEADERS += \
    mainwindow.h \
    escenario.h \
    vehiculo.h \
    seleccionvehiculo.h \
    tiposvehiculo.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../../../../imagenes/motoCenital.png \
    ../motoCenital.png

RESOURCES += \
    recursos.qrc
