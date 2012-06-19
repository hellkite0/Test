// $Id: BooleanExpression.cpp 3959 2009-09-25 09:50:04Z hyojin.lee $

#include "Stdafx.h"
#include "BooleanExpression.h"

using namespace J9;

////////////////////////////////////////////////////////////////////////////////
// BooleanOperator_Unary
////////////////////////////////////////////////////////////////////////////////
void
BooleanOperator_Unary::SetOperand(Ptr<BooleanExpression> pOperand)
{
	ASSERT(pOperand);
	pOperand_ = pOperand;
}

////////////////////////////////////////////////////////////////////////////////
// BooleanOperator_Binary
////////////////////////////////////////////////////////////////////////////////
void
BooleanOperator_Binary::SetOperand(Ptr<BooleanExpression> pLhs, Ptr<BooleanExpression> pRhs)
{
	ASSERT(pLhs && pRhs);
	pLhs_ = pLhs;
	pRhs_ = pRhs;
}

////////////////////////////////////////////////////////////////////////////////
// BooleanOperator_NOT
////////////////////////////////////////////////////////////////////////////////
bool
BooleanOperator_NOT::Evaluate(const Anys& rParams)
{
	return !pOperand_->Evaluate(rParams);
}

////////////////////////////////////////////////////////////////////////////////
// BooleanOperator_AND
////////////////////////////////////////////////////////////////////////////////
bool
BooleanOperator_AND::Evaluate(const Anys& rParams)
{
	return pLhs_->Evaluate(rParams) && pRhs_->Evaluate(rParams);
}

////////////////////////////////////////////////////////////////////////////////
// BooleanOperator_OR
////////////////////////////////////////////////////////////////////////////////
bool
BooleanOperator_OR::Evaluate(const Anys& rParams)
{
	return pLhs_->Evaluate(rParams) || pRhs_->Evaluate(rParams);
}

////////////////////////////////////////////////////////////////////////////////
// BooleanExpressionParser
////////////////////////////////////////////////////////////////////////////////
void
BooleanExpressionParser::SetExpression(const StringW& rExpression)
{
	sExpression_ = rExpression;
	iPos_ = 0;
}

bool
BooleanExpressionParser::Tokenize(PosfixTokens& rPosfix)
{
	rPosfix.clear();

	std::stack<Token>	cStack;

	while(true)
	{
		Token	cToken;

		// 5. ��� �Է��� ������ ���ÿ� �ִ� �����ڵ��� ��� ���Ͽ� ����Ѵ�.
		if (!GetToken(cToken))
		{
			while(!cStack.empty())
			{
				rPosfix.push_back(cStack.top());
				cStack.pop();
			}
			break;
		}

		// 1. '('�� ������ ���ÿ� Ǫ���Ѵ�.
		if (cToken.iTokenType_ == eTTLeftParenthesis)
		{
			cStack.push(cToken);
		}
		// 2. ')'�� ������ ���ÿ��� '('�� ���� �� ���� ���Ͽ� ����ϰ�, '('�� ���Ͽ� ������.
		else if (cToken.iTokenType_ == eTTRightParenthesis)
		{
			while(true)
			{
				const Token&	rToken = cStack.top();
				if (rToken.iTokenType_ == eTTLeftParenthesis)
				{
					// '('�� ������ �׸�
					cStack.pop();
					break;
				}
				else
				{
					// ���
                    rPosfix.push_back(rToken);
					cStack.pop();
				}
			}
		}
		// 3. �����ڸ� ������ ���ÿ��� �� �����ں��� ���� �켱������ �����ڸ� ���� ������ ���Ͽ� ����� �ڿ� �ڽ��� Ǫ���Ѵ�.
		else if (cToken.iTokenType_ == eTTOperator)
		{
			Size	iCurPrecedence = GetPrecedence(cToken);

			while(true)
			{
				// ������ ��������� �ڽ��� Ǫ���ϰ� �׸�
				if (cStack.empty())
				{
					cStack.push(cToken);
					break;
				}

				const Token&	rToken = cStack.top();
				// �������̸�
				if (rToken.iTokenType_ == eTTOperator || rToken.iTokenType_ == eTTLeftParenthesis)
				{
					Size	iTargetPrecedence = GetPrecedence(rToken);

					// �켱������ ���ų� ������ ���
					if (iCurPrecedence < iTargetPrecedence)
					{
						rPosfix.push_back(rToken);
						cStack.pop();
					}
					// �켱������ ���� �����ڸ� ������ �ڽ��� Ǫ���ϰ� �׸�
					else
					{
						cStack.push(cToken);
						break;
					}
				}
				// �����ڰ� �ƴϸ� �� �Ͽ� ���
				else
				{
					const Token&	rToken = cStack.top();
					rPosfix.push_back(rToken);
					cStack.pop();
				}
			}
		}
		// 4. �ǿ����ڴ� �׳� ����Ѵ�.
		else if (cToken.iTokenType_ == eTTVariable || cToken.iTokenType_ == eTTValue)
		{
			cStack.push(cToken);
		}
		// ���鹮���� ��� ����
		else if (cToken.iTokenType_ == eTTWhiteSpace)
		{
		}
		// �� �� ���� ���� ����
		else
		{
			ASSERT(0);
			return false;
		}
	}

	ASSERT(iPos_ == StringW::npos);
	ASSERT(cStack.empty());

	bTokenized_ = true;

	return true;
}

