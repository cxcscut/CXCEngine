#ifdef WIN32

#include "..\..\Engine\General\DefineTypes.h"

#else

#include "../../Engine/General/DefineTypes.h"

#endif // WIN32

#ifndef ROBOTHAND_GLOBALS_H
#define ROBOTHAND_GLOBALS_H

static const std::string LeftHandName = "SZRobothandL";
static const std::string RightHandName = "SZRobothandR";

static const std::string ModelPath_Lefthand = ".\\Examples\\RobotSim\\Model\\SZrobotl.obj";
static const std::string ModelPath_RightHand = ".\\Examples\\RobotSim\\Model\\SZrobotr.obj";

static const std::string PlaneFile = ".\\Examples\\RobotSim\\Model\\plane.obj";

static const std::string TableFile = ".\\Examples\\RobotSim\\Model\\table.obj";

static const std::string WidgetFile = ".\\Examples\\RobotSim\\Model\\widget.obj";

static const std::string ScriptDir = ".\\Examples\\RobotSim\\";

static const std::string VertexShaderPath = "..\\Engine\\Shader\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "..\\Engine\\Shader\\StandardFragmentShader.glsl";

// Glove calibration file
static const std::string DataGloveCalibFile = ".\\Examples\\RobotSim\\5DT\\jc.cal";

// Rotation axis description
using RotationAxis = struct RotationAxis {

	glm::vec3 StartPoint;
	glm::vec3 Direction;

	RotationAxis() :
		StartPoint(glm::vec3(0.0f, 0.0f, 0.0f)),
		Direction(glm::vec3(0.0f, 0.0f, 0.0f))
	{}

	RotationAxis(const glm::vec3 &start_point, const glm::vec3 &dir)
	{
		StartPoint = start_point;
		Direction = dir;
	}

};

// table origin
static const glm::vec3 table_origin = {-719.453f,1211.043f,500.0f};

// Right hand

// Rotation Axis
static const std::map<std::string, RotationAxis> g_Axis_R = {
	{ "arm_right2",{{ 0.0,0.0,0.0 },{0,0,1}}},
	{ "arm_right3",{{-220.0,-70.069,380.948},{0,1,0}}},
	{"arm_right4",{{302.331,55.044,380.948},{0,1,0}}},
	{ "arm_right5",{{252.331,-4.976,460.948},{1,0,0}}},
	{"arm_right6",{{-257.849,-33.536,472.143},{0,1,0}}},
	{"palm_right",{{-257.849,-4.376,566.913},{0,0,1}}},
	{ "thumb_palm_right",{ { -258.476,46.682,614.544 },{ 0,0,1 } } },
	{"thumb_right1",{{ -259.879,62.302,668.935 },{1,0,0}}},
	{"thumb_right2",{{ -250.304,84.84,683.291 },{0,-22.44,12.987}}},
	{"thumb_right3",{{ -250.304,100.607,710.610},{ 0,-22.44,12.987 }}},
	{"thumb_right4",{{ -250.304,116.69,737.591 },{ 0,-22.44,12.987 }}},
	{"index_right1",{{ -258.867,24.067,684.956 },{1,0,0}}},
	{"index_right2",{{ -249.336,46.742,710.653 },{0,1,0}}},
	{"index_right3",{{ -249.350,46.737,774.005 },{0,1,0}}},
	{"index_right4",{{ -249.100,46.729,830.120 },{0,1,0}}},
	{"middle_right1",{{ -262.829,9.653,689.711 },{1,0,0}}},
	{ "middle_right2",{ { -253.298,22.274,714.522 },{0,1,0} } },
	{ "middle_right3",{ { -253.316,22.269,797.213 },{0,1,0} } },
	{"middle_right4",{ { -253.067,22.261,853.762 },{0,1,0} }},
	{"ring_right1",{{ -262.689,-16.309,688.362 },{1,0,0}}},
	{ "ring_right2",{ { -253.298,-3.634,714.430 },{0,1,0} } },
	{ "ring_right3",{ { -253.304,-3.639,778.123 },{0,1,0} } },
	{ "ring_right4",{ { -253.056,-3.647,834.454 },{0,1,0} } },
	{ "little_right1",{ { -262.1,-40.095,687.857 },{1,0,0} } },
	{ "little_right2",{ { -252.569,-27.421,713.652 },{0,1,0} } },
	{ "little_right3",{ { -252.584,-27.426,751.865 },{0,1,0} } },
	{ "little_right4",{ { -252.333,-27.433,789.557 },{0,1,0} } }
};

