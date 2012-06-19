// $Id: Test.cpp 1576 2008-12-14 03:30:01Z hyojin.lee $

#include "stdafx.h"

#include "XMLAttributeToChild.h"
#include "Lazy.h"
#include "EventHandler.h"

using namespace J9;

const WChar* pNames[] =
{
	L"�ȿ���\n������",
	L"��⼷\n������",
	L"�̿���\n������",
	L"�ں���\n������",
	L"�����\n������",
	L"������\n������",
	L"�����\n������",
	L"����â\n������",
	L"�忬��\n������",
	L"�ǿ�ȣ\n������",
	L"���й�\n������",
	L"�ɱԼ�\n������",
	L"����ȫ\n������",
	L"ȫ����\n������",
	L"����\n������",
	L"�ڸ���\n������",
	L"������\n������",
	L"�۰��\n������",
	L"������\n������",
	L"�����\n������",
	L"������\n������",
	L"������\n������",
	L"�ǿ���\n������",
	L"�迵��\n������",
	L"������\n������",
	L"�뵿ǥ\n������",
	L"������\n������",
	L"������\n������",
	L"�̿��\n������",
	L"�迵ö\n������",
	L"����\n������",
	L"�̱�õ\n������",
	L"��â��\n������",
	L"�����\n������",
	L"���ö\n������",
	L"�����\n������",
	L"��â��\n������",
	L"�̻�\n������",
	L"�躴��\n������",
	L"��â��\n������",
	L"������\n������",
	L"������\n������",
	L"���μ�\n������",
	L"������\n������",
	L"����ȭ\n������",
	L"������\n������",
	L"�����\n������",
	L"�̻���\n������",
	L"�̸���\n������",
	L"������\n������",
	L"������\n������",
	L"��ȣ��\n������",
	L"���\n������",
	L"�����\n������",
	L"������\n������",
	L"�賲��\n������",
	L"�̽¿�\n������",
	L"������\n������",
	L"�����\n������",
	L"�����\n������",
	L"�ڰ���\n������",
	L"������\n������",
};

void Do()
{
	int NoPerson = sizeof(pNames) / sizeof(WChar*);
	int	NumRows = 4;
	int	NumCols = 4;

	for (int i = 0; i < 1000; ++i)
	{
		const WChar**	pBoard = new const WChar*[NoPerson];
		for (int j = 0; j < NoPerson; ++j)
			pBoard[j] = pNames[j];

		for (int j = 0; j < 1000; ++j)
		{
			int lhs = ::rand() % NoPerson;
			int rhs = ::rand() % NoPerson;

			const WChar*	pTemp = pBoard[lhs];
			pBoard[lhs] = pBoard[rhs];
			pBoard[rhs] = pTemp;
		}

		for (int j = 0; j < NumRows; ++j)
		{
			for (int k = 0; k < NumCols; ++k)
			{
                LOG(_W("\"%s\""), pBoard[j * NumCols + k]);
				if (k + 1 < NumCols)
					LOG(_W("\t"));
			}
			LOG(_W("\r\n"));
		}

		LOG(_W("\r\n"));

		delete[] pBoard;
	}
}

S32 wmain(int argc, TCHAR* argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	Do();
	//return XATC::DoMain(argc, argv);
	//return Lazy::DoMain(argc, argv);
	//return EH::DoMain(argc, argv);
	return 0;
}
