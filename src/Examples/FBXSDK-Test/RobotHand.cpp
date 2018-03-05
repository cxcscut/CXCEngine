#include "RobotHand.h"

Robothand::Robothand():
	HType(ROBOTHAND_LEFT), OriginPos(glm::vec3({0.0f,0.0f,0.0f}))
{
	script_parser = Parser::GetInstance();

}

Robothand::Robothand(HandType type,const std::string &model_file)
	:Robothand()
{
	if (LoadRobothand(type,model_file))
		SetLoaded();

	HType = type;
}

Robothand::~Robothand()
{

}

void Robothand::InitOriginalDegrees() noexcept
{
	for (auto it : m_Joint_Axis)
		m_JointDegrees.insert(std::make_pair(it.first, 0.0f));
}

void Robothand::SetBaseDegrees(const std::vector<float> &bases) noexcept
{
	if (bases.size() > m_Joint_Axis.size())
		return;

	auto p = bases.cbegin();

	for (auto it : m_Joint_Axis)
		if (p == bases.cend())
			m_BaseDegrees.insert(std::make_pair(it.first,0.0f));
		else
			m_BaseDegrees.insert(std::make_pair(it.first, *p++));
}

void Robothand::addKnuckleLength(HandType type) noexcept
{

	auto lengths = (type == ROBOTHAND_LEFT) ? g_FinLength_L : g_FinLength_R;

	//  Add kunckle length
	for (auto it : lengths)
	{
		auto fin_name = it.first;

		auto p = m_FingerLength.find(fin_name);
		if (p != m_FingerLength.end())
			// if exists, update it
			m_FingerLength[fin_name] = it.second;
		else
			m_FingerLength.insert(std::make_pair(fin_name,it.second));
	}
}

void Robothand::addKeyPoints(HandType type) noexcept
{

	auto points = (type == ROBOTHAND_LEFT) ? g_KeyPoints_L : g_KeyPoints_R;

	// Add key points to model
	for (auto it : points)
	{
		auto joint_name = it.first;
		auto ModelPtr = GetModelByName(joint_name);
		if (ModelPtr)
			ModelPtr->addKeyPoints(LeftToRightCoordinateTrans(CoordinateSysTransToOGL(it.second)));
	}
}

void Robothand::addJointConstraint() noexcept
{

}

void Robothand::LoadRoot(HandType type) noexcept
{
	auto roots = (type == ROBOTHAND_LEFT) ? g_RootNode_L : g_RootNode_R;

	// Add root node
	for (auto it : roots)
	{
		AddRoot(it);
	}
}

void Robothand::LoadChild(HandType type) noexcept
{
	auto childs = (type == ROBOTHAND_LEFT) ? g_ChildNodes_L : g_ChildNodes_R;

	// Add child nodes
	for (auto it : childs)
		AddChild(it.first, it.second);
}

bool Robothand::LoadRobothand(HandType type,const std::string &model_file) noexcept
{
	// Set Name
	auto ObjectName = (type == ROBOTHAND_LEFT) ? LeftHandName : RightHandName;

	SetObjectName(ObjectName);

	SetTag("robot");

	if (!LoadOBJFromFile(model_file))
		return false;

	LoadRotationAxis(type);

	LoadRoot(type);

	LoadChild(type);

	//addJointConstraint();

	addKeyPoints(type);

	addKnuckleLength(type);

	return true;
}

void Robothand::LoadMotionsFromScript(const std::string &scriptfile) noexcept
{
	// Load script file
	if (!script_parser->ParseScript(scriptfile))
		return;

	// Parsing
	auto it = script_parser->m_TokenAttribs.find("Rotation");
	if (it != script_parser->m_TokenAttribs.end())
	{
		auto attribs = it->second;

		std::string str1 = "JointName", str2 = "Angle";
		auto len1 = str1.length(), len2 = str2.length();

		for (auto attrib : attribs)
		{
			// Get Joint name
			auto pos = attrib.find_first_of(str1);
			if (pos == std::string::npos)	break;

			auto first_pos = attrib.find_first_of('=',pos + len1);
			auto last_pos = attrib.find_first_of(',',first_pos + 1);

			if (first_pos == std::string::npos || last_pos == std::string::npos)	break;

			auto joint_name = attrib.substr(first_pos + 1,last_pos - first_pos - 1);

			/********************************************************************************************/
			// Get rotation angle

			pos = attrib.find_first_of(str2,last_pos);
			if (pos == std::string::npos)	break;

			first_pos = attrib.find_first_of('=', pos + len2);
			last_pos = attrib.length();

			if (first_pos == std::string::npos || last_pos == std::string::npos)	break;

			auto angle = atof(attrib.substr(first_pos + 1, last_pos - first_pos - 1).c_str());

			// Perform rotation
			RotateJoint(joint_name,angle);
		}
	}

	// Translation

	// Scaling
}

