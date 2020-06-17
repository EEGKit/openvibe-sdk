#pragma once

#include "../ovp_defines.h"

#include "ovpCEquationParserGrammar.h"
#include "ovpCEquationParser.h"

#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>

#include <boost/spirit/include/classic_ast.hpp>

#include <vector>
#include <string>

class CEquationParser;

/**
* Abstract class for an AST tree node
*
*/
class CAbstractTreeNode
{
protected:

	//! True if this is a terminal node
	bool m_isTerminal = false;
	//! True if this node contains a constant value
	bool m_isConstant = false;

public:

	CAbstractTreeNode(const bool bTerminal, const bool bIsConstant) : m_isTerminal(bTerminal), m_isConstant(bIsConstant) { }

	//! virtual destructor
	virtual ~CAbstractTreeNode() { }

	/**
	* Used to know if this node is a leaf.
	* \return True if the node is a leaf.
	*/
	virtual bool isTerminal() const { return m_isTerminal; }

	/**
	 * Used to know if this node is a constant value node.
	 * \return True if the node is a constant value node.
	 */
	virtual bool isConstant() const { return m_isConstant; }

	//! Prints the node to stdout.
	virtual void print(OpenViBE::Kernel::ILogManager& rLogManager) = 0;

	/**
	* Used to simplify this node (and its children if any).
	* \param pModifiedNode Reference to a pointer to modify if the
	* current node object is to be destroyed and replaced. This pointer
	* will contain the address of the new node.
	*/
	virtual bool simplify(CAbstractTreeNode*& pModifiedNode) = 0;

	/**
	* Part of the process of simplification.
	* Levels recursively the associative operators nodes.
	*/
	virtual void levelOperators() = 0;

	/**
	* Changes the tree so it uses the NEG operator whenever it is possible.
	* (ie replaces (* -1 X) by (NEG X)
	*/
	virtual void useNegationOperator() = 0;

	/**
	* Generates the set of function calls needed to do the desired computation.
	* \param oParser The parser containing the function pointers stack and function contexts stack.
	*/
	virtual void generateCode(CEquationParser& oParser) = 0;
};

/**
* A tree's parent node (has children).
*
*/
class CAbstractTreeParentNode : public CAbstractTreeNode
{
public:

	//! Children of this node
	std::vector<CAbstractTreeNode*> m_Children;

	//! The node operator's identifier
	uint64_t m_ID = 0;

	//! True if the node is "associative"
	bool m_IsAssociative = false;

	//Constructors
	CAbstractTreeParentNode(const uint64_t nodeId, const bool isAssociative = false)
		: CAbstractTreeNode(false, false), m_ID(nodeId), m_IsAssociative(isAssociative) { }

	CAbstractTreeParentNode(const uint64_t nodeId, CAbstractTreeNode* child, const bool isAssociative = false)
		: CAbstractTreeNode(false, false), m_ID(nodeId), m_IsAssociative(isAssociative) { m_Children.push_back(child); }

	CAbstractTreeParentNode(const uint64_t nodeId, CAbstractTreeNode* leftChild, CAbstractTreeNode* rightChild, const bool isAssociative = false)
		: CAbstractTreeNode(false, false), m_ID(nodeId), m_IsAssociative(isAssociative)
	{
		m_Children.push_back(leftChild);
		m_Children.push_back(rightChild);
	}

	CAbstractTreeParentNode(const uint64_t nodeId, CAbstractTreeNode* testChild, CAbstractTreeNode* ifChild, CAbstractTreeNode* thenChild,
							const bool isAssociative = false)
		: CAbstractTreeNode(false, false), m_ID(nodeId), m_IsAssociative(isAssociative)
	{
		m_Children.push_back(testChild);
		m_Children.push_back(ifChild);
		m_Children.push_back(thenChild);
	}

	/**
	 * Returns the node's operator identifier.
	 * \return The operator identifier
	 */
	uint64_t getOperatorIdentifier() const { return m_ID; }

	/**
	 * Used to know if the node is an associative node.
	 * \return True if the node is an associative one.
	 */
	bool isAssociative() const { return m_IsAssociative; }

	/**
	 * Returns the vector of children of the node.
	 * \return A reference to the vector of children.
	 */
	virtual std::vector<CAbstractTreeNode*>& getChildren() { return m_Children; }

	/**
	 * Adds a child to this node.
	 * \param child The child to add.
	 */
	virtual void addChild(CAbstractTreeNode* child) { m_Children.push_back(child); }

	//! Destructor.
	~CAbstractTreeParentNode() override;

	//! Debug function, prints the node and its children (prefix notation)
	void print(OpenViBE::Kernel::ILogManager& logManager) override
	{
		std::string op;
		switch (m_ID)
		{
			case OP_NEG: op = "-";
				break;
			case OP_ADD: op = "+";
				break;
			case OP_SUB: op = "-";
				break;
			case OP_MUL: op = "*";
				break;
			case OP_DIV: op = "/";
				break;

			case OP_ABS: op = "abs";
				break;
			case OP_ACOS: op = "cos";
				break;
			case OP_ASIN: op = "sin";
				break;
			case OP_ATAN: op = "atan";
				break;
			case OP_CEIL: op = "ceil";
				break;
			case OP_COS: op = "cos";
				break;
			case OP_EXP: op = "exp";
				break;
			case OP_FLOOR: op = "floor";
				break;
			case OP_LOG: op = "log";
				break;
			case OP_LOG10: op = "log10";
				break;
			case OP_POW: op = "pow";
				break;
			case OP_RAND: op = "rand";
				break;
			case OP_SIN: op = "sin";
				break;
			case OP_SQRT: op = "sqrt";
				break;
			case OP_TAN: op = "tan";
				break;

			case OP_IF_THEN_ELSE: op = "?:";
				break;

			case OP_CMP_L: op = "<";
				break;
			case OP_CMP_G: op = ">";
				break;
			case OP_CMP_LE: op = "<=";
				break;
			case OP_CMP_GE: op = ">=";
				break;
			case OP_CMP_E: op = "==";
				break;
			case OP_CMP_NE: op = "!=";
				break;

			case OP_BOOL_AND: op = "&";
				break;
			case OP_BOOL_OR: op = "|";
				break;
			case OP_BOOL_NOT: op = "!";
				break;
			case OP_BOOL_XOR: op = "^";
				break;

			case OP_USERDEF: op = "UserDefined";
				break;
			case OP_NONE: op = "None";
				break;
			case OP_X2: op = "X^2";
				break;

			default: op = "UnknownOp";
				break;
		}

		logManager << "(" << op << " ";
		for (size_t i = 0; i < m_Children.size(); ++i)
		{
			if (m_Children[i] == nullptr) { }
			else { m_Children[i]->print(logManager); }
			logManager << " ";
		}
		logManager << ")";
	}

