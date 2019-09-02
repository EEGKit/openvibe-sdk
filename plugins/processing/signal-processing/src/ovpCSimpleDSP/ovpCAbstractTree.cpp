#include "ovpCAbstractTree.h"

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
		CAbstractTreeNode* node = m_pRoot;

		//tries to simplify the tree.
		change = m_pRoot->simplify(node);

		//if the root node has changed
		if (node != m_pRoot)
		{
			//delete the old one
			delete m_pRoot;
			m_pRoot = node;
		}
	}
}

// Dirty hack to avoid GCC 4.3 crash at compilation time
static void clear_m_oChildren(std::vector<CAbstractTreeNode *>& m_oChildren) { for (size_t i = 0; i < m_oChildren.size(); i++) { delete m_oChildren[i]; } }

CAbstractTreeParentNode::~CAbstractTreeParentNode()
{
	// Dirty hack to avoid GCC 4.3 crash at compilation time
	clear_m_oChildren(m_oChildren);
}

void CAbstractTreeParentNode::levelOperators()
{
	uint64_t nChildren = m_oChildren.size();

	vector<CAbstractTreeNode*> l_oNewChildren;

	//for all the node's children
	for (uint64_t i = 0; i < nChildren; i++)
	{
		CAbstractTreeNode* l_pChild = m_oChildren[(size_t)i];

		//recursively try to level the childs' operators
		l_pChild->levelOperators();

		//if the child is a terminal node
		if (l_pChild->isTerminal())
		{
			//add it to the children list
			l_oNewChildren.push_back(l_pChild);
		}
		else
		{
			//else it's a parent node
			CAbstractTreeParentNode* l_pChildParentNode = reinterpret_cast<CAbstractTreeParentNode*>(l_pChild);

			//if the child and the current node have the same id
			if (m_ui64Identifier == l_pChildParentNode->getOperatorIdentifier())
			{
				switch (m_ui64Identifier)
				{
						//check if it is the ID of the + or * operators
					case OP_ADD:
					case OP_MUL:

						//if it is, we can group the child's children with the current node's children
						l_oNewChildren.insert(l_oNewChildren.end(), l_pChildParentNode->getChildren().begin(), l_pChildParentNode->getChildren().end());

						//we don't want it to destroy its old children
						l_pChildParentNode->getChildren().clear();

						//we no longer need this child
						delete l_pChildParentNode;
						l_pChildParentNode = nullptr;

						break;

					default:
						//this kind of node isn't an associative one, so keep the child
						l_oNewChildren.push_back(l_pChild);
						break;
				}
			}
			else { l_oNewChildren.push_back(l_pChild); }
		}
	}

	m_oChildren = l_oNewChildren;

	//for + or *
	if (isAssociative())
	{
		//if the node is associative/commutative, reorder the children
		sort(m_oChildren.begin(), m_oChildren.end(), CAbstractTreeNodeOrderingFunction());
	}
}

