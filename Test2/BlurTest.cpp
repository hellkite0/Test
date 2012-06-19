// BlurTest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define BMP_ID		0x4d42	// 'BM'
#define NUMCOLOR	4		// RGBA format

unsigned int FilterSize_;

#pragma pack(push,1)
typedef struct //tagBITMAPFILEHEADER
{
	unsigned short nID;
	unsigned long nSize;
	unsigned short nReserved1;
	unsigned short nReserved2;
	unsigned long nOffset;
} BMPHeader;

typedef struct //tagBITMAPINFOHEADER
{
	unsigned long nSize;  /* 40 bytes */
	unsigned long nWidth;
	unsigned long nHeight;
	unsigned short nPlanes; /* must be 1 */
	unsigned short nBitCount; /* 24 for this particular example */
	unsigned long nCompression; /* 0 for this particular example */
	unsigned long nImageSize; /* raw bitmap size */
	unsigned long nHorRes; 
	unsigned long nVerRes;
	unsigned long nColor;
	unsigned long nImportantColor; /* generally ignored */
} BMPInfo;
#pragma pack(pop)

int
ReadBMPImage(char *pImageName, BMPHeader *pHeader, BMPInfo *pHeaderInfo, unsigned char **ppImage)
{
	FILE *pInputBMP = NULL;
	unsigned int nFileTemp;

	// Read the input BMP
	fopen_s(&pInputBMP, pImageName, "rb");
	if (pInputBMP == NULL) {
		printf("Failed in reading %s\n", pImageName);
		return 1;
	}

	// Read the header
	nFileTemp = min(14L, sizeof(BMPHeader));
	if (fread(pHeader, 1, nFileTemp, pInputBMP)==nFileTemp) {
		if (pHeader->nID != BMP_ID) { // BM
			printf("This file is not a BMP.\n");
			return 1;
		}
	}
	else{
		printf("Cannot read this file.\n");
		return 1;
	}

	// Read the header info
	nFileTemp = min(40L, sizeof(BMPInfo));
	if (fread(pHeaderInfo, 1, nFileTemp, pInputBMP) != nFileTemp) {
		printf("Cannot read this file.\n");
		return 1;
	}
	if ((pHeaderInfo->nBitCount != 32) || (pHeaderInfo->nCompression != 0)) {
		printf("Unsupported BMP format.\n");
		return 1;
	}

	*ppImage = (unsigned char*) malloc(pHeaderInfo->nWidth * pHeaderInfo->nHeight * NUMCOLOR);
	if (*ppImage == NULL) {
		printf("Out of memory.\n");
		return 1;
	}
	if (fread(*ppImage, 1, pHeaderInfo->nWidth * pHeaderInfo->nHeight * NUMCOLOR, pInputBMP) != pHeaderInfo->nWidth * pHeaderInfo->nHeight * NUMCOLOR) {
		printf("Cannot read this file.\n");
		return 1;
	}

	if (pInputBMP != NULL)
		fclose(pInputBMP);

	return 0;
}


int
WriteBMPImage(char *pOutputName, BMPHeader *pHeader, BMPInfo *pHeaderInfo, unsigned char *pImage)
{
	FILE *pOutputBMP = NULL;

	// Write the filtered image out
	fopen_s(&pOutputBMP, pOutputName, "wb");
	if (pOutputBMP == NULL) {
		printf("Cannot write %s\n", pOutputName);
		return 1;
	}

	// Use the same BMP header and header info
	if (fwrite(pHeader, 1, sizeof(BMPHeader), pOutputBMP) != sizeof(BMPHeader)) {
		printf("Cannot write output bitmap\n");
		return 1;
	}

	if (fwrite(pHeaderInfo, 1, sizeof(BMPInfo), pOutputBMP) != sizeof(BMPInfo)) {
		printf("Cannot write output bitmap\n");
		return 1;
	}

	if (fwrite(pImage, 1, pHeaderInfo->nWidth * pHeaderInfo->nHeight * NUMCOLOR, pOutputBMP) != pHeaderInfo->nWidth * pHeaderInfo->nHeight * NUMCOLOR) {
		printf("Cannot write output bitmap\n");
		return 1;
	}

	return 0;
}


