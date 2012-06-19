#include "StdAfx.h"
#include "FuncHelper.h"

class AchieveID
{
	union ID
	{
		struct
		{
			unsigned short Achieve;
			unsigned char Challenge;
			unsigned char dummy;
		};
		unsigned long UniqueID;
	};

public:
	AchieveID() {}
	AchieveID(unsigned short iAchieveID, unsigned char iChallengeID)
	{
		iID_.Achieve = iAchieveID;
		iID_.Challenge = iChallengeID;
		iID_.dummy = 0;
	}
	~AchieveID() {}

	unsigned long GetID() { return iID_.UniqueID; }
	void SetID(unsigned long iID) { iID_.UniqueID = iID; }
	unsigned short GetAchieveID() { return iID_.Achieve; }
	unsigned char GetChallengeID() { return iID_.Challenge; }
	void testFunc() { printf("Test FUnc\n"); }
private:
	ID iID_;
};

using namespace std;

int 
main(void)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	AchieveID* pID = new AchieveID(10, 10);
	pFuncCallMgr->Register(pID, &AchieveID::testFunc);
	pFuncCallMgr->DoAction();
	
	return 0;
}