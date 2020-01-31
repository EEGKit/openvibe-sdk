#pragma once

#include "../ovp_defines.h"

#include <toolkit/ovtk_all.h>

#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_ast.hpp>


/**
* Enum of parent nodes identifiers.
*/
enum EByteCodes
{
	OP_NEG,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,

	OP_ABS,
	OP_ACOS,
	OP_ASIN,
	OP_ATAN,
	OP_CEIL,
	OP_COS,
	OP_EXP,
	OP_FLOOR,
	OP_LOG,
	OP_LOG10,
	OP_POW,
	OP_RAND,
	OP_SIN,
	OP_SQRT,
	OP_TAN,

	OP_IF_THEN_ELSE,

	OP_CMP_L,
	OP_CMP_G,
	OP_CMP_LE,
	OP_CMP_GE,
	OP_CMP_E,
	OP_CMP_NE,

	OP_BOOL_AND,
	OP_BOOL_OR,
	OP_BOOL_NOT,
	OP_BOOL_XOR,

	//used for special tree recognition

	//The equation is not a special one
	OP_USERDEF,
	//Identity
	OP_NONE,
	//X*X
	OP_X2
};

enum EVariables
{
	OP_VAR_X=0,
	OP_VAR_A,
	OP_VAR_B,
	OP_VAR_C,
	OP_VAR_D,
	OP_VAR_E,
	OP_VAR_F,
	OP_VAR_G,
	OP_VAR_H,
	OP_VAR_I,
	OP_VAR_J,
	OP_VAR_K,
	OP_VAR_L,
	OP_VAR_M,
	OP_VAR_N,
	OP_VAR_O,
	OP_VAR_P,
};

/**
* Symbols table for unary functions.
*
*/

struct SUnaryFunctionSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SUnaryFunctionSymbols()
	{
		add
				("abs", OP_ABS)
				("acos", OP_ACOS)
				("asin", OP_ASIN)
				("atan", OP_ATAN)
				("ceil", OP_CEIL)
				("cos", OP_COS)
				("exp", OP_EXP)
				("floor", OP_FLOOR)
				("log", OP_LOG)
				("log10", OP_LOG10)
				("rand", OP_RAND)
				("sin", OP_SIN)
				("sqrt", OP_SQRT)
				("tan", OP_TAN);
	}
};

/**
* Symbols table for binary functions.
*
*/
struct SBinaryFunctionSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SBinaryFunctionSymbols()
	{
		add
				("pow", OP_POW);
	}
};

/**
* Symbol tables for unary boolean operators
*
*/
struct SUnaryBooleanFunctionSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SUnaryBooleanFunctionSymbols()
	{
		add
				("!", OP_BOOL_NOT);
	}
};

/**
* Symbol tables for binary boolean operators
*
*/
struct SBinaryBoolean1FunctionSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SBinaryBoolean1FunctionSymbols()
	{
		add
				("&&", OP_BOOL_AND)
				("&", OP_BOOL_AND);
	}
};

/**
* Symbol tables for binary boolean operators
*
*/
struct SBinaryBoolean2FunctionSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SBinaryBoolean2FunctionSymbols()
	{
		add
				("~", OP_BOOL_XOR)
				("^", OP_BOOL_XOR);
	}
};

/**
* Symbol tables for binary boolean operators
*
*/
struct SBinaryBoolean3FunctionSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SBinaryBoolean3FunctionSymbols()
	{
		add
				("||", OP_BOOL_OR)
				("|", OP_BOOL_OR);
	}
};

/**
* Symbols table for comparison 1 functions.
*
*/
struct SComparison1FunctionSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SComparison1FunctionSymbols()
	{
		add
				("<", OP_CMP_L)
				(">", OP_CMP_G)
				("<=", OP_CMP_LE)
				(">=", OP_CMP_GE);
	}
};

/**
* Symbols table for comparison 2 functions.
*
*/
struct SComparison2FunctionSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SComparison2FunctionSymbols()
	{
		add
				("==", OP_CMP_E)
				("!=", OP_CMP_NE)
				("<>", OP_CMP_NE);
	}
};

/**
* Symbols table for mathematical constants.
*
*/
struct SMathConstantSymbols : boost::spirit::classic::symbols<double>
{
	SMathConstantSymbols()
	{
		add
				("m_pi", 3.14159265358979323846)
				("m_pi_2", 1.57079632679489661923)
				("m_pi_4", 0.78539816339744830962)
				("m_1_pi", 0.31830988618379067154)
				("m_2_pi", 0.63661977236758134308)
				("m_2_sqrt", 1.12837916709551257390)
				("m_sqrt2", 1.41421356237309504880)
				("m_sqrt1_2", 0.70710678118654752440)
				("m_e", 2.7182818284590452354)
				("m_log2e", 1.4426950408889634074)
				("m_log10e", 0.43429448190325182765)
				("m_ln", 0.69314718055994530942)
				("m_ln10", 2.30258509299404568402);
	}
};

