#pragma once
#include<iostream>
using namespace std;

class MyString {
private:
	char* chArr;
	int size;


	char lower(const char ch)const {
		if (ch >= 'A' && ch <= 'Z') {
			return ch + 32;
		}
		return ch;
	}

	char Upper(const char ch)const {
		if (ch >= 'a' && ch <= 'z') {
			return ch - 32;
		}
		return ch;
	}
	bool isDelim(char ch, const char* delim) const {
		for (int i = 0; delim[i] != '\0'; i++) {
			if (ch == delim[i]) return true;
		}
		return false;
	}




public:
	//Constructors
	MyString() :size(0) {
		this->chArr = new char[size + 1];
		this->chArr[0] = '\0';
	}
	MyString(const char ch) :size(1) {
		this->chArr = new char[size + 1];
		this->chArr[0] = ch;
		this->chArr[1] = '\0';
	}
	MyString(const char* ch) {
		int s = 0;
		for (int i = 0; ch[i] != '\0'; i++, s++) {}
		this->size = s;
		this->chArr = new char[this->size + 1];
		for (int i = 0; i < this->size; i++) {
			this->chArr[i] = ch[i];
		}
		this->chArr[this->size] = '\0';
	}
	MyString(const char ch, int s) :size(s) {
		this->chArr = new char[this->size + 1];
		for (int i = 0; i < this->size; i++) {
			this->chArr[i] = ch;
		}
		this->chArr[this->size] = '\0';
	}
	MyString(int num) {
		int dig = 0;
		int temp = num;
		while (temp > 0) {
			temp /= 10;
			dig++;
		}
		this->size = dig;
		this->chArr = new char[this->size + 1];
		for (int i = this->size - 1; i >= 0; i--) {
			this->chArr[i] = char(num % 10) + '0';
			num /= 10;
		}
		this->chArr[this->size] = '\0';
	}
	MyString(const MyString& other) {
		this->size = other.size;
		this->chArr = new char[this->size + 1];
		for (int i = 0; i <= this->size; i++) {
			this->chArr[i] = other.chArr[i];
		}
	}
	//Destructor
	~MyString() {
		delete[] this->chArr;
	}
	//Assignment Operator
	MyString& operator=(const MyString& other) {
		if (this == &other) {
			return *this;
		}
		delete[] this->chArr;
		this->size = other.size;
		this->chArr = new char[this->size + 1];
		for (int i = 0; i <= this->size; i++) {
			this->chArr[i] = other.chArr[i];
		}
		return *this;
	}
	//Member
	friend ostream& operator<<(ostream& out,const MyString& other) {
		cout << " " <<other.chArr << " ";
		return out;
	}
	int Length() const {
		int i = 0;
		for (; this->chArr[i] != '\0'; i++) {}
		return i;
	}
	MyString concat_no_space(const MyString& s2) const {
		int newSize = this->size + s2.size;
		char* newArr = new char[newSize + 1];
		for (int i = 0; i < this->size; i++)
			newArr[i] = this->chArr[i];
		for (int i = 0; i < s2.size; i++)
			newArr[this->size + i] = s2.chArr[i];
		newArr[newSize] = '\0';
		MyString result;
		delete[] result.chArr;
		result.chArr = newArr;
		result.size = newSize;
		return result;
	}
	int stoi() {
		int num = 0;
		for (int i = 0; i < this->size; i++) {
			num *= 10;
			num += int(this->chArr[i]) - 48;
		}
		return num;
	}
	void ReplaceFirst(char ch) {
		this->chArr[0] = ch;
	}
	MyString itos(int num) {
		int temp = num, dig = 0;
		while (temp > 0) {
			temp /= 10;
			dig++;
		}
		delete[] this->chArr;
		this->size = dig;
		this->chArr = new char[this->size + 1];
		for (int i = this->size - 1; i >= 0; i--) {
			this->chArr[i] = char(num % 10) + '0';
			num /= 10;
		}
		this->chArr[this->size] = '\0';
		return *this;
	}
	MyString& trim() {
		int start = 0;
		while (start < this->size) {
			if (this->chArr[start] == ' ' ||
				this->chArr[start] == '\t' ||
				this->chArr[start] == '\n') {
				start++;
			}
			else {
				break;
			}
		}
		int end = this->size - 1;
		while (end >= start) {
			if (this->chArr[end] == ' ' ||
				this->chArr[end] == '\t' ||
				this->chArr[end] == '\n') {
				end--;
			}
			else {
				break;
			}
		}
		int newSize = 0;
		if (end >= start) {
			newSize = end - start + 1;
		}
		char* newArr = new char[newSize + 1];

		for (int i = 0; i < newSize; i++) {
			newArr[i] = this->chArr[start + i];
		}
		newArr[newSize] = '\0';
		delete[] this->chArr;
		this->chArr = newArr;
		this->size = newSize;
		return *this;
	}