// Finger length
static const std::unordered_map<std::string,float> g_FinLength_R = {
	{"thumb_right1",26.774},{"thumb_right2",38.032},{ "thumb_right3",37.733},{ "thumb_right4",30.67},
	{"index_right1",26.699},{ "index_right2",64.024},{ "index_right3",56.479},{ "index_right4",30.365},
	{ "middle_right1",25.522},{ "middle_right2",83.379},{ "middle_right3",56.974},{ "middle_right1",31.208},
	{ "ring_right1",31.765},{ "ring_right2",56.417},{ "ring_right3",63.376},{ "ring_right4",26.009},
	{ "little_right1",24.72},{ "little_right2",33.074},{ "little_right3",30.678},{ "little_right4",32.495}
};

// Key points
static const std::unordered_map<std::string, glm::vec3> g_KeyPoints_R = {
	{"arm_right1",{ 0.0,-4.956,300.948 }},{ "arm_right2",{ 0.0,-42.469,300.948 } },
	{ "arm_right3",{ 302.331,4.976,390.948 } },{ "arm_right4",{ 252.331,-4.976,460.948 } },
	{ "arm_right5",{ -225.170,-4.956,460.948 } },{ "arm_right6",{ -257.849,0,566.913 } },
	{ "palm_right",{ -258.823,8.698,641.778 } },{ "index_right1",{ -249.336,46.742,710.653 } },
	{ "index_right2",{ -249.350,46.737,774.005 } },{ "index_right3",{ -249.100,46.729,830.120 } },
	{ "index_right4",{ -258.592,34.073,850.650 } },{ "thumb_right1",{ -250.304,73.718,689.784 } },
	{ "thumb_right2",{ -250.304,89.485,717.103 } },{ "thumb_right3",{ -250.304,105.568,744.084 } },
	{ "thumb_right4",{-259.616,116.827,763.362} },{ "little_right1",{-252.569,-27.421,713.652} },
	{ "little_right2",{-252.584,-27.426,751.865} },{ "little_right3",{-252.333,-27.433,789.557} },
	{ "little_right4",{-261.825,-40.109,809.530} },{ "ring_right1",{-253.298,-3.634,714.430} },
	{ "ring_right2",{-253.304,-3.639,778.123} },{ "ring_right3",{253.056,-3.647,834.454} },
	{ "ring_right4",{-262.544,-16.313,854.530} },{ "middle_right1",{-253.298,22.274,714.522} },
	{ "middle_right2",{-253.316,22.269,797.213} },{ "middle_right3",{-253.067,22.261,853.762} },
	{ "middle_right4",{-262.554,9.586,873.870} },{ "palm_right",{-250.343,62.165,666.508} }
};

// Root node
static const std::vector<std::string> g_RootNode_R = {
	"arm_right1"
};

// Child nodes
static const std::vector <std::pair<std::string, std::string>> g_ChildNodes_R = {
	{ "arm_right1","arm_right2" },{ "arm_right2","arm_right3" },{ "arm_right3","arm_right4" },
	{ "arm_right4","arm_right5" },{ "arm_right5","arm_right6" },{ "arm_right6","palm_right" },
	{ "palm_right","thumb_palm_right" },{ "palm_right","index_right1" },{ "palm_right","middle_right1" },
	{ "palm_right","ring_right1" },{ "palm_right","little_right1" },{"thumb_palm_right","thumb_right1"},
	{ "thumb_right1","thumb_right2" },{ "thumb_right2","thumb_right3" },{ "thumb_right3","thumb_right4" },
	{ "index_right1","index_right2" },{ "index_right2","index_right3" },{ "index_right3","index_right4" },
	{ "middle_right1","middle_right2" },{ "middle_right2","middle_right3" },{ "middle_right3","middle_right4" },
	{ "ring_right1","ring_right2" },{ "ring_right2","ring_right3" },{ "ring_right3","ring_right4" },
	{ "little_right1","little_right2" },{ "little_right2","little_right3" },{ "little_right3","little_right4" }
};