glm::vec3 Robothand::CoordinateSysTransToOGL(const glm::vec3 &vec) noexcept
{
	return { vec.x,vec.z,vec.y };
}

glm::vec3 Robothand::LeftToRightCoordinateTrans(const glm::vec3 &vec) noexcept
{

	// Transformation rule (x,y,z) => (x,y,-z)

	return { vec.x, vec.y, -vec.z };
}

glm::vec3 Robothand::CoordinateSysTransTo3DS(const glm::vec3 &vec) noexcept
{
	return {vec.x,vec.z,vec.y};
}

glm::vec3 Robothand::RightToLeftCoordinateTrans(const glm::vec3 &vec) noexcept
{
	return {vec.x,vec.y,-vec.z};
}

void Robothand::LoadRotationAxis(HandType type) noexcept
{

	auto axis = (type == ROBOTHAND_LEFT) ? g_Axis_L : g_Axis_R;

	for (auto it : axis)
	{
		auto Joint_name = it.first;
		auto axis_pos = CoordinateSysTransToOGL(it.second.StartPoint);
		auto axis_dir = CoordinateSysTransToOGL(it.second.Direction);

		// store axis

		auto _it = m_Joint_Axis.find(Joint_name);
		if (_it == m_Joint_Axis.end())
			m_Joint_Axis.insert(std::make_pair(Joint_name, std::make_shared<RotationAxis>(LeftToRightCoordinateTrans(axis_pos), LeftToRightCoordinateTrans(axis_dir))));
		else
			// if exist, then update it
			_it->second = std::make_shared<RotationAxis>(LeftToRightCoordinateTrans(axis_pos), LeftToRightCoordinateTrans(axis_dir));

	}

}

std::shared_ptr<RotationAxis> Robothand::GetRotationAxisPtr(const std::string &JointName) const noexcept
{
	if (!CheckLoadingStatus())
		return nullptr;

	auto it = m_Joint_Axis.find(JointName);
	if (it != m_Joint_Axis.end())
		return it->second;
	else
		return nullptr;
}

bool Robothand::CheckAngleConstraint(const std::string &JointName, float degree) const noexcept
{
	auto it = m_JointConstraints.find(JointName);

	// if no constraint, always return true
	if (it == m_JointConstraints.end())
		return true;

	return degree >= it->second.first && degree <= it->second.second;
}

void Robothand::ResetAllJointPose() noexcept
{
	for (auto it : m_JointDegrees)
		RotateJoint(it.first, -(it.second));
}

void Robothand::ResetJointPose(const std::string &JointName) noexcept
{
	auto it = m_JointDegrees.find(JointName);
	if  (it != m_JointDegrees.end())
		RotateJoint(JointName,-(it->second));
}

std::map<std::string, std::shared_ptr<RotationAxis>> Robothand::GetJointAxisMap() const noexcept {
	return m_Joint_Axis;
}

float Robothand::GetJointDegree(const std::string &jointname) const noexcept
{
	auto it = m_JointDegrees.find(jointname);
	if (it != m_JointDegrees.end())
		return it->second;
	else
		return 65535.0f;
}

bool Robothand::RotateJoint(const std::string &JointName, float degree) noexcept
{
	if (!CheckAngleConstraint(JointName, degree))
		return false;

	auto RotationAxisPtr = GetRotationAxisPtr(JointName);
	if (RotationAxisPtr == nullptr)
		return false;

	RotateWithArbitraryAxis(JointName,RotationAxisPtr->StartPoint,RotationAxisPtr->Direction,glm::radians(degree));

	UpdateRotationAxisPos(JointName, RotationAxisPtr->StartPoint, RotationAxisPtr->Direction, glm::radians(degree));

	// Constraint range from 0 to 360 degrees
	auto it = m_JointDegrees.find(JointName);
	if (it != m_JointDegrees.end())
	{
		if (it->second + degree > 360.0f)
			it->second = it->second + degree - 360.0f;
		else if (it->second + degree < -360.0f)
			it->second = it->second + degree + 360.0f;
		else
			it->second += degree;
	}

	return true;
}

