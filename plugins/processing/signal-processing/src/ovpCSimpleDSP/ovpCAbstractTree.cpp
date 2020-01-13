#include "ovpCAbstractTree.h"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
using namespace boost::spirit;
using namespace OpenViBE;

//#define CABSTRACTTREE_DEBUG

void CAbstractTree::simplifyTree()
{
	bool change = true;

	//while stability hasn't been reached
	while (change)
	{
		CAbstractTreeNode* node = m_root;

		//tries to simplify the tree.
		change = m_root->simplify(node);

		//if the root node has changed
		if (node != m_root)
		{
			//delete the old one
			delete m_root;
			m_root = node;
		}
	}
}

// Dirty hack to avoid GCC 4.3 crash at compilation time
static void ClearChildren(std::vector<CAbstractTreeNode *>& children) { for (size_t i = 0; i < children.size(); ++i) { delete children[i]; } }

CAbstractTreeParentNode::~CAbstractTreeParentNode()
{
	// Dirty hack to avoid GCC 4.3 crash at compilation time
	ClearChildren(m_Children);
}

void CAbstractTreeParentNode::levelOperators()
{
	const size_t nChildren = m_Children.size();

	vector<CAbstractTreeNode*> newChildren;

	//for all the node's children
	for (size_t i = 0; i < nChildren; ++i)
	{
		CAbstractTreeNode* child = m_Children[i];

		//recursively try to level the childs' operators
		child->levelOperators();

		//if the child is a terminal node
		if (child->isTerminal())
		{
			//add it to the children list
			newChildren.push_back(child);
		}
		else
		{
			//else it's a parent node
			CAbstractTreeParentNode* childParentNode = reinterpret_cast<CAbstractTreeParentNode*>(child);

			//if the child and the current node have the same id
			if (m_ID == childParentNode->getOperatorIdentifier())
			{
				switch (m_ID)
				{
						//check if it is the ID of the + or * operators
					case OP_ADD:
					case OP_MUL:

						//if it is, we can group the child's children with the current node's children
						newChildren.insert(newChildren.end(), childParentNode->getChildren().begin(), childParentNode->getChildren().end());

						//we don't want it to destroy its old children
						childParentNode->getChildren().clear();

						//we no longer need this child
						delete childParentNode;
						childParentNode = nullptr;

						break;

					default:
						//this kind of node isn't an associative one, so keep the child
						newChildren.push_back(child);
						break;
				}
			}
			else { newChildren.push_back(child); }
		}
	}

	m_Children = newChildren;

	//for + or *
	if (isAssociative())
	{
		//if the node is associative/commutative, reorder the children
		sort(m_Children.begin(), m_Children.end(), CAbstractTreeNodeOrderingFunction());
	}
}

