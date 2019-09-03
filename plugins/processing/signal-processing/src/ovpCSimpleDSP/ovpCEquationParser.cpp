#include "ovpCEquationParser.h"

#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>

using namespace boost;
using namespace spirit;
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEToolkit;

#if 1
#define _EQ_PARSER_DEBUG_LOG_(level, message) m_oParentPlugin.getLogManager() << level << message << "\n";
#define _EQ_PARSER_DEBUG_PRINT_TREE_(level) { m_oParentPlugin.getLogManager() << level; m_pTree->printTree(m_oParentPlugin.getLogManager()); m_oParentPlugin.getLogManager() << "\n"; }
#else
 #define _EQ_PARSER_DEBUG_LOG_(level, message)
 #define _EQ_PARSER_DEBUG_PRINT_TREE_(level)
#endif

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class charT>
	charT to_lower(charT c) { return std::tolower(c); }

	// BOOST::Ast should be able to remove spaces / tabs etc but
	// unfortunately, it seems it does not work correcly in some
	// cases so I add this sanitizer function to clear the Simple DSP
	// equation before sending it to BOOST::Ast
	std::string find_and_replace(std::string s, const std::string& f, const std::string& r)
	{
		size_t i;
		while ((i = s.find(f)) != std::string::npos) { s.replace(i, f.length(), r); }
		return s;
	}
} // namespace

functionPointer CEquationParser::m_pFunctionTable[] =
{
	&op_neg, &op_add, &op_sub, &op_mul, &op_div,
	&op_abs, &op_acos, &op_asin, &op_atan,
	&op_ceil, &op_cos, &op_exp, &op_floor,
	&op_log, &op_log10, &op_power, &op_rand, &op_sin,
	&op_sqrt, &op_tan,
	&op_if_then_else,
	&op_cmp_lower,
	&op_cmp_greater,
	&op_cmp_lower_equal,
	&op_cmp_greater_equal,
	&op_cmp_equal,
	&op_cmp_not_equal,
	&op_bool_and,
	&op_bool_or,
	&op_bool_not,
	&op_bool_xor,
};

CEquationParser::CEquationParser(TBoxAlgorithm<IBoxAlgorithm>& oPlugin, double** ppVariable, uint32_t ui32VariableCount)
	: m_ppVariable(ppVariable), m_ui32VariableCount(ui32VariableCount), m_oParentPlugin(oPlugin) {}

CEquationParser::~CEquationParser()
{
	delete[] m_pFunctionListBase;
	delete[] m_pFunctionContextListBase;
	delete[] m_pStack;
	delete m_pTree;
}

bool CEquationParser::compileEquation(const char* pEquation)
{
	// BOOST::Ast should be able to remove spaces / tabs etc but
	// unfortunately, it seems it does not work correcly in some
	// cases so I add this sanitizer function to clear the Simple DSP
	// equation before sending it to BOOST::Ast
	std::string l_sEquation(pEquation);
	l_sEquation = find_and_replace(l_sEquation, " ", "");
	l_sEquation = find_and_replace(l_sEquation, "\t", "");
	l_sEquation = find_and_replace(l_sEquation, "\n", "");

	//parses the equation
	_EQ_PARSER_DEBUG_LOG_(LogLevel_Trace, "Parsing equation [" << CString(l_sEquation.c_str()) << "]...");
	tree_parse_info<> l_oInfo = ast_parse(l_sEquation.c_str(), m_oGrammar >> end_p, space_p);

	//If the parsing was successful
	if (l_oInfo.full)
	{
		//creates the AST
		_EQ_PARSER_DEBUG_LOG_(LogLevel_Trace, "Creating abstract tree...");
		createAbstractTree(l_oInfo);
		_EQ_PARSER_DEBUG_PRINT_TREE_(LogLevel_Debug);

#if 0
		//CONSTANT FOLDING
		//levels the associative/commutative operators (+ and *)
		_EQ_PARSER_DEBUG_LOG_(LogLevel_Trace, "Leveling tree...");
		m_pTree->levelOperators();
		_EQ_PARSER_DEBUG_PRINT_TREE_(LogLevel_Debug);

		//simplifies the AST
		_EQ_PARSER_DEBUG_LOG_(LogLevel_Trace, "Simplifying tree...");
		m_pTree->simplifyTree();
		_EQ_PARSER_DEBUG_PRINT_TREE_(LogLevel_Debug);

		//tries to replace nodes to use the NEG operator and reduce complexity
		_EQ_PARSER_DEBUG_LOG_(LogLevel_Trace, "Generating bytecode...");
		m_pTree->useNegationOperator();
		_EQ_PARSER_DEBUG_PRINT_TREE_(LogLevel_Debug);
#endif

#if 0
		//Detects if it is a special tree (updates m_ui64TreeCategory and m_f64TreeParameter)
		_EQ_PARSER_DEBUG_LOG_(LogLevel_Trace, "Recognizing special tree...");
		m_pTree->recognizeSpecialTree(m_ui64TreeCategory, m_f64TreeParameter);
		_EQ_PARSER_DEBUG_PRINT_TREE_(LogLevel_Debug);

		//Unrecognize special tree
		_EQ_PARSER_DEBUG_LOG_("Unrecognizing special tree...");
		m_ui64TreeCategory = OP_USERDEF;
		_EQ_PARSER_DEBUG_PRINT_TREE_(LogLevel_Debug);
#endif

		//If it is not a special tree, we need to generate some code to reach the result
		if (m_ui64TreeCategory == OP_USERDEF)
		{
			//allocates the function stack
			m_pFunctionList     = new functionPointer[m_ui32FunctionStackSize];
			m_pFunctionListBase = m_pFunctionList;

			//Allocates the function context stack
			m_pFunctionContextList     = new functionContext[(size_t)m_ui64FunctionContextStackSize];
			m_pFunctionContextListBase = m_pFunctionContextList;
			m_pStack                   = new double[(size_t)m_ui64StackSize];

			//generates the code
			m_pTree->generateCode(*this);

			//computes the number of steps to get to the result
			m_ui64NumberOfOperations = m_pFunctionList - m_pFunctionListBase;
		}

		return true;
	}
	std::string l_oErrorString;

	const size_t errorPosition = l_sEquation.find(l_oInfo.stop);
	if (errorPosition != std::string::npos)
	{
		for (size_t i = 0; i < errorPosition; i++) { l_oErrorString += " "; }
		l_oErrorString += "^--Here\n";
	}

	OV_ERROR("Failed parsing equation \n[" << pEquation << "]\n " << l_oErrorString.c_str(), OpenViBE::Kernel::ErrorType::BadParsing, false,
			 m_oParentPlugin.getBoxAlgorithmContext()->getPlayerContext()->getErrorManager(),
			 m_oParentPlugin.getBoxAlgorithmContext()->getPlayerContext()->getLogManager());
}

