#pragma once
#include <cmath>

struct Coordinates
{
	Coordinates() {
		this->row = -1;
		this->column = -1;
	}
	Coordinates(const Coordinates &other) {
		this->row = other.row;
		this->column = other.column;
	}
	inline Coordinates(int row, int column) {
		this->row = row;
		this->column = column;
	}
	void operator+=(const Coordinates& other) {
		this->Update(other);
	}
	bool operator==(const Coordinates& other) {
		return this->row == other.row && this->column == other.column;
	}
	bool operator<(const Coordinates& other) const {
		return (this->row*8+this->column) < (other.row*8+other.column);
	}
	operator bool()const {
		return (this->row != -1) && (this->column!=-1);
	}
	void Update(const Coordinates& other) {
		this->row += other.row;
		this->column += other.column;
	}
	int row;
	int column;
};