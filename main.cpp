#include "MineSweeping.h"
#include <QtWidgets/QApplication>

float middleNumber(std::vector<int> nums1, std::vector<int> nums2)
{
	std::vector<int> m;
	int aP = 0;
	int bP = 0;
	int aSize = nums1.size();
	int bSize = nums2.size();
	while ((aP < aSize) || (bP < bSize))
	{
		if (aP == aSize)
		{
			while (bP < bSize)
				m.push_back(nums2[bP++]);
			break;
		}
		if (bP == bSize)
		{
			while (aP < aSize)
				m.push_back(nums1[aP++]);
			break;
		}
		if (nums1[aP] < nums2[bP])
			m.push_back(nums1[aP++]);
		else
			m.push_back(nums2[bP++]);
	}
	int mSize = m.size();
	
	if (mSize % 2 == 0)
	{
		return (double)(m[mSize / 2 -1] + m[mSize / 2 ])/2;
	}
	else
	{
		return m[mSize / 2];
	}
	return 0;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MineSweeping w;

	w.show();
	return a.exec();
}