	bool simplify(CAbstractTreeNode*& node) override;
	void levelOperators() override;
	void useNegationOperator() override;
	void generateCode(CEquationParser& parser) override;
};

/**
 * Class for terminal nodes containing a single value.
 *
 */
class CAbstractTreeValueNode : public CAbstractTreeNode
{
protected:

	//! Value associated with the node.
	double m_value = 0;

public:

	explicit CAbstractTreeValueNode(const double value) : CAbstractTreeNode(true, true), m_value(value) {}

	//! Destructor
	~CAbstractTreeValueNode() override { }

	/**
	* Used to set the value of the node.
	* \param value The node's new value.
	*/
	void setValue(const double value) { m_value = value; }

	/**
	 * Used to know the value of the node.
	 * \return The node's value.
	 */
	double getValue() const { return m_value; }

	void print(OpenViBE::Kernel::ILogManager& logManager) override { logManager << m_value; }

	bool simplify(CAbstractTreeNode*& modifiedNode) override
	{
		modifiedNode = this;
		return false;
	}

	void levelOperators() override { }
	void useNegationOperator() override { }
	void generateCode(CEquationParser& parser) override;
};

/**
 * Class for terminal nodes referencing a variable.
 */
class CAbstractTreeVariableNode : public CAbstractTreeNode
{
public:

	explicit CAbstractTreeVariableNode(const size_t index) : CAbstractTreeNode(true, false), m_index(index) { }

	~CAbstractTreeVariableNode() override { }

	void print(OpenViBE::Kernel::ILogManager& logManager) override
	{
		char name[2];
		name[0] = char('a' + m_index);
		name[1] = 0;
		logManager << name;
	}

	bool simplify(CAbstractTreeNode*& modifiedNode) override
	{
		modifiedNode = this;
		return false;
	}

	void levelOperators() override { }
	void useNegationOperator() override { }
	void generateCode(CEquationParser& parser) override;

protected:

	size_t m_index = 0;
};

/**
* Main class for the AST.
* Contains the root of the tree.
*/
class CAbstractTree
{
protected:

	//! the root of the AST tree.
	CAbstractTreeNode* m_root = nullptr;

public:

	//! Constructor
	explicit CAbstractTree(CAbstractTreeNode* root) : m_root(root) { }

	//! Destructor
	~CAbstractTree() { delete m_root; }

	//! Prints the whole tree.
	void printTree(OpenViBE::Kernel::ILogManager& logManager) const { m_root->print(logManager); }

	/**
	 * Used to simplify the tree.
	 */
	void simplifyTree();

	/**
	 * Part of the process of simplification.
	 * Levels recursively the associative operators nodes.
	 */
	void levelOperators() const { m_root->levelOperators(); }

	/**
	 * Changes the tree so it uses the NEG operator whenever it is possible.
	 * (ie replaces (* -1 X) by (NEG X)
	 */
	void useNegationOperator() const { m_root->useNegationOperator(); }

	/**
	 * Generates the set of function calls needed to do the desired computation.
	 * \param parser The parser containing the function pointers stack and function contexts stack.
	 */
	void generateCode(CEquationParser& parser);

	/**
	* Tries to recognize simple tree structures (X*X, X*Cste, X+Cste,...)
	* \param treeId The identifier of the tree (OP_USERDEF for non special tree).
	* \param parameter The optional parameter if it is a special tree.
	*/
	void recognizeSpecialTree(uint64_t& treeId, double& parameter);
};

/**
* Functor used to compare two nodes.
* The order is as follow : Constants, Variables, ParentNodes
*/
struct CAbstractTreeNodeOrderingFunction
{
	bool operator()(CAbstractTreeNode* const & pFirstNode, CAbstractTreeNode* const & pSecondNode) const
	{
#if 0
		if( (pFirstNode->isConstant()) ||
			(pFirstNode->isTerminal() && !pSecondNode->isConstant()) ||
			(!pFirstNode->isTerminal() && !pSecondNode->isTerminal())) { return true; }
		else { return false; }
#else

		// Check isConstant flag
		if (pFirstNode->isConstant() && !pSecondNode->isConstant()) { return true; }
		if (!pFirstNode->isConstant() && pSecondNode->isConstant()) { return false; }

		// Check isTerminal flag
		if (pFirstNode->isTerminal() && !pSecondNode->isTerminal()) { return true; }
		if (!pFirstNode->isTerminal() && pSecondNode->isTerminal()) { return false; }

		// At this point, isTerminal and isConstant are the same for both value
		// Order is not important any more, we just compare the pointer values
		// so to have strict ordering function
		return pFirstNode < pSecondNode;
#endif
	}
};
