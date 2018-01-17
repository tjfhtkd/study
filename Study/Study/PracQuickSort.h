#pragma once
class PracQuickSort
{
public :
	PracQuickSort();
	~PracQuickSort();

	void sort(int data[], int len);

private :
	void runSort(int data[], int start, int end);
	void swap(int data[], int i, int j);
};

