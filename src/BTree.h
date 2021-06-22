#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
using namespace std;

string ndub = "NoDublicate";
string dub = "Dublicate";
string NoChild = "No  Child"; //9 charter as each file pointer in 9 characters in eclipse
int SizeoffirstLine = 6; //make it 6 after writing isleaf
int SizeofPointers = 10;
int SizeofData = 66 - 41;
int SizeofKey;
int sizeofmaindata = 103;

template<typename DT>
class BT {
	struct BTNode {
		string DataFile;	//contain the keys and child pointers
		bool isLeaf;
		BTNode() {
			this->DataFile = "\0";
			isLeaf = true;
		}
		BTNode(string _FileName) {
			this->DataFile = _FileName;
			isLeaf = true;
		}
		string Is_Leaf() {
			ifstream read(DataFile.c_str());
			string line;
			read.seekg(4, ios::beg);	//skip nkey
			getline(read, line);
			read.close();
			return line;
		}
		void updateIsleaf(bool _Leaf) {
			this->isLeaf = _Leaf;
			ofstream write(DataFile.c_str());
			write.seekp(4, ios::beg);
			write << this->isLeaf << endl;
			write.close();
		}
		/*read datafile, go to sepacific lines
		 * containing keys and return
		 * keys of vector
		 * */
		void Getobject(string* V, int size) {
			if (this->DataFile == "\0") {	//Node is empty
				return;
			}
			ifstream read(DataFile.c_str());
			if (read.is_open()) {			//file open
				int curson = SizeoffirstLine + SizeofPointers; //curson in file
				string key;
				for (int i = 0; i < size; i++) {
					read.seekg(curson, ios::cur);
					getline(read, key);
					V[i] = key;
					curson = (SizeofData + SizeofPointers);
				}
				read.close();
				return;
			} else {
				cout << "file is not open" << endl;
				return;
			}
		}
		/*read datafile, go to sepacific lines
		 * containing Pointers and return
		 * keys of vector
		 * */
		string GetData(int index) {
			string V;
			if (this->DataFile == "\0") {	//Node is empty
				return V;
			}
			ifstream read(this->DataFile.c_str());
			if (read.is_open()) {			//file open
				read.seekg(SizeoffirstLine + SizeofPointers, ios::beg);
				getline(read, V);
				for (int i = 0; i < index; i++) {
					read.seekg(SizeofData + SizeofPointers, ios::cur);
					getline(read, V);
				}
				getline(read, V);
				return V;				//to do return all keys
			} else
				return V;				//file not open correctly
		}

		string GetChild(int index) {
			string V;
			if (this->DataFile == "\0") {	//Node is empty
				return V;
			}
			ifstream read(this->DataFile.c_str());
			if (read.is_open()) {			//file open
				read.seekg(SizeoffirstLine, ios::beg);
				getline(read, V);
				for (int i = 0; i < index; i++) {
					read.seekg(SizeofData + SizeofKey, ios::cur);
					getline(read, V);
				}
				return V;				//to do return all keys
			} else
				return V;				//file not open correctly
		}
		string getNkeys() {
			string str;
			ifstream read(this->DataFile.c_str(), ios::in);
			getline(read, str);
			read.close();
			return str;
		}
	};
	int Degree;
	int NonRootNodes;
	string TempFileName;	//to give the name to files
	BTNode* root;
	BTNode* Parent;		//parent node is empty as root has no parent

public:
	BT(int _Degree, string _KeyType) {
		this->Degree = _Degree;
		this->NonRootNodes = 0;
		this->TempFileName = "A0000.txt";
		this->root = new BTNode;
		this->Parent = new BTNode;
		if (_KeyType == "ID") {
			this->readfile(0);
		} else if (_KeyType == "Year") {
			this->readfile(1);
		} else if (_KeyType == "Age-adjusted Death Rate") {
			this->readfile(6);
		}
	}
	string Search(DT _Key) {
		string line =  GetDataRec(_Key, root);	//read line from file
		string filename;
		string linenum;
		int w = 0;
		for(int i = 0;line[i] != '\0'; i++){
			if(line[i] == ' ')
				w++;
			if(w == 1){			//get filename from that line
				filename += line[i];
			}
			else if(w == 2){	//get line number
				linenum += line[i];
			}
		}
		int lineNum = strToDT(linenum);
		ifstream read(filename.c_str(),ios::in);	//open the main file
		getline(read,line);							//ignore the first line
		read.seekg(lineNum * sizeofmaindata,ios::beg);		//go to line number
		getline(read,line);
		return line;
	}
private:

	/*search the data*/
	string GetDataRec(DT _Key, BTNode* _Root) {
		string strKey = _Root->getNkeys();
		string filename = _Root->DataFile;
		const int __N = strToDT(strKey);
		string Obj[__N];
		//get the pointer of keys
		_Root->Getobject(Obj, __N);
		int i;
		DT keytocomp;
		string child;
		for (i = 0; i < __N; i++) {
			keytocomp = strToDT(Obj[i]);
			if (_Key < keytocomp) {	//child to enter
				child = _Root->GetChild(i);	//get pointer of child
				if (child == NoChild || child == "\0") {//mean no child present
					cout << "Data not found" << endl;
					return "\0";
				} else {								//enter to child
					//enter to that child
					BTNode* temp = new BTNode;
					temp->DataFile = child;
					return GetDataRec(_Key, temp);
				}
			} else if (_Key == keytocomp) {
				return _Root->GetData(i);
			}
		}
		if (i == __N) {	//to fild key is greater then all of keys in present node
			child = _Root->GetChild(i);
			if (child == NoChild || child == "\0") {	//mean no child present
				cout << "Data not found" << endl;
				return "\0";
			} else {								//enter to child
				//enter to that child
				BTNode* temp = new BTNode;
				temp->DataFile = child;
				return GetDataRec(_Key, temp);
			}
		}
		return "\0";
	}
	/*create the file
	 * */
	void insert(DT _Key, BTNode* root, string _MainDataFileName, int _LineNum,
			BTNode* _ParentNode, int _IndexOfParent = 0) {
		string nodefile = root->DataFile;
		if (nodefile == "\0") {		//root is empty
			//cout << "new key " << _Key << " is inserting" << endl;
			nodefile = this->TempFileName;	//create the file of tempFileName
			incFileName(this->TempFileName);	//increment in tempFilename
			root->DataFile = nodefile;
			//open file
			ofstream write(nodefile.c_str());
			int nKey = 1;
			//first line store the number of keys
			write << nKey << "  " << endl << true << endl;
			write << NoChild << endl;

			//set size of key by subtracting after key insertion to before key insertion
			unsigned int start = write.tellp();
			write << _Key << endl;
			SizeofKey = write.tellp() - start;
			write << ndub << " " << _MainDataFileName << " " << _LineNum << endl
					<< NoChild << endl;
		} else {					//root is non-empty
			int nKey;
			DT key;
			//string childfile;
			ifstream read(nodefile.c_str());
			read >> nKey;
			if (nKey >= Degree)
				nKey = 0;
			//cout << nKey << endl;
			string line;
			read.seekg(SizeoffirstLine + SizeofPointers, ios::beg);	//ignore the remaining part of line 01 and first pointer
			//cout << "Line = " << line << endl;
			for (int i = 0; i < nKey; i++) {
				read >> line;	//read key as a string
				key = strToDT(line);
				//cout << "key = " << key << endl;
				if (key < _Key) {
					//cout << "i = " << i << endl;
				}
				read >> line;
				read >> line;
			}
			read.close();
			const int __N = nKey;
			string Obj[__N];
			//get the pointer of keys
			root->Getobject(Obj, nKey);
			//check the appropriate position where to insert the key or go to children
			if (nKey < this->Degree - 1) {		//new key is inserted
				incKey(nodefile, nKey);	//increment the number of keys in node file
				ofstream write(nodefile.c_str(), ios::app);
				write << _Key << endl << ndub << " " << _MainDataFileName << " "
						<< _LineNum << endl;
				write << NoChild << endl;
				write.close();
			} else if (nKey == (this->Degree - 1)) {	//node is full
				//cout << "root->Is_Leaf() " << root->Is_Leaf() << endl;
				//cout << "splite child node" << endl;
				spliteNode(root, _ParentNode, _IndexOfParent, Obj);
			}
		}
	}
	/*get file name, parent file name and data of node */
	void spliteNode(BTNode* _CurNode, BTNode* _ParentNode, int _IndexOfParent,
			string* Obj) {

		if (_ParentNode->DataFile == "\0") {
			//cout << "new file is going to create" << endl;
			string temp = "Temp.txt";

			_ParentNode->DataFile = TempFileName;
			ofstream writeParent(TempFileName.c_str());
			writeParent << "0  " << endl;
			writeParent << "0" << endl;

			//change the file name for next use
			incFileName(this->TempFileName);

			ofstream writeChild1("temp.txt");
			writeChild1 << "0   " << endl;
			writeChild1 << "1" << endl;
			string newchild = TempFileName;
			ofstream writeChild2(newchild.c_str());
			writeChild2 << "0   " << endl;
			writeChild2 << "1" << endl;
			//change the file name for next use
			incFileName(this->TempFileName);

			ifstream readNode(_CurNode->DataFile.c_str());
			int childkey = (this->Degree / 2) + 1;
			//cout << "cjil  " << childkey << endl;
			string line;
			readNode.seekg(SizeoffirstLine, ios::beg);//no of keys and is leaf ignored
			int filenum = 1;
			while (!readNode.eof()) {
				if (filenum < childkey) {	//first child is going to write
					getline(readNode, line);	//less then key Ptr
					writeChild1 << line << endl;
					getline(readNode, line);	//key
					writeChild1 << line << endl;
					getline(readNode, line);	//data of key
					writeChild1 << line << endl;
					if (filenum == childkey - 1) {
						getline(readNode, line);	//greater then key ptr
						writeChild1 << line << endl;
					}
				} else if (filenum == childkey) { // Parent file writing
					writeParent << _CurNode->DataFile << endl; //write first child
					getline(readNode, line);	// key
					writeParent << line << endl;
					getline(readNode, line);	//data of key
					writeParent << line << endl;
					writeParent << newchild << endl;
				} else {						//second child is going to write
					getline(readNode, line);	//less then to key ptr
					writeChild2 << line << endl;
					getline(readNode, line);	//key
					writeChild2 << line << endl;
					getline(readNode, line);	//data of key
					writeChild2 << line << endl;
					getline(readNode, line);	//greater then to key ptr
					writeChild2 << line << endl;
				}
				filenum++;
			}
			readNode.close();
			writeChild1.close();
			writeChild2.close();
			writeParent.close();
			incKey(_ParentNode->DataFile, 0);
			incKey("temp.txt", childkey - 2);
			incKey(newchild, Degree - (childkey + 2));

			remove(_CurNode->DataFile.c_str());
			rename("temp.txt", _CurNode->DataFile.c_str());
			_CurNode->DataFile = _ParentNode->DataFile;
			_ParentNode->DataFile = "\0";
		}
	}
	void Update(DT _Key, int _LineNumber) {
		string data;
		data = Search(_Key);

	}
	/*take file name in char+index format as argument
	 * increment in it index
	 * if index reached to 9999 then it increment the character
	 * A9999 become B0000*/
	void incFileName(string& _FileName) {
		if (_FileName[4] != '9') {
			_FileName[4]++;
		} else {
			for (int i = 4; i >= 1; i--) {
				if (_FileName[i] == '9') {
					_FileName[i] = '0';
					if (_FileName[i - 1] != '9') {
						_FileName[i - 1]++;
						break;			//to save extra iterations
					}
				}

			}
		}
	}
	/*take file and old number of keys
	 * write incrmented key on file*/
	void incKey(string NodeFile, int nKey) {
		ofstream write(NodeFile.c_str(), ios::in);
		nKey++;
		write << nKey;
		write.close();
	}
	void readfile(int _ColNum) {

		ifstream readMainData;
		string filename;
		string line;
		string word;

		int linenum;
		//string datafile = root->DataFile;
		int nfile = 0;

		filename = "test_1.csv";
		//for (int i = 1; i <= 9; i++) {	//to read 9 files
		//filename[46] = i + 48;
		readMainData.open(filename.c_str());
		linenum = 1;
		if (readMainData.is_open()) {		//check the file is open
			getline(readMainData, line);	//first line has to be ignored
			while (!readMainData.eof()) {	//read to end of file
				getline(readMainData, line);	//read line by line
				int ColNum = 0;
				word = "\0";
				for (int j = 0; line[j] != '\0' && line[j] != '\n'; j++) {//tokenize the key from line
					if (line[j] == ',')
						ColNum++;
					else if (ColNum == _ColNum) {
						word += line[j];
					} else if (ColNum == ColNum + 1)
						break;
				}	//end for
				DT k = strToDT(word);

				insert(k, root, filename, linenum, Parent);	//store key and it data
				linenum++;
			}	//while eof
		} else {
			cout << filename << " is not open" << endl;
		}
		readMainData.close();
		//}
	}
	/*take string as a argument and convert it to DT
	 * if string has int value it will convert it into int
	 * or if it has dot it convert it into float of single decimal
	 * */