void Robothand::IntegralTranslation(float x, float y, float z) noexcept
{
	auto TreePtr = GetObjectTreePtr();

	if (!CheckLoaded() || TreePtr.empty())
		return;

	for (auto &it : TreePtr)
	{
		auto RootModelPtr = it->root;
		auto RootName = RootModelPtr->GetModelName();

		Translation(RootName,glm::vec3(x, z, -y));

		UpdateRotationAxisPos(RootName, glm::vec3(x,z,-y));
	}

	OriginPos += glm::vec3({x,y,z});

}

glm::vec3 Robothand::GetOriginPos() const noexcept
{
	return OriginPos;
}

void Robothand::IntegralRotation(float sx, float sy, float sz, float dx, float dy, float dz, float degree) noexcept
{
	auto TreePtr = GetObjectTreePtr();

	if (!CheckLoaded() || TreePtr.empty())
		return;

	for (auto &it : TreePtr)
	{
		auto RootModelPtr = it->root;
		auto RootName = RootModelPtr->GetModelName();
		RotateWithArbitraryAxis(RootName, glm::vec3(sz, sy, sz), glm::vec3(dx, dy, dz), degree);

		UpdateRotationAxisPos(RootName, glm::vec3(sx, sy, sz), glm::vec3(dx,dy,dz),degree);
	}
}

float Robothand::GetBaseDegrees(const std::string &joint_name) const noexcept
{
	auto it = m_BaseDegrees.find(joint_name);
	if (it != m_BaseDegrees.end())
		return it->second;
	else return 0.0f;
}

void Robothand::UpdateRotationAxisPos(const std::string &JointName, const glm::vec3 &TransVec) noexcept
{
	auto object_tree = GetObjectTreePtr();
	for (auto rootNode : object_tree)
	{
		auto root_name = rootNode->root->GetModelName();

		if (root_name== JointName)
		{
			// Update Root node
			auto it = m_Joint_Axis.find(JointName);
			if (it != m_Joint_Axis.end())
				it->second->StartPoint += TransVec;

			// Update Child node
			auto children = rootNode->GetAllChildren();
			for (auto childNode : children)
			{
				auto _it = m_Joint_Axis.find(childNode->root->GetModelName());
				if (_it != m_Joint_Axis.end())
					_it->second->StartPoint += TransVec;
			}
			break;
		}
		else
		{
			std::shared_ptr<ObjectTree> ChildAxisPtr;
			if (FindChild(root_name, JointName, ChildAxisPtr))
			{
				// update root node
				auto it = m_Joint_Axis.find(ChildAxisPtr->root->GetModelName());
				if (it != m_Joint_Axis.end())
					it->second->StartPoint += TransVec;

				// update child node
				auto child_vec = ChildAxisPtr->GetAllChildren();
				for (auto childnode : child_vec)
				{
					auto _it = m_Joint_Axis.find(childnode->root->GetModelName());
					if (_it != m_Joint_Axis.end())
						_it->second->StartPoint += TransVec;
				}

			}
		}
	}
}

