/* -------------------------------------------------------------------------- *
 *                                   Lepton                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the Lepton expression parser originating from              *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2009-2013 Stanford University and the Authors.      *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */
#pragma once

#include "windowsIncludes.h"
#include "CustomFunction.h"
#include "Exception.h"
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace Lepton {
class ExpressionTreeNode;

/**
 * An Operation represents a single step in the evaluation of an expression, such as a function,
 * an operator, or a constant value.  Each Operation takes some number of values as arguments
 * and produces a single value.
 *
 * This is an abstract class with subclasses for specific operations.
 */

class LEPTON_EXPORT Operation
{
public:
	virtual ~Operation() { }
	/**
	 * This enumeration lists all Operation subclasses.  This is provided so that switch statements
	 * can be used when processing or analyzing parsed expressions.
	 */
	enum Id
	{
		CONSTANT, VARIABLE, CUSTOM, ADD, SUBTRACT, MULTIPLY, DIVIDE, POWER, NEGATE, SQRT, EXP, LOG,
		SIN, COS, SEC, CSC, TAN, COT, ASIN, ACOS, ATAN, SINH, COSH, TANH, ERF, ERFC, STEP, DELTA, SQUARE, CUBE, RECIPROCAL,
		ADD_CONSTANT, MULTIPLY_CONSTANT, POWER_CONSTANT, MIN, MAX, ABS
	};
	/**
	 * Get the name of this Operation.
	 */
	virtual std::string getName() const = 0;
	/**
	 * Get this Operation's ID.
	 */
	virtual Id getId() const = 0;
	/**
	 * Get the number of arguments this operation expects.
	 */
	virtual int getNumArguments() const = 0;
	/**
	 * Create a clone of this Operation.
	 */
	virtual Operation* clone() const = 0;
	/**
	 * Perform the computation represented by this operation.
	 *
	 * @param args        the array of arguments
	 * @param variables   a map containing the values of all variables
	 * @return the result of performing the computation.
	 */
	virtual double evaluate(double* args, const std::map<std::string, double>& variables) const = 0;
	/**
	 * Return an ExpressionTreeNode which represents the analytic derivative of this Operation with respect to a variable.
	 *
	 * @param children     the child nodes
	 * @param childDerivs  the derivatives of the child nodes with respect to the variable
	 * @param variable     the variable with respect to which the derivate should be taken
	 */
	virtual ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
											 const std::string& variable) const = 0;
	/**
	 * Get whether this operation should be displayed with infix notation.
	 */
	virtual bool isInfixOperator() const { return false; }
	/**
	 * Get whether this is a symmetric binary operation, such that exchanging its arguments
	 * does not affect the result.
	 */
	virtual bool isSymmetric() const { return false; }

	virtual bool operator!=(const Operation& op) const { return op.getId() != getId(); }

	virtual bool operator==(const Operation& op) const { return !(*this != op); }

	class Constant;
	class Variable;
	class Custom;
	class Add;
	class Subtract;
	class Multiply;
	class Divide;
	class Power;
	class Negate;
	class Sqrt;
	class Exp;
	class Log;
	class Sin;
	class Cos;
	class Sec;
	class Csc;
	class Tan;
	class Cot;
	class Asin;
	class Acos;
	class Atan;
	class Sinh;
	class Cosh;
	class Tanh;
	class Erf;
	class Erfc;
	class Step;
	class Delta;
	class Square;
	class Cube;
	class Reciprocal;
	class AddConstant;
	class MultiplyConstant;
	class PowerConstant;
	class Min;
	class Max;
	class Abs;
};

class LEPTON_EXPORT Operation::Constant final : public Operation
{
public:
	Constant(double value) : value(value) { }

	std::string getName() const override
	{
		std::stringstream name;
		name << value;
		return name.str();
	}

	Id getId() const override { return CONSTANT; }

	int getNumArguments() const override { return 0; }

	Operation* clone() const override { return new Constant(value); }

