HEADERS += \
    onebox.h \
    boxgroup.h \
    myview.h \
    mywidget.h \
    tipwidget.h \
    centersqlmodel.h \
    mypushbutton.h \
    mylabel.h \
    global.h

SOURCES += \
    onebox.cpp \
    boxgroup.cpp \
    myview.cpp \
    main.cpp \
    mywidget.cpp \
    tipwidget.cpp \
    centersqlmodel.cpp \
    mypushbutton.cpp \
    mylabel.cpp \
    global.cpp

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RESOURCES += \
    1.qrc
QT += sql multimedia
CONFIG+=resources_big
OTHER_FILES +=

RC_FILE += icon.rc
