# CxcEngine

## Introduction

 **CxcEngine** is a tiny cross-platform OpenGL engine developed with C++ 11 which can be used in 3D graphics rendering, it supports muliti-thread redering, basic shading and model loading from obj format file which can be exported by 3DS MAX or blender.

![ShadowMapping of Directional light](http://github.com/cxcscut/CxcEngine/raw/master/Img/Shading.bmp)

![ShadowMapping of Omni-Directional light](http://github.com/cxcscut/CxcEngine/raw/master/Img/Omni-Directional_ShadowMapping.bmp)

## Dependencies

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
- **Open Dynamics Engine**  
  Used for loading the FBX file exported from 3DS MAX.
  
## Projects

### RobotSim

It is a robot hand simulation program in which you can load and draw the robot hand model(it is built in 3DS MAX) and perform a series of actions according to the inputs, for an example, joint angles.

![6-DOF Robot arm and 21-DOF dexterous hand simulation](http://github.com/cxcscut/CxcEngine/raw/master/Img/robot.bmp)


## Current Progress:

- [x] **Forward Kinematics**
- [x] Script parser and interface
- [x] GUI based on MFC
- [x] Data glove interface
- [x] **Inverse Kinematics**
- [x] Kinect sensor interface
- [x] Winsock interface for remote connections
- [x] Object recognition and analysis
- [x] Optimization of draw calls 
- [x] Physics engine using Open Dynamics Engine
- [x] Blinn-Phong shading
- [ ] Shadowmap && Normalmap
- [ ] Motion planning for obstacle advoidance
- [ ] Animation
- [ ] GUI based on QT
