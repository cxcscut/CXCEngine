#ifndef CXC_MULTITREE_INL
#define CXC_MULTITREE_INL

#include <memory>
#include <vector>
#include "General/EngineCore.h"

namespace cxc
{
	template <class T>
	class CXC_ENGINECORE_API MultiTree
	{
	
	public:

		MultiTree()
		{

		}

		~MultiTree()
		{

		}

		MultiTree(const T & _root)
		{
			root = _root;
		}

		MultiTree(const MultiTree<T> &newtree)
		{
			root = newtree.root;
			children = newtree.children;
		}

	public:

		void addChildNode(const std::shared_ptr<MultiTree<T>> &child_node) noexcept {
			children.emplace_back(child_node);
		}

		bool findChildNode(const T &child_node,std::shared_ptr<MultiTree<T>> &ret_node) noexcept {

			for (auto it : children)
			{
				if (it->root == child_node)
				{
					ret_node = it;
					return true;
				}
				else
				{
					auto ret = it->findChildNode(child_node,ret_node);
					if (ret)
						return true;
				}
			}

			return false;
		}

		std::vector<std::shared_ptr<MultiTree<T>>> GetAllChildren() const noexcept
		{
			std::vector<std::shared_ptr<MultiTree<T>>> ret;

			if (children.empty())
				return ret;

			for (auto it : children) {
				ret.push_back(it);
				
				auto more_child = it->GetAllChildren();

				for (auto _it : more_child)
					ret.push_back(_it);
			}

			return ret;
		}

	public:

		// root node
		T root;
		
		// children nodes
		std::vector<std::shared_ptr<MultiTree<T>>> children;
	};

}

#endif  // CXC_MULTITREE_INL
