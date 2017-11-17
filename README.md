# CxcEngine

## Introduction

 **CxcEngine** is a tiny cross-platform OpenGL engine developed with C++ which can be used in 3D graphics rendering, it supports basic shading and model loading from obj format file which can be exported by 3DS MAX or blender.

## Prerequisites

- **OpenGL Library**  
  Including the header file gl.h and glu.h.
- **OpenGL Utility Toolkit**  
  Including the header file glut.h and static libray glut32.lib.
- **GLFW**  
  It is a open source and multi-platform library for OpenGL which provides a simple API for creating windows, contexts and surfaces, receiving input and events. it includes the header file glfw3.h, static libray glfw3.lib as well as dynamic libray glfw3.dll.
- **GLEW**  
  It is the OpenGL Extension Wrangler library which includes the header file glew.h and static library glew32.lib as well as dynamic library glew32.dll.
- **GLM**  
  The OpenGL mathematics library including glm.hpp and matrix_transformation.hpp etc.
  
## Examples

  It is a robot hand simulation program in which you can load and draw the robot hand model(it is built in 3DS MAX) and perform a series of actions according to the inputs, for an example, joint angles.

![6-DOF Robot arm and 21-DOF dexterous hand simulation](http://github.com/cxcscut/CxcEngine/raw/master/Img/Robothand.png)

Work to be done:

- [x] Definition of  robot hand data structure
- [x] Implementation of robot hand data structure
- [x] Building 3D model of the robot hand using 3DS MAX
- [x] Implementation of **Drirect Kinematics**
- [x] Testing translation, rotation for each joint
- [x] Implementation of script parser and interface
- [x] Multi-threading support for rendering 
- [x] GUI based on MFC
- [x] Implementation of data glove interface
- [ ] Implementation of 20 kinds of actions （11/20 Finished）
- [ ] Implementation of network interface
- [ ] Implementation of skeleton animation
- [ ] Implementation of **Inverse Kinematics**