// Left hand

// Rotation Axis
static const std::map<std::string, RotationAxis> g_Axis_L = {
	{ "arm_left2",{ { 0.0,-4.956,300.948 },{ 0,0,1 } } },
	{ "arm_left3",{ { -220.0,-70.069,380.948 },{ 0,1,0 } } },
	{ "arm_left4",{ { 302.331,55.044,380.948 },{ 0,1,0 } } },
	{ "arm_left5",{ { 252.331,-4.976,460.948 },{ 1,0,0 } } },
	{ "arm_left6",{ { -257.849,-33.536,472.143 },{ 0,1,0 } } },
	{ "palm_left",{ { -257.849,-4.376,566.913 },{ 0,0,1 } } },
	{ "thumb_palm_left",{ { -261.196,-58.824,620.737 },{ 0,0,1 } } },
	{ "thumb_left1",{ { -243.575,-74.314,674.019 },{ 1,0,0 } } },
	{ "thumb_left2",{ { -253.313,-98.078,684.486 },{ 0,-21.921,-12.625 } } },
	{ "thumb_left3",{ { -253.313,-91.967,724.775 },{ 0,-21.921,-12.625 } } },
	{ "thumb_left4",{ { -253.313,-108.469,752.997 },{ 0,-21.921,-12.625 } } },
	{ "index_left1",{ { -242.563,-45.987,683.965 },{ 1,0,0 } } },
	{ "index_left2",{ { -251.957,-33.365,710.017 },{ 0,1,0 } } },
	{ "index_left3",{ { -251.957,-33.609,774.178 },{ 0,1,0 } } },
	{ "index_left4",{ {  -251.957,-33.529,830.701},{ 0,1,0 } } },
	{ "middle_left1",{ { -246.525,-21.536,689.007 },{ 1,0,0 } } },
	{ "middle_left2",{ { -255.944,-8.951,713.775 },{ 0,1,0 } } },
	{ "middle_left3",{ { -255.944,-9.066,796.718 },{ 0,1,0 } } },
	{ "middle_left4",{ { -255.944,-9.061,853.91},{ 0,1,0 } } },
	{ "ring_left1",{ { -264.121,4.576,688.189 },{ 1,0,0 } } },
	{ "ring_left2",{ { -258.503,17.03,713.941 },{ 0,1,0 } } },
	{ "ring_left3",{ { -258.503,16.842,777.68 },{ 0,1,0 } } },
	{ "ring_left4",{ { -258.503,16.847,834.018 },{ 0,1,0 } } },
	{ "little_left1",{ { -245.797,28.121,688.224 },{ 1,0,0 } } },
	{ "little_left2",{ { -255.033,40.816,713.45 },{ 0,1,0 } } },
	{ "little_left3",{ { -255.033,40.553,752.039},{ 0,1,0 } } },
	{ "little_left4",{ { -255.033,40.698,790.395},{ 0,1,0 } } }
};

// Finger length
static const std::unordered_map<std::string, float> g_FinLength_L = {
	{ "thumb_left1",26.774 },{ "thumb_left2",38.032 },{ "thumb_left3",37.733 },{ "thumb_left4",30.67 },
	{ "index_left1",26.699 },{ "index_left2",64.024 },{ "index_left3",56.479 },{ "index_left4",30.365 },
	{ "middle_left1",25.522 },{ "middle_left2",83.379 },{ "middle_left3",56.974 },{ "middle_left1",31.208 },
	{ "ring_left1",31.765 },{ "ring_left2",56.417 },{ "ring_left3",63.376 },{ "ring_left4",26.009 },
	{ "little_left1",24.72 },{ "little_left2",33.074 },{ "little_left3",30.678 },{ "little_left4",32.495 }
};