void CEquationParser::createAbstractTree(tree_parse_info<> oInfo) { m_pTree = new CAbstractTree(createNode(oInfo.trees.begin())); }

CAbstractTreeNode* CEquationParser::createNode(iter_t const& i)
{
	if (i->value.id() == CEquationGrammar::expressionID)
	{
		if (*i->value.begin() == '+')
		{
			return new CAbstractTreeParentNode(OP_ADD, createNode(i->children.begin()), createNode(i->children.begin() + 1), true);
		}
		//replaces (- X Y) by (+ X (-Y)) (in fact (+ X (* -1 Y)) )
		if (*i->value.begin() == '-')
		{
			return new CAbstractTreeParentNode(OP_ADD, createNode(i->children.begin()),
											   new CAbstractTreeParentNode(OP_MUL, new CAbstractTreeValueNode(-1), createNode(i->children.begin() + 1), true),
											   true);
		}
	}
	else if (i->value.id() == CEquationGrammar::termID)
	{
		if (*i->value.begin() == '*')
		{
			return new CAbstractTreeParentNode(OP_MUL, createNode(i->children.begin()), createNode(i->children.begin() + 1), true);
		}
		if (*i->value.begin() == '/') { return new CAbstractTreeParentNode(OP_DIV, createNode(i->children.begin()), createNode(i->children.begin() + 1)); }
	}
	else if (i->value.id() == CEquationGrammar::factorID)
	{
		if (*i->value.begin() == '-')
		{
			// -X => (* -1 X), useful to simplify the tree later
			return new CAbstractTreeParentNode(OP_MUL, new CAbstractTreeValueNode(-1), createNode(i->children.begin()), true);
		}
		if (*i->value.begin() == '+') { return createNode(i->children.begin()); }
	}
	else if (i->value.id() == CEquationGrammar::realID)
	{
		std::string l_sValue(i->value.begin(), i->value.end());
		return new CAbstractTreeValueNode(strtod(l_sValue.c_str(), nullptr));
	}
	else if (i->value.id() == CEquationGrammar::variableID)
	{
		uint32_t l_ui32Index = 0;
		std::string l_sValue(i->value.begin(), i->value.end());
		if (l_sValue != "x" && l_sValue != "X")
		{
			if (l_sValue[0] >= 'a' && l_sValue[0] <= 'z') { l_ui32Index = l_sValue[0] - 'a'; }
			if (l_sValue[0] >= 'A' && l_sValue[0] <= 'Z') { l_ui32Index = l_sValue[0] - 'A'; }
		}

		if (l_ui32Index >= m_ui32VariableCount)
		{
			OV_WARNING("Missing input " << l_ui32Index+1 << " (referenced with variable [" << CString(l_sValue.c_str()) << "])",
					   m_oParentPlugin.getBoxAlgorithmContext()->getPlayerContext()->getLogManager());
			return new CAbstractTreeValueNode(0);
		}
		return new CAbstractTreeVariableNode(l_ui32Index);
	}
	else if (i->value.id() == CEquationGrammar::constantID)
	{
		std::string l_sValue(i->value.begin(), i->value.end());

		//converts the string to lowercase
		std::transform(l_sValue.begin(), l_sValue.end(), l_sValue.begin(), ::to_lower<std::string::value_type>);

		//creates a new value node from the value looked up in the constant's symbols table
		return new CAbstractTreeValueNode(*find(mathConstant_p, l_sValue.c_str()));
	}
	else if (i->value.id() == CEquationGrammar::functionID)
	{
		std::string l_sValue(i->value.begin(), i->value.end());
		uint64_t* l_ui64FunctionIdentifier;

		//converts the string to lowercase
		std::transform(l_sValue.begin(), l_sValue.end(), l_sValue.begin(), ::to_lower<std::string::value_type>);

		//gets the function's Id from the unary function's symbols table
		if ((l_ui64FunctionIdentifier = find(unaryFunction_p, l_sValue.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*l_ui64FunctionIdentifier, createNode(i->children.begin()), false);
		}
		//gets the function's Id from the binary function's symbols table
		if ((l_ui64FunctionIdentifier = find(binaryFunction_p, l_sValue.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*l_ui64FunctionIdentifier, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
	}
	else if (i->value.id() == CEquationGrammar::ifthenID)
	{
		return new CAbstractTreeParentNode(OP_IF_THEN_ELSE, createNode(i->children.begin()), createNode(i->children.begin() + 1),
										   createNode(i->children.begin() + 2), false);
	}
	else if (i->value.id() == CEquationGrammar::comparisonID)
	{
		std::string l_sValue(i->value.begin(), i->value.end());
		uint64_t* l_ui64FunctionIdentifier;

		//converts the string to lowercase
		std::transform(l_sValue.begin(), l_sValue.end(), l_sValue.begin(), ::to_lower<std::string::value_type>);

		//gets the function's Id from the comparison function's symbols table
		if ((l_ui64FunctionIdentifier = find(comparison1Function_p, l_sValue.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*l_ui64FunctionIdentifier, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
		//gets the function's Id from the comparison function's symbols table
		if ((l_ui64FunctionIdentifier = find(comparison2Function_p, l_sValue.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*l_ui64FunctionIdentifier, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
	}
	else if (i->value.id() == CEquationGrammar::booleanID)
	{
		std::string l_sValue(i->value.begin(), i->value.end());
		uint64_t* l_ui64FunctionIdentifier;

		//converts the string to lowercase
		std::transform(l_sValue.begin(), l_sValue.end(), l_sValue.begin(), ::to_lower<std::string::value_type>);

		//gets the function's Id from the binary boolean function's symbols table
		if ((l_ui64FunctionIdentifier = find(binaryBoolean1Function_p, l_sValue.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*l_ui64FunctionIdentifier, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
		//gets the function's Id from the binary boolean function's symbols table
		if ((l_ui64FunctionIdentifier = find(binaryBoolean2Function_p, l_sValue.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*l_ui64FunctionIdentifier, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
		//gets the function's Id from the binary boolean function's symbols table
		if ((l_ui64FunctionIdentifier = find(binaryBoolean3Function_p, l_sValue.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*l_ui64FunctionIdentifier, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
		//gets the function's Id from the binary boolean function's symbols table
		if ((l_ui64FunctionIdentifier = find(unaryBooleanFunction_p, l_sValue.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*l_ui64FunctionIdentifier, createNode(i->children.begin()), false);
		}
	}

	return nullptr;
}

void CEquationParser::push_value(double f64Value)
{
	*(m_pFunctionList++)                           = op_loadVal;
	(*(m_pFunctionContextList++)).m_f64DirectValue = f64Value;
}

void CEquationParser::push_var(uint32_t index)
{
	*(m_pFunctionList++)                            = op_loadVar;
	(*(m_pFunctionContextList++)).m_ppIndirectValue = &m_ppVariable[index];
}

void CEquationParser::push_op(uint64_t ui64Operator)
{
	*(m_pFunctionList++)                            = m_pFunctionTable[ui64Operator];
	(*(m_pFunctionContextList++)).m_ppIndirectValue = nullptr;
}

// Functions called by our "pseudo - VM"

void CEquationParser::op_neg(double*& pStack, functionContext& /*pContext*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "neg : " << *(pStack);
#endif
	*pStack = - (*pStack);
#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *pStack << std::endl;
#endif
}

void CEquationParser::op_add(double*& pStack, functionContext& /*pContext*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "add : " << *(pStack) << " + " << *(pStack-1);
#endif
	pStack--;
	*(pStack) = *(pStack + 1) + *(pStack);
#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *pStack << std::endl;
#endif
}

void CEquationParser::op_sub(double*& pStack, functionContext& /*pContext*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "sub : " << *(pStack) << " - " << *(pStack-1);
#endif
	pStack--;
	*(pStack) = *(pStack + 1) - *(pStack);
#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *pStack << std::endl;
#endif
}

void CEquationParser::op_mul(double*& pStack, functionContext& /*pContext*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "mult : " << *(pStack) << " * " << *(pStack-1);
#endif
	pStack--;
	*(pStack) = *(pStack + 1) * *(pStack);
#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *pStack << std::endl;
#endif
}

void CEquationParser::op_div(double*& pStack, functionContext& /*pContext*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "divi : " << *(pStack) << " / " << *(pStack-1);
#endif
	pStack--;
	*(pStack) = *(pStack + 1) / *(pStack);

#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *pStack << std::endl;
#endif
}

void CEquationParser::op_power(double*& pStack, functionContext& /*pContext*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "pow: " << *(pStack) << " " << *(pStack-1) << std::endl;
#endif
	pStack--;
	*pStack = pow(*(pStack + 1), *(pStack));
#ifdef EQ_PARSER_DEBUG
	std::cout << "Pow Result: " << *pStack << std::endl;
#endif
}

void CEquationParser::op_abs(double*& pStack, functionContext& /*pContext*/) { *pStack = fabs(*(pStack)); }
void CEquationParser::op_acos(double*& pStack, functionContext& /*pContext*/) { *pStack = acos(*(pStack)); }
void CEquationParser::op_asin(double*& pStack, functionContext& /*pContext*/) { *pStack = asin(*(pStack)); }
void CEquationParser::op_atan(double*& pStack, functionContext& /*pContext*/) { *pStack = atan(*(pStack)); }
void CEquationParser::op_ceil(double*& pStack, functionContext& /*pContext*/) { *pStack = ceil(*(pStack)); }
void CEquationParser::op_cos(double*& pStack, functionContext& /*pContext*/) { *pStack = cos(*(pStack)); }
void CEquationParser::op_exp(double*& pStack, functionContext& /*pContext*/) { *pStack = exp(*(pStack)); }
void CEquationParser::op_floor(double*& pStack, functionContext& /*pContext*/) { *pStack = floor(*(pStack)); }
void CEquationParser::op_log(double*& pStack, functionContext& /*pContext*/) { *pStack = log(*(pStack)); }
void CEquationParser::op_log10(double*& pStack, functionContext& /*pContext*/) { *pStack = log10(*(pStack)); }
void CEquationParser::op_rand(double*& pStack, functionContext& /*pContext*/) { *pStack = rand() * *(pStack) / RAND_MAX; }
void CEquationParser::op_sin(double*& pStack, functionContext& /*pContext*/) { *pStack = sin(*(pStack)); }
void CEquationParser::op_sqrt(double*& pStack, functionContext& /*pContext*/) { *pStack = sqrt(*(pStack)); }
void CEquationParser::op_tan(double*& pStack, functionContext& /*pContext*/) { *pStack = tan(*(pStack)); }

void CEquationParser::op_if_then_else(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack--;
	if (*(pStack + 2)) { *pStack = *(pStack + 1); }
	else
	{
		// *pStack = *pStack;
	}
}

void CEquationParser::op_cmp_lower(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] < pStack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_greater(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] > pStack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_lower_equal(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] <= pStack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_greater_equal(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] >= pStack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_equal(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] == pStack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_not_equal(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] != pStack[0] ? 1 : 0);
}

void CEquationParser::op_bool_and(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] != 0 && pStack[0] != 0 ? 1 : 0);
}

void CEquationParser::op_bool_or(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] != 0 || pStack[0] != 0 ? 1 : 0);
}

void CEquationParser::op_bool_not(double*& pStack, functionContext& /*pContext*/) { pStack[0] = pStack[0] != 0 ? 0 : 1; }

void CEquationParser::op_bool_xor(double*& pStack, functionContext& /*pContext*/)
{
	pStack--;
	pStack[0] = (pStack[1] != pStack[0] ? 1 : 0);
}

void CEquationParser::op_loadVal(double*& pStack, functionContext& pContext)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "loadVal : " << pContext.m_f64DirectValue << std::endl;
#endif
	*(++pStack) = pContext.m_f64DirectValue;
}

void CEquationParser::op_loadVar(double*& pStack, functionContext& pContext)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "loadVar : " << **(pContext.m_ppIndirectValue) << std::endl;
#endif
	*(++pStack) = **(pContext.m_ppIndirectValue);
}
