#include "stdafx.h"
#include <cstdio>

#include "BitmapLoader.h"

BitmapLoader::BitmapLoader()
{
}


BitmapLoader::~BitmapLoader()
{
}

BMP* BitmapLoader::LoadBmp(const wchar_t* path, const wchar_t* fileName)
{
	wchar_t fullPath[256];
	wsprintfW(fullPath, L"%s/%s", path, fileName);

	FILE* fp;
	if (_wfopen_s(&fp, fullPath, L"rb, ccs=UTF-16LE") != 0)
	{
		return nullptr;
	}

	BMP* bmp = (BMP*)malloc(sizeof(BMP));

	int fileHeaderSize = sizeof(BITMAPFILEHEADER);
	BITMAPFILEHEADER* bmpFileHeader = (BITMAPFILEHEADER*)malloc(fileHeaderSize);
	int checkHeaderSize = fread_s(bmpFileHeader, fileHeaderSize, sizeof(BYTE), fileHeaderSize, fp);
	if (checkHeaderSize != fileHeaderSize)
	{
		free(bmpFileHeader);
		bmpFileHeader = nullptr;
		fclose(fp);
		return nullptr;
	}
	bmp->bmpFileHeader = bmpFileHeader;

	int fileInfoSize = sizeof(BITMAPINFO);
	BITMAPINFO* bmpInfoHeader = (BITMAPINFO*)malloc(fileInfoSize);
	int checkInfoSize = fread_s(bmpInfoHeader, fileInfoSize, sizeof(BYTE), fileInfoSize, fp);
	if (checkInfoSize != fileInfoSize)
	{
		free(bmpFileHeader);
		free(bmpInfoHeader);
		bmpFileHeader = nullptr;
		bmpInfoHeader = nullptr;
		fclose(fp);
		return nullptr;
	}
	bmp->bmpInfoHeader = bmpInfoHeader;

	// BITMAPINFOHEADER에서 RGB quard 공간에 아예 사라져버림. 그래서 이미지가 1픽셀 더 땡겨서 시작함.
	if (bmpFileHeader->bfOffBits == 0x36)
	{
		fseek(fp, sizeof(PIXEL) * (-1), SEEK_CUR);
	}

	int pitch		= (bmpInfoHeader->bmiHeader.biWidth * (bmpInfoHeader->bmiHeader.biBitCount >> 3) + 3) & ~3;
	bmp->pitch	= pitch;

	int imgSize			= pitch * bmpInfoHeader->bmiHeader.biHeight;
	BYTE* rawData		= (BYTE*)malloc(imgSize);
	int checkImgSize	= fread_s(rawData, imgSize, sizeof(BYTE), imgSize, fp);
	if (checkImgSize <= 0)
	{
		free(bmpFileHeader);
		free(bmpInfoHeader);
		free(rawData);
		bmpFileHeader = nullptr;
		bmpInfoHeader = nullptr;
		rawData = nullptr;
		fclose(fp);
		return nullptr;
	}
	bmp->data = (PIXEL*)rawData;

	return bmp;
}