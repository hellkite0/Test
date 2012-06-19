// $Id$

#ifndef __J9_HARDWARE_H__
#define __J9_HARDWARE_H__

namespace J9
{
	namespace Hardware
	{
		void	Init();
		void	Dump();

		// __cpuid()�� ���ؼ� cpu�� ������ �о �����صδ� �Լ�
		// �ϴ� �ʿ��� ������ �����Ѵ�
		// �߰��� �ʿ��� ���� ������ cpuid.cpp�� �����Ͽ� �߰��ϵ��� �Ѵ�
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