bool CAbstractTreeParentNode::simplify(CAbstractTreeNode*& node)
{
	//result boolean, true if a child has changed
	bool l_bHasChanged = false;

	//true if a child has changed
	bool l_bChildrenChanged = true;

	//number of children of this node
	uint64_t nChildren = m_oChildren.size();

	//while the children aren't stable
	while (l_bChildrenChanged)
	{
		l_bChildrenChanged = false;

		//try to simplify all the children
		for (size_t i = 0; i < nChildren; i++)
		{
			CAbstractTreeNode* l_pChild = m_oChildren[i];
			l_bChildrenChanged          = l_pChild->simplify(l_pChild);

			//if there has been a change, actualize l_bHasChanged
			l_bHasChanged |= l_bChildrenChanged;

			//if the child has become a new node
			if (m_oChildren[i] != l_pChild)
			{
				//delete the old one and replace it
				delete m_oChildren[i];
				m_oChildren[i] = l_pChild;
			}
		}
	}

	//unary operator
	if (nChildren == 1)
	{
		//if we can already compute the result
		if (m_oChildren[0]->isConstant())
		{
			double value = reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[0])->getValue();
			switch (m_ui64Identifier)
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
			l_bHasChanged = true;
		}
	}
		//binary operator not associative
	else if (nChildren == 2 && !isAssociative())
	{
		double l_f64TotalValue = 0;

		//if we can already compute the result
		if (m_oChildren[0]->isConstant() && m_oChildren[1]->isConstant())
		{
			switch (m_ui64Identifier)
			{
				case OP_DIV:
					l_f64TotalValue = reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[0])->getValue() / reinterpret_cast<CAbstractTreeValueNode*>(
										  m_oChildren[1])->getValue();

					//delete the old value nodes
					delete m_oChildren[0];
					m_oChildren[0] = nullptr;
					delete m_oChildren[1];
					m_oChildren[1] = nullptr;

					node          = new CAbstractTreeValueNode(l_f64TotalValue);
					l_bHasChanged = true;

					break;

				case OP_POW:

					l_f64TotalValue = pow(reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[0])->getValue(),
										  reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[1])->getValue());

					//delete the old value nodes
					delete m_oChildren[0];
					m_oChildren[0] = nullptr;
					delete m_oChildren[1];
					m_oChildren[1] = nullptr;

					node          = new CAbstractTreeValueNode(l_f64TotalValue);
					l_bHasChanged = true;
					break;
				default: break;
			}
		}

			//test special cases (X/1), ..., simplify
		else if (m_ui64Identifier == OP_DIV)
		{
			if (!m_oChildren[0]->isConstant() && m_oChildren[1]->isConstant())
			{
				if (reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[1])->getValue() == 1)
				{
					node = m_oChildren[0];
					m_oChildren.clear();
					l_bHasChanged = true;
				}
			}
		}
	}
		//if the node is an associative operation node, there are at least two children and at least two are constants
	else if (nChildren >= 2 && isAssociative())
	{
		//For commutative nodes
		//The order of the children may have changed due to previous child simplification
		sort(m_oChildren.begin(), m_oChildren.end(), CAbstractTreeNodeOrderingFunction());

		//the new children if there are changes
		vector<CAbstractTreeNode *> l_oNewChildren;

		//iterator on the children
		size_t i               = 0;
		double l_f64TotalValue = 0;

		switch (m_ui64Identifier)
		{
			case OP_ADD:
				l_f64TotalValue = 0;

				//add the values of all the constant children
				for (i = 0; i < nChildren && m_oChildren[i]->isConstant(); i++)
				{
					l_f64TotalValue += reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[i])->getValue();

					//delete the old value nodes
					delete m_oChildren[i];
					m_oChildren[i] = nullptr;
				}
				break;

			case OP_MUL:
				l_f64TotalValue = 1;
				//multiply the values of all the constant children
				for (i = 0; i < nChildren && m_oChildren[i]->isConstant(); i++)
				{
					l_f64TotalValue *= reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[i])->getValue();

					//delete the old value nodes
					delete m_oChildren[i];
					m_oChildren[i] = nullptr;
				}
				break;
			default: break;
		}

		//if there were only value nodes, we can replace the current parent node by a value node
		if (i == nChildren)
		{
			node          = new CAbstractTreeValueNode(l_f64TotalValue);
			l_bHasChanged = true;
			// cout<<l_f64TotalValue<<endl;
		}
			//if there are still some other children, but we reduced at least two children
		else if (i > 1)
		{
			//adds the new result node to the list
			l_oNewChildren.push_back(new CAbstractTreeValueNode(l_f64TotalValue));

			//adds the other remaining children
			for (; i < nChildren; i++) { l_oNewChildren.push_back(m_oChildren[i]); }
			//we keep this node, but modify its children
			m_oChildren = l_oNewChildren;

			l_bHasChanged = true;
		}
		else if (i == 1)
		{
			//nothing changed
			if ((l_f64TotalValue == 0 && m_ui64Identifier == OP_ADD) ||
				(l_f64TotalValue == 1 && m_ui64Identifier == OP_MUL))
			{
				if (nChildren - i == 1)
				{
					node = m_oChildren[i];
					m_oChildren.clear();
				}
				else
				{
					//don't keep the valueNode
					//adds the other remaining children
					for (; i < nChildren; i++) { l_oNewChildren.push_back(m_oChildren[i]); }

					//we keep this node, but modify its children
					m_oChildren = l_oNewChildren;
				}
				l_bHasChanged = true;
			}
			else if (l_f64TotalValue == 0 && m_ui64Identifier == OP_MUL)
			{
				//kill this node and replace it by a 0 node
				node          = new CAbstractTreeValueNode(0);
				l_bHasChanged = true;
			}
			else
			{
				//undo changes
				m_oChildren[0] = new CAbstractTreeValueNode(l_f64TotalValue);
			}
		}
	}

	return l_bHasChanged;
}