	DT strToDT(string str) {
		for (int i = 0; i < str.length(); i++) {
			if (str[i] == ' ')
				str.erase(i);
		}
		DT d = 0;
		if (str.find('.') < str.length()) {	//. is present
			int lentodot = str.find('.');
			for (int i = 0; i < lentodot; i++) {	//convert digits of string
				int digit = str[i] - 48;
				digit *= (pow(double(10), double(lentodot - i - 1)));
				d += digit;
			}
			DT decimal = 0;
			int len = str.length();
			for (int i = lentodot + 1; i < len; i++) {//convert decimals of string
				int digit = str[i] - 48;
				digit *= (pow(double(10), double(len - i - 1)));
				decimal += digit;
			}
			len = len - lentodot;		//number of decimals
			decimal = (decimal / pow(double(10), double(len - 1)));	//devide by number of decimals to get decimal part of string
			d += decimal;
		} else {
			int len = str.length();
			for (int i = 0; i < len; i++) {
				int digit = str[i] - 48;
				digit *= (pow(double(10), double(len - i - 1)));
				d += digit;
			}
		}
		return d;
	}

};

template<>
class BT<string> {
	struct BTNode {
		string DataFile;	//contain the keys and child pointers
		bool isLeaf;
		BTNode() {
			this->DataFile = "\0";
			isLeaf = true;
		}
		BTNode(string _FileName) {
			this->DataFile = _FileName;
			isLeaf = true;
		}
		string Is_Leaf() {
			ifstream read(DataFile.c_str());
			string line;
			read.seekg(4, ios::beg);	//skip nkey
			getline(read, line);
			read.close();
			return line;
		}
		void updateIsleaf(bool _Leaf) {
			this->isLeaf = _Leaf;
			ofstream write(DataFile.c_str());
			write.seekp(4, ios::beg);
			write << this->isLeaf << endl;
			write.close();
		}
		/*read datafile, go to sepacific lines
		 * containing keys and return
		 * keys of vector
		 * */
		void Getobject(string* V, int size) {
			if (this->DataFile == "\0") {	//Node is empty
				return;
			}
			ifstream read(DataFile.c_str());
			if (read.is_open()) {			//file open
				int curson = SizeoffirstLine + SizeofPointers; //curson in file
				string key;
				for (int i = 0; i < size; i++) {
					read.seekg(curson, ios::cur);
					getline(read, key);
					V[i] = key;
					curson = (SizeofData + SizeofPointers);
				}
				read.close();
				return;
			} else {
				cout << "file is not open" << endl;
				return;
			}
		}
		/*read datafile, go to sepacific lines
		 * containing Pointers and return
		 * keys of vector
		 * */
		string GetData(int index) {
			string V;
			if (this->DataFile == "\0") {	//Node is empty
				return V;
			}
			ifstream read(this->DataFile.c_str());
			if (read.is_open()) {			//file open
				read.seekg(SizeoffirstLine + SizeofPointers, ios::beg);
				getline(read, V);
				for (int i = 0; i < index; i++) {
					read.seekg(SizeofData + SizeofPointers, ios::cur);
					getline(read, V);
				}
				getline(read, V);
				return V;				//to do return all keys
			} else
				return V;				//file not open correctly
		}