// Key points
static const std::unordered_map<std::string, glm::vec3> g_KeyPoints_L = {
	{ "arm_left1",{ 0.0,-4.956,300.948 } },{ "arm_left2",{ 0.0,-42.469,300.948 } },
	{ "arm_left3",{ 302.331,4.976,390.948 } },{ "arm_left4",{ 252.331,-4.976,460.948 } },
	{ "arm_left5",{ -225.170,-4.956,460.948 } },{ "arm_left6",{ -257.849,0,566.913 } },
	{ "palm_left",{ -258.823,8.698,641.778 } },{ "index_left1",{ -251.957,-33.365,710.017 } },
	{ "index_left2",{ -251.957,-33.609,774.178 } },{ "index_left3",{ -251.957,-33.529,830.701 } },
	{ "index_left4",{ -257.514,-45.782,852.708 } },{ "thumb_left1",{ -253.313,-98.078,684.486 } },
	{ "thumb_left2",{ -253.313,-91.967,724.775 } },{ "thumb_left3",{ -253.313,-108.469,752.997 } },
	{ "thumb_left4",{ -258.539,-129.061,765.381 } },{ "little_left1",{ -255.033,40.816,713.45 } },
	{ "little_left2",{ -255.033,40.553,752.039 } },{ "little_left3",{ -255.033,40.698,790.395 } },
	{ "little_left4",{ -264.544,28.38,811.584 } },{ "ring_left1",{ -258.503,17.03,713.941 } },
	{ "ring_left2",{ -258.503,16.842,777.68 } },{ "ring_left3",{ -258.503,16.847,834.018 } },
	{ "ring_left4",{ -261.476,4.594,856.658 } },{ "middle_left1",{ -255.944,-8.951,713.775 } },
	{ "middle_left2",{ -255.944,-9.066,796.718 } },{ "middle_left3",{ 255.944,-9.061,853.91 } },
	{ "middle_left4",{ -261.476,-21.314,875.917 } },{ "palm_left",{ -250.343,62.165,666.508 } }
};

// Root node
static const std::vector<std::string> g_RootNode_L = {
	"arm_left1"
};

// Child nodes
static const std::vector <std::pair<std::string, std::string>> g_ChildNodes_L = {
	{ "arm_left1","arm_left2" },{ "arm_left2","arm_left3" },{ "arm_left3","arm_left4" },
	{ "arm_left4","arm_left5" },{ "arm_left5","arm_left6" },{ "arm_left6","palm_left" },
	{ "palm_left","thumb_palm_left" },{ "palm_left","index_left1" },{ "palm_left","middle_left1" },
	{ "palm_left","ring_left1" },{ "palm_left","little_left1" },{"thumb_palm_left","thumb_left1"},
	{ "thumb_left1","thumb_left2" },{ "thumb_left2","thumb_left3" },{ "thumb_left3","thumb_left4" },
	{ "index_left1","index_left2" },{ "index_left2","index_left3" },{ "index_left3","index_left4" },
	{ "middle_left1","middle_left2" },{ "middle_left2","middle_left3" },{ "middle_left3","middle_left4" },
	{ "ring_left1","ring_left2" },{ "ring_left2","ring_left3" },{ "ring_left3","ring_left4" },
	{ "little_left1","little_left2" },{ "little_left2","little_left3" },{ "little_left3","little_left4" }
};

