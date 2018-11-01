#ifndef CXC_ENGINECOMPOENTS_H
#define CXC_ENGINECOMPOENTS_H

#ifdef WIN32

#include "..\Graphics\RendererManager.h"
#include "..\Controller\Camera.h"
#include "..\Controller\InputManager.h"
#include "..\Graphics\MaterialManager.h"
#include "..\Graphics\SceneManager.h"
#include "..\Window\Window.h"
#include "..\Script\Parser.h"
#include "..\Controller\CommandHandler.h"

#else

#include "../Graphics/RendererManager.h"
#include "../Controller/Camera.h"
#include "../Controller/InputManager.h"
#include "../Graphics/MaterialManager.h"
#include "../Graphics/SceneManager.h"
#include "../Window/Window.h"
#include "../Script/Parser.h"
#include "../Controller/CommandHandler.h"

#endif

#endif // CXC_ENGINECOMPOENTS_H