int
FilterImage(unsigned char *pImage, unsigned char *pFiltered, unsigned int nWidth, unsigned int nHeight)
{
	// 여기를 고치면 됩니다. 

	unsigned int c, i, j, ki, kj;
	int nX, nY;
	float fSum;
	int iFilterPixelCount;

	unsigned int halfFilterSize = FilterSize_ / 2;

	for (c = 1 ; c <= NUMCOLOR ; c++) {
		for (i = 0 ; i < nHeight ; i++) {
			for (j = 0 ; j < nWidth ; j++) {
				fSum = 0.0f;
				iFilterPixelCount = 0;

				for (ki = 0 ; ki < FilterSize_ ; ki++) {
					for (kj = 0 ; kj < FilterSize_ ; kj++) {
						nX = j + kj - halfFilterSize;
						nY = i + ki - halfFilterSize;
						
						if (nX < 0) continue;
						if (nY < 0) continue;
						if (nX >= (int)nWidth) continue;
						if (nY >= (int)nHeight) continue;

						fSum += (float)pImage[nY * nWidth * NUMCOLOR+ nX * NUMCOLOR + c - 1];
						iFilterPixelCount++;
					}
				}

				fSum /= iFilterPixelCount;

				if (fSum < 0.0f)
					fSum = 0.0f;
				else if (fSum > 255.0f)
					fSum = 255.0f;
				fSum += 0.5f;
				pFiltered[i * nWidth * NUMCOLOR + j * NUMCOLOR + c - 1] = (unsigned char)fSum;
			}
		}
	}

	return 0;
}


void ProcessImage(char* sReadImageName, char* sWriteImageName)
{
	unsigned char* pImage = NULL;
	unsigned char* pFilteredImage = NULL;
	BMPHeader header;
	BMPInfo headerinfo;

	ReadBMPImage(sReadImageName, &header, &headerinfo, &pImage);

	pFilteredImage = (unsigned char*) malloc(headerinfo.nWidth * headerinfo.nHeight * NUMCOLOR);

	time_t frequency, startTime, endTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
    FilterImage(pImage, pFilteredImage, headerinfo.nWidth, headerinfo.nHeight);
    QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    printf("%s : %8.3fms\n", sReadImageName, ((endTime - startTime) * 1000 / (float)frequency));

	WriteBMPImage(sWriteImageName, &header, &headerinfo, pFilteredImage);
}


int _tmain(int argc, _TCHAR* argv[])
{
	FilterSize_ = 11;

	// 여기 파일이름을 바꿔주면 됩니다. (입력파일 이름, 출력파일 이름)
	ProcessImage("50x50_1.bmp", "res_50x50_1.bmp");
    ProcessImage("50x50_2.bmp", "res_50x50_2.bmp");
    ProcessImage("50x50_3.bmp", "res_50x50_3.bmp");
    ProcessImage("50x50_4.bmp", "res_50x50_4.bmp");
    ProcessImage("50x50_5.bmp", "res_50x50_5.bmp");
    ProcessImage("50x50_6.bmp", "res_50x50_6.bmp");
    ProcessImage("50x50_7.bmp", "res_50x50_7.bmp");
    ProcessImage("50x50_8.bmp", "res_50x50_8.bmp");
    ProcessImage("50x50_9.bmp", "res_50x50_9.bmp");
    ProcessImage("50x50_10.bmp", "res_50x50_10.bmp");
    ProcessImage("500x500_1.bmp", "res_500x500_1.bmp");
    ProcessImage("500x500_2.bmp", "res_500x500_2.bmp");
    ProcessImage("500x500_3.bmp", "res_500x500_3.bmp");
    ProcessImage("500x500_4.bmp", "res_500x500_4.bmp");
    ProcessImage("2000x2000_1.bmp", "res_2000x2000_1.bmp");
    ProcessImage("2000x2000_2.bmp", "res_2000x2000_2.bmp");
	return 0;
}