bool
BooleanExpressionParser::GetToken(Token& rToken)
{
	static const StringW	sOperators = _W("()&|! ");

	Size	iStart = iPos_;
	Size	iCur = sExpression_.find_first_of(sOperators, iStart);

	if (iStart != iCur)
	{
		StringW	sValue = StringW(sExpression_, iStart, iCur - iStart);

		if (sValue.empty())
		{
			iPos_ = _sc<Size>(StringW::npos);
			return false;
		}

		rToken.iTokenType_ = eTTVariable;
		rToken.iPos_ = iStart;
		rToken.sValue_ = sValue;

		iPos_ = iCur;
		return true;
	}

	if (iCur == StringW::npos)
	{
		iPos_ = _sc<Size>(StringW::npos);
		return false;
	}

	WChar	iOperator = sExpression_[iCur];
	switch(iOperator)
	{
	case '(':
		rToken.iTokenType_ = eTTLeftParenthesis;
		rToken.iPos_ = iCur;
		rToken.sValue_ = StringW(sExpression_, iCur, 1);
		++iCur;
		break;
	case ')':
		rToken.iTokenType_ = eTTRightParenthesis;
		rToken.iPos_ = iCur;
		rToken.sValue_ = StringW(sExpression_, iCur, 1);
		++iCur;
		break;
	case '&':
		rToken.iTokenType_ = eTTOperator;
		rToken.iOperatorType_ = eOTLogicalAND;
		rToken.iPos_ = iCur;
		rToken.sValue_ = StringW(sExpression_, iCur, 1);
		++iCur;
		break;
	case '|':
		rToken.iTokenType_ = eTTOperator;
		rToken.iOperatorType_ = eOTLogicalOR;
		rToken.iPos_ = iCur;
		rToken.sValue_ = StringW(sExpression_, iCur, 1);
		++iCur;
		break;
	case '!':
		rToken.iTokenType_ = eTTOperator;
		rToken.iOperatorType_ = eOTLogicalNOT;
		rToken.iPos_ = iCur;
		rToken.sValue_ = StringW(sExpression_, iCur, 1);
		++iCur;
		break;
	case ' ':
		rToken.iTokenType_ = eTTWhiteSpace;
		rToken.iPos_ = iCur;
		rToken.sValue_ = StringW(sExpression_, iCur, 1);
		++iCur;
		break;
	default:
		ASSERT(0);
		break;
	}
	iPos_ = iCur;

	return true;
}

Size
BooleanExpressionParser::GetPrecedence(const Token& rToken)
{
	ASSERT(rToken.iTokenType_ == eTTOperator || rToken.iTokenType_ == eTTLeftParenthesis);

	if (rToken.iTokenType_ == eTTOperator && rToken.iOperatorType_ == eOTLogicalNOT)
		return 3;
	else if (rToken.iTokenType_ == eTTOperator && rToken.iOperatorType_ == eOTLogicalAND)
		return 2;
	else if (rToken.iTokenType_ == eTTOperator && rToken.iOperatorType_ == eOTLogicalOR)
		return 1;
	else if (rToken.iTokenType_ == eTTLeftParenthesis)
		return 0;

	ASSERT(0);
	return SIZE_MAX;
}

Ptr<BooleanExpression>
BooleanExpressionParser::GetExpression()
{
	if (!bTokenized_)	Tokenize(cPosfix_);

	if (!bTokenized_)	return NULL;

	std::stack<Ptr<BooleanExpression> >	cStack;

	PosfixTokens::iterator	cIter = cPosfix_.begin();
	PosfixTokens::iterator	cIterEnd = cPosfix_.end();

	for (; cIter != cIterEnd; ++cIter)
	{
		const Token&	rToken = *cIter;

		switch(rToken.iTokenType_)
		{
		case eTTOperator:
			switch(rToken.iOperatorType_)
			{
			case eOTLogicalNOT:
				{
					ASSERT(!cStack.empty());

					Ptr<BooleanOperator_NOT>	pOperator(ALLOC);

					Ptr<BooleanExpression>	pOperand = cStack.top();	cStack.pop();

					pOperator->SetOperand(pOperand);
					cStack.push(pOperator);
				}
				break;

			case eOTLogicalAND:
				{
					ASSERT(2 <= cStack.size());

					Ptr<BooleanOperator_AND>	pOperator(ALLOC);

					Ptr<BooleanExpression>	pRhs = cStack.top();	cStack.pop();
					Ptr<BooleanExpression>	pLhs = cStack.top();	cStack.pop();

					pOperator->SetOperand(pLhs, pRhs);
					cStack.push(pOperator);
				}
				break;

			case eOTLogicalOR:
				{
					ASSERT(2 <= cStack.size());

					Ptr<BooleanOperator_OR>	pOperator(ALLOC);

					Ptr<BooleanExpression>	pRhs = cStack.top();	cStack.pop();
					Ptr<BooleanExpression>	pLhs = cStack.top();	cStack.pop();

					pOperator->SetOperand(pLhs, pRhs);
					cStack.push(pOperator);
				}
				break;
			default:
				ASSERT(0);
				break;
			}
			break;

		case eTTVariable:
			{
				Ptr<BooleanExpression>	pExpression = GetOperandExpression(rToken);

				ASSERT(pExpression);
				cStack.push(pExpression);
			}
			break;
		}
	}

	ASSERT(cStack.size() == 1);

	return cStack.top();
}

StringW
BooleanExpressionParser::GetPosfixExpression()
{
	if (!bTokenized_)	Tokenize(cPosfix_);

	if (!bTokenized_)	return _W("");

	StringW	sPosfix;

	PosfixTokens::iterator	cIter = cPosfix_.begin();
	PosfixTokens::iterator	cIterEnd = cPosfix_.end();

	for (; cIter != cIterEnd; ++cIter)
	{
		const Token&	rToken = *cIter;
		sPosfix += _W('[');
		sPosfix += rToken.sValue_;
		sPosfix += _W(']');
	}

	return sPosfix;
}
