#include "viewer.h"

#include <math.h>
#include <iostream>
#include <QTime>

using namespace std;


Viewer::Viewer(const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentstep(5),
    _stepnumber(6),
    _amplitude1(2.0),
    _amplitude2(4.0),
    _position_x(0.0),
    _position_y(0.0),
    _anim(0),
    _anim_x(1),
    _anim_y(0),
    _do_rice(false),
    _shadowmap_resol(800),
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

  glGenFramebuffers(1,&_fbo_renderer);
  glGenTextures(1,&_rendColorId);
  glGenTextures(1,&_rendNormalId);
  glGenTextures(1,&_rendDepthId);

  glGenFramebuffers(1,&_fbo_shadow);
  glGenTextures(1,&_texShadow);

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

  // attach textures to framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo_normal);
  glBindTexture(GL_TEXTURE_2D,_perlHeightId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_perlHeightId,0);
  glBindTexture(GL_TEXTURE_2D,_perlNormalId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_perlNormalId,0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);


  /////////////////////////////////////////////////////////////////////////////


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
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo_renderer);
  glBindTexture(GL_TEXTURE_2D,_rendColorId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_rendColorId,0);
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_rendNormalId,0);
  glBindTexture(GL_TEXTURE_2D,_rendDepthId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_rendDepthId,0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);


  /////////////////////////////////////////////////////////////////////////////


  // create the texture for rendering depth values
  glBindTexture(GL_TEXTURE_2D,_texShadow);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,_shadowmap_resol,_shadowmap_resol,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);


  // attach textures to framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo_shadow);
  glBindTexture(GL_TEXTURE_2D,_texShadow);
  glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_texShadow,0);

  // test if everything is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "Warning: FBO not complete!" << endl;

  // disable FBO
  glBindFramebuffer(GL_FRAMEBUFFER,0);

}

void Viewer::deleteFBO() {
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fbo_normal);
  glDeleteTextures(1,&_perlHeightId);
  glDeleteTextures(1,&_perlNormalId);

  glDeleteFramebuffers(1,&_fbo_renderer);
  glDeleteTextures(1,&_rendColorId);
  glDeleteTextures(1,&_rendNormalId);
  glDeleteTextures(1,&_rendDepthId);

  glDeleteFramebuffers(1,&_fbo_shadow);
  glDeleteTextures(1,&_texShadow);
}

void Viewer::createTexture(){
  // generate 1 texture id
  glGenTextures(1,&_texLave);
  // load image
  QImage image = QGLWidget::convertToGLFormat(QImage("textures/lave.jpg"));

  // activate texture
  glBindTexture(GL_TEXTURE_2D,_texLave);

  // set texture parameters
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);

  // store texture in the GPU
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width(),image.height(),0,
           GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());

  //////////////////////////////////////////////////////////

  // generate 1 texture id
  glGenTextures(1,&_texEau);
  // load image
  image = QGLWidget::convertToGLFormat(QImage("textures/eau.jpg"));

  // activate texture
  glBindTexture(GL_TEXTURE_2D,_texEau);

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
  glDeleteTextures(1,&_texLave);
  glDeleteTextures(1,&_texEau);
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
  _shaderPerlinNoise = new Shader();
  _shaderNormal = new Shader();
  _shaderDisplacementRendering = new Shader();
  _shaderPostProcessing = new Shader();

  _shaderShadowMap = new Shader();
  _debugShaderShadowMap = new Shader();

  _shaderPerlinNoise->load("shaders/noise.vert","shaders/noise.frag");
  _shaderNormal->load("shaders/normal.vert","shaders/normal.frag");
  _shaderDisplacementRendering->load("shaders/displacement_rendering.vert","shaders/displacement_rendering.frag");
  _shaderPostProcessing->load("shaders/post-processing.vert","shaders/post-processing.frag");

  _shaderShadowMap->load("shaders/shadow-map.vert","shaders/shadow-map.frag");
  _debugShaderShadowMap->load("shaders/show-shadow-map.vert","shaders/show-shadow-map.frag");

}

void Viewer::deleteShaders() {
  delete _shaderPerlinNoise;  _shaderPerlinNoise = NULL;
  delete _shaderNormal; _shaderNormal = NULL;
  delete _shaderDisplacementRendering; _shaderDisplacementRendering = NULL;
  delete _shaderShadowMap; _shaderShadowMap = NULL;
  delete _debugShaderShadowMap; _debugShaderShadowMap = NULL;
  delete _shaderPostProcessing; _shaderPostProcessing = NULL;
}


void Viewer::drawQuad() {

  // Draw the 2 triangles !
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);
}


