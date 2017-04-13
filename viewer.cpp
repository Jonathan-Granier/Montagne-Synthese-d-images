#include "viewer.h"

#include <math.h>
#include <iostream>
#include <QTime>

using namespace std;


Viewer::Viewer(const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentstep(0),
    _stepnumber(3),
    _light(glm::vec3(0,0,1)),
    _mode(false) {

  setlocale(LC_ALL,"C");
   _grid = new Grid();
   _cam = new Camera(sqrt(2),glm::vec3(0,0,0));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _grid;
  delete _cam;

  // delete all GPU objects
  deleteShaders();
  deleteVAO();
  deleteFBO();
  deleteTexture();
}

void Viewer::createFBO() {
  // Ids needed for the FBO and associated textures
  glGenFramebuffers(1,&_fbo_normal);
  glGenTextures(1,&_perlHeightId);
  glGenTextures(1,&_perlNormalId);
  glGenTextures(1,&_perlDepthId);
  /*
  glGenFramebuffers(1,&_fbo_displaced);
  glGenTextures(1,&_rendColorId);
  glGenTextures(1,&_rendNormalId);
  glGenTextures(1,&_rendDepthId);
  */
}

void Viewer::initFBO() {

 // create the texture for perlin colors
  glBindTexture(GL_TEXTURE_2D,_perlHeightId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,_grid->width(),_grid->height(),0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // create the texture for perlin normals
  glBindTexture(GL_TEXTURE_2D,_perlNormalId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,_grid->width(),_grid->height(),0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // create the texture for perlin depth values
  glBindTexture(GL_TEXTURE_2D,_perlDepthId);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,_grid->width(),_grid->height(),0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // attach textures to framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo_normal);
  glBindTexture(GL_TEXTURE_2D,_perlHeightId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_perlHeightId,0);
  glBindTexture(GL_TEXTURE_2D,_perlNormalId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_perlNormalId,0);
  glBindTexture(GL_TEXTURE_2D,_perlDepthId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_perlDepthId,0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  /////////////////////////////////////////////////////////////////////////////
/*
  // create the texture for rendering colors
  glBindTexture(GL_TEXTURE_2D,_rendColorId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,width(),height(),0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // create the texture for rendering normals
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,width(),height(),0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // create the texture for rendering depth values
  glBindTexture(GL_TEXTURE_2D,_rendDepthId);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width(),height(),0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // attach textures to framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo_displaced);
  glBindTexture(GL_TEXTURE_2D,_rendColorId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_rendColorId,0);
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_rendNormalId,0);
  glBindTexture(GL_TEXTURE_2D,_rendDepthId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_rendDepthId,0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
*/
}

void Viewer::deleteFBO() {
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fbo_normal);
  glDeleteTextures(1,&_perlHeightId);
  glDeleteTextures(1,&_perlNormalId);
  glDeleteTextures(1,&_perlDepthId);
  /*
  glDeleteFramebuffers(1,&_fbo_displaced);
  glDeleteTextures(1,&_rendColorId);
  glDeleteTextures(1,&_rendNormalId);
  glDeleteTextures(1,&_rendDepthId);
  */
}

void Viewer::createTexture(){
  // generate 1 texture ids
  glGenTextures(1,&_texRoche);
  // load image
  QImage image = QGLWidget::convertToGLFormat(QImage("textures/roche.jpg"));

  // activate texture
  glBindTexture(GL_TEXTURE_2D,_texRoche);

  // set texture parameters
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);

  // store texture in the GPU
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width(),image.height(),0,
           GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());
}

void Viewer::deleteTexture(){
  glDeleteTextures(1,&_texRoche);
}

void Viewer::createVAO() {

  const GLfloat quadData[] = {
    -1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f };

  glGenBuffers(2,_terrain);
  glGenBuffers(1,&_quad);
  glGenVertexArrays(1,&_vaoTerrain);
  glGenVertexArrays(1,&_vaoQuad);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // vertices
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW);

  // create the VBO associated with the screen quad
  glBindVertexArray(_vaoQuad);
  glBindBuffer(GL_ARRAY_BUFFER,_quad); // vertices
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
}

void Viewer::deleteVAO() {
  glDeleteBuffers(2,_terrain);
  glDeleteBuffers(1,&_quad);
  glDeleteVertexArrays(1,&_vaoTerrain);
  glDeleteVertexArrays(1,&_vaoQuad);
}

