#include <iostream>

#include "../FBXSDK-Test/RobotHand.h"
#include "../FBXSDK-Test/Kinematics.h"

#include <ompl/base/spaces/RealVectorStateSpace.h>
#include <ompl/base/objectives/PathLengthOptimizationObjective.h>
#include <ompl/geometric/planners/rrt/RRTConnect.h>
#include <ompl/geometric/planners/rrt/RRTstar.h>
#include <ompl/geometric/planners/prm/SPARStwo.h>
#include <ompl/geometric/PathGeometric.h>

using namespace std;

#define MOVING_STEP 20.0f
#define MOVING_ORIENTATION_STEP (glm::radians(45.0f))

using namespace cxc;
namespace ob = ompl::base;
namespace og = ompl::geometric;

#ifdef WIN32

static const std::string hand_file = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Examples\\RobotSim\\Model\\SZrobotl.obj";
static const std::string plane_file = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Examples\\RobotSim\\Model\\plane.obj";
static const std::string table_file = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Examples\\RobotSim\\Model\\table.obj";

static const std::string VertexShaderPath = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Engine\\Shader\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Engine\\Shader\\StandardFragmentShader.glsl";

#else

static const std::string hand_file = "../RobotSim/Model/SZrobotl.obj";
static const std::string plane_file = "../RobotSim/Model/plane.obj";
static const std::string table_file = "../RobotSim/Model/table.obj";

static const std::string Obstacles_file = "../RobotSim/Model/Obstacles.obj";

static const std::string VertexShaderPath = "../../Engine/Shader/StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "../../Engine/Shader/StandardFragmentShader.glsl";

#endif // WIN32

void DisplayState(const ob::State *state);

std::shared_ptr<Robothand> m_LeftPtr, m_RightPtr;
std::shared_ptr<Object3D> Table, Plane,Obstacles;
std::vector<ompl::base::State*> path;
int state_count = 0;

bool isStateValid(const ob::State *state)
{
    auto pEngine = EngineFacade::GetInstance();

    auto degrees = state->as<ob::RealVectorStateSpace::StateType>()->values;

    m_LeftPtr->RotateJoint("arm_left2",degrees[0]);
    m_LeftPtr->RotateJoint("arm_left3",degrees[1]);
    m_LeftPtr->RotateJoint("arm_left4",degrees[2]);
    m_LeftPtr->RotateJoint("arm_left5",degrees[3]);
    m_LeftPtr->RotateJoint("arm_left6",degrees[4]);
    m_LeftPtr->RotateJoint("palm_left",degrees[5]);

    pEngine->ProcessingPhysics();

     // Reset pose
    m_LeftPtr->ResetAllJointPose();

    if(pEngine->m_pSceneMgr->Collision)
    {
        pEngine->m_pSceneMgr->Collision = false;
        return false;
    }
    else
        return true;

};

auto keycallback = [=](int key, int scancode, int action, int mods) {
	auto pEngine = EngineFacade::GetInstance();

	// W pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_W) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({MOVING_STEP,0,0}));
		}
	}

	// S pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_S) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({ -MOVING_STEP,0,0 }));
		}
	}

	// A pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_A) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({0, MOVING_STEP,0 }));
		}
	}

	// D pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_D) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({ 0, -MOVING_STEP,0 }));
		}
	}

	// PageUp pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({ 0, 0,MOVING_STEP }));
		}
	}

	// PageDown pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({ 0, 0,-MOVING_STEP }));
		}
	}

    if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			if(state_count >= path.size())
            {
                state_count = 0;
            }
            DisplayState(path[state_count]);
            state_count++;
		}
	}

	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		pEngine->GameOver = true;
	}
};

void DisplayState(const ob::State *state)
{
    auto degrees = state->as<ob::RealVectorStateSpace::StateType>()->values;

    m_LeftPtr->ResetAllJointPose();

    m_LeftPtr->RotateJoint("arm_left2",degrees[0]);
    m_LeftPtr->RotateJoint("arm_left3",degrees[1]);
    m_LeftPtr->RotateJoint("arm_left4",degrees[2]);
    m_LeftPtr->RotateJoint("arm_left5",degrees[3]);
    m_LeftPtr->RotateJoint("arm_left6",degrees[4]);
    m_LeftPtr->RotateJoint("palm_left",degrees[5]);

}

