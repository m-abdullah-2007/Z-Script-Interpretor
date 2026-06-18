#pragma once
#include<iostream>
#include<string>
using namespace std;

template <typename T>
class myVector {
	int _size;
	int _cap;
	T* ptr;
public:
	//Constructor
	myVector() :_size(0), _cap(0), ptr(nullptr) {}
	myVector(T* vc, int size) :_size(size), _cap(size * 2) {
		this->ptr = new T[_cap];
		for (int i = 0; i < this->_size; i++) {
			this->ptr[i] = vc[i];
		}
	}
	myVector(const myVector& other) :_size(other._size), _cap(other._cap) {
		this->ptr = new T[_cap];
		for (int i = 0; i < this->_size; i++) {
			this->ptr[i] = other.ptr[i];
		}
	}
	//Destructor
	~myVector() {
		delete[] this->ptr;
	}
	//Assignment Operator
	myVector& operator=(const myVector& other) {
		if (this == &other) {
			return *this;
		}
		delete[] this->ptr;
		this->_size = other._size;
		this->_cap = other._cap;
		this->ptr = new T[_cap];
		for (int i = 0; i < this->_size; i++) {
			this->ptr[i] = other.ptr[i];
		}
		return *this;
	}

	T& operator[](int pos) {
		static T emp{};
		try {
			if (pos < 0 || pos >= _size)
				throw string("index is out of range");

			return this->ptr[pos];
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
			emp = T{};
			return emp;
		}
	}

	// Read-only index access.
	// Throws if the index is negative or out of range.
	const T& operator[](int pos) const {
		static T emp{};
		try {
			if (pos < 0 || pos >= _size)
				throw string("index " + std::to_string(pos) + " is out of range, vector size is " + std::to_string(_size));

			return this->ptr[pos];
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
			emp = T{};
			return emp;
		}
	}

	// Element Access

	// Returns a reference to the element at the given position.
	// Throws if the index is negative or out of range.
	T& at(int pos) {
		static T emp{};
		try {
			if (pos < 0 || pos >= _size)
				throw string("index " + std::to_string(pos) + " is out of range, vector size is " + std::to_string(_size));

			return this->ptr[pos];
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
			emp = T{};
			return emp;
		}
	}

	// Returns a reference to the first element.
	// Throws if the vector is empty.
	T& front() {
		static T emp{};
		try {
			if (this->empty())
				throw string("cannot get front element because the vector is empty");

			return this->ptr[0];
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
			emp = T{};
			return emp;
		}
	}

	// Returns a reference to the last element.
	// Throws if the vector is empty.
	T& back() {
		static T emp{};
		try {
			if (this->empty())
				throw string("cannot get back element because the vector is empty");

			return this->ptr[this->_size - 1];
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
			emp = T{};
			return emp;
		}
	}

	//Capacity
	bool empty() const {
		if (this->_size == 0) {
			return true;
		}
		return false;
	}
	int size() const {
		return this->_size;
	}
	int capacity() const {
		return this->_cap;
	}
	void reserve(int new_cap) {
		if (this->_cap >= new_cap) {
			return;
		}
		T* temp = nullptr;
		temp = new T[new_cap];
		for (int i = 0; i < this->_size; i++) {
			temp[i] = this->ptr[i];
		}
		delete[] this->ptr;
		this->ptr = temp;
		this->_cap = new_cap;
	}
	void shrink_to_fit() {
		T* temp = nullptr;
		temp = new T[this->_size];
		for (int i = 0; i < this->_size; i++) {
			temp[i] = this->ptr[i];
		}
		delete[] this->ptr;
		this->ptr = temp;
		this->_cap = this->_size;
		return;
	}
	//Modifiers
	void clear() {
		T* temp = nullptr;
		temp = new T[this->_cap]{};
		delete[] this->ptr;
		this->ptr = temp;
		this->_size = 0;
		return;
	}
	void insert_at_last(const T value) {
		if (this->_size + 1 >= this->_cap) {
			if (_cap == 0) {
				reserve(2);
			}
			else {
				reserve(2 * this->_cap);
			}
		}
		this->ptr[_size++] = value;
		return;
	}
	void insert_at_first(const T value) {
		if (this->_size + 1 >= this->_cap) {
			if (_cap == 0) {
				reserve(2);
			}
			else {
				reserve(2 * this->_cap);
			}
		}
		for (int i = this->_size; i >= 1; i--) {
			this->ptr[i] = this->ptr[i - 1];
		}
		this->ptr[0] = value;
		this->_size++;
		return;
	}

	// Inserts a value at the given position.
	// Throws if the position is negative or greater than the current size.
	void insert_at_pos(const T value, int pos) {
		try {
			if (pos < 0 || pos > this->_size)
				throw string("insert position " + std::to_string(pos) + " is out of range, valid range is 0 to " + std::to_string(this->_size));

			if (this->_size + 1 >= this->_cap) {
				if (_cap == 0) {
					reserve(2);
				}
				else {
					reserve(2 * this->_cap);
				}
			}
			for (int i = this->_size; i >= pos + 1; i--) {
				this->ptr[i] = this->ptr[i - 1];
			}
			this->ptr[pos] = value;
			this->_size++;
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
		}
	}

