#pragma once

class CScreenDIB;

class ResourceStorage
{
public:
	/*
	* @brief : ������ ũ���� Vector�� �̹����� �����صд�.
	* @param resourceData : ������ �̹��� ������.
	* @return : ���� ���� ��ġ�� �ʰ��ϰų� paramater�� null�� ��� false, �ƴϸ� true.
	*/
	bool AddResource(KsDIB* resourceData);
	KsDIB* GetResource(Constants::ResourceName resourceName);

	/*
	* @brief : ������ �̹��� ���ҽ��� �޸𸮿��� �����Ѵ�.
	* @detail : �� �Լ��� ȣ������ �ʴ� �̻� �ı��ڰ� ���� ���ҽ� �޸𸮸� ���������� �ʴ´�.
	*/
	void ReleaseAllResource(void);

	size_t GetSize(void);
	size_t GetCapacity(void);

private:
	CScreenDIB*			dib;
	KsDIB*				background;

	// For resource management.
	std::vector<KsDIB*>		resources;	//�ʱ⿡ �ε��� ���ҽ� ���� �����Ǿ� �����Ƿ� �ڿ� Ȯ�� �뷮 �� Ž���ð�O(1)�� ���� Vector�� ����.

public:
	ResourceStorage(size_t reserveSize);
	~ResourceStorage();
};

