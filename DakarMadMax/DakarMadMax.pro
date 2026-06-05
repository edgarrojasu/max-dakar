QT += core gui widgets multimedia

CONFIG += c++17

SOURCES += \
    carretera.cpp \
    fango.cpp \
    llanta.cpp \
    main.cpp \
    mainwindow.cpp \
    escenario.cpp \
    terreno.cpp \
    vehiculo.cpp \
    seleccionvehiculo.cpp \
    nivel2.cpp

HEADERS += \
    carretera.h \
    fango.h \
    llanta.h \
    mainwindow.h \
    escenario.h \
    terreno.h \
    vehiculo.h \
    seleccionvehiculo.h \
    tiposvehiculo.h \
    nivel2.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    recursos.qrc