// Action examples for hands
// <Action name, Joint angles>
// Joint angles : Left thumb palm|Right thumb palm|Left thumb||Right thumb|Left index|Right index|Left middle|Right middle|Left ring|Right ring|Left little|Right little
// Each sets of joint angles has 4 degrees corresponding to each joint, for examples:
// Left thumb <===> thumb_left1, thumb_left2, thumb_left3, thumb_left4
static const std::unordered_map<std::string, std::vector<double>> g_ActionExams_Hand = {
	{ "动作 : 挤",{30,30, -10,30,30,70, -10,30,30,70, -10,60,60,80, -10,60,60,80, 0,60,60,80, 0,60,60,80, 10,60,60,80, 10,60,60,80, 10,60,60,80, 10,60,60,80}},
	{ "动作 : 夹",{60,60, 5,0,50,0, 5,0,50,0, 9.01,-23.03,74.9,49.93, 9.01,-23.03,74.9,49.93, 5.86,-0.42,29.48,19.66, 5.86,-0.42,29.48,19.66, 6.11,8.19,16.22,10.82, 6.11,8.19,16.22,10.82, 8.54,17.3,13.39,8.92, 8.54,17.3,13.39,8.92,}},
	{ "动作 : 剪",{100,60, -10,30,60,0, -10,0,60,20, -2,-5,15,0, 0,0,20,10, 0,-5,15,0, 0,10,70,80, 2,-5,15,0, 0,10,70,80, 4,-5,15,0, 0,10,70,80}},
	{ "动作 : 推",{0,0, -20,-20,40,5, -20,-20,40,5, -5,-15,30,0 ,-5,-15,30,0, 0,-15,30,0 ,0,-15,30,0, 5,-15,30,0, 5,-15,30,0, 5,-15,30,0, 5,-15,30,0} },
	{ "动作 : 托",{30,30, 30,0,30,0, 30,0,30,0, 0,0,30,30, 0,0,30,30, 0,0,30,30, 0,0,30,30, 0,0,30,30, 0,0,30,30, 0,0,30,30, 0,0,30,30 } },
	{ "动作 : 握",{30,30, 0,32.02,101.87,67.91, 0,32.02,101.87,67.91, 0,65.15,116.02,77.35, 0,65.15,116.02,77.35, 0,69.69,107.37,71.58, 0,69.69,107.37,71.58, 0,68.46,114.15,76.1, 0,68.46,114.15,76.1, 0,52.93,91.94,61.29, 0,52.93,91.94,61.29 }},
	{ "动作 : 抓",{40,40, 5,10,50,30, 5,10,50,30, 0,-23.29,75.87,50.58, 0,-23.29,75.87,50.58, 0,-19.46,86.23,57.49, 0,-19.46,86.23,57.49, 0,-23.82,78.06,52.04, 0,-23.82,78.06,52.04, 0,2.9,32.89,21.93,0,2.9,32.89,21.93 }},
	{ "动作 : 拉",{70,70, 0,0,20,5, 0,0,20,5, 0,0,60,30, 0,0,60,30, 0,0,60,30, 0,0,60,30, 0,0,60,30, 0,0,60,30, 0,0,60,30, 0,0,60,30} },
	{ "动作 : 插",{60,60, 5,0,50,0, 0,60,60,10, 9.01,-23.03,74.9,49.93, 0,60,60,40, 7.21,-16.57,65.34,43.56, 0,60,60,40, 7.21,-13.59,52.13,34.75, 0,60,60,40, 8.54,17.3,13.39,8.92, 0,60,60,40}},
	{ "动作 : 钳",{60,60, -10,0,30,10, 0,10,70,90, -12,-10,90,10, 0,0,20,10, 0,69.69,107.37,71.58, 0,10,70,90, 0,68.46,114.15,76.1, 0,10,70,90, 0,55.83,100.1,66.73, 0,10,70,90}},
	{ "动作 : 撕",{60,60, -10,0,30,10, -10,0,30,10, -12,-10,90,10, -12,-10,90,10, 0,69.69,107.37,71.58, 0,69.69,107.37,71.58, 0,68.46,114.15,76.1, 0,68.46,114.15,76.1, 0,55.83,100.1,66.73, 0,55.83,100.1,66.73 }}
};

// Action examples for arms
// <Action name, Joint angles>
// Joint angles : Left arm | Left palm | Right arm | Right palm
// Each sets of joint angles has 5 degrees corresponding to each joint, for examples:
// Left arm <===> arm_left2, arm_left3, arm_left4, arm_left5, arm_left6