int main()
{
	// Accquire engine pointer
	auto pEngine = EngineFacade::GetInstance();

	// Engine configuration
	{

		pEngine->SetGravity(0, -0.1, 0);

		pEngine->m_pWindowMgr->InitGL();

		pEngine->m_pWindowMgr->SetWindowHeight(600);
		pEngine->m_pWindowMgr->SetWindowWidth(800);

		pEngine->SetVertexShaderPath(VertexShaderPath);
		pEngine->SetFragmentShaderPath(FragmentShaderPath);

		pEngine->m_pWindowMgr->SetWindowTitle("Engine test");
		pEngine->m_pWindowMgr->isDecoraded = true;

		pEngine->m_pSceneMgr->m_pCamera->eye_pos = glm::vec3(0, 2000, 2000);

		EngineFacade::KeyInputCallBack = keycallback;

		pEngine->InitWindowPosition(200,200);

		pEngine->MultiThreadingEnable();
	}

	pEngine->Init();

	// Object definition and loading
	{

		auto LoadRobothand = [&](int type) {
			if (type == ROBOTHAND_LEFT)
				m_LeftPtr = std::make_shared<Robothand>(type, hand_file);
			else
				m_RightPtr = std::make_shared<Robothand>(type, hand_file);
		};

		auto LoadPlane = [&]() {Plane = std::make_shared<Object3D>("plane", plane_file,"env"); };
		auto LoadEnv = [&]() {
			Table = std::make_shared<Object3D>("table", table_file, "env");
			Obstacles = std::make_shared<Object3D>("obstacle",Obstacles_file,"env");
		};

		std::thread left_hand(LoadRobothand, ROBOTHAND_LEFT);
		std::thread plane(LoadPlane);
		std::thread table(LoadEnv);

		left_hand.join();
		plane.join();
		table.join();

		if (!Plane || !Plane->CheckLoaded()) return 0;
		if (!m_LeftPtr || !m_LeftPtr->CheckLoaded()) return 0;
		if (!Table || !Table->CheckLoaded()) return 0;
        if (!Obstacles || !Obstacles->CheckLoaded()) return 0;

		pEngine->addObject(Plane, true);
		pEngine->addObject(m_LeftPtr,true);
		pEngine->addObject(Table,true);
        pEngine->addObject(Obstacles,true);
	}

	// Adding user code here
	{
		m_LeftPtr->InitOriginalDegrees();

		m_LeftPtr->RotateJoint("arm_left2", 90.0f);

		// Set initial pose of robot arm
		m_LeftPtr->SetBaseDegrees({ 0,0,90,0,-90,0 });

		m_LeftPtr->GetModelByName("arm_left1")->SetTag("Collision_free");
	}

    auto armStateSpace = ob::StateSpacePtr(new ob::RealVectorStateSpace(6));
    ob::RealVectorBounds bounds(6);

    for(auto i =0;i<6;i++)
    {
        bounds.setLow(i,-180.0f);
        bounds.setHigh(i,180.0f);
    }

    armStateSpace->as<ob::RealVectorStateSpace>()->setBounds(bounds);

    auto armSpaceInformation = ob::SpaceInformationPtr(new ob::SpaceInformation(armStateSpace));

    armSpaceInformation->setStateValidityChecker(isStateValid);

    armSpaceInformation->setup();

    auto armProblemDefinition = ob::ProblemDefinitionPtr(new ob::ProblemDefinition(armSpaceInformation));

    //auto armPlanner = ob::PlannerPtr(new og::RRTConnect(armSpaceInformation));

    auto armPlanner = ob::PlannerPtr(new og::SPARStwo(armSpaceInformation));

    armPlanner->setProblemDefinition(armProblemDefinition);
    armPlanner->setup();

    ob::ScopedState<> start_ompl(armStateSpace);
    ob::ScopedState<> goal_ompl(armStateSpace);

    // start state
    start_ompl->as<ob::RealVectorStateSpace::StateType>()->values[0] = 0;
    start_ompl->as<ob::RealVectorStateSpace::StateType>()->values[1] = 0;
    start_ompl->as<ob::RealVectorStateSpace::StateType>()->values[2] = 90.0f;
    start_ompl->as<ob::RealVectorStateSpace::StateType>()->values[3] = 0;
    start_ompl->as<ob::RealVectorStateSpace::StateType>()->values[4] = -90.0f;
    start_ompl->as<ob::RealVectorStateSpace::StateType>()->values[5] = 0;

    // goal state
    goal_ompl->as<ob::RealVectorStateSpace::StateType>()->values[0] = 90.0f;
    goal_ompl->as<ob::RealVectorStateSpace::StateType>()->values[1] = -135.0f;
    goal_ompl->as<ob::RealVectorStateSpace::StateType>()->values[2] = 135.0f;
    goal_ompl->as<ob::RealVectorStateSpace::StateType>()->values[3] = 0;
    goal_ompl->as<ob::RealVectorStateSpace::StateType>()->values[4] = -90.0f;
    goal_ompl->as<ob::RealVectorStateSpace::StateType>()->values[5] = 0;

    armProblemDefinition->setStartAndGoalStates(start_ompl, goal_ompl);

    ompl::base::PlannerStatus solved = armPlanner->solve(30.0f);

    if(solved){
        std::cout << "solved!" <<std::endl;

        std::cout <<std::endl;
        std::cout << "Print path:"<<std::endl;

        auto armPathInGridOmpl = armProblemDefinition->getSolutionPath();

        path = std::static_pointer_cast<ompl::geometric::PathGeometric>(armPathInGridOmpl)->getStates();

        std::cout << "States count = " << path.size() <<std::endl;

        for(auto i = 0 ;i<path.size() ; i++)
        {
            std::cout << "States " <<i <<std::endl;
            for(auto k = 0;k<6;k++)
            {
                std::cout << path[i]->as<ob::RealVectorStateSpace::StateType>()->values[k] <<std::endl;
            }
            std::cout <<std::endl;
        }

        auto goal = goal_ompl->as<ob::RealVectorStateSpace::StateType>()->values;

        DisplayState(path[1]);

    /*
        m_LeftPtr->RotateJoint("arm_left2",goal[0]);
        m_LeftPtr->RotateJoint("arm_left3",goal[1]);
        m_LeftPtr->RotateJoint("arm_left4",goal[2]);
        m_LeftPtr->RotateJoint("arm_left5",goal[3]);
        m_LeftPtr->RotateJoint("arm_left6",goal[4]);
        m_LeftPtr->RotateJoint("palm_left",goal[5]);
*/
    }
    else
        std::cout << "solutions not found" <<std::endl;

    pEngine->run();

    pEngine->waitForStop();

    return 0;
}
