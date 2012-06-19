// $Id$

#include "Stdafx.h"
#include "Hardware.h"

#include "UtilityString.h"

////////////////////////////////////////////////////////////////////////////////
/// Hardware
////////////////////////////////////////////////////////////////////////////////
void
J9::Hardware::Init()
{
	CPU::Init();
}

void
J9::Hardware::Dump()
{
	CPU::Dump();
}

////////////////////////////////////////////////////////////////////////////////
/// CPU
////////////////////////////////////////////////////////////////////////////////
int		J9::Hardware::CPU::iVendor = 0;
int		J9::Hardware::CPU::iCacheLineSize = 0;
int		J9::Hardware::CPU::iNumLogicalCores = 1;
int		J9::Hardware::CPU::iNumPhysicalCores = 1;
bool	J9::Hardware::CPU::bHyperThread = false;

// implement from cpuid.cpp 
// processor: x86, x64
// Use the __cpuid intrinsic to get information about a CPU
#include <intrin.h>

void
J9::Hardware::CPU::Init()
{
	char CPUString[0x20];
	int CPUInfo[4] = {-1};
	int nCLFLUSHcachelinesize = 0;
	unsigned    iMaxInfoType;
	unsigned	iMaxExtendedInfoType;

	// __cpuid with an InfoType argument of 0 returns the number of
	// valid Ids in CPUInfo[0] and the CPU identification string in
	// the other three array elements. The CPU identification string is
	// not in linear order. The code below arranges the information 
	// in a human readable form.
	__cpuid(CPUInfo, 0);
	iMaxInfoType = CPUInfo[0];
	memset(CPUString, 0, sizeof(CPUString));
	*((int*)CPUString) = CPUInfo[1];
	*((int*)(CPUString+4)) = CPUInfo[3];
	*((int*)(CPUString+8)) = CPUInfo[2];

	__cpuid(CPUInfo, 0x80000000);
	iMaxExtendedInfoType = CPUInfo[0];

	if (::strcmp(CPUString, "GenuineIntel") == 0)
		iVendor = eIntel;
	else if (::strcmp(CPUString, "AuthenticAMD") == 0)
		iVendor = eAMD;
	else
	{
		iVendor = 0;
		J9::Throw<J9Error>(J9ECHardware);
	}

	if (iMaxInfoType < 1)
	{
		J9::Throw<J9Error>(J9ECHardware);
	}

	// vendor dependent
	if (IsIntel())
	{
		__cpuid(CPUInfo, 1);
		nCLFLUSHcachelinesize = ((CPUInfo[1] >> 8) & 0xff) * 8;
	}
	else if (IsAMD())
	{
	}

	// Detect hyper-threads
	if (1 <= iMaxInfoType)
	{
		int iRegisters[4]; 

		// Get CPU features
		__cpuid(iRegisters, 1);

		// Logical core count per CPU
		iNumLogicalCores = (iRegisters[1] >> 16) & 0xff; // EBX[23:16]
		iNumPhysicalCores = iNumLogicalCores;

		unsigned iCPUFeatures = iRegisters[3]; // EDX

		// Detect hyper-threads
		bHyperThread = false;
		if (iVendor == eIntel)
		{
			if (4 <= iMaxInfoType)
			{
				// Get DCP cache info 
				__cpuid(iRegisters, 4);
				iNumPhysicalCores = ((iRegisters[0] >> 26) & 0x3f) + 1; // EAX[31:26] + 1
			}
		}
		else if (iVendor == eAMD && iCPUFeatures & (1 << 28)) // HTT bit
		{
			if (0x80000008 <= iMaxExtendedInfoType)
			{
				// Get NC: Number of CPU cores - 1
				__cpuid(iRegisters, 0x80000008);
				iNumPhysicalCores = ((unsigned)(iRegisters[2] & 0xff)) + 1; // ECX[7:0] + 1
			}
		}

		if (iNumPhysicalCores < iNumLogicalCores)
			bHyperThread = true; 
	}
}

