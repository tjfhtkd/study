#pragma once
class BitmapLoader
{
public:
	BitmapLoader();
	~BitmapLoader();

public:
	/*
	* @brief : ���� ��θ� ���� Bitmap �̹����� �о� ��ȯ�Ѵ�.
	* @param path : ���� ������ ����ִ� ���丮 ���
	* @param fileName : ���� ������ [�̸�.Ȯ����]
	* @return : ��Ʈ�� �԰��� ����
	*/
	BMP* LoadBmp(const wchar_t* path, const wchar_t* fileName);

	/*
	* @brief : ��Ʈ�� ������ ������Ʈ���� �ʿ��� �̹��� �԰����� ��ȯ�Ѵ�. �̹��� ���� ������ �͵� �ٽ� �ǵ�����.
	* @param transTarget : ��ȯ�� ��Ʈ��
	* @return : BMP���� ������Ʈ�� �ʿ��� ���� ����� ����ü
	*/
	KsDIB* TranslateBmp(BMP* transTarget);
};