		string GetChild(int index) {
			string V;
			if (this->DataFile == "\0") {	//Node is empty
				return V;
			}
			ifstream read(this->DataFile.c_str());
			if (read.is_open()) {			//file open
				read.seekg(SizeoffirstLine, ios::beg);
				getline(read, V);
				for (int i = 0; i < index; i++) {
					read.seekg(SizeofData + SizeofKey, ios::cur);
					getline(read, V);
				}
				return V;				//to do return all keys
			} else
				return V;				//file not open correctly
		}
		string getNkeys() {
			string str;
			ifstream read(this->DataFile.c_str(), ios::in);
			getline(read, str);
			read.close();
			return str;
		}
	};
	int Degree;
	int NonRootNodes;
	string TempFileName;	//to give the name to files
	BTNode* root;
	BTNode* Parent;		//parent node is empty as root has no parent

public:
	BT(int _Degree, string _KeyType) {
		this->Degree = _Degree;
		this->NonRootNodes = 0;
		this->TempFileName = "A0000.txt";
		this->root = new BTNode;
		this->Parent = new BTNode;
		if (_KeyType == "Cause Name") {
			this->readfile(3);
		} else if (_KeyType == "State") {
			this->readfile(4);
		}
	}
	string Search(string _Key) {
		string line =  GetDataRec(_Key, root);	//read line from file
		string filename;
		string linenum;
		int w = 0;
		for(int i = 0;line[i] != '\0'; i++){
			if(line[i] == ' ')
				w++;
			if(w == 1){			//get filename from that line
				filename += line[i];
			}
			else if(w == 2){	//get line number
				linenum += line[i];
			}
		}

		int lineNum = strToint(linenum);
		ifstream read(filename.c_str(),ios::in);	//open the main file
		getline(read,line);							//ignore the first line
		read.seekg(lineNum * sizeofmaindata,ios::beg);		//go to line number
		getline(read,line);
		return line;
	}
private:

	/*search the data*/
	string GetDataRec(string _Key, BTNode* _Root) {
		string strKey = _Root->getNkeys();
		string filename = _Root->DataFile;
		const int __N = strToint(strKey);
		string Obj[__N];
		//get the pointer of keys
		_Root->Getobject(Obj, __N);
		int i;
		string keytocomp;
		string child;
		for (i = 0; i < __N; i++) {
			keytocomp = strToint(Obj[i]);
			if (_Key < keytocomp) {	//child to enter
				child = _Root->GetChild(i);	//get pointer of child
				if (child == NoChild || child == "\0") {//mean no child present
					cout << "Data not found" << endl;
					return "\0";
				} else {								//enter to child
					//enter to that child
					BTNode* temp = new BTNode;
					temp->DataFile = child;
					return GetDataRec(_Key, temp);
				}
			} else if (_Key == keytocomp) {
				return _Root->GetData(i);
			}
		}
		if (i == __N) {	//to fild key is greater then all of keys in present node
			child = _Root->GetChild(i);
			if (child == NoChild || child == "\0") {	//mean no child present
				cout << "Data not found" << endl;
				return "\0";
			} else {								//enter to child
				//enter to that child
				BTNode* temp = new BTNode;
				temp->DataFile = child;
				return GetDataRec(_Key, temp);
			}
		}
		return "\0";
	}
	/*create the file
	 * */
	void insert(string _Key, BTNode* root, string _MainDataFileName,
			int _LineNum, BTNode* _ParentNode, int _IndexOfParent = 0) {
		string nodefile = root->DataFile;
		if (nodefile == "\0") {		//root is empty
			cout << "new key " << _Key << " is inserting" << endl;
			nodefile = this->TempFileName;	//create the file of tempFileName
			incFileName(this->TempFileName);	//increment in tempFilename
			root->DataFile = nodefile;
			//open file
			ofstream write(nodefile.c_str());
			int nKey = 1;
			//first line store the number of keys
			write << nKey << "  " << endl << true << endl;
			write << NoChild << endl;

			//set size of key by subtracting after key insertion to before key insertion
			unsigned int start = write.tellp();
			write << _Key << endl;
			SizeofKey = write.tellp() - start;
			write << ndub << " " << _MainDataFileName << " " << _LineNum << endl
					<< NoChild << endl;
		} else {					//root is non-empty
			int nKey;
			string key;
			//string childfile;
			ifstream read(nodefile.c_str());
			read >> nKey;
			if (nKey >= Degree)
				nKey = 0;
			cout << nKey << endl;
			string line;
			read.seekg(SizeoffirstLine + SizeofPointers, ios::beg);	//ignore the remaining part of line 01 and first pointer
			cout << "Line = " << line << endl;
			for (int i = 0; i < nKey; i++) {
				read >> line;	//read key as a string
				key = line;
				cout << "key = " << key << endl;
				if (key < _Key) {
					cout << "i = " << i << endl;
				}
				read >> line;
				read >> line;
			}
			read.close();
			const int __N = nKey;
			string Obj[__N];
			//get the pointer of keys
			root->Getobject(Obj, nKey);
			//check the appropriate position where to insert the key or go to children
			if (nKey < this->Degree - 1) {		//new key is inserted
				incKey(nodefile, nKey);	//increment the number of keys in node file
				ofstream write(nodefile.c_str(), ios::app);
				write << _Key << endl << ndub << " " << _MainDataFileName << " "
						<< _LineNum << endl;
				write << NoChild << endl;
				write.close();
			} else if (nKey == (this->Degree - 1)) {	//node is full
				cout << "root->Is_Leaf() " << root->Is_Leaf() << endl;
				cout << "splite child node" << endl;
				spliteNode(root, _ParentNode, _IndexOfParent, Obj);
			}
		}
	}
	/*get file name, parent file name and data of node */
	void spliteNode(BTNode* _CurNode, BTNode* _ParentNode, int _IndexOfParent,
			string* Obj) {

		if (_ParentNode->DataFile == "\0") {
			cout << "new file is going to create" << endl;
			string temp = "Temp.txt";

			_ParentNode->DataFile = TempFileName;
			ofstream writeParent(TempFileName.c_str());
			writeParent << "0  " << endl;
			writeParent << "0" << endl;

			//change the file name for next use
			incFileName(this->TempFileName);

			ofstream writeChild1("temp.txt");
			writeChild1 << "0   " << endl;
			writeChild1 << "1" << endl;
			string newchild = TempFileName;
			ofstream writeChild2(newchild.c_str());
			writeChild2 << "0   " << endl;
			writeChild2 << "1" << endl;
			//change the file name for next use
			incFileName(this->TempFileName);

			ifstream readNode(_CurNode->DataFile.c_str());
			int childkey = (this->Degree / 2) + 1;
			cout << "cjil  " << childkey << endl;
			string line;
			readNode.seekg(SizeoffirstLine, ios::beg);//no of keys and is leaf ignored
			int filenum = 1;
			while (!readNode.eof()) {
				if (filenum < childkey) {	//first child is going to write
					getline(readNode, line);	//less then key Ptr
					writeChild1 << line << endl;
					getline(readNode, line);	//key
					writeChild1 << line << endl;
					getline(readNode, line);	//data of key
					writeChild1 << line << endl;
					if (filenum == childkey - 1) {
						getline(readNode, line);	//greater then key ptr
						writeChild1 << line << endl;
					}
				} else if (filenum == childkey) { // Parent file writing
					writeParent << _CurNode->DataFile << endl; //write first child
					getline(readNode, line);	// key
					writeParent << line << endl;
					getline(readNode, line);	//data of key
					writeParent << line << endl;
					writeParent << newchild << endl;
				} else {						//second child is going to write
					getline(readNode, line);	//less then to key ptr
					writeChild2 << line << endl;
					getline(readNode, line);	//key
					writeChild2 << line << endl;
					getline(readNode, line);	//data of key
					writeChild2 << line << endl;
					getline(readNode, line);	//greater then to key ptr
					writeChild2 << line << endl;
				}
				filenum++;
			}
			readNode.close();
			writeChild1.close();
			writeChild2.close();
			writeParent.close();
			incKey(_ParentNode->DataFile, 0);
			incKey("temp.txt", childkey - 2);
			incKey(newchild, Degree - (childkey + 2));

			remove(_CurNode->DataFile.c_str());
			rename("temp.txt", _CurNode->DataFile.c_str());
			_CurNode->DataFile = _ParentNode->DataFile;
			_ParentNode->DataFile = "\0";
		}
	}
	void Update(string _Key, int _LineNumber) {
		string data;
		data = Search(_Key);
		if(data != "\0"){

		}
	}
	/*take file name in char+index format as argument
	 * increment in it index
	 * if index reached to 9999 then it increment the character
	 * A9999 become B0000*/
	void incFileName(string& _FileName) {
		if (_FileName[4] != '9') {
			_FileName[4]++;
		} else {
			for (int i = 4; i >= 1; i--) {
				if (_FileName[i] == '9') {
					_FileName[i] = '0';
					if (_FileName[i - 1] != '9') {
						_FileName[i - 1]++;
						break;			//to save extra iterations
					}
				}

			}
		}
	}
	/*take file and old number of keys
	 * write incrmented key on file*/
	void incKey(string NodeFile, int nKey) {
		ofstream write(NodeFile.c_str(), ios::in);
		nKey++;
		write << nKey;
		write.close();
	}
	void readfile(int _ColNum) {

		ifstream readMainData;
		string filename;
		string line;
		string word;

		int linenum;
		//string datafile = root->DataFile;
		int nfile = 0;

		filename = "test_1.csv";
		//for (int i = 1; i <= 9; i++) {	//to read 9 files
		//filename[46] = i + 48;
		readMainData.open(filename.c_str());
		linenum = 1;
		if (readMainData.is_open()) {		//check the file is open
			getline(readMainData, line);	//first line has to be ignored
			while (!readMainData.eof()) {	//read to end of file
				getline(readMainData, line);	//read line by line
				int ColNum = 0;
				word = "\0";
				for (int j = 0; line[j] != '\0' && line[j] != '\n'; j++) {//tokenize the key from line
					if (line[j] == ',')
						ColNum++;
					else if (ColNum == _ColNum) {
						word += line[j];
					} else if (ColNum == ColNum + 1)
						break;
				}	//end for
				string k = word;

				insert(k, root, filename, linenum, Parent);	//store key and it data
				linenum++;
			}	//while eof
		} else {
			cout << filename << " is not open" << endl;
		}
		readMainData.close();
		//}
	}
	/*take string as a argument and convert it to DT
	 * if string has int value it will convert it into int
	 * or if it has dot it convert it into float of single decimal
	 * */

	int strToint(string str) {
		for (int i = 0; i < str.length(); i++) {
			if (str[i] == ' ')
				str.erase(i);
		}
		int d = 0;
		int len = str.length();
		for (int i = 0; i < len; i++) {
			int digit = str[i] - 48;
			digit *= (pow(double(10), double(len - i - 1)));
			d += digit;
		}

		return d;
	}

};

