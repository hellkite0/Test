// $Id: BooleanExpression.h 1 2008-01-11 09:29:48Z hyojin.lee $

#ifndef __J9_BOOLEAN_EXPRESSION_H__
#define __J9_BOOLEAN_EXPRESSION_H__

namespace J9
{
	class Anys;
	class BooleanExpression
	{
	public:
		virtual ~BooleanExpression() {}
		virtual bool	Evaluate(const Anys& rParams) = 0;
	};

	class BooleanOperator : public BooleanExpression
	{
	public:
		virtual bool	Evaluate(const Anys& rParams) = 0;
	};

	class BooleanOperator_Unary : public BooleanOperator
	{
	public:
		virtual bool	Evaluate(const Anys& rParams) = 0;

		void			SetOperand(Ptr<BooleanExpression> pOperand);

	protected:
		Ptr<BooleanExpression>	pOperand_;
	};

	class BooleanOperator_Binary : public BooleanOperator
	{
	public:
		virtual bool	Evaluate(const Anys& rParams) = 0;

		void			SetOperand(Ptr<BooleanExpression> pLhs, Ptr<BooleanExpression> pRhs);

	protected:
		Ptr<BooleanExpression>	pLhs_;
		Ptr<BooleanExpression>	pRhs_;
	};

	class BooleanOperator_NOT : public BooleanOperator_Unary
	{
	public:
		virtual bool	Evaluate(const Anys& rParams);
	};

	class BooleanOperator_AND : public BooleanOperator_Binary
	{
	public:
		virtual bool	Evaluate(const Anys& rParams);
	};

	class BooleanOperator_OR : public BooleanOperator_Binary
	{
	public:
		virtual bool	Evaluate(const Anys& rParams);
	};

	class BooleanOeprand : public BooleanExpression
	{
	public:
		virtual bool	Evaluate(const Anys& rParams) = 0;
	};

	class BooleanExpressionParser
	{
		// AS IS
		// ���� �����ϴ� �����ڴ� !, &, |, (, )�̴�.

		// TO BE
		// �����ڰ� �߰��� ���
		// left -> right operator�� right -> left operator�� ���� �Ľ� ����� �����Ͽ��� �Ѵ�.
		// ������� �����ϴ� �����ڿ����� ���� ����
		// ==, !=, <, <=, >, >= ������ �߰�

	public:
		enum	TokenType
		{
			eTTUnknown,
			eTTWhiteSpace,
			eTTLeftParenthesis,
			eTTRightParenthesis,
			eTTOperator,
			eTTVariable,
			eTTValue,
		};

		enum	OperatorType
		{
			eOTUnknown,
			eOTLogicalNOT,
			eOTLogicalAND,
			eOTLogicalOR,
		};

		struct	Token
		{
			Token() : iTokenType_(eTTUnknown), iOperatorType_(eOTUnknown), iPos_(SIZE_MAX) {}

			TokenType		iTokenType_;
			OperatorType	iOperatorType_;
			Size			iPos_;
			StringW			sValue_;
		};

		typedef std::vector<Token>
		PosfixTokens;

	public:
		BooleanExpressionParser() : bTokenized_(false) {}

		void							SetExpression(const StringW& rExpression);
		StringW							GetPosfixExpression();
		Ptr<BooleanExpression>		GetExpression();

	protected:
		virtual Ptr<BooleanExpression>	GetOperandExpression(const Token& rToken) = 0;

		// return false when no token remains
		bool							Tokenize(PosfixTokens& rPosfix);
		bool							GetToken(Token& rToken);
		Size							GetPrecedence(const Token& rToken);

	protected:
		StringW							sExpression_;
		Size							iPos_;
		PosfixTokens					cPosfix_;
		bool							bTokenized_;
	};
}

#endif//__J9_BOOLEAN_EXPRESSION_H__