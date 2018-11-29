#ifndef CXC_ENGINECOMPOENTS_H
#define CXC_ENGINECOMPOENTS_H

#ifdef WIN32

#include "..\Rendering\RenderManager.h"
#include "..\Scene\Camera.h"
#include "..\Controller\InputManager.h"
#include "..\Material\MaterialManager.h"
#include "..\Scene\SceneManager.h"
#include "..\Window\Window.h"
#include "..\Script\Parser.h"

#else

#include "../Rendering/RenderManager.h"
#include "../Scene/Camera.h"
#include "../Controller/InputManager.h"
#include "../Material/MaterialManager.h"
#include "../Scene/SceneManager.h"
#include "../Window/Window.h"
#include "../Script/Parser.h"

#endif

#endif // CXC_ENGINECOMPOENTS_H
