#include "stdafx.h"
#include "ResourceStorage.h"


ResourceStorage::ResourceStorage(size_t reserveSize)
{
	resources.reserve(reserveSize);
}


ResourceStorage::~ResourceStorage()
{
}



bool ResourceStorage::AddResource(KsDIB* resourceData)
{
	if (resources.size() >= resources.capacity())
	{
		return false;
	}

	if (resourceData != nullptr)
	{
		resources.push_back(resourceData);
		return true;
	}
	return false;
}

KsDIB* ResourceStorage::GetResource(Constants::ResourceName resourceName)
{
	return resources.at((int)resourceName);;
}

void ResourceStorage::ReleaseAllResource(void)
{
	// 문제 : 아직 돌려보지 않아서 진짜 제대로 잘 지워질지 모르겠음...
	size_t correctionAddress = sizeof(BITMAPFILEHEADER*);
	auto begin	= resources.begin();
	auto end		= resources.end();
	while (begin != end)
	{
		BYTE* p = (BYTE*) &((*begin)->bmpInfoHeader);
		BMP* del = (BMP*) (p - correctionAddress);
		free(del);
		del = nullptr;
		begin++;
	}
	resources.clear();
}

size_t ResourceStorage::GetSize(void)
{
	return resources.size();
}

size_t ResourceStorage::GetCapacity(void)
{
	return resources.capacity();
}