/**
* Symbols table for variables.
*
*/
struct SVariableSymbols : boost::spirit::classic::symbols<uint64_t>
{
	SVariableSymbols()
	{
		add
				("x", OP_VAR_X)
				("a", OP_VAR_A)
				("b", OP_VAR_B)
				("c", OP_VAR_C)
				("d", OP_VAR_D)
				("e", OP_VAR_E)
				("f", OP_VAR_F)
				("g", OP_VAR_G)
				("h", OP_VAR_H)
				("i", OP_VAR_I)
				("j", OP_VAR_J)
				("k", OP_VAR_K)
				("l", OP_VAR_L)
				("m", OP_VAR_M)
				("n", OP_VAR_N)
				("o", OP_VAR_O)
				("p", OP_VAR_P);
	}
};

static SUnaryFunctionSymbols unaryFunction_p;
static SBinaryFunctionSymbols binaryFunction_p;
static SUnaryBooleanFunctionSymbols unaryBooleanFunction_p;
static SBinaryBoolean1FunctionSymbols binaryBoolean1Function_p;
static SBinaryBoolean2FunctionSymbols binaryBoolean2Function_p;
static SBinaryBoolean3FunctionSymbols binaryBoolean3Function_p;
static SComparison1FunctionSymbols comparison1Function_p;
static SComparison2FunctionSymbols comparison2Function_p;
static SMathConstantSymbols mathConstant_p;
static SVariableSymbols variable_p;

/**
* The parser's grammar.
*/
struct SEquationGrammar : boost::spirit::classic::grammar<SEquationGrammar>
{
	static const int realID       = 1;
	static const int variableID   = 2;
	static const int functionID   = 3;
	static const int constantID   = 4;
	static const int factorID     = 6;
	static const int termID       = 7;
	static const int expressionID = 8;
	static const int ifthenID     = 9;
	static const int comparisonID = 10;
	static const int booleanID    = 11;

	template <typename ScannerT>
	struct definition
	{
		explicit definition(SEquationGrammar const& /*grammar*/)
		{
			using namespace boost::spirit::classic;
			real     = leaf_node_d[real_p];
			variable = leaf_node_d[as_lower_d[variable_p]];
			constant = leaf_node_d[as_lower_d[mathConstant_p]];

			function = (root_node_d[as_lower_d[unaryFunction_p]] >> no_node_d[ch_p('(')] >> ifthen >> no_node_d[ch_p(')')])
					   | (root_node_d[as_lower_d[binaryFunction_p]] >> no_node_d[ch_p('(')] >> infix_node_d[(ifthen >> ',' >> ifthen)] >> no_node_d[ch_p(')')]);

			factor = (function | constant | variable | real) | inner_node_d['(' >> expression >> ')']
					 | inner_node_d['(' >> ifthen >> ')'] | (root_node_d[ch_p('-')] >> factor) | (root_node_d[ch_p('+')] >> factor);

			boolean     = (root_node_d[unaryBooleanFunction_p] >> factor) | factor;
			term        = boolean >> *((root_node_d[ch_p('*')] >> boolean) | (root_node_d[ch_p('/')] >> boolean));
			expression  = term >> *((root_node_d[ch_p('+')] >> term) | (root_node_d[ch_p('-')] >> term));
			comparison1 = (expression >> root_node_d[comparison1Function_p] >> expression) | expression;
			comparison2 = (comparison1 >> root_node_d[comparison2Function_p] >> comparison1) | comparison1;
			boolean1    = (comparison2 >> root_node_d[binaryBoolean1Function_p] >> comparison2) | comparison2;
			boolean2    = (boolean1 >> root_node_d[binaryBoolean2Function_p] >> boolean1) | boolean1;
			boolean3    = (boolean2 >> root_node_d[binaryBoolean3Function_p] >> boolean2) | boolean2;
			ifthen      = (boolean3 >> root_node_d[ch_p('?')] >> boolean3 >> no_node_d[ch_p(':')] >> boolean3) | boolean3;
		}

		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<booleanID>> boolean;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<booleanID>> boolean1;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<booleanID>> boolean2;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<booleanID>> boolean3;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<comparisonID>> comparison1;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<comparisonID>> comparison2;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<ifthenID>> ifthen;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<expressionID>> expression;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<termID>> term;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<factorID>> factor;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<realID>> real;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<variableID>> variable;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<functionID>> function;
		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<constantID>> constant;

		boost::spirit::classic::rule<ScannerT, boost::spirit::classic::parser_context<>, boost::spirit::classic::parser_tag<ifthenID>> const& start() const
		{
			return ifthen;
		}
	};
};
