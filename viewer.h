#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h>

// OpenGL library
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "shader.h"
#include "grid.h"


class Viewer : public QGLWidget {
 public:
  Viewer(const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();

 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
  void createVAO();
  void deleteVAO();

  void createShaders();
  void deleteShaders();
  void disableShader();

  void createFBO();
  void deleteFBO();
  void initFBO();

  void createTexture();
  void deleteTexture();

  void drawQuad();
  void drawPerlinNoise();
  void drawNormals();
  void drawSceneFromLight();
  void drawShadowMap();
  void drawTerrain(bool do_shadows);
  void doPostProcessing();



  QTimer        *_timer;    // timer that controls the animation

  unsigned int   _currentstep; // current step of rendering
  unsigned int   _stepnumber;  // number of steps

  // Perlin noise parameters
  float          _amplitude1; // the amplitude 1 of the perlin noise
  float          _amplitude2; // the amplitude 2 of the perlin noise
  float          _position_x; // the "position" to translate in the perlin noise
  float          _position_y;
  float          _anim;
  float          _anim_x;
  float          _anim_y;
  bool           _do_rice;

  unsigned int   _shadowmap_resol; // resolution of the shadowmap

  Grid   *_grid;      // the grid
  Camera *_cam;    // the camera

  glm::vec3 _light; // light direction
  bool      _mode;  // camera motion or light motion

  Shader *_shaderPerlinNoise;
  Shader *_shaderNormal;
  Shader *_shaderShadowMap;
  Shader *_debugShaderShadowMap;
  Shader *_shaderDisplacementRendering;
  Shader *_shaderPostProcessing;

  // vao/vbo ids (1 for the object, 1 for the viewport quad)
  GLuint _vaoTerrain;
  GLuint _vaoQuad;
  GLuint _terrain[2];
  GLuint _quad;


  // perlin texture ids
  GLuint _perlHeightId;
  GLuint _perlNormalId;

  // render texture ids
  GLuint _rendColorId;
  GLuint _rendNormalId;
  GLuint _rendDepthId;

  // shadow texture id
  GLuint _texShadow;


  // fbo id
  GLuint _fbo_normal;
  GLuint _fbo_renderer;
  GLuint _fbo_shadow;


  GLuint _texLave;
  GLuint _texEau;
};

#endif // VIEWER_H
