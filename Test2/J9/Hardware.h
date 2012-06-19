// $Id$

#ifndef __J9_HARDWARE_H__
#define __J9_HARDWARE_H__

namespace J9
{
	namespace Hardware
	{
		void	Init();
		void	Dump();

		// __cpuid()를 통해서 cpu의 정보를 읽어서 저장해두는 함수
		// 일단 필요한 정보만 저장한다
		// 추가로 필요한 것이 있으면 cpuid.cpp를 참고하여 추가하도록 한다
		class CPU
		{
		private:
			enum
			{
				// cpu vendor
				eIntel = 1,
				eAMD,
			};

		public:
			static void	Init();
			static void	Dump();

			static bool	IsIntel()				{ return iVendor == eIntel; }
			static bool	IsAMD()					{ return iVendor == eAMD; }
			static int	GetCacheLineSize()		{ return iCacheLineSize; }
			static int	GetNumPhysicalCores()	{ return iNumPhysicalCores; }
			static int	GetNumLogicalCores()	{ return iNumLogicalCores; }

		private:
			static int	iVendor;
			static int	iCacheLineSize;
			static int	iNumLogicalCores;
			static int	iNumPhysicalCores;
			static bool	bHyperThread;
		};
	}
}

#endif//__J9_HARDWARE_H__