	// Inserts a range of values starting at the given position.
	// Throws if the starting position is negative or greater than the current size.
	void insert_range(T* rng, int count, int pos) {
		try {
			if (pos < 0 || pos > this->_size)
				throw string("insert range start position " + std::to_string(pos) + " is out of range, valid range is 0 to " + std::to_string(this->_size));

			if (this->_size + count >= this->_cap - 1) {
				reserve(2 * (this->_cap + count));
			}
			for (int ct = 0; ct < count; ct++) {
				insert_at_pos(rng[ct], pos++);
			}
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
		}
	}

	void remove_at_last() {
		if (this->_size == 0) return;
		this->ptr[this->_size - 1] = T{};
		this->_size--;
		return;
	}
	void remove_at_first() {
		if (this->_size == 0) return;
		for (int i = 0; i < this->_size - 1; i++) {
			this->ptr[i] = this->ptr[i + 1];
		}
		this->ptr[this->_size - 1] = T{};
		this->_size--;
		return;
	}

	// Removes the element at the given position.
	// Throws if the vector is empty or the position is out of range.
	void remove_at_pos(int pos) {
		try {
			if (this->_size == 0)
				throw string("cannot remove from an empty vector");

			if (pos < 0 || pos >= this->_size)
				throw string("remove position " + std::to_string(pos) + " is out of range, valid range is 0 to " + std::to_string(this->_size - 1));

			for (int i = pos; i < this->_size - 1; i++) {
				this->ptr[i] = this->ptr[i + 1];
			}
			this->ptr[this->_size - 1] = T{};
			this->_size--;
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
		}
	}

	// Removes a range of elements starting at the given position.
	// Throws if the starting position is out of range or the count goes beyond the vector.
	void remove_range(int pos, int t) {
		try {
			if (pos < 0 || pos >= this->_size)
				throw string("remove range start position " + std::to_string(pos) + " is out of range, valid range is 0 to " + std::to_string(this->_size - 1));

			if (pos + t > this->_size)
				throw string("remove range goes beyond the vector, only " + std::to_string(this->_size - pos) + " elements are available from position " + std::to_string(pos));

			for (int i = 0; i < t; i++) {
				remove_at_pos(pos);
			}
		}
		catch (string& e) {
			cout << "error: " << e << "\n";
		}
	}

	void push_back(const T value) {
		insert_at_last(value);
	}
	void append_range(T* rng, int size) {
		insert_range(rng, size, this->_size);
	}
	void pop_back() {
		remove_at_last();
	}
	void resize(int ct, const T value) {
		for (int i = 0; i < ct; i++) {
			insert_at_last(value);
		}
	}
	void swap(myVector& other) {
		if (this == &other) return;

		int  tempSize = this->_size;
		int  tempCap = this->_cap;
		T* tempPtr = this->ptr;

		this->_size = other._size;
		this->_cap = other._cap;
		this->ptr = other.ptr;

		other._size = tempSize;
		other._cap = tempCap;
		other.ptr = tempPtr;
	}
	//Non Member
	friend bool operator==(const myVector& lhs, const myVector& rhs) {
		if (lhs._size != rhs._size) {
			return false;
		}
		for (int i = 0; i < lhs._size; i++) {
			if (lhs.ptr[i] != rhs.ptr[i]) {
				return false;
			}
		}
		return true;
	}
	friend bool operator!=(const myVector& lhs, const myVector& rhs) {
		return !(lhs == rhs);
	}
	friend bool operator<(const myVector& lhs, const myVector& rhs) {
		int minSize = lhs._size;
		if (rhs._size < minSize) {
			minSize = rhs._size;
		}
		for (int i = 0; i < minSize; i++) {
			if (lhs.ptr[i] < rhs.ptr[i]) {
				return true;
			}
			if (lhs.ptr[i] > rhs.ptr[i]) {
				return false;
			}
		}
		return lhs._size < rhs._size;
	}
	friend bool operator>=(const myVector& lhs, const myVector& rhs) {
		return !(lhs < rhs);
	}
	friend bool operator>(const myVector& lhs, const myVector& rhs) {
		int minSize = lhs._size;
		if (rhs._size < minSize) {
			minSize = rhs._size;
		}
		for (int i = 0; i < minSize; i++) {
			if (lhs.ptr[i] > rhs.ptr[i]) {
				return true;
			}
			if (lhs.ptr[i] < rhs.ptr[i]) {
				return false;
			}
		}
		return lhs._size > rhs._size;
	}
	friend bool operator<=(const myVector& lhs, const myVector& rhs) {
		return !(lhs > rhs);
	}
	//iostream
	friend ostream& operator<<(ostream& out, const myVector& other) {
		for (int i = 0; i < other._size; i++) {
			out << other.ptr[i];
			if (i < other._size - 1) {
				out << ",";
			}
		}
		return out;
	}
	friend istream& operator>>(istream& in, myVector& other) {
		cout << "Enter " << other._size << " elements: ";
		for (int i = 0; i < other._size; i++) {
			in >> other.ptr[i];
		}
		return in;
	}
};

