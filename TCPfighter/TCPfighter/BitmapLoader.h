#pragma once
class BitmapLoader
{
public:
	BitmapLoader();
	~BitmapLoader();

public:
	/*
	* @brief : 파일 경로를 통해 Bitmap 이미지를 읽어 반환한다.
	* @param path : 읽을 파일이 들어있는 디렉토리 경로
	* @param fileName : 읽을 파일의 [이름.확장자]
	* @return : 비트맵 규격의 포맷
	*/
	BMP* LoadBmp(const wchar_t* path, const wchar_t* fileName);

	/*
	* @brief : 비트맵 포맷을 프로젝트에서 필요한 이미지 규격으로 변환한다. 이미지 상하 반전된 것도 다시 되돌린다.
	* @param transTarget : 변환할 비트맵
	* @return : BMP에서 프로젝트에 필요한 값만 추출된 구조체
	*/
	KsDIB* TranslateBmp(BMP* transTarget);
};

