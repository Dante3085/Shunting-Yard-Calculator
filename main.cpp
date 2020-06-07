
#include "pch.h"

enum class OperandType
{
	OPERATOR,
	VALUE
};

struct Operand
{
	OperandType operandType;
	virtual double evaluate() = 0;
};

struct Value : public Operand
{
	double val;

	Value(double val)
		: val(val)
	{
		operandType = OperandType::VALUE;
	}

	double evaluate() override
	{
		return val;
	}
};

struct Operator : public Operand
{
	Operand* leftOp;
	Operand* rightOp;

	Operator()
		: leftOp(nullptr), rightOp(nullptr)
	{
		operandType = OperandType::OPERATOR;
	}
};

struct Plus : public Operator
{
	double evaluate() override
	{
		return leftOp->evaluate() + rightOp->evaluate();
	}
};

struct Minus : public Operator
{
	double evaluate() override
	{
		return leftOp->evaluate() - rightOp->evaluate();
	}
};

struct Mult : public Operator
{
	double evaluate() override
	{
		return leftOp->evaluate() * rightOp->evaluate();
	}
};

struct Div : public Operator
{
	double evaluate() override
	{
		return leftOp->evaluate() / rightOp->evaluate();
	}
};

struct Pow : public Operator
{
	double evaluate() override
	{
		double result = 1;

		for (int i = 0; i < rightOp->evaluate(); ++i)
		{
			result *= leftOp->evaluate();
		}

		return result;
	}
};

bool isNum(char c)
{
	// ASCII Table numbers for digits 0 - 9
	return c > 47 && c < 58 || c == '.';
}

bool isNum(std::string const& s)
{
	for (char c : s)
	{
		if (!isNum(c))
			return false;
	}
	return true;
}

bool isOperator(char c)
{
	return c == '+' ||
		c == '-' ||
		c == '/' ||
		c == '*' ||
		c == '^';
}

bool isOperator(std::string const& s)
{
	return s == "+" ||
		s == "-" ||
		s == "/" ||
		s == "*" ||
		s == "^";
}

int opPrecedence(char op)
{
	if (op == '+' || op == '-')
		return 1;
	else if (op == '*' || op == '/')
		return 2;
	else if (op == '^')
		return 3;
}

int opPrecedence(std::string const& op)
{
	if (op == "+" || op == "-")
		return 1;
	else if (op == "*" || op == "/")
		return 2;
	else if (op == "^")
		return 3;
}

std::vector<std::string> tokenize(std::string const& input)
{
	std::vector<std::string> tokens;
	std::string numBuff = "";
	for (int i = 0; i < input.size(); ++i)
	{
		if (isNum(input[i]))
		{
			numBuff += input[i];
			if (i == input.size() - 1)
			{
				tokens.push_back(numBuff);
			}
		}
		else
		{
			if (!numBuff.empty())
			{
				tokens.push_back(numBuff);
				numBuff = "";
			}
			tokens.push_back(std::string(1, input[i]));
		}
	}
	return tokens;
}

std::vector<std::string> shuntingYard(std::vector<std::string> const& tokens)
{
	std::vector<char> opStack;
	std::vector<std::string> output;
	for (std::string const& t : tokens)
	{
		if (isNum(t))
		{
			output.push_back(t);
		}
		else if (isOperator(t))
		{
			while (!opStack.empty() &&
				opStack.back() != '(' &&
				(opPrecedence(opStack.back()) - opPrecedence(t)) >= 0)
			{
				output.push_back(std::string(1, opStack.back()));
				opStack.erase(opStack.end() - 1);
			}
			opStack.push_back(t[0]);
		}
		else if (t == "(")
		{
			opStack.push_back('(');
		}
		else if (t == ")")
		{
			while (!opStack.empty() &&
				opStack.back() != '(')
			{
				output.push_back(std::string(1, opStack.back()));
				opStack.erase(opStack.end() - 1);
			}
			if (opStack.back() == '(')
			{
				opStack.erase(opStack.end() - 1);
			}
		}
	}
	while (!opStack.empty())
	{
		output.push_back(std::string(1, opStack.back()));
		opStack.erase(opStack.end() - 1);
	}

	return output;
}

std::vector<Operand*> createOperands(std::vector<std::string> const& syOutput)
{
	std::vector<Operand*> operands;
	for (std::string const& t : syOutput)
	{
		if (isNum(t))
		{
			operands.push_back(new Value(std::atof(t.c_str())));
		}
		else if (t == "+")
		{
			operands.push_back(new Plus());
		}
		else if (t == "-")
		{
			operands.push_back(new Minus());
		}
		else if (t == "*")
		{
			operands.push_back(new Mult());
		}
		else if (t == "/")
		{
			operands.push_back(new Div());
		}
		else if (t == "^")
		{
			operands.push_back(new Pow());
		}
	}

	return operands;
}

Operand* detectOperands(std::vector<Operand*>& operands)
{
	for (int i = 0; i < operands.size(); ++i)
	{
		if (operands[i]->operandType == OperandType::OPERATOR)
		{
			Operator* oper = (Operator*)operands[i];
			if (oper->leftOp != nullptr) continue;

			oper->leftOp = operands[i - 2];
			oper->rightOp = operands[i - 1];

			operands.erase(operands.begin() + i - 2, operands.begin() + i);
			i = 0;
		}
	}

	return operands[0];
}

// TODO free memory

template <class T>
std::string toString(std::vector<T> const& vec)
{
	std::ostringstream vts;
	vts << "{ ";

	if (!vec.empty())
	{
		std::copy(vec.begin(), vec.end() - 1,
			std::ostream_iterator<T>(vts, ", "));

		vts << vec.back();
	}
	vts << " }";
	return vts.str();
}

// TODO: Warum funktioniert das mit dem if (argv[1] == "-loop") nicht.
//       Ich kriege immer Segmentation fault core dumped.

int main(int argc, char** argv)
{
	/*if (argc != 2)
	{
		std::cout << "Only 2 arguments allowed. 1. Program name which is always given. 2. You're arithmetic expression." << std::endl;
		return 0;
	}*/

	/*if (argv[1] == "-loop")
	{*/
		std::string input = "";
		while (input != "exit")
		{
			std::cout << ">>";
			std::cin >> input;
			std::vector<std::string> tokens = tokenize(input);
			std::vector<std::string> syOutput = shuntingYard(tokens);
			std::vector<Operand*> operands = createOperands(syOutput);
			Operand* root = detectOperands(operands);

			/*std::cout << "tokens: " << toString(tokens) << std::endl;
			std::cout << "syOutput: " << toString(syOutput) << std::endl;
			std::cout << "Operands: " << toString(operands) << std::endl;
			std::cout << "root: " << root << std::endl;*/
			std::cout << std::setprecision(10) << root->evaluate() << std::endl;
		}
	// }
	/*else
	{
		std::string input(argv[1]);
		std::vector<std::string> tokens = tokenize(input);
		std::vector<std::string> syOutput = shuntingYard(tokens);
		std::vector<Operand*> operands = createOperands(syOutput);
		Operand* root = detectOperands(operands);
		std::cout << std::setprecision(10) << root->evaluate() << std::endl;
	}*/

	return 1;
}