void Viewer::drawPerlinNoise(){
  const int id = _shaderPerlinNoise->id();

  // send the parameters to the shader
  glUniform1f(glGetUniformLocation(id,"amplitude1"),_amplitude1);
  glUniform1f(glGetUniformLocation(id,"amplitude2"),_amplitude2);
  glUniform1f(glGetUniformLocation(id,"position_x"),_position_x);
  glUniform1f(glGetUniformLocation(id,"position_y"),_position_y);
  if(_do_rice)
    glUniform1i(glGetUniformLocation(id,"do_rice"),1);
  else
    glUniform1i(glGetUniformLocation(id,"do_rice"),0);

  // draw base quad
  drawQuad();
}


void Viewer::drawNormals(){
  const int id = _shaderNormal->id();

  // send the result of Perlin noise to shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_perlHeightId);
  glUniform1i(glGetUniformLocation(id,"heightmap"),0);
  // draw base quad
  drawQuad();
}


void Viewer::drawSceneFromLight() {
  const int id = _shaderShadowMap->id();

  // mdv matrix from the light point of view
  const float size = 1;
  glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
  glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
  glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,-1,0));
  glm::mat4 m   = glm::mat4(1.0);
  glm::mat4 mv  = v*m;

  // send the modelview projection depth matrix
  const glm::mat4 mvpDepth = p*mv;
  glUniformMatrix4fv(glGetUniformLocation(id,"mvpMat"),1,GL_FALSE,&mvpDepth[0][0]);

  // send the result of Perlin noise to shader (we want to re-displace to compute the shadows)
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_perlHeightId);
  glUniform1i(glGetUniformLocation(id,"heightmap"),0);

  // activate faces and draw!
  glBindVertexArray(_vaoTerrain);
  glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
  glBindVertexArray(0);
}


void Viewer::drawShadowMap() {
  // send depth texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texShadow);
  glUniform1i(glGetUniformLocation(_debugShaderShadowMap->id(),"shadowmap"),0);

  // draw the quad
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);

  // disable VAO
  glBindVertexArray(0);
}


void Viewer::drawTerrain(bool do_shadows) {
  // shader id
  const int id = _shaderDisplacementRendering->id();

  // mdv matrix from the light point of view
  const float size = 1;
  glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
  glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
  glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,-1,0));
  glm::mat4 m   = glm::mat4(1.0);
  glm::mat4 mv  = v*m;
  glm::mat4 mvp  = p*mv;

  // send uniform (constant) variables to the shader
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(id,"mvpLightMat"),1,GL_FALSE,&mvp[0][0]);
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));

  // send the normal map (which contains the height)
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_perlNormalId);
  glUniform1i(glGetUniformLocation(id,"normalmap"),0);

  glActiveTexture(GL_TEXTURE0+1);
  glBindTexture(GL_TEXTURE_2D,_texLave);
  glUniform1i(glGetUniformLocation(id,"texlave"),1);

  glActiveTexture(GL_TEXTURE0+2);
  glBindTexture(GL_TEXTURE_2D,_texEau);
  glUniform1i(glGetUniformLocation(id,"texeau"),2);

  //send the shadow map
  glActiveTexture(GL_TEXTURE0+3);
  glBindTexture(GL_TEXTURE_2D,_texShadow);
  glUniform1i(glGetUniformLocation(id,"shadowmap"),3);

  glUniform1f(glGetUniformLocation(id,"position_x"),_position_x);
  glUniform1f(glGetUniformLocation(id,"position_y"),_position_y);
  glUniform1f(glGetUniformLocation(id,"anim_x"),_anim_x);
  glUniform1f(glGetUniformLocation(id,"anim_y"),_anim_y);
  if(do_shadows)
      glUniform1i(glGetUniformLocation(id,"do_shadows"),1);
  else
      glUniform1i(glGetUniformLocation(id,"do_shadows"),0);

  // activate faces and draw!
  glBindVertexArray(_vaoTerrain);
  glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
  glBindVertexArray(0);

}


void Viewer::doPostProcessing(){
  const int id = _shaderPostProcessing->id();

  // send computed textures to the shader
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_rendColorId);
  glUniform1i(glGetUniformLocation(id,"colormap"),0);
  glActiveTexture(GL_TEXTURE0+1);
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glUniform1i(glGetUniformLocation(id,"normalmap"),1);
  // draw base quad
  drawQuad();
}