	double evaluate(double* /*args*/, const std::map<std::string, double>& /*variables*/) const override { return value; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	double getValue() const { return value; }

	bool operator!=(const Operation& op) const override
	{
		const Constant* o = dynamic_cast<const Constant*>(&op);
		return (o == nullptr || o->value != value);
	}

private:
	double value = 0;
};

class LEPTON_EXPORT Operation::Variable final : public Operation
{
public:
	Variable(const std::string& name) : name(name) { }

	std::string getName() const override { return name; }

	Id getId() const override { return VARIABLE; }

	int getNumArguments() const override { return 0; }

	Operation* clone() const override { return new Variable(name); }

	double evaluate(double* /*args*/, const std::map<std::string, double>& variables) const override
	{
		const auto iter = variables.find(name);
		if (iter == variables.end()) { throw Exception("No value specified for variable " + name); }
		return iter->second;
	}

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	bool operator!=(const Operation& op) const override
	{
		const Variable* o = dynamic_cast<const Variable*>(&op);
		return (o == nullptr || o->name != name);
	}

private:
	std::string name;
};

class LEPTON_EXPORT Operation::Custom final : public Operation
{
public:
	Custom(const std::string& name, CustomFunction* function) : name(name), function(function), derivOrder(function->getNumArguments(), 0) { }

	Custom(const Custom& base, int derivIndex) : name(base.name), function(base.function->clone()), isDerivative(true), derivOrder(base.derivOrder)
	{
		derivOrder[derivIndex]++;
	}

	~Custom() override { delete function; }

	std::string getName() const override { return name; }

	Id getId() const override { return CUSTOM; }

	int getNumArguments() const override { return function->getNumArguments(); }

	Operation* clone() const override
	{
		Custom* clone       = new Custom(name, function->clone());
		clone->isDerivative = isDerivative;
		clone->derivOrder   = derivOrder;
		return clone;
	}

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override
	{
		if (isDerivative) { return function->evaluateDerivative(args, &derivOrder[0]); }
		return function->evaluate(args);
	}

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	const std::vector<int>& getDerivOrder() const { return derivOrder; }

	bool operator!=(const Operation& op) const override
	{
		const Custom* o = dynamic_cast<const Custom*>(&op);
		return (o == nullptr || o->name != name || o->isDerivative != isDerivative || o->derivOrder != derivOrder);
	}

private:
	std::string name;
	CustomFunction* function = nullptr;
	bool isDerivative        = false;
	std::vector<int> derivOrder;
};

class LEPTON_EXPORT Operation::Add final : public Operation
{
public:
	Add() { }

	std::string getName() const override { return "+"; }

	Id getId() const override { return ADD; }

	int getNumArguments() const override { return 2; }