void Robothand::UpdateRotationAxisPos(const std::string &JointName, const glm::vec3 &start, const glm::vec3 &direction, float degree) noexcept
{
	auto object_tree = GetObjectTreePtr();
	for (auto rootNode : object_tree)
	{
		auto root_name = rootNode->root->GetModelName();
		if (root_name == JointName)
		{
			// Update root node
			auto it = m_Joint_Axis.find(JointName);
			if (it != m_Joint_Axis.end())
			{
				auto start_pos = it->second->StartPoint;

				it->second->StartPoint = CalculateRotatedCoordinate(start_pos, start, direction, degree);

				it->second->Direction = CalculateRotatedCoordinate(start_pos + it->second->Direction, start, direction, degree)
					- it->second->StartPoint;
			}

			// Update child node
			auto child_vec = rootNode->GetAllChildren();
			for (auto childnode : child_vec)
			{
				auto _it = m_Joint_Axis.find(childnode->root->GetModelName());
				if (_it != m_Joint_Axis.end())
				{
					auto start_pos = it->second->StartPoint;

					_it->second->StartPoint = CalculateRotatedCoordinate(start_pos, start, direction, degree);

					_it->second->Direction = CalculateRotatedCoordinate(start_pos + _it->second->Direction, start, direction, degree)
						- _it->second->StartPoint;
				}
			}
		}
		else
		{
			std::shared_ptr<ObjectTree> ChildAxisPtr;
			if (FindChild(root_name, JointName, ChildAxisPtr))
			{
				// update root node
				auto it = m_Joint_Axis.find(ChildAxisPtr->root->GetModelName());
				if (it != m_Joint_Axis.end())
				{
					auto start_pos = it->second->StartPoint;

					it->second->StartPoint = CalculateRotatedCoordinate(start_pos, start, direction, degree);

					it->second->Direction = CalculateRotatedCoordinate(start_pos + it->second->Direction, start, direction, degree)
						- it->second->StartPoint;
				}

				// update child node
				auto child_vec = ChildAxisPtr->GetAllChildren();
				for (auto childnode : child_vec)
				{
					auto _it = m_Joint_Axis.find(childnode->root->GetModelName());
					if (_it != m_Joint_Axis.end())
					{
						auto start_pos = _it->second->StartPoint;

						_it->second->StartPoint = CalculateRotatedCoordinate(start_pos, start, direction, degree);

						_it->second->Direction = CalculateRotatedCoordinate(start_pos + _it->second->Direction, start, direction, degree)
							- _it->second->StartPoint;
					}
				}
			}
		}
	}
}

void Robothand::MovingArm(const glm::mat4 &pose) noexcept
{
	auto target_pose = pose;
	std::vector<float> base_degree, current_degree;
	std::vector<std::string> arm_joint_names;

	auto pBaseDegrees = m_BaseDegrees.cbegin();
	auto pJointDegrees = m_JointDegrees.cbegin();
	for (size_t i = 0; i < ARM_JOINT_NUM-1; i++) {
		base_degree.emplace_back(pBaseDegrees->second);
		current_degree.emplace_back(pJointDegrees->second);
		arm_joint_names.emplace_back(pBaseDegrees->first);
		pBaseDegrees++; pJointDegrees++;
	}

	if (HType == ROBOTHAND_LEFT)
	{
		base_degree.emplace_back(GetBaseDegrees("palm_left"));
		current_degree.emplace_back(GetJointDegree("palm_left"));
		arm_joint_names.emplace_back("palm_left");
	}
	else
	{
		base_degree.emplace_back(GetBaseDegrees("palm_right"));
		current_degree.emplace_back(GetJointDegree("palm_right"));
		arm_joint_names.emplace_back("palm_right");
	}

	target_pose[3][1] = -target_pose[3][1];

	target_pose[3][0] -= OriginPos.x;
	target_pose[3][1] -= OriginPos.y;
	target_pose[3][2] -= OriginPos.z;

	std::vector<float> optimal_sol;

	if (KineSolver::getOptimalSolution(KineSolver::InverseKinematics(target_pose), target_pose, 0.1f, optimal_sol))
		for (size_t k = 0; k < arm_joint_names.size(); k++)
			RotateJoint(arm_joint_names[k], -current_degree[k] + base_degree[k] - optimal_sol[k]);
}

void Robothand::MovingArm(const std::vector<float> &eular_space) noexcept
{
	auto target_pose = KineSolver::EularSpaceToOrientation(eular_space);
	std::vector<float> base_degree,current_degree;
	std::vector<std::string> arm_joint_names;

	auto pBaseDegrees = m_BaseDegrees.cbegin();
	auto pJointDegrees = m_JointDegrees.cbegin();
	for (size_t i = 0; i < ARM_JOINT_NUM-1; i++) {
		base_degree.emplace_back(pBaseDegrees->second);
		current_degree.emplace_back(pJointDegrees->second);
		arm_joint_names.emplace_back(pBaseDegrees->first);
		pBaseDegrees++; pJointDegrees++;
	}

	if (HType == ROBOTHAND_LEFT)
	{
		base_degree.emplace_back(GetBaseDegrees("palm_left"));
		current_degree.emplace_back(GetJointDegree("palm_left"));
		arm_joint_names.emplace_back("palm_left");
	}
	else
	{
		base_degree.emplace_back(GetBaseDegrees("palm_right"));
		current_degree.emplace_back(GetJointDegree("palm_right"));
		arm_joint_names.emplace_back("palm_right");
	}

	target_pose[3][1] = -target_pose[3][1];

	target_pose[3][0] -= OriginPos.x;
	target_pose[3][1] -= OriginPos.y;
	target_pose[3][2] -= OriginPos.z;

	std::vector<float> optimal_sol;

	if(KineSolver::getOptimalSolution(KineSolver::InverseKinematics(target_pose), target_pose,0.1f,optimal_sol))
		for (size_t k = 0; k < arm_joint_names.size(); k++)
			RotateJoint(arm_joint_names[k], -current_degree[k] + base_degree[k] - optimal_sol[k]);

}