bool CAbstractTreeParentNode::simplify(CAbstractTreeNode*& node)
{
	//result boolean, true if a child has changed
	bool hasChanged = false;

	//true if a child has changed
	bool childrenChanged = true;

	//number of children of this node
	const size_t nChildren = m_Children.size();

	//while the children aren't stable
	while (childrenChanged)
	{
		childrenChanged = false;

		//try to simplify all the children
		for (size_t i = 0; i < nChildren; ++i)
		{
			CAbstractTreeNode* child = m_Children[i];
			childrenChanged          = child->simplify(child);

			//if there has been a change, actualize hasChanged
			hasChanged |= childrenChanged;

			//if the child has become a new node
			if (m_Children[i] != child)
			{
				//delete the old one and replace it
				delete m_Children[i];
				m_Children[i] = child;
			}
		}
	}

	//unary operator
	if (nChildren == 1)
	{
		//if we can already compute the result
		if (m_Children[0]->isConstant())
		{
			const double value = reinterpret_cast<CAbstractTreeValueNode*>(m_Children[0])->getValue();
			switch (m_ID)
			{
				case OP_NEG: node = new CAbstractTreeValueNode(-value);
					break;
				case OP_ABS: node = new CAbstractTreeValueNode(abs(value));
					break;
				case OP_ACOS: node = new CAbstractTreeValueNode(acos(value));
					break;
				case OP_ASIN: node = new CAbstractTreeValueNode(asin(value));
					break;
				case OP_ATAN: node = new CAbstractTreeValueNode(atan(value));
					break;
				case OP_CEIL: node = new CAbstractTreeValueNode(ceil(value));
					break;
				case OP_COS: node = new CAbstractTreeValueNode(cos(value));
					break;
				case OP_EXP: node = new CAbstractTreeValueNode(exp(value));
					break;
				case OP_FLOOR: node = new CAbstractTreeValueNode(floor(value));
					break;
				case OP_LOG: node = new CAbstractTreeValueNode(log(value));
					break;
				case OP_LOG10: node = new CAbstractTreeValueNode(log10(value));
					break;
				case OP_RAND: node = new CAbstractTreeValueNode(rand() * value / RAND_MAX);
					break;
				case OP_SIN: node = new CAbstractTreeValueNode(sin(value));
					break;
				case OP_SQRT: node = new CAbstractTreeValueNode(sqrt(value));
					break;
				case OP_TAN: node = new CAbstractTreeValueNode(tan(value));
					break;
				default: break;
			}
			hasChanged = true;
		}
	}
		//binary operator not associative
	else if (nChildren == 2 && !isAssociative())
	{

		//if we can already compute the result
		if (m_Children[0]->isConstant() && m_Children[1]->isConstant())
		{
			switch (m_ID)
			{
				case OP_DIV:
				{
					const double total = reinterpret_cast<CAbstractTreeValueNode*>(m_Children[0])->getValue() / reinterpret_cast<CAbstractTreeValueNode*>(m_Children[1])->getValue();

					//delete the old value nodes
					delete m_Children[0];
					m_Children[0] = nullptr;
					delete m_Children[1];
					m_Children[1] = nullptr;

					node       = new CAbstractTreeValueNode(total);
					hasChanged = true;

					break;
				}
				case OP_POW:
				{
					const double total = pow(reinterpret_cast<CAbstractTreeValueNode*>(m_Children[0])->getValue(), reinterpret_cast<CAbstractTreeValueNode*>(m_Children[1])->getValue());

					//delete the old value nodes
					delete m_Children[0];
					m_Children[0] = nullptr;
					delete m_Children[1];
					m_Children[1] = nullptr;

					node       = new CAbstractTreeValueNode(total);
					hasChanged = true;
					break;
				}
				default: break;
			}
		}

			//test special cases (X/1), ..., simplify
		else if (m_ID == OP_DIV)
		{
			if (!m_Children[0]->isConstant() && m_Children[1]->isConstant())
			{
				if (reinterpret_cast<CAbstractTreeValueNode*>(m_Children[1])->getValue() == 1)
				{
					node = m_Children[0];
					m_Children.clear();
					hasChanged = true;
				}
			}
		}
	}
		//if the node is an associative operation node, there are at least two children and at least two are constants
	else if (nChildren >= 2 && isAssociative())
	{
		//For commutative nodes
		//The order of the children may have changed due to previous child simplification
		sort(m_Children.begin(), m_Children.end(), CAbstractTreeNodeOrderingFunction());

		//the new children if there are changes
		vector<CAbstractTreeNode *> newChildren;

		//iterator on the children
		size_t i     = 0;
		double total = 0;

		switch (m_ID)
		{
			case OP_ADD:
				total = 0;

				//add the values of all the constant children
				for (i = 0; i < nChildren && m_Children[i]->isConstant(); ++i)
				{
					total += reinterpret_cast<CAbstractTreeValueNode*>(m_Children[i])->getValue();

					//delete the old value nodes
					delete m_Children[i];
					m_Children[i] = nullptr;
				}
				break;

			case OP_MUL:
				total = 1;
				//multiply the values of all the constant children
				for (i = 0; i < nChildren && m_Children[i]->isConstant(); ++i)
				{
					total *= reinterpret_cast<CAbstractTreeValueNode*>(m_Children[i])->getValue();

					//delete the old value nodes
					delete m_Children[i];
					m_Children[i] = nullptr;
				}
				break;
			default: break;
		}

		//if there were only value nodes, we can replace the current parent node by a value node
		if (i == nChildren)
		{
			node       = new CAbstractTreeValueNode(total);
			hasChanged = true;
			// cout<<l_TotalValue<<endl;
		}
			//if there are still some other children, but we reduced at least two children
		else if (i > 1)
		{
			//adds the new result node to the list
			newChildren.push_back(new CAbstractTreeValueNode(total));

			//adds the other remaining children
			for (; i < nChildren; ++i) { newChildren.push_back(m_Children[i]); }
			//we keep this node, but modify its children
			m_Children = newChildren;

			hasChanged = true;
		}
		else if (i == 1)
		{
			//nothing changed
			if ((total == 0 && m_ID == OP_ADD) ||
				(total == 1 && m_ID == OP_MUL))
			{
				if (nChildren - i == 1)
				{
					node = m_Children[i];
					m_Children.clear();
				}
				else
				{
					//don't keep the valueNode
					//adds the other remaining children
					for (; i < nChildren; ++i) { newChildren.push_back(m_Children[i]); }

					//we keep this node, but modify its children
					m_Children = newChildren;
				}
				hasChanged = true;
			}
			else if (total == 0 && m_ID == OP_MUL)
			{
				//kill this node and replace it by a 0 node
				node       = new CAbstractTreeValueNode(0);
				hasChanged = true;
			}
			else
			{
				//undo changes
				m_Children[0] = new CAbstractTreeValueNode(total);
			}
		}
	}

	return hasChanged;
}