void
J9::Hardware::CPU::Dump()
{
#if 0
	const WChar* szFeatures[] =
	{
		_W("x87 FPU On Chip"),
		_W("Virtual-8086 Mode Enhancement"),
		_W("Debugging Extensions"),
		_W("Page Size Extensions"),
		_W("Time Stamp Counter"),
		_W("RDMSR and WRMSR Support"),
		_W("Physical Address Extensions"),
		_W("Machine Check Exception"),
		_W("CMPXCHG8B Instruction"),
		_W("APIC On Chip"),
		_W("Unknown1"),
		_W("SYSENTER and SYSEXIT"),
		_W("Memory Type Range Registers"),
		_W("PTE Global Bit"),
		_W("Machine Check Architecture"),
		_W("Conditional Move/Compare Instruction"),
		_W("Page Attribute Table"),
		_W("36-bit Page Size Extension"),
		_W("Processor Serial Number"),
		_W("CFLUSH Extension"),
		_W("Unknown2"),
		_W("Debug Store"),
		_W("Thermal Monitor and Clock Ctrl"),
		_W("MMX Technology"),
		_W("FXSAVE/FXRSTOR"),
		_W("SSE Extensions"),
		_W("SSE2 Extensions"),
		_W("Self Snoop"),
		_W("Multithreading Technology"),
		_W("Thermal Monitor"),
		_W("Unknown4"),
		_W("Pend. Brk. EN.")
	};

	char CPUString[0x20];
	char CPUBrandString[0x40];
	int CPUInfo[4] = {-1};
	int nSteppingID = 0;
	int nModel = 0;
	int nFamily = 0;
	int nProcessorType = 0;
	int nExtendedmodel = 0;
	int nExtendedfamily = 0;
	int nBrandIndex = 0;
	int nCLFLUSHcachelinesize = 0;
	int nLogicalProcessors = 0;
	int nAPICPhysicalID = 0;
	int nFeatureInfo = 0;
	int nCacheLineSize = 0;
	int nL2Associativity = 0;
	int nCacheSizeK = 0;
	int nPhysicalAddress = 0;
	int nVirtualAddress = 0;
	unsigned    nIds, nExIds, i;

	bool    bSSE3Instructions = false;
	bool    bMONITOR_MWAIT = false;
	bool    bCPLQualifiedDebugStore = false;
	bool    bVirtualMachineExtensions = false;
	bool    bEnhancedIntelSpeedStepTechnology = false;
	bool    bThermalMonitor2 = false;
	bool    bSupplementalSSE3 = false;
	bool    bL1ContextID = false;
	bool    bCMPXCHG16B = false;
	bool    bxTPRUpdateControl = false;
	bool    bPerfDebugCapabilityMSR = false;
	bool    bSSE41Extensions = false;
	bool    bSSE42Extensions = false;
	bool    bPOPCNT = false;

	bool    bMultithreading = false;

	bool    bLAHF_SAHFAvailable = false;
	bool    bCmpLegacy = false;
	bool    bSVM = false;
	bool    bExtApicSpace = false;
	bool    bAltMovCr8 = false;
	bool    bLZCNT = false;
	bool    bSSE4A = false;
	bool    bMisalignedSSE = false;
	bool    bPREFETCH = false;
	bool    bSKINITandDEV = false;
	bool    bSYSCALL_SYSRETAvailable = false;
	bool    bExecuteDisableBitAvailable = false;
	bool    bMMXExtensions = false;
	bool    bFFXSR = false;
	bool    b1GBSupport = false;
	bool    bRDTSCP = false;
	bool    b64Available = false;
	bool    b3DNowExt = false;
	bool    b3DNow = false;
	bool    bNestedPaging = false;
	bool    bLBRVisualization = false;
	bool    bFP128 = false;
	bool    bMOVOptimization = false;

	// __cpuid with an InfoType argument of 0 returns the number of
	// valid Ids in CPUInfo[0] and the CPU identification string in
	// the other three array elements. The CPU identification string is
	// not in linear order. The code below arranges the information 
	// in a human readable form.
	__cpuid(CPUInfo, 0);
	nIds = CPUInfo[0];
	memset(CPUString, 0, sizeof(CPUString));
	*((int*)CPUString) = CPUInfo[1];
	*((int*)(CPUString+4)) = CPUInfo[3];
	*((int*)(CPUString+8)) = CPUInfo[2];

	// Get the information associated with each valid Id
	for (i=0; i<=nIds; ++i)
	{
		__cpuid(CPUInfo, i);
		LOG(_W("\nFor InfoType %d\n"), i); 
		LOG(_W("CPUInfo[0] = 0x%x\n"), CPUInfo[0]);
		LOG(_W("CPUInfo[1] = 0x%x\n"), CPUInfo[1]);
		LOG(_W("CPUInfo[2] = 0x%x\n"), CPUInfo[2]);
		LOG(_W("CPUInfo[3] = 0x%x\n"), CPUInfo[3]);

		// Interpret CPU feature information.
		if  (i == 1)
		{
			nSteppingID = CPUInfo[0] & 0xf;
			nModel = (CPUInfo[0] >> 4) & 0xf;
			nFamily = (CPUInfo[0] >> 8) & 0xf;
			nProcessorType = (CPUInfo[0] >> 12) & 0x3;
			nExtendedmodel = (CPUInfo[0] >> 16) & 0xf;
			nExtendedfamily = (CPUInfo[0] >> 20) & 0xff;
			nBrandIndex = CPUInfo[1] & 0xff;
			nCLFLUSHcachelinesize = ((CPUInfo[1] >> 8) & 0xff) * 8;
			nLogicalProcessors = ((CPUInfo[1] >> 16) & 0xff);
			nAPICPhysicalID = (CPUInfo[1] >> 24) & 0xff;
			bSSE3Instructions = (CPUInfo[2] & 0x1) || false;
			bMONITOR_MWAIT = (CPUInfo[2] & 0x8) || false;
			bCPLQualifiedDebugStore = (CPUInfo[2] & 0x10) || false;
			bVirtualMachineExtensions = (CPUInfo[2] & 0x20) || false;
			bEnhancedIntelSpeedStepTechnology = (CPUInfo[2] & 0x80) || false;
			bThermalMonitor2 = (CPUInfo[2] & 0x100) || false;
			bSupplementalSSE3 = (CPUInfo[2] & 0x200) || false;
			bL1ContextID = (CPUInfo[2] & 0x300) || false;
			bCMPXCHG16B= (CPUInfo[2] & 0x2000) || false;
			bxTPRUpdateControl = (CPUInfo[2] & 0x4000) || false;
			bPerfDebugCapabilityMSR = (CPUInfo[2] & 0x8000) || false;
			bSSE41Extensions = (CPUInfo[2] & 0x80000) || false;
			bSSE42Extensions = (CPUInfo[2] & 0x100000) || false;
			bPOPCNT= (CPUInfo[2] & 0x800000) || false;
			nFeatureInfo = CPUInfo[3];
			bMultithreading = (nFeatureInfo & (1 << 28)) || false;
		}
	}

	// Calling __cpuid with 0x80000000 as the InfoType argument
	// gets the number of valid extended IDs.
	__cpuid(CPUInfo, 0x80000000);
	nExIds = CPUInfo[0];
	memset(CPUBrandString, 0, sizeof(CPUBrandString));

	// Get the information associated with each extended ID.
	for (i=0x80000000; i<=nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		LOG(_W("\nFor InfoType %x\n"), i); 
		LOG(_W("CPUInfo[0] = 0x%x\n"), CPUInfo[0]);
		LOG(_W("CPUInfo[1] = 0x%x\n"), CPUInfo[1]);
		LOG(_W("CPUInfo[2] = 0x%x\n"), CPUInfo[2]);
		LOG(_W("CPUInfo[3] = 0x%x\n"), CPUInfo[3]);

		if  (i == 0x80000001)
		{
			bLAHF_SAHFAvailable = (CPUInfo[2] & 0x1) || false;
			bCmpLegacy = (CPUInfo[2] & 0x2) || false;
			bSVM = (CPUInfo[2] & 0x4) || false;
			bExtApicSpace = (CPUInfo[2] & 0x8) || false;
			bAltMovCr8 = (CPUInfo[2] & 0x10) || false;
			bLZCNT = (CPUInfo[2] & 0x20) || false;
			bSSE4A = (CPUInfo[2] & 0x40) || false;
			bMisalignedSSE = (CPUInfo[2] & 0x80) || false;
			bPREFETCH = (CPUInfo[2] & 0x100) || false;
			bSKINITandDEV = (CPUInfo[2] & 0x1000) || false;
			bSYSCALL_SYSRETAvailable = (CPUInfo[3] & 0x800) || false;
			bExecuteDisableBitAvailable = (CPUInfo[3] & 0x10000) || false;
			bMMXExtensions = (CPUInfo[3] & 0x40000) || false;
			bFFXSR = (CPUInfo[3] & 0x200000) || false;
			b1GBSupport = (CPUInfo[3] & 0x400000) || false;
			bRDTSCP = (CPUInfo[3] & 0x8000000) || false;
			b64Available = (CPUInfo[3] & 0x20000000) || false;
			b3DNowExt = (CPUInfo[3] & 0x40000000) || false;
			b3DNow = (CPUInfo[3] & 0x80000000) || false;
		}

		// Interpret CPU brand string and cache information.
		if  (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if  (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if  (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		else if  (i == 0x80000006)
		{
			nCacheLineSize = CPUInfo[2] & 0xff;
			nL2Associativity = (CPUInfo[2] >> 12) & 0xf;
			nCacheSizeK = (CPUInfo[2] >> 16) & 0xffff;
		}
		else if  (i == 0x80000008)
		{
			nPhysicalAddress = CPUInfo[0] & 0xff;
			nVirtualAddress = (CPUInfo[0] >> 8) & 0xff;
		}
		else if  (i == 0x8000000A)
		{
			bNestedPaging = (CPUInfo[3] & 0x1) || false;
			bLBRVisualization = (CPUInfo[3] & 0x2) || false;
		}
		else if  (i == 0x8000001A)
		{
			bFP128 = (CPUInfo[0] & 0x1) || false;
			bMOVOptimization = (CPUInfo[0] & 0x2) || false;
		}
	}

	// Display all the information in user-friendly format.

	LOG(_W("\n\nCPU String: %S\n"), CPUString);

	if  (nIds >= 1)
	{
		if  (nSteppingID)
			LOG(_W("Stepping ID = %d\n"), nSteppingID);
		if  (nModel)
			LOG(_W("Model = %d\n"), nModel);
		if  (nFamily)
			LOG(_W("Family = %d\n"), nFamily);
		if  (nProcessorType)
			LOG(_W("Processor Type = %d\n"), nProcessorType);
		if  (nExtendedmodel)
			LOG(_W("Extended model = %d\n"), nExtendedmodel);
		if  (nExtendedfamily)
			LOG(_W("Extended family = %d\n"), nExtendedfamily);
		if  (nBrandIndex)
			LOG(_W("Brand Index = %d\n"), nBrandIndex);
		if  (nCLFLUSHcachelinesize)
			LOG(_W("CLFLUSH cache line size = %d\n"),
			nCLFLUSHcachelinesize);
		if (bMultithreading && (nLogicalProcessors > 0))
			LOG(_W("Logical Processor Count = %d\n"), nLogicalProcessors);
		if  (nAPICPhysicalID)
			LOG(_W("APIC Physical ID = %d\n"), nAPICPhysicalID);

		if  (nFeatureInfo || bSSE3Instructions ||
			bMONITOR_MWAIT || bCPLQualifiedDebugStore ||
			bVirtualMachineExtensions || bEnhancedIntelSpeedStepTechnology ||
			bThermalMonitor2 || bSupplementalSSE3 || bL1ContextID || 
			bCMPXCHG16B || bxTPRUpdateControl || bPerfDebugCapabilityMSR || 
			bSSE41Extensions || bSSE42Extensions || bPOPCNT || 
			bLAHF_SAHFAvailable || bCmpLegacy || bSVM ||
			bExtApicSpace || bAltMovCr8 ||
			bLZCNT || bSSE4A || bMisalignedSSE ||
			bPREFETCH || bSKINITandDEV || bSYSCALL_SYSRETAvailable || 
			bExecuteDisableBitAvailable || bMMXExtensions || bFFXSR || b1GBSupport ||
			bRDTSCP || b64Available || b3DNowExt || b3DNow || bNestedPaging || 
			bLBRVisualization || bFP128 || bMOVOptimization )
		{
			LOG(_W("\nThe following features are supported:\n"));

			if  (bSSE3Instructions)
				LOG(_W("\tSSE3\n"));
			if  (bMONITOR_MWAIT)
				LOG(_W("\tMONITOR/MWAIT\n"));
			if  (bCPLQualifiedDebugStore)
				LOG(_W("\tCPL Qualified Debug Store\n"));
			if  (bVirtualMachineExtensions)
				LOG(_W("\tVirtual Machine Extensions\n"));
			if  (bEnhancedIntelSpeedStepTechnology)
				LOG(_W("\tEnhanced Intel SpeedStep Technology\n"));
			if  (bThermalMonitor2)
				LOG(_W("\tThermal Monitor 2\n"));
			if  (bSupplementalSSE3)
				LOG(_W("\tSupplemental Streaming SIMD Extensions 3\n"));
			if  (bL1ContextID)
				LOG(_W("\tL1 Context ID\n"));
			if  (bCMPXCHG16B)
				LOG(_W("\tCMPXCHG16B Instruction\n"));
			if  (bxTPRUpdateControl)
				LOG(_W("\txTPR Update Control\n"));
			if  (bPerfDebugCapabilityMSR)
				LOG(_W("\tPerf\\Debug Capability MSR\n"));
			if  (bSSE41Extensions)
				LOG(_W("\tSSE4.1 Extensions\n"));
			if  (bSSE42Extensions)
				LOG(_W("\tSSE4.2 Extensions\n"));
			if  (bPOPCNT)
				LOG(_W("\tPPOPCNT Instruction\n"));

			i = 0;
			nIds = 1;
			while (i < (sizeof(szFeatures)/sizeof(const char*)))
			{
				if  (nFeatureInfo & nIds)
				{
					LOG(_W("\t"));
					LOG(szFeatures[i]);
					LOG(_W("\n"));
				}

				nIds <<= 1;
				++i;
			}
			if (bLAHF_SAHFAvailable)
				LOG(_W("\tLAHF/SAHF in 64-bit mode\n"));
			if (bCmpLegacy)
				LOG(_W("\tCore multi-processing legacy mode\n"));
			if (bSVM)
				LOG(_W("\tSecure Virtual Machine\n"));
			if (bExtApicSpace)
				LOG(_W("\tExtended APIC Register Space\n"));
			if (bAltMovCr8)
				LOG(_W("\tAltMovCr8\n"));
			if (bLZCNT)
				LOG(_W("\tLZCNT instruction\n"));
			if (bSSE4A)
				LOG(_W("\tSSE4A (EXTRQ, INSERTQ, MOVNTSD, MOVNTSS)\n"));
			if (bMisalignedSSE)
				LOG(_W("\tMisaligned SSE mode\n"));
			if (bPREFETCH)
				LOG(_W("\tPREFETCH and PREFETCHW Instructions\n"));
			if (bSKINITandDEV)
				LOG(_W("\tSKINIT and DEV support\n"));
			if (bSYSCALL_SYSRETAvailable)
				LOG(_W("\tSYSCALL/SYSRET in 64-bit mode\n"));
			if (bExecuteDisableBitAvailable)
				LOG(_W("\tExecute Disable Bit\n"));
			if (bMMXExtensions)
				LOG(_W("\tExtensions to MMX Instructions\n"));
			if (bFFXSR)
				LOG(_W("\tFFXSR\n"));
			if (b1GBSupport)
				LOG(_W("\t1GB page support\n"));
			if (bRDTSCP)
				LOG(_W("\tRDTSCP instruction\n"));
			if (b64Available)
				LOG(_W("\t64 bit Technology\n"));
			if (b3DNowExt)
				LOG(_W("\t3Dnow Ext\n"));
			if (b3DNow)
				LOG(_W("\t3Dnow! instructions\n"));
			if (bNestedPaging)
				LOG(_W("\tNested Paging\n"));
			if (bLBRVisualization)
				LOG(_W("\tLBR Visualization\n"));
			if (bFP128)
				LOG(_W("\tFP128 optimization\n"));
			if (bMOVOptimization)
				LOG(_W("\tMOVU Optimization\n"));
		}
	}

	if  (nExIds >= 0x80000004)
		LOG(_W("\nCPU Brand String: %S\n"), CPUBrandString);

	if  (nExIds >= 0x80000006)
	{
		LOG(_W("Cache Line Size = %d\n"), nCacheLineSize);
		LOG(_W("L2 Associativity = %d\n"), nL2Associativity);
		LOG(_W("Cache Size = %dK\n"), nCacheSizeK);
	}

	if  (nExIds >= 0x80000008)
	{
		LOG(_W("Physical Address = %d\n"), nPhysicalAddress);
		LOG(_W("Virtual Address = %d\n"), nVirtualAddress);
	}
#endif
}
