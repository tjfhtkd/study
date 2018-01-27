#pragma once

#include <cstdio>

class GoFCollector
{
private:
	static GoFCollector* instance;
	
	int i = 0;

	GoFCollector();
	GoFCollector(const GoFCollector& ref) {}
	GoFCollector& operator=(const GoFCollector& ref) {}

public:
	~GoFCollector();
	static GoFCollector* getInstance() {
		if (instance == nullptr) {
			instance = new GoFCollector();
		}
		return instance;
	}

	int test() {
		i++;
		return i;
	}
};