// ATTENTION: arm_left1 can't not be moved or rotated
static const std::unordered_map<std::string, std::vector<double>> g_ActionExams_Arm = {
	{"动作 : 挤",{ 140,0,135,0,-75,0, -140,0,135,0,-75,0 }},
	{"动作 : 夹",{150,0,135,0,-45,0, -150,0,135,0,-45,0}},
	{"动作 : 剪",{150,0,135,0,-45,0, -150,0,135,0,-45,0}},
	{"动作 : 撕",{140,0,135,0,-75,0, -140,0,135,0,-75,0}},
	{"动作 : 推",{150,0,150,0,-120,0, -150,0,150,0,-120,0}},
	{"动作 : 托",{150,0,135,180,-135,0, -150,0,135,180,-135,0}},
	{"动作 : 握",{ 150,0,135,180,-135,0, -150,0,135,180,-135,0 }},
	{"动作 : 抓",{ 150,0,150,0,-120,0, -150,0,150,0,-120,0 }},
	{"动作 : 拉",{ 150,0,135,0,-45,0, -150,0,135,0,-45,0 }},
	{"动作 : 插",{140,0,135,0,-75,0 , -140,0,135,0,-75,0}},
	{"动作 : 钳",{ 140,0,135,0,-75,0, -140,0,135,0,-75,0 }}
};

// Name mapping
static const std::unordered_map<std::string, std::string> g_NameMap = {
	{"arm_left1","左-手臂1"},{"arm_right1","右-手臂1"},
	{"arm_left2","左-手臂2"},{"arm_right2","右-手臂2"},
	{"arm_left3","左-手臂3"},{"arm_right3","右-手臂3"},
	{"arm_left4","左-手臂4"},{"arm_right4","右-手臂4"},
	{"arm_left5","左-手臂5"},{"arm_right5","右-手臂5"},
	{"arm_left6","左-手臂6"},{"arm_right6","右-手臂6"},
	{"palm_left","左-手掌"},{"palm_right","右-手掌"},
	{ "thumb_palm_left","左-手掌-大拇指" },{ "thumb_palm_right","右-手掌-大拇指" },
	{ "thumb_left1","左-大拇指-近端" },{ "thumb_right1","右-大拇指-近端" },
	{ "thumb_left2","左-大拇指-中间" },{ "thumb_right2","右-大拇指-中间" },
	{ "thumb_left3","左-大拇指-远端" },{ "thumb_right3","右-大拇指-远端" },
	{ "thumb_left4","左-大拇指-末端" },{ "thumb_right4","右-大拇指-末端" },
	{ "index_left1","左-食指-近端" },{ "index_right1","右-食指-近端" },
	{ "index_left2","左-食指-中间" },{ "index_right2","右-食指-中间" },
	{ "index_left3","左-食指-远端" },{ "index_right3","右-食指-远端" },
	{ "index_left4","左-食指-末端" },{ "index_right4","右-食指-末端" },
	{ "middle_left1","左-中指-近端" },{ "middle_right1","右-中指-近端" },
	{ "middle_left2","左-中指-中间" },{ "middle_right2","右-中指-中间" },
	{ "middle_left3","左-中指-远端" },{ "middle_right3","右-中指-远端" },
	{ "middle_left4","左-中指-末端" },{ "middle_right4","右-中指-末端" },
	{ "ring_left1","左-无名指-近端" },{ "ring_right1","右-无名指-近端" },
	{ "ring_left2","左-无名指-中间" },{ "ring_right2","右-无名指-中间" },
	{ "ring_left3","左-无名指-远端" },{ "ring_right3","右-无名指-远端" },
	{ "ring_left4","左-无名指-末端" },{ "ring_right4","右-无名指-末端" },
	{ "little_left1","左-小指-近端" },{ "little_right1","右-小指-近端" },
	{ "little_left2","左-小指-中间" },{ "little_right2","右-小指-中间" },
	{ "little_left3","左-小指-远端" },{ "little_right3","右-小指-远端" },
	{ "little_left4","左-小指-末端" },{ "little_right4","右-小指-末端" }
};

#endif // ROBOTHAND_GLOBALS_H
