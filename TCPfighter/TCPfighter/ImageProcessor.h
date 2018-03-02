#pragma once

class ImageProcessor
{
public:
	ImageProcessor();
	~ImageProcessor();

public:
	KsDIB* TranslateBmp(BMP* transTarget);
	KsDIB* ReverseBmpData(BMP* target);
	KsDIB* ReverseBmpData(KsDIB* target);
};