	Operation* clone() const override { return new Add(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return args[0] + args[1]; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	bool isInfixOperator() const override { return true; }

	bool isSymmetric() const override { return true; }
};

class LEPTON_EXPORT Operation::Subtract final : public Operation
{
public:
	Subtract() { }

	std::string getName() const override { return "-"; }

	Id getId() const override { return SUBTRACT; }

	int getNumArguments() const override { return 2; }

	Operation* clone() const override { return new Subtract(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return args[0] - args[1]; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	bool isInfixOperator() const override { return true; }
};

class LEPTON_EXPORT Operation::Multiply final : public Operation
{
public:
	Multiply() { }

	std::string getName() const override { return "*"; }

	Id getId() const override { return MULTIPLY; }

	int getNumArguments() const override { return 2; }

	Operation* clone() const override { return new Multiply(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return args[0] * args[1]; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	bool isInfixOperator() const override { return true; }

	bool isSymmetric() const override { return true; }
};

class LEPTON_EXPORT Operation::Divide final : public Operation
{
public:
	Divide() { }

	std::string getName() const override { return "/"; }

	Id getId() const override { return DIVIDE; }

	int getNumArguments() const override { return 2; }

	Operation* clone() const override { return new Divide(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return args[0] / args[1]; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	bool isInfixOperator() const override { return true; }
};

class LEPTON_EXPORT Operation::Power final : public Operation
{
public:
	Power() { }

	std::string getName() const override { return "^"; }

	Id getId() const override { return POWER; }

	int getNumArguments() const override { return 2; }

	Operation* clone() const override { return new Power(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::pow(args[0], args[1]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	bool isInfixOperator() const override { return true; }
};

class LEPTON_EXPORT Operation::Negate final : public Operation
{
public:
	Negate() { }

	std::string getName() const override { return "-"; }

	Id getId() const override { return NEGATE; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Negate(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return -args[0]; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Sqrt final : public Operation
{
public:
	Sqrt() { }

	std::string getName() const override { return "sqrt"; }

	Id getId() const override { return SQRT; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Sqrt(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::sqrt(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Exp final : public Operation
{
public:
	Exp() { }

	std::string getName() const override { return "exp"; }

	Id getId() const override { return EXP; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Exp(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::exp(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Log final : public Operation
{
public:
	Log() { }

	std::string getName() const override { return "log"; }

	Id getId() const override { return LOG; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Log(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::log(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Sin final : public Operation
{
public:
	Sin() { }

	std::string getName() const override { return "sin"; }

	Id getId() const override { return SIN; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Sin(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::sin(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Cos final : public Operation
{
public:
	Cos() { }

	std::string getName() const override { return "cos"; }

	Id getId() const override { return COS; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Cos(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::cos(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Sec final : public Operation
{
public:
	Sec() { }

	std::string getName() const override { return "sec"; }

	Id getId() const override { return SEC; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Sec(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return 1.0 / std::cos(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Csc final : public Operation
{
public:
	Csc() { }

	std::string getName() const override { return "csc"; }

	Id getId() const override { return CSC; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Csc(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return 1.0 / std::sin(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Tan final : public Operation
{
public:
	Tan() { }

	std::string getName() const override { return "tan"; }

	Id getId() const override { return TAN; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Tan(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::tan(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Cot final : public Operation
{
public:
	Cot() { }

	std::string getName() const override { return "cot"; }

	Id getId() const override { return COT; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Cot(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return 1.0 / std::tan(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Asin final : public Operation
{
public:
	Asin() { }

	std::string getName() const override { return "asin"; }

	Id getId() const override { return ASIN; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Asin(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::asin(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Acos final : public Operation
{
public:
	Acos() { }

	std::string getName() const override { return "acos"; }

	Id getId() const override { return ACOS; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Acos(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::acos(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Atan final : public Operation
{
public:
	Atan() { }

	std::string getName() const override { return "atan"; }

	Id getId() const override { return ATAN; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Atan(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::atan(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Sinh final : public Operation
{
public:
	Sinh() { }

	std::string getName() const override { return "sinh"; }

	Id getId() const override { return SINH; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Sinh(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::sinh(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Cosh final : public Operation
{
public:
	Cosh() { }

	std::string getName() const override { return "cosh"; }

	Id getId() const override { return COSH; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Cosh(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::cosh(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Tanh final : public Operation
{
public:
	Tanh() { }

	std::string getName() const override { return "tanh"; }

	Id getId() const override { return TANH; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Tanh(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::tanh(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Erf final : public Operation
{
public:
	Erf() { }

	std::string getName() const override { return "erf"; }

	Id getId() const override { return ERF; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Erf(); }

	double evaluate(double* args, const std::map<std::string, double>& variables) const override;
	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Erfc final : public Operation
{
public:
	Erfc() { }

	std::string getName() const override { return "erfc"; }

	Id getId() const override { return ERFC; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Erfc(); }

	double evaluate(double* args, const std::map<std::string, double>& variables) const override;
	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Step final : public Operation
{
public:
	Step() { }

	std::string getName() const override { return "step"; }

	Id getId() const override { return STEP; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Step(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return (args[0] >= 0.0 ? 1.0 : 0.0); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Delta final : public Operation
{
public:
	Delta() { }

	std::string getName() const override { return "delta"; }

	Id getId() const override { return DELTA; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Delta(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return (args[0] == 0.0 ? 1.0 : 0.0); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Square final : public Operation
{
public:
	Square() { }

	std::string getName() const override { return "square"; }

	Id getId() const override { return SQUARE; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Square(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return args[0] * args[0]; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Cube final : public Operation
{
public:
	Cube() { }

	std::string getName() const override { return "cube"; }

	Id getId() const override { return CUBE; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Cube(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return args[0] * args[0] * args[0]; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Reciprocal final : public Operation
{
public:
	Reciprocal() { }

	std::string getName() const override { return "recip"; }

	Id getId() const override { return RECIPROCAL; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Reciprocal(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return 1.0 / args[0]; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::AddConstant final : public Operation
{
public:
	AddConstant(double value) : value(value) { }

	std::string getName() const override
	{
		std::stringstream name;
		name << value << "+";
		return name.str();
	}

	Id getId() const override { return ADD_CONSTANT; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new AddConstant(value); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return args[0] + value; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	double getValue() const { return value; }

	bool operator!=(const Operation& op) const override
	{
		const AddConstant* o = dynamic_cast<const AddConstant*>(&op);
		return (o == nullptr || o->value != value);
	}

private:
	double value = 0;
};

class LEPTON_EXPORT Operation::MultiplyConstant final : public Operation
{
public:
	MultiplyConstant(double value) : value(value) { }

	std::string getName() const override
	{
		std::stringstream name;
		name << value << "*";
		return name.str();
	}

	Id getId() const override { return MULTIPLY_CONSTANT; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new MultiplyConstant(value); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return args[0] * value; }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	double getValue() const { return value; }

	bool operator!=(const Operation& op) const override
	{
		const MultiplyConstant* o = dynamic_cast<const MultiplyConstant*>(&op);
		return (o == nullptr || o->value != value);
	}

private:
	double value = 0;
};

class LEPTON_EXPORT Operation::PowerConstant final : public Operation
{
public:
	PowerConstant(double value) : value(value)
	{
		intValue   = int(value);
		isIntPower = (intValue == value);
	}

	std::string getName() const override
	{
		std::stringstream name;
		name << "^" << value;
		return name.str();
	}

	Id getId() const override { return POWER_CONSTANT; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new PowerConstant(value); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override
	{
		if (isIntPower)
		{
			// Integer powers can be computed much more quickly by repeated multiplication.

			int exponent = intValue;
			double base  = args[0];
			if (exponent < 0)
			{
				exponent = -exponent;
				base     = 1.0 / base;
			}
			double result = 1.0;
			while (exponent != 0)
			{
				if ((exponent & 1) == 1) { result *= base; }
				base *= base;
				exponent = exponent >> 1;
			}
			return result;
		}
		return std::pow(args[0], value);
	}

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;

	double getValue() const { return value; }

	bool operator!=(const Operation& op) const override
	{
		const PowerConstant* o = dynamic_cast<const PowerConstant*>(&op);
		return (o == nullptr || o->value != value);
	}

	bool isInfixOperator() const override { return true; }

private:
	double value = 0;
	int intValue = 0;
	bool isIntPower;
};

class LEPTON_EXPORT Operation::Min final : public Operation
{
public:
	Min() { }

	std::string getName() const override { return "min"; }

	Id getId() const override { return MIN; }

	int getNumArguments() const override { return 2; }

	Operation* clone() const override { return new Min(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override
	{
		// parens around (std::min) are workaround for horrible microsoft max/min macro trouble
		return (std::min)(args[0], args[1]);
	}

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Max final : public Operation
{
public:
	Max() { }

	std::string getName() const override { return "max"; }

	Id getId() const override { return MAX; }

	int getNumArguments() const override { return 2; }

	Operation* clone() const override { return new Max(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override
	{
		// parens around (std::min) are workaround for horrible microsoft max/min macro trouble
		return (std::max)(args[0], args[1]);
	}

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};

class LEPTON_EXPORT Operation::Abs final : public Operation
{
public:
	Abs() { }

	std::string getName() const override { return "abs"; }

	Id getId() const override { return ABS; }

	int getNumArguments() const override { return 1; }

	Operation* clone() const override { return new Abs(); }

	double evaluate(double* args, const std::map<std::string, double>& /*variables*/) const override { return std::abs(args[0]); }

	ExpressionTreeNode differentiate(const std::vector<ExpressionTreeNode>& children, const std::vector<ExpressionTreeNode>& childDerivs,
									 const std::string& variable) const override;
};
} // namespace Lepton