void Viewer::paintGL() {

  bool is_not_last_step;

//////////// PERLIN ////////////

  is_not_last_step = (_currentstep > 0);

  if(is_not_last_step){  // Ce n'est pas la derniere etape, on stocke dans un fbo
      glViewport(0,0,_grid->width(),_grid->height());
      // activate the created framebuffer object
      glBindFramebuffer(GL_FRAMEBUFFER,_fbo_normal);
      // draw in _perlHeightId
      GLenum buffer_height [] = {GL_COLOR_ATTACHMENT0};
      glDrawBuffers(1,buffer_height);
  }
  else
      glViewport(0,0,width(),height());

  // activate the shader
  glUseProgram(_shaderPerlinNoise->id());
  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw the Perlin noise
  drawPerlinNoise();

  // disable shader
  glUseProgram(0);
  // desactivate fbo
  glBindFramebuffer(GL_FRAMEBUFFER,0);


//////////// NORMAL ////////////

  is_not_last_step = (_currentstep > 1);

  if(_currentstep >= 1){

      if(is_not_last_step){
          glViewport(0,0,_grid->width(),_grid->height());
          // activate the created framebuffer object
          glBindFramebuffer(GL_FRAMEBUFFER,_fbo_normal);
          // draw in _perlNormalId
          GLenum buffer_normal [] = {GL_COLOR_ATTACHMENT1};
          glDrawBuffers(1,buffer_normal);
      }
      else
          glViewport(0,0,width(),height());

      // activate the shader
      glUseProgram(_shaderNormal->id());
      // clear buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // draw the normal map
      drawNormals();

      // disable shader
      glUseProgram(0);
      // desactivate fbo
      glBindFramebuffer(GL_FRAMEBUFFER,0);
  }


//////////// SHADOW_MAP ////////////

  bool is_last_step = (_currentstep == 2);

  if(_currentstep >= 2){
      glBindFramebuffer(GL_FRAMEBUFFER,_fbo_shadow);
      // draw in nothing, everything is implicit by openGL
      glDrawBuffer(GL_NONE);
      glClear(GL_DEPTH_BUFFER_BIT);
      glUseProgram(_shaderShadowMap->id());
      glViewport(0,0,_shadowmap_resol,_shadowmap_resol);            //On dessine dans les dimensions de la shadowmap

      drawSceneFromLight();

      glBindFramebuffer(GL_FRAMEBUFFER,0);
      glViewport(0,0,width(),height());

      if(is_last_step){
          // activate the test shader
          glUseProgram(_debugShaderShadowMap->id());

          // clear buffers
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          // display the shadow map
          drawShadowMap();
      }
  }

//////////// DISPLACEMENT/RENDERING ////////////

  is_not_last_step = (_currentstep > 4);

  if(_currentstep >= 3){

      if(is_not_last_step){
          // activate the created framebuffer object
          glBindFramebuffer(GL_FRAMEBUFFER,_fbo_renderer);
          // draw in _rendColorId & _rendNormalId
          GLenum buffer_render [] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
          glDrawBuffers(2,buffer_render);
      }
      // activate the shader
      glUseProgram(_shaderDisplacementRendering->id());
      // clear buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // draw terrain
      bool do_shadows = (_currentstep>=4);
      drawTerrain(do_shadows);

      // disable shader
      glUseProgram(0);
      // desactivate fbo
      glBindFramebuffer(GL_FRAMEBUFFER,0);
  }

//////////// POST-PROCESSING ////////////

    if(_currentstep >= 5){

      // activate the shader
      glUseProgram(_shaderPostProcessing->id());
      // clear buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // draw
      doPostProcessing();

      // disable shader
      glUseProgram(0);
  }

  // Animation
  _anim = _anim+0.0003;
  _anim_x = cos(_anim);
  _anim_y = sin(_anim);
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
    _shaderDisplacementRendering->load("shaders/displacement_rendering.vert","shaders/displacement_rendering.frag");
    _shaderPostProcessing->load("shaders/post-processing.vert","shaders/post-processing.frag");

    _shaderShadowMap->load("shaders/shadow-map.vert","shaders/shadow-map.frag");
    _debugShaderShadowMap->load("shaders/show-shadow-map.vert","shaders/show-shadow-map.frag");
  }

  // space bar : switch to next step
  if(ke->key()==Qt::Key_Space) {
    _currentstep = (_currentstep+1)%_stepnumber;
  }

  // + key : increase amplitude1
  if(ke->key()==Qt::Key_Plus) {
    _amplitude1 += 0.1;
  }

  // - key : decrease amplitude1
  if(ke->key()==Qt::Key_Minus) {
    _amplitude1 -= 0.1;
  }

  // page up key : increase amplitude2
  if(ke->key()==Qt::Key_PageUp) {
    _amplitude2 += 0.1;
  }

  // page down key : decrease amplitude2
  if(ke->key()==Qt::Key_PageDown) {
    _amplitude2 -= 0.1;
  }

  if(ke->key() == Qt::Key_Z){
      _do_rice = !_do_rice;
  }


  // arrow keys : move the perlin noise
  if(ke->key()==Qt::Key_Up)
    _position_y -= 0.03;
  if(ke->key()==Qt::Key_Down)
    _position_y += 0.03;
  if(ke->key()==Qt::Key_Left)
    _position_x += 0.03;
  if(ke->key()==Qt::Key_Right)
    _position_x -= 0.03;

  // backspace : return to (0,0)
  if(ke->key()==Qt::Key_Backspace){
      _position_x = 0;
      _position_y = 0;
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

