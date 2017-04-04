GLM_PATH  = ../../ext/glm-0.9.4.1

TEMPLATE  = app
TARGET    = projet


LIBS     += -lGLEW -lGL -lGLU -lm
INCLUDEPATH  += $${GLM_PATH}

SOURCES   = main.cpp viewer.cpp \
    viewer.cpp \
    grid.cpp \
    main.cpp \
    shader.cpp

HEADERS   = viewer.h \
    viewer.h \
    grid.h \
    vec4.h \
    vec3.h \
    vec2.h \
    shader.h \
    mat4.h \
    mat3.h

CONFIG   += qt opengl warn_on thread uic4 release
QT       *= xml opengl core
