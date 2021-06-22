//============================================================================
// Name        : Tree.cpp
// Author      : Faizan Ahmed
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "BTree.h"
#include <math.h>

int main() {
	BT<int> bt(5, "Year");
	cout << bt.Search(2001) << endl;;
	cout << bt.Search(2090) << endl;
//	system("pause");
	return 0;
}
