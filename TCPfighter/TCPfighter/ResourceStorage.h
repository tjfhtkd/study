#pragma once

class CScreenDIB;

class ResourceStorage
{
public:
	/*
	* @brief : 정해진 크기의 Vector에 이미지를 저장해둔다.
	* @param resourceData : 저장할 이미지 데이터.
	* @return : 수용 가능 수치를 초과하거나 paramater가 null인 경우 false, 아니면 true.
	*/
	bool AddResource(KsDIB* resourceData);
	KsDIB* GetResource(Constants::ResourceName resourceName);

	/*
	* @brief : 보유한 이미지 리소스를 메모리에서 해제한다.
	* @detail : 이 함수를 호출하지 않는 이상 파괴자가 따로 리소스 메모리를 해제하지는 않는다.
	*/
	void ReleaseAllResource(void);

	size_t GetSize(void);
	size_t GetCapacity(void);

private:
	CScreenDIB*			dib;
	KsDIB*				background;

	// For resource management.
	std::vector<KsDIB*>		resources;	//초기에 로딩할 리소스 수는 고정되어 있으므로 자원 확보 용량 및 탐색시간O(1)을 위해 Vector를 선택.

public:
	ResourceStorage(size_t reserveSize);
	~ResourceStorage();
};