void CAbstractTreeParentNode::useNegationOperator()
{
	const size_t nChildren = m_Children.size();

	//try to use the negation operator in all the children
	for (size_t i = 0; i < nChildren; ++i)
	{
		CAbstractTreeNode* child = m_Children[i];
		child->useNegationOperator();
	}

	//replace (/ Something -1) by (NEG Something)
	if (m_ID == OP_DIV)
	{
		if (m_Children[1]->isConstant())
		{
			if (reinterpret_cast<CAbstractTreeValueNode*>(m_Children[1])->getValue() == -1)
			{
				m_ID = OP_NEG;
				m_Children.pop_back();
			}
		}
	}
		//replace (* -1 ...) by (NEG (* ...))
	else if (m_ID == OP_MUL)
	{
		if (m_Children[0]->isConstant())
		{
			if (reinterpret_cast<CAbstractTreeValueNode*>(m_Children[0])->getValue() == -1)
			{
				m_ID            = OP_NEG;
				m_IsAssociative = false;

				//if there were just two children : replace (* -1 Sth) by (NEG Sth)
				if (nChildren == 2)
				{
					m_Children[0] = m_Children[1];
					m_Children.pop_back();
				}
					//>2 there were more than two children
				else
				{
					CAbstractTreeParentNode* newOperatorNode = new CAbstractTreeParentNode(OP_MUL, true);

					for (size_t i = 1; i < nChildren; ++i) { newOperatorNode->addChild(m_Children[i]); }

					m_Children.clear();
					m_Children.push_back(newOperatorNode);
				}
			}
		}
	}
}

void CAbstractTree::generateCode(CEquationParser& parser) { m_root->generateCode(parser); }

void CAbstractTreeParentNode::generateCode(CEquationParser& parser)
{
	const size_t nChildren = m_Children.size();
	parser.push_op(m_ID);
	for (size_t i = 0; i < nChildren; ++i) { m_Children[i]->generateCode(parser); }
}

void CAbstractTreeValueNode::generateCode(CEquationParser& parser) { parser.push_value(m_value); }

void CAbstractTreeVariableNode::generateCode(CEquationParser& parser) { parser.push_var(m_index); }

void CAbstractTree::recognizeSpecialTree(uint64_t& treeId, double& parameter)
{
	//default
	treeId    = OP_USERDEF;
	parameter = 0;

	//the root node is a value node or variable node
	if (m_root->isTerminal())
	{
		//if it is a variable node
		if (!m_root->isConstant()) { treeId = OP_NONE; }
		return;
	}

	CAbstractTreeParentNode* parent = reinterpret_cast<CAbstractTreeParentNode *>(m_root);

	std::vector<CAbstractTreeNode *>& children = parent->getChildren();
	const size_t nChildren                     = children.size();
	const uint64_t nodeId                      = parent->getOperatorIdentifier();

	//unary operator/function
	if (nChildren == 1) { if (children[0]->isTerminal() && !children[0]->isConstant()) { treeId = nodeId; } }
		//binary
	else if (nChildren == 2)
	{
		std::array<bool, 2> isVariable;
		isVariable[0] = children[0]->isTerminal() && !children[0]->isConstant();
		isVariable[1] = children[1]->isTerminal() && !children[1]->isConstant();

		//(* X X)
		if (nodeId == OP_MUL && isVariable[0] && isVariable[1]) { treeId = OP_X2; }
			//pow(X,2)
		else if (nodeId == OP_POW && isVariable[0] && children[1]->isConstant()) { treeId = OP_X2; }
			//(+ Cst X) or (* Cst X)
		else if (parent->isAssociative() && children[0]->isConstant() && isVariable[1])
		{
			treeId    = nodeId;
			parameter = reinterpret_cast<CAbstractTreeValueNode*>(children[0])->getValue();
		}
			// (/ X Cst)
		else if (nodeId == OP_DIV && isVariable[0] && children[1]->isConstant())
		{
			treeId    = OP_DIV;
			parameter = reinterpret_cast<CAbstractTreeValueNode*>(children[1])->getValue();
		}
	}
	//else do nothing
}