void CAbstractTreeParentNode::useNegationOperator()
{
	uint64_t nChildren = m_oChildren.size();

	//try to use the negation operator in all the children
	for (size_t i = 0; i < nChildren; i++)
	{
		CAbstractTreeNode* l_pChild = m_oChildren[i];
		l_pChild->useNegationOperator();
	}

	//replace (/ Something -1) by (NEG Something)
	if (m_ui64Identifier == OP_DIV)
	{
		if (m_oChildren[1]->isConstant())
		{
			if (reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[1])->getValue() == -1)
			{
				m_ui64Identifier = OP_NEG;
				m_oChildren.pop_back();
			}
		}
	}
		//replace (* -1 ...) by (NEG (* ...))
	else if (m_ui64Identifier == OP_MUL)
	{
		if (m_oChildren[0]->isConstant())
		{
			if (reinterpret_cast<CAbstractTreeValueNode*>(m_oChildren[0])->getValue() == -1)
			{
				m_ui64Identifier = OP_NEG;
				m_bIsAssociative = false;

				//if there were just two children : replace (* -1 Sth) by (NEG Sth)
				if (nChildren == 2)
				{
					m_oChildren[0] = m_oChildren[1];
					m_oChildren.pop_back();
				}
					//>2 there were more than two children
				else
				{
					CAbstractTreeParentNode* l_pNewOperatorNode = new CAbstractTreeParentNode(OP_MUL, true);

					for (size_t i = 1; i < nChildren; i++) { l_pNewOperatorNode->addChild(m_oChildren[i]); }

					m_oChildren.clear();
					m_oChildren.push_back(l_pNewOperatorNode);
				}
			}
		}
	}
}

void CAbstractTree::generateCode(CEquationParser& parser) { m_pRoot->generateCode(parser); }

void CAbstractTreeParentNode::generateCode(CEquationParser& parser)
{
	const uint64_t nChildren = m_oChildren.size();

#if 0

	// REMOVED BY YRD
	// Bruno's implementation of the stack filling looked weird
	// and bugged to me. I simplified/corrected it

	//if it is a unary operator/function
	if(nChildren==1) { parser.push_op(m_ui64Identifier); }
	else
	{
		for(size_t i = 0 ; i<nChildren-1 ; i++)
		{
			parser.push_op(m_ui64Identifier);
			m_oChildren[i] -> generateCode(parser);
		}
	}
	m_oChildren[size_t(nChildren-1)] -> generateCode(parser);
#else
	parser.push_op(m_ui64Identifier);
	for (size_t i = 0; i < nChildren; i++) { m_oChildren[i]->generateCode(parser); }
#endif
}

void CAbstractTreeValueNode::generateCode(CEquationParser& parser) { parser.push_value(m_f64Value); }

void CAbstractTreeVariableNode::generateCode(CEquationParser& parser) { parser.push_var(m_ui32Index); }

void CAbstractTree::recognizeSpecialTree(uint64_t& treeId, double& parameter)
{
	//default
	treeId    = OP_USERDEF;
	parameter = 0;

	//the root node is a value node or variable node
	if (m_pRoot->isTerminal())
	{
		//if it is a variable node
		if (!m_pRoot->isConstant()) { treeId = OP_NONE; }
		return;
	}

	CAbstractTreeParentNode* l_pParent = reinterpret_cast<CAbstractTreeParentNode *>(m_pRoot);

	std::vector<CAbstractTreeNode *>& l_oChildren = l_pParent->getChildren();
	const uint64_t nChildren                      = l_oChildren.size();
	const uint64_t nodeId                         = l_pParent->getOperatorIdentifier();

	//unary operator/function
	if (nChildren == 1) { if (l_oChildren[0]->isTerminal() && !l_oChildren[0]->isConstant()) { treeId = nodeId; } }
		//binary
	else if (nChildren == 2)
	{
		bool l_bIsVariable[2];
		l_bIsVariable[0] = l_oChildren[0]->isTerminal() && !l_oChildren[0]->isConstant();
		l_bIsVariable[1] = l_oChildren[1]->isTerminal() && !l_oChildren[1]->isConstant();

		//(* X X)
		if (nodeId == OP_MUL && l_bIsVariable[0] && l_bIsVariable[1]) { treeId = OP_X2; }
			//pow(X,2)
		else if (nodeId == OP_POW && l_bIsVariable[0] && l_oChildren[1]->isConstant()) { treeId = OP_X2; }
			//(+ Cst X) or (* Cst X)
		else if (l_pParent->isAssociative() && l_oChildren[0]->isConstant() && l_bIsVariable[1])
		{
			treeId    = nodeId;
			parameter = reinterpret_cast<CAbstractTreeValueNode*>(l_oChildren[0])->getValue();
		}
			// (/ X Cst)
		else if (nodeId == OP_DIV && l_bIsVariable[0] && l_oChildren[1]->isConstant())
		{
			treeId    = OP_DIV;
			parameter = reinterpret_cast<CAbstractTreeValueNode*>(l_oChildren[1])->getValue();
		}
	}
	//else do nothing
}