void Robothand::MovingArmOrientationOffset(GLfloat x, GLfloat y, GLfloat z) noexcept
{
	std::vector<float> degrees;
	std::vector<std::string> arm_joint_names;

	auto pBaseDegrees = m_BaseDegrees.cbegin();
	auto pJointDegrees = m_JointDegrees.cbegin();
	for (size_t i = 0; i < ARM_JOINT_NUM - 1; i++) {
		degrees.push_back(pBaseDegrees->second - pJointDegrees->second);
		arm_joint_names.emplace_back(pBaseDegrees->first);
		pBaseDegrees++; pJointDegrees++;
	}

	if (HType == ROBOTHAND_LEFT) {
		degrees.push_back(GetBaseDegrees("palm_left") - GetJointDegree("palm_left"));
		arm_joint_names.emplace_back("palm_left");
	}
	else {
		degrees.push_back(GetBaseDegrees("palm_right") - GetJointDegree("palm_right"));
		arm_joint_names.emplace_back("palm_right");
	}

	auto pose_matrix = KineSolver::ForwardKinematics(degrees);

	auto pose_offset = KineSolver::EularSpaceToOrientation(std::vector<float>({0,0,0,x,y,z}));

	auto target_pose = pose_offset * pose_matrix;

	std::vector<float> optimal_sol;

	if (KineSolver::getOptimalSolution(KineSolver::InverseKinematics(target_pose), target_pose, 0.1f, optimal_sol))
		for (size_t k = 0; k < arm_joint_names.size(); k++)
			RotateJoint(arm_joint_names[k], degrees[k] - optimal_sol[k]);
}

void Robothand::MovingArmOffset(const glm::vec3 &pos_offset) noexcept
{
	std::vector<float> degrees;
	std::vector<std::string> arm_joint_names;

	auto pBaseDegrees = m_BaseDegrees.cbegin();
	auto pJointDegrees = m_JointDegrees.cbegin();
	for (size_t i = 0; i < ARM_JOINT_NUM - 1; i++) {
		degrees.push_back(pBaseDegrees->second - pJointDegrees->second);
		arm_joint_names.emplace_back(pBaseDegrees->first);
		pBaseDegrees++; pJointDegrees++;
	}

	if (HType == ROBOTHAND_LEFT) {
		degrees.push_back(GetBaseDegrees("palm_left") - GetJointDegree("palm_left"));
		arm_joint_names.emplace_back("palm_left");
	}
	else {
		degrees.push_back(GetBaseDegrees("palm_right") - GetJointDegree("palm_right"));
		arm_joint_names.emplace_back("palm_right");
	}

	auto pose_matrix = KineSolver::ForwardKinematics(degrees);

	// position transformation
	auto pose_offset = glm::translate(glm::mat4(1.0f), glm::vec3({pos_offset.x,-pos_offset.y,pos_offset.z}));

	// transforamtion performed on the global coordinate systems, so left-multiplication used here
	auto target_pose = pose_offset * pose_matrix;

	std::vector<float> optimal_sol;

	if(KineSolver::getOptimalSolution(KineSolver::InverseKinematics(target_pose), target_pose,0.1f,optimal_sol))
		for (size_t k = 0; k < arm_joint_names.size(); k++)
			RotateJoint(arm_joint_names[k], degrees[k] - optimal_sol[k]);

    std::cout << target_pose[3][0] << ","<<target_pose[3][1] << "," <<target_pose[3][2]<<std::endl;
}
#include "stdafx.h"

