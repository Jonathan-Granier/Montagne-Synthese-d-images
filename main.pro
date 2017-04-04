GLM_PATH  = ../../ext/glm-0.9.4.1

TEMPLATE  = app
TARGET    = montagnes


LIBS     += -lGLEW -lGL -lGLU -lm
INCLUDEPATH  += $${GLM_PATH}

SOURCES   = main.cpp viewer.cpp \
    viewer.cpp \
    grid.cpp \
    main.cpp \
    shader.cpp \
    camera.cpp \
    meshLoader.cpp \
    trackball.cpp

HEADERS   = viewer.h \
    viewer.h \
    grid.h \
    vec4.h \
    vec3.h \
    vec2.h \
    shader.h \
    mat4.h \
    mat3.h \
    camera.h \
    meshLoader.h \
    quat.h \
    trackball.h

CONFIG   += qt opengl warn_on thread uic4 release
QT       *= xml opengl core