void Viewer::createShaders() {
  // create 2 shaders: one for the first pass, one for the second pass
  _shaderPerlinNoise = new Shader();
  _shaderNormal = new Shader();
  _shaderDisplacement = new Shader();

  _shaderPerlinNoise->load("shaders/noise.vert","shaders/noise.frag");
  _shaderNormal->load("shaders/normal.vert","shaders/normal.frag");
  _shaderDisplacement->load("shaders/displacement.vert","shaders/displacement.frag");


}

void Viewer::deleteShaders() {
  delete _shaderPerlinNoise;  _shaderPerlinNoise = NULL;
  delete _shaderNormal; _shaderNormal = NULL;
  delete _shaderDisplacement; _shaderDisplacement = NULL;
}


void Viewer::drawObject(const glm::vec3 &pos,const glm::vec3 &col) {
  // shader id
  const int id = _shaderDisplacement->id();

  // send uniform (constant) variables to the shader
  glm::mat4 mdv = glm::translate(_cam->mdvMatrix(),pos);
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(mdv[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));
  glUniform3fv(glGetUniformLocation(id,"color"),1,&(col[0]));
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));

  // send the normal map (which contains the height)
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_perlNormalId);
  glUniform1i(glGetUniformLocation(id,"normalmap"),0);

  glActiveTexture(GL_TEXTURE0+1);
  glBindTexture(GL_TEXTURE_2D,_texRoche);
  glUniform1i(glGetUniformLocation(id,"texroche"),1);

  // activate faces and draw!
  glBindVertexArray(_vaoTerrain);
  glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
  glBindVertexArray(0);

}

void Viewer::drawQuad() {

  // Draw the 2 triangles !
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);
}

void Viewer::paintGL() {

  glViewport(0,0,_grid->width(),_grid->height());

//////////// PERLIN ////////////

  if(_currentstep > 0){
      // activate the created framebuffer object
      glBindFramebuffer(GL_FRAMEBUFFER,_fbo_normal);
      // set the buffers to draw in
      GLenum buffer_height [] = {GL_COLOR_ATTACHMENT0};
      glDrawBuffers(1,buffer_height);
  }

  // activate the shader
  glUseProgram(_shaderPerlinNoise->id());
  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // draw base quad
  drawQuad();
  // disable shader
  glUseProgram(0);

  if(_currentstep > 0){
      // desactivate fbo
      glBindFramebuffer(GL_FRAMEBUFFER,0);
      // clear everything
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

//////////// NORMAL ////////////

  if(_currentstep > 0){
      if(_currentstep > 1){
          // activate the created framebuffer object
          glBindFramebuffer(GL_FRAMEBUFFER,_fbo_normal);
          // set the buffers to draw in
          GLenum buffer_normal [] = {GL_COLOR_ATTACHMENT1};
          glDrawBuffers(1,buffer_normal);
      }

      // activate the shader
      glUseProgram(_shaderNormal->id());
      // clear buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // send the result of Perlin noise to shader
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,_perlHeightId);
      glUniform1i(glGetUniformLocation(_shaderNormal->id(),"heightmap"),0);
      // draw base quad
      drawQuad();

      // disable shader
      glUseProgram(0);

      if(_currentstep > 1){
          // desactivate fbo
          glBindFramebuffer(GL_FRAMEBUFFER,0);
          // clear everything
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
  }

//////////// DISPLACEMENT ////////////

  if(_currentstep > 1){
      glViewport(0,0,width(),height());
      // activate the shader
      glUseProgram(_shaderDisplacement->id());

      // draw base quad
      drawObject(glm::vec3(0,0,0),glm::vec3(0,1,0));

      // disable shader
      glUseProgram(0);
  }

}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);

  // re-init the FBO (textures need to be resized to the new viewport size)
  initFBO();
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  }

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}



void Viewer::keyPressEvent(QKeyEvent *ke) {

  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive())
      _timer->stop();
    else
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }

  // key r: reload shaders
  if(ke->key()==Qt::Key_R) {
    _shaderPerlinNoise->load("shaders/noise.vert","shaders/noise.frag");
    _shaderNormal->load("shaders/normal.vert","shaders/normal.frag");
    _shaderDisplacement->load("shaders/displacement.vert","shaders/displacement.frag");
  }

  // space bar : switch to next step
  if(ke->key()==Qt::Key_Space) {
    _currentstep = (_currentstep+1)%_stepnumber;
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and check glew
  glewExperimental = GL_TRUE;

  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }

  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());


  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();

  // init VAO
  createVAO();

  // create/init FBO
  createFBO();
  initFBO();

  createTexture();

  // starts the timer
  _timer->start();
}