	char operator[](int i) const {
		if (i >= this->size) {
			cout << "Out of Bounds!\n";
			return '\0';
		}
		return this->chArr[i];
	}
	char& operator[](int i) {
		if (i >= this->size) {
			cout << "Out of Bounds!\n";
			return this->chArr[this->size];
		}
		return this->chArr[i];
	}
	//Comparison
	bool operator==(const MyString& rhs)const {
		int len1 = this->size;
		int len2 = rhs.size;
		if (len1 != len2) {
			return false;
		}
		for (int i = 0; i < len1; i++) {
			if (this->chArr[i] != rhs.chArr[i]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const MyString& rhs)const {
		return !(*this == rhs);
	}
	bool operator<(const MyString& rhs) const {
		int len1 = this->size;
		int len2 = rhs.size;
		int minLen = len2;
		if (len1 < len2) {
			minLen = len1;
		}
		for (int i = 0; i < minLen; i++) {
			char a = lower(this->chArr[i]);
			char b = lower(rhs.chArr[i]);
			if (a < b) return true;
			if (a > b) return false;
		}
		return len1 < len2;
	}
	bool operator>(const MyString& rhs)const {
		int len1 = this->size;
		int len2 = rhs.size;
		int minLen = len2;
		if (len1 < len2) {
			minLen = len1;
		}
		for (int i = 0; i < minLen; i++) {
			char a = lower(this->chArr[i]);
			char b = lower(rhs.chArr[i]);
			if (a > b) return true;
			if (a < b) return false;
		}
		return len1 > len2;
	}
	MyString operator+(const MyString& s2) const {
		MyString result;
		result.size = this->size + s2.size;  
		result.chArr = new char[result.size + 1];
		for (int i = 0; i < this->size; i++)
			result.chArr[i] = this->chArr[i];
		for (int i = 0; i < s2.size; i++)
			result.chArr[this->size + i] = s2.chArr[i];  
		result.chArr[result.size] = '\0';
		return result;
	}
	MyString concat(const MyString& s2) const {
		return *this + s2;
	}
	MyString& operator+=(const MyString& s2) {
		int oldSize = this->size;
		int newSize = oldSize + s2.size;  
		char* newArr = new char[newSize + 1];
		for (int i = 0; i < oldSize; i++)
			newArr[i] = this->chArr[i];
		for (int i = 0; i < s2.size; i++)
			newArr[oldSize + i] = s2.chArr[i];  
		newArr[newSize] = '\0';
		delete[] this->chArr;
		this->chArr = newArr;
		this->size = newSize;
		return *this;
	}
	void append(const MyString& s2) {
		*this += s2;
	}
	int find_first(char ch) {
		for (int i = 0; i < this->size; i++) {
			if (ch == this->chArr[i]) {
				return i;
			}
		}
		return -1;
	}
	int find_first(const MyString& other) const {
		if (other.size == 0) {
			return 0;
		}
		if (this->size < other.size) {
			return -1;
		}

		for (int i = 0; i <= this->size - other.size; i++) {
			bool matchFound = true;
			for (int j = 0; j < other.size; j++) {
				if (this->chArr[i + j] != other.chArr[j]) {
					matchFound = false;
					break;
				}
			}
			if (matchFound) {
				return i;
			}
		}
		return -1;
	}
	int find_last(char ch) {
		for (int i = this->size - 1; i >= 0; i--) {
			if (ch == this->chArr[i]) {
				return i;
			}
		}
		return -1;
	}
	int find_last(const MyString& other) const {
		if (other.size == 0) {
			return 0;
		}
		if (this->size < other.size) {
			return -1;
		}

		for (int i = this->size - other.size; i >= 0; i--) {
			bool matchFound = true;
			for (int j = other.size - 1; j >= 0; j--) {
				if (this->chArr[i + j] != other.chArr[j]) {
					matchFound = false;
					break;
				}
			}
			if (matchFound) {
				return i;
			}
		}
		return -1;
	}


	void insert(int pos, const char ch) {
		int len = this->size;
		char* temp = new char[len + 2] {};
		for (int i = 0; i < pos; i++)
			temp[i] = this->chArr[i];
		temp[pos] = ch;
		for (int i = pos + 1; i <= len; i++)
			temp[i] = this->chArr[i - 1];
		delete[] this->chArr;
		this->chArr = temp;
		this->size++;
		this->chArr[this->size] = '\0';
		return;
	}

	void insert(int pos, const MyString sub) {
		int len1 = this->size;
		int len2 = sub.size;
		int newLen = len1 + len2;

		char* temp = new char[newLen + 1];

		for (int i = 0; i < pos; i++) {
			temp[i] = this->chArr[i];
		}
		for (int i = 0; i < len2; i++) {
			temp[pos + i] = sub.chArr[i];
		}
		for (int i = pos; i < len1; i++) {
			temp[i + len2] = this->chArr[i];
		}
		temp[newLen] = '\0';

		delete[] this->chArr;
		this->chArr = temp;
		this->size = newLen;
	}
	void remove_at(int pos) {
		int len = this->size;
		this->size--;
		char* temp = new char[len] {};
		for (int i = 0; i < pos; i++)
			temp[i] = this->chArr[i];
		for (int i = pos; i < len - 1; i++)
			temp[i] = this->chArr[i + 1];
		temp[this->size - 1] = '\0';
		delete[] this->chArr;
		this->chArr = temp;
		return;
	}
	void remove_first(char ch) {
		int pos = this->find_first(ch);
		this->remove_at(pos);
	}
	void remove_last(char ch) {
		int pos = this->find_last(ch);
		this->remove_at(pos);
	}
	void remove_all(char ch) {
		int pos = this->find_first(ch);
		while (pos != -1) {
			this->remove_at(pos);
			pos = this->find_first(ch);
		}
	}

	MyString* Split(char delim, int& count) const {
		count = 0;
		bool inWord = false;

		for (int i = 0; i < size; i++) {
			if (this->chArr[i] != delim && !inWord) {
				inWord = true;
				count++;
			}
			else if (this->chArr[i] == delim) {
				inWord = false;
			}
		}
		if (count == 0) {
			return nullptr;
		}
		MyString* parts = new MyString[count];


		int partIndex = 0;
		int i = 0;

		while (i < size) {
			while (i < size && this->chArr[i] == delim) {
				i++;
			}

			int start = i;
			while (i < size && this->chArr[i] != delim) {
				i++;
			}
			int end = i;

			if (start < end) {
				int wordLen = end - start;
				char* temp = new char[wordLen + 1];
				for (int j = 0; j < wordLen; j++) {
					temp[j] = this->chArr[start + j];
				}
				temp[wordLen] = '\0';
				parts[partIndex] = MyString(temp);
				delete[] temp;
				partIndex++;
			}
		}

		return parts;
	}
	MyString* tokenize(const char* delim, int& count) const {
		count = 0;
		bool inWord = false;
		for (int i = 0; i < this->size; i++) {
			if (!isDelim(this->chArr[i], delim) && !inWord) {
				inWord = true;
				count++;
			}
			else if (isDelim(this->chArr[i], delim)) {
				inWord = false;
			}
		}

		if (count == 0) return nullptr;
		MyString* parts = new MyString[count];
		int partIndex = 0;
		int i = 0;

		while (i < this->size) {
			while (i < this->size && isDelim(this->chArr[i], delim)) {
				i++;
			}
			int start = i;
			while (i < this->size && !isDelim(this->chArr[i], delim)) {
				i++;
			}
			int end = i;
			if (start < end) {
				int wordLen = end - start;
				char* temp = new char[wordLen + 1];
				for (int j = 0; j < wordLen; j++) {
					temp[j] = this->chArr[start + j];
				}
				temp[wordLen] = '\0';
				parts[partIndex] = MyString(temp);
				delete[] temp;
				partIndex++;
			}
		}

		return parts;
	}
	int* AllSubStrings(const char* ch, int& count)const {
		count = 0;
		bool inWord = false;
		for (int i = 0; i < this->size; i++) {
			if (!isDelim(this->chArr[i], ch) && !inWord) {
				inWord = true;
				count++;
			}
			else if (isDelim(this->chArr[i], ch)) {
				inWord = false;
			}
		}
		if (count == 0) return nullptr;
		int* res = new int[count];
		int partIndex = 0;
		int i = 0;
		while (i < this->size) {
			while (i < this->size && isDelim(this->chArr[i], ch)) {
				i++;
			}
			int start = i;
			while (i < this->size && !isDelim(this->chArr[i], ch)) {
				i++;
			}

			if (start < i) {
				res[partIndex] = start;
				partIndex++;
			}

		}
		return res;
	}
	int* find_all(const MyString& other, int& count) const {
		count = 0;
		if (other.size == 0 || this->size < other.size) {
			return nullptr;
		}
		for (int i = 0; i <= this->size - other.size; i++) {
			bool match = true;
			for (int j = 0; j < other.size; j++) {
				if (this->chArr[i + j] != other.chArr[j]) {
					match = false;
					break;
				}
			}
			if (match) {
				count++;
			}
		}

		if (count == 0) return nullptr;
		int* res = new int[count];
		int partIndex = 0;

		for (int i = 0; i <= this->size - other.size; i++) {

			bool match = true;
			for (int j = 0; j < other.size; j++) {
				if (this->chArr[i + j] != other.chArr[j]) {
					match = false;
					break;
				}
			}
			if (match) {
				res[partIndex] = i;
				partIndex++;
			}
		}

		return res;
	}

	int* find_all(char ch, int& count)const {
		count = 0;
		for (int i = 0; i < this->size; i++) {
			if (ch == this->chArr[i]) {
				count++;
			}
		}

		if (count == 0) return nullptr;
		int* res = new int[count];
		int partIndex = 0;

		for (int i = 0; i < this->size; i++) {
			if (ch == this->chArr[i]) {
				res[partIndex] = i;
				partIndex++;
			}
		}
		return res;
	}
	void clear() {
		delete[] this->chArr;
		this->size = 0;
		this->chArr = new char[size + 1];
		this->chArr[0] = '\0';
	}

};

