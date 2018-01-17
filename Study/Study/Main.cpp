#include <iostream>

#include "PracQuickSort.h"

using namespace std;

int main(int argc, char** argv) {
	PracQuickSort quickSortObj;
	int data[11] = { 4, 23, 6, 234, 1, -1, 9 ,1 , -2, 99, 1234};
	quickSortObj.sort(data, 11);

	for (int i = 0; i < 11; i++) {
		cout << data[i] << ", ";
	}

	getchar();
	return 0;
}