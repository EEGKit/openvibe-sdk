#ifndef __OpenViBEPlugins_SignalProcessing_CEquationParser_H__
#define __OpenViBEPlugins_SignalProcessing_CEquationParser_H__

#include "../ovp_defines.h"

#include "ovpCEquationParserGrammar.h"
#include "ovpCAbstractTree.h"

#include <toolkit/ovtk_all.h>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_ast.hpp>

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>

using namespace boost::spirit;

typedef char const* iterator_t;
typedef tree_match<iterator_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator iter_t;

class CAbstractTree;
class CAbstractTreeNode;

/**
* Used to store the optional parameter of the function used by the pseudo-VM.
*
*/
union functionContext
{
	double m_f64DirectValue;//if the parameter if a value (push_val)
	double** m_ppIndirectValue;//if it is a pointer to a value (push_var)
};

//! Type of the functions in the function stack generated from the equation.
typedef void (*functionPointer)(double*& pStack, functionContext& oContext);

class CEquationParser
{
protected:

	//! The AST produced by the parsing of the equation
	CAbstractTree* m_pTree;

	//! Grammar to use
	CEquationGrammar m_oGrammar;

	//! Pointer to the data referenced by X in the equation
	double** m_ppVariable;
	//! Number of accessible variables
	uint32_t m_ui32VariableCount;

	//! Size of the "function stack" (where the sucessive function pointers are stored)
	const uint32_t m_ui32FunctionStackSize;
	//! Pointer to the top of the function stack
	functionPointer* m_pFunctionList;
	//! Pointer to the base of the function stack
	functionPointer* m_pFunctionListBase;

	//! Size of the "function context stack" (where the sucessive function context are stored)
	const uint64_t m_ui64FunctionContextStackSize;
	//! Pointer to the top of the function context stack
	functionContext* m_pFunctionContextList;
	//! Pointer to the base of the function context stack
	functionContext* m_pFunctionContextListBase;

	//! Size of the "local" stack
	const uint64_t m_ui64StackSize;
	//! Pointer to the top of the "local" stack
	double* m_pStack;

	//! Number of pointers/contexts in the function/context stacks (same for both)
	uint64_t m_ui64NumberOfOperations;

	//! Table of function pointers
	static functionPointer m_pFunctionTable[32];

	//! Category of the tree (OP_USERDEF or Special tree)
	uint64_t m_ui64TreeCategory;
	//! Optional parameter in case of a special tree
	double m_f64TreeParameter;

	OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& m_oParentPlugin;

public:

	/**
	* Constructor.
	* \param pVariable Pointer to the data known as X in the equation.
	*/
	CEquationParser(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>& oPlugin, double** ppVarialbe, uint32_t ui32VariableCount);

	//! Destructor.
	~CEquationParser();

#if 0
	//
	void setVariable(double * pVariable){ m_pVariable=pVariable; }
#endif

	/**
	* Compiles the given equation, and generates the successive function calls to achieve the
	* same result if needed (depends on m_ui64TreeCategory).
	* \param pEquation The equation to use.
	*/
	bool compileEquation(const char* pEquation);

	void push_op(uint64_t ui64Operator);
	void push_value(double f64Value);
	void push_var(uint32_t ui32Index);

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

		functionContext* l_pCurrentFunctionContext = m_pFunctionContextList - 1;

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

	static void op_neg(double*& pStack, functionContext& pContext);
	static void op_add(double*& pStack, functionContext& pContext);
	static void op_div(double*& pStack, functionContext& pContext);
	static void op_sub(double*& pStack, functionContext& pContext);
	static void op_mul(double*& pStack, functionContext& pContext);

	static void op_power(double*& pStack, functionContext& pContext);

	static void op_abs(double*& pStack, functionContext& pContext);
	static void op_acos(double*& pStack, functionContext& pContext);
	static void op_asin(double*& pStack, functionContext& pContext);
	static void op_atan(double*& pStack, functionContext& pContext);
	static void op_ceil(double*& pStack, functionContext& pContext);
	static void op_cos(double*& pStack, functionContext& pContext);
	static void op_exp(double*& pStack, functionContext& pContext);
	static void op_floor(double*& pStack, functionContext& pContext);
	static void op_log(double*& pStack, functionContext& pContext);
	static void op_log10(double*& pStack, functionContext& pContext);
	static void op_rand(double*& pStack, functionContext& pContext);
	static void op_sin(double*& pStack, functionContext& pContext);
	static void op_sqrt(double*& pStack, functionContext& pContext);
	static void op_tan(double*& pStack, functionContext& pContext);

	static void op_if_then_else(double*& pStack, functionContext& pContext);

	static void op_cmp_lower(double*& pStack, functionContext& pContext);
	static void op_cmp_greater(double*& pStack, functionContext& pContext);
	static void op_cmp_lower_equal(double*& pStack, functionContext& pContext);
	static void op_cmp_greater_equal(double*& pStack, functionContext& pContext);
	static void op_cmp_equal(double*& pStack, functionContext& pContext);
	static void op_cmp_not_equal(double*& pStack, functionContext& pContext);

	static void op_bool_and(double*& pStack, functionContext& pContext);
	static void op_bool_or(double*& pStack, functionContext& pContext);
	static void op_bool_not(double*& pStack, functionContext& pContext);
	static void op_bool_xor(double*& pStack, functionContext& pContext);

	static void op_loadVal(double*& pStack, functionContext& pContext);
	static void op_loadVar(double*& pStack, functionContext& pContext);
};


#endif
