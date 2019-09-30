#pragma once

#include "../ovp_defines.h"

#include "ovpCEquationParserGrammar.h"
#include "ovpCAbstractTree.h"

#include <toolkit/ovtk_all.h>

#include <boost/spirit/include/classic_ast.hpp>

#include <vector>

typedef char const* iterator_t;
typedef tree_match<iterator_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator iter_t;

class CAbstractTree;
class CAbstractTreeNode;

/**
* Used to store the optional parameter of the function used by the pseudo-VM.
*
*/
union UFunctionContext
{
	double direct_value = 0;	//if the parameter if a value (push_val)
	double** indirect_value;		//if it is a pointer to a value (push_var)
};

//! Type of the functions in the function stack generated from the equation.
typedef void (*functionPointer)(double*& stack, UFunctionContext& oContext);

class CEquationParser
{
protected:

	//! The AST produced by the parsing of the equation
	CAbstractTree* m_pTree = nullptr;

	//! Grammar to use
	CEquationGrammar m_oGrammar;

	//! Pointer to the data referenced by X in the equation
	double** m_ppVariable = nullptr;
	//! Number of accessible variables
	uint32_t m_nVariable = 0;

	//! Size of the "function stack" (where the sucessive function pointers are stored)
	const uint32_t m_ui32FunctionStackSize = 1024;
	//! Pointer to the top of the function stack
	functionPointer* m_pFunctionList = nullptr;
	//! Pointer to the base of the function stack
	functionPointer* m_pFunctionListBase = nullptr;

	//! Size of the "function context stack" (where the sucessive function context are stored)
	const uint64_t m_ui64FunctionContextStackSize = 1024;
	//! Pointer to the top of the function context stack
	UFunctionContext* m_pFunctionContextList = nullptr;
	//! Pointer to the base of the function context stack
	UFunctionContext* m_pFunctionContextListBase = nullptr;

	//! Size of the "local" stack
	const uint64_t m_ui64StackSize = 1024;
	//! Pointer to the top of the "local" stack
	double* m_pStack = nullptr;

	//! Number of pointers/contexts in the function/context stacks (same for both)
	uint64_t m_ui64NumberOfOperations = 0;

	//! Table of function pointers
	static functionPointer m_pFunctionTable[32];

	//! Category of the tree (OP_USERDEF or Special tree)
	uint64_t m_ui64TreeCategory = OP_USERDEF;
	//! Optional parameter in case of a special tree
	double m_f64TreeParameter = 0;

	OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& m_oParentPlugin;

public:

	/**
	* Constructor.
	* \param plugin
	* \param variable Pointer to the data known as X in the equation.
	* \param nVariable
	*/
	CEquationParser(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& plugin, double** variable, const uint32_t nVariable)
		: m_ppVariable(variable), m_nVariable(nVariable), m_oParentPlugin(plugin) {}

	//! Destructor.
	~CEquationParser();

#if 0
	//
	void setVariable(double * pVariable){ m_pVariable=pVariable; }
#endif

	/**
	* Compiles the given equation, and generates the successive function calls to achieve the
	* same result if needed (depends on m_ui64TreeCategory).
	* \param equation The equation to use.
	*/
	bool compileEquation(const char* equation);

	void push_op(uint64_t ui64Operator);
	void push_value(double f64Value);
	void push_var(uint32_t index);

	/**
	* Returns the tree's category.
	* \return The tree's category.
	*/
	uint64_t getTreeCategory() const { return m_ui64TreeCategory; }

	/**
	 * Returns the optional parameter.
	 * \return The optional parameter.
	 */
	double getTreeParameter() const { return m_f64TreeParameter; }

	/**
	* Executes the successive function calls from the function stack and returns
	* the result.
	* \return The result of the equation applied to the value referenced by X.
	*/
	double executeEquation()
	{
		functionPointer* l_pCurrentFunction     = m_pFunctionList - 1;
		functionPointer* l_pLastFunctionPointer = l_pCurrentFunction - m_ui64NumberOfOperations;

		UFunctionContext* l_pCurrentFunctionContext = m_pFunctionContextList - 1;

		//while there are function pointers
		while (l_pCurrentFunction != l_pLastFunctionPointer)
		{
			//calls the function with the current function context
			(*l_pCurrentFunction)(m_pStack, *l_pCurrentFunctionContext);

			//updates the stack pointers
			l_pCurrentFunction--;
			l_pCurrentFunctionContext--;
		}

		//pop and return the result
		return *(m_pStack--);
	}

private:

	void createAbstractTree(tree_parse_info<> oInfo);
	CAbstractTreeNode* createNode(iter_t const& i);

public:

	static void op_neg(double*& stack, UFunctionContext& ctx);
	static void op_add(double*& stack, UFunctionContext& ctx);
	static void op_div(double*& stack, UFunctionContext& ctx);
	static void op_sub(double*& stack, UFunctionContext& ctx);
	static void op_mul(double*& stack, UFunctionContext& ctx);

	static void op_power(double*& stack, UFunctionContext& ctx);

	static void op_abs(double*& stack, UFunctionContext& ctx);
	static void op_acos(double*& stack, UFunctionContext& ctx);
	static void op_asin(double*& stack, UFunctionContext& ctx);
	static void op_atan(double*& stack, UFunctionContext& ctx);
	static void op_ceil(double*& stack, UFunctionContext& ctx);
	static void op_cos(double*& stack, UFunctionContext& ctx);
	static void op_exp(double*& stack, UFunctionContext& ctx);
	static void op_floor(double*& stack, UFunctionContext& ctx);
	static void op_log(double*& stack, UFunctionContext& ctx);
	static void op_log10(double*& stack, UFunctionContext& ctx);
	static void op_rand(double*& stack, UFunctionContext& ctx);
	static void op_sin(double*& stack, UFunctionContext& ctx);
	static void op_sqrt(double*& stack, UFunctionContext& ctx);
	static void op_tan(double*& stack, UFunctionContext& ctx);

	static void op_if_then_else(double*& stack, UFunctionContext& ctx);

	static void op_cmp_lower(double*& stack, UFunctionContext& ctx);
	static void op_cmp_greater(double*& stack, UFunctionContext& ctx);
	static void op_cmp_lower_equal(double*& stack, UFunctionContext& ctx);
	static void op_cmp_greater_equal(double*& stack, UFunctionContext& ctx);
	static void op_cmp_equal(double*& stack, UFunctionContext& ctx);
	static void op_cmp_not_equal(double*& stack, UFunctionContext& ctx);

	static void op_bool_and(double*& stack, UFunctionContext& ctx);
	static void op_bool_or(double*& stack, UFunctionContext& ctx);
	static void op_bool_not(double*& stack, UFunctionContext& ctx);
	static void op_bool_xor(double*& stack, UFunctionContext& ctx);

	static void op_loadVal(double*& stack, UFunctionContext& ctx);
	static void op_loadVar(double*& stack, UFunctionContext& ctx);
};
