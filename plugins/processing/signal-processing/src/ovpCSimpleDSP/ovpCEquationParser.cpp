#include "ovpCEquationParser.h"

#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>

using namespace boost::spirit;
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEToolkit;

#define _EQ_PARSER_DEBUG_LOG_(level, message) m_oParentPlugin.getLogManager() << level << message << "\n";
#define _EQ_PARSER_DEBUG_PRINT_TREE_(level) { m_oParentPlugin.getLogManager() << level; m_pTree->printTree(m_oParentPlugin.getLogManager()); m_oParentPlugin.getLogManager() << "\n"; }

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class TChar>
	TChar ToLower(TChar c) { return std::tolower(c); }

	// BOOST::Ast should be able to remove spaces / tabs etc but
	// unfortunately, it seems it does not work correcly in some
	// cases so I add this sanitizer function to clear the Simple DSP
	// equation before sending it to BOOST::Ast
	std::string FindAndReplace(std::string s, const std::string& f, const std::string& r)
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

CEquationParser::~CEquationParser()
{
	delete[] m_pFunctionListBase;
	delete[] m_pFunctionContextListBase;
	delete[] m_pStack;
	delete m_pTree;
}

bool CEquationParser::compileEquation(const char* equation)
{
	// BOOST::Ast should be able to remove spaces / tabs etc but
	// unfortunately, it seems it does not work correcly in some
	// cases so I add this sanitizer function to clear the Simple DSP
	// equation before sending it to BOOST::Ast
	std::string str(equation);
	str = FindAndReplace(str, " ", "");
	str = FindAndReplace(str, "\t", "");
	str = FindAndReplace(str, "\n", "");

	//parses the equation
	_EQ_PARSER_DEBUG_LOG_(LogLevel_Trace, "Parsing equation [" << CString(str.c_str()) << "]...");
	const tree_parse_info<> info = ast_parse(str.c_str(), m_oGrammar >> end_p, space_p);

	//If the parsing was successful
	if (info.full)
	{
		//creates the AST
		_EQ_PARSER_DEBUG_LOG_(LogLevel_Trace, "Creating abstract tree...");
		createAbstractTree(info);
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
			m_pFunctionContextList     = new UFunctionContext[m_ui64FunctionContextStackSize];
			m_pFunctionContextListBase = m_pFunctionContextList;
			m_pStack                   = new double[m_ui64StackSize];

			//generates the code
			m_pTree->generateCode(*this);

			//computes the number of steps to get to the result
			m_ui64NumberOfOperations = m_pFunctionList - m_pFunctionListBase;
		}

		return true;
	}
	std::string error;
	const size_t pos = str.find(info.stop);
	if (pos != std::string::npos)
	{
		for (size_t i = 0; i < pos; i++) { error += " "; }
		error += "^--Here\n";
	}

	OV_ERROR("Failed parsing equation \n[" << equation << "]\n " << error.c_str(), OpenViBE::Kernel::ErrorType::BadParsing, false,
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
		const std::string value(i->value.begin(), i->value.end());
		return new CAbstractTreeValueNode(strtod(value.c_str(), nullptr));
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

		if (l_ui32Index >= m_nVariable)
		{
			OV_WARNING("Missing input " << l_ui32Index+1 << " (referenced with variable [" << CString(l_sValue.c_str()) << "])",
					   m_oParentPlugin.getBoxAlgorithmContext()->getPlayerContext()->getLogManager());
			return new CAbstractTreeValueNode(0);
		}
		return new CAbstractTreeVariableNode(l_ui32Index);
	}
	else if (i->value.id() == CEquationGrammar::constantID)
	{
		std::string value(i->value.begin(), i->value.end());

		//converts the string to lowercase
		std::transform(value.begin(), value.end(), value.begin(), ::ToLower<std::string::value_type>);

		//creates a new value node from the value looked up in the constant's symbols table
		return new CAbstractTreeValueNode(*find(mathConstant_p, value.c_str()));
	}
	else if (i->value.id() == CEquationGrammar::functionID)
	{
		std::string value(i->value.begin(), i->value.end());
		uint64_t* functionID;

		//converts the string to lowercase
		std::transform(value.begin(), value.end(), value.begin(), ::ToLower<std::string::value_type>);

		//gets the function's Id from the unary function's symbols table
		if ((functionID = find(unaryFunction_p, value.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*functionID, createNode(i->children.begin()), false);
		}
		//gets the function's Id from the binary function's symbols table
		if ((functionID = find(binaryFunction_p, value.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*functionID, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
	}
	else if (i->value.id() == CEquationGrammar::ifthenID)
	{
		return new CAbstractTreeParentNode(OP_IF_THEN_ELSE, createNode(i->children.begin()), createNode(i->children.begin() + 1),
										   createNode(i->children.begin() + 2), false);
	}
	else if (i->value.id() == CEquationGrammar::comparisonID)
	{
		std::string value(i->value.begin(), i->value.end());
		uint64_t* functionID;

		//converts the string to lowercase
		std::transform(value.begin(), value.end(), value.begin(), ::ToLower<std::string::value_type>);

		//gets the function's Id from the comparison function's symbols table
		if ((functionID = find(comparison1Function_p, value.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*functionID, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
		//gets the function's Id from the comparison function's symbols table
		if ((functionID = find(comparison2Function_p, value.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*functionID, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
	}
	else if (i->value.id() == CEquationGrammar::booleanID)
	{
		std::string value(i->value.begin(), i->value.end());
		uint64_t* functionID;

		//converts the string to lowercase
		std::transform(value.begin(), value.end(), value.begin(), ::ToLower<std::string::value_type>);

		//gets the function's Id from the binary boolean function's symbols table
		if ((functionID = find(binaryBoolean1Function_p, value.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*functionID, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
		//gets the function's Id from the binary boolean function's symbols table
		if ((functionID = find(binaryBoolean2Function_p, value.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*functionID, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
		//gets the function's Id from the binary boolean function's symbols table
		if ((functionID = find(binaryBoolean3Function_p, value.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*functionID, createNode(i->children.begin()), createNode(i->children.begin() + 1), false);
		}
		//gets the function's Id from the binary boolean function's symbols table
		if ((functionID = find(unaryBooleanFunction_p, value.c_str())) != nullptr)
		{
			return new CAbstractTreeParentNode(*functionID, createNode(i->children.begin()), false);
		}
	}

	return nullptr;
}

void CEquationParser::push_value(double f64Value)
{
	*(m_pFunctionList++)                           = op_loadVal;
	(*(m_pFunctionContextList++)).direct_value = f64Value;
}

void CEquationParser::push_var(uint32_t index)
{
	*(m_pFunctionList++)                            = op_loadVar;
	(*(m_pFunctionContextList++)).indirect_value = &m_ppVariable[index];
}

void CEquationParser::push_op(uint64_t ui64Operator)
{
	*(m_pFunctionList++)                            = m_pFunctionTable[ui64Operator];
	(*(m_pFunctionContextList++)).indirect_value = nullptr;
}

// Functions called by our "pseudo - VM"

void CEquationParser::op_neg(double*& stack, UFunctionContext& /*ctx*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "neg : " << *(stack);
#endif
	*stack = - (*stack);
#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *stack << std::endl;
#endif
}

void CEquationParser::op_add(double*& stack, UFunctionContext& /*ctx*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "add : " << *(stack) << " + " << *(stack-1);
#endif
	stack--;
	*(stack) = *(stack + 1) + *(stack);
#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *stack << std::endl;
#endif
}

void CEquationParser::op_sub(double*& stack, UFunctionContext& /*ctx*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "sub : " << *(stack) << " - " << *(stack-1);
#endif
	stack--;
	*(stack) = *(stack + 1) - *(stack);
#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *stack << std::endl;
#endif
}

void CEquationParser::op_mul(double*& stack, UFunctionContext& /*ctx*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "mult : " << *(stack) << " * " << *(stack-1);
#endif
	stack--;
	*(stack) = *(stack + 1) * *(stack);
#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *stack << std::endl;
#endif
}

void CEquationParser::op_div(double*& stack, UFunctionContext& /*ctx*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "divi : " << *(stack) << " / " << *(stack-1);
#endif
	stack--;
	*(stack) = *(stack + 1) / *(stack);

#ifdef EQ_PARSER_DEBUG
	std::cout << " = " << *stack << std::endl;
#endif
}

void CEquationParser::op_power(double*& stack, UFunctionContext& /*ctx*/)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "pow: " << *(stack) << " " << *(stack-1) << std::endl;
#endif
	stack--;
	*stack = pow(*(stack + 1), *(stack));
#ifdef EQ_PARSER_DEBUG
	std::cout << "Pow Result: " << *stack << std::endl;
#endif
}

void CEquationParser::op_abs(double*& stack, UFunctionContext& /*ctx*/) { *stack = fabs(*(stack)); }
void CEquationParser::op_acos(double*& stack, UFunctionContext& /*ctx*/) { *stack = acos(*(stack)); }
void CEquationParser::op_asin(double*& stack, UFunctionContext& /*ctx*/) { *stack = asin(*(stack)); }
void CEquationParser::op_atan(double*& stack, UFunctionContext& /*ctx*/) { *stack = atan(*(stack)); }
void CEquationParser::op_ceil(double*& stack, UFunctionContext& /*ctx*/) { *stack = ceil(*(stack)); }
void CEquationParser::op_cos(double*& stack, UFunctionContext& /*ctx*/) { *stack = cos(*(stack)); }
void CEquationParser::op_exp(double*& stack, UFunctionContext& /*ctx*/) { *stack = exp(*(stack)); }
void CEquationParser::op_floor(double*& stack, UFunctionContext& /*ctx*/) { *stack = floor(*(stack)); }
void CEquationParser::op_log(double*& stack, UFunctionContext& /*ctx*/) { *stack = log(*(stack)); }
void CEquationParser::op_log10(double*& stack, UFunctionContext& /*ctx*/) { *stack = log10(*(stack)); }
void CEquationParser::op_rand(double*& stack, UFunctionContext& /*ctx*/) { *stack = rand() * *(stack) / RAND_MAX; }
void CEquationParser::op_sin(double*& stack, UFunctionContext& /*ctx*/) { *stack = sin(*(stack)); }
void CEquationParser::op_sqrt(double*& stack, UFunctionContext& /*ctx*/) { *stack = sqrt(*(stack)); }
void CEquationParser::op_tan(double*& stack, UFunctionContext& /*ctx*/) { *stack = tan(*(stack)); }

void CEquationParser::op_if_then_else(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack--;
	if (*(stack + 2)) { *stack = *(stack + 1); }
	// else { *stack = *stack; }
}

void CEquationParser::op_cmp_lower(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] < stack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_greater(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] > stack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_lower_equal(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] <= stack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_greater_equal(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] >= stack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_equal(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] == stack[0] ? 1 : 0);
}

void CEquationParser::op_cmp_not_equal(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] != stack[0] ? 1 : 0);
}

void CEquationParser::op_bool_and(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] != 0 && stack[0] != 0 ? 1 : 0);
}

void CEquationParser::op_bool_or(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] != 0 || stack[0] != 0 ? 1 : 0);
}

void CEquationParser::op_bool_not(double*& stack, UFunctionContext& /*ctx*/) { stack[0] = stack[0] != 0 ? 0 : 1; }

void CEquationParser::op_bool_xor(double*& stack, UFunctionContext& /*ctx*/)
{
	stack--;
	stack[0] = (stack[1] != stack[0] ? 1 : 0);
}

void CEquationParser::op_loadVal(double*& stack, UFunctionContext& ctx)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "loadVal : " << ctx.direct_value << std::endl;
#endif
	*(++stack) = ctx.direct_value;
}

void CEquationParser::op_loadVar(double*& stack, UFunctionContext& ctx)
{
#ifdef EQ_PARSER_DEBUG
	std::cout << "loadVar : " << **(ctx.indirect_value) << std::endl;
#endif
	*(++stack) = **(ctx.indirect_value);
}
