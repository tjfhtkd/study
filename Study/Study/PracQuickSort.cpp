#include "PracQuickSort.h"


PracQuickSort::PracQuickSort()
{
}


PracQuickSort::~PracQuickSort()
{
}


void PracQuickSort::sort(int data[], int len) {
	runSort(data, 0, len - 1);
}

void PracQuickSort::runSort(int data[], int start, int end) {
	if (end - start < 1) {
		return;
	}

	int pivot = start;
	int left = start + 1;
	int right = end;
	while (left < right) {
		while (left < right) {
			if (data[left] >= data[pivot]) {
				break;
			}
			left++;
		}

		while (right >= left) {
			if (data[right] < data[pivot]) {
				break;
			}
			right--;
		}

		if (left < right) {
			swap(data, left, right);
		}
	}
	if (data[pivot] > data[right]) {
		swap(data, pivot, right);
	}

	runSort(data, start, left - 1);
	runSort(data, left, end);
}

void PracQuickSort::swap(int data[], int i, int j) {
	int tmp = data[i];
	data[i] = data[j];
	data[j] = tmp;
}