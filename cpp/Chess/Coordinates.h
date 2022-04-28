#pragma once
#include <cmath>
#include <string>
#include <array>
#include <map>

struct Coordinates
{
	Coordinates() {
		this->row = -1;
		this->column = -1;
	}
	Coordinates(std::string str) {
		std::array<char, 2> pos {str.c_str()[0], str.c_str()[1]};
		std::map<char, int> columnMap{ {'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5}, {'g', 6}, {'h', 7} };
		std::map<char, int> rowMap{ {'8', 0}, {'7', 1}, {'6', 2}, {'5', 3}, {'4', 4}, {'3', 5}, {'2', 6}, {'1', 7} };
		this->row = rowMap[pos[1]];
		this->column = columnMap[pos[0]];
	}
	Coordinates(const Coordinates &other) {
		this->row = other.row;
		this->column = other.column;
	}
	inline Coordinates(int row, int column) {
		this->row = row;
		this->column = column;
	}
	bool operator=(const Coordinates& other) {
		return Coordinates(other);
	}
	void operator+=(const Coordinates& other) {
		this->Update(other);
	}
	bool operator==(const Coordinates& other) {
		return this->row == other.row && this->column == other.column;
	}
	bool operator!=(const Coordinates& other) {
		return this->row != other.row || this->column != other.column;
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
	std::string ToString() const{
		std::map<int, std::string> columnMap{ {-1, "-"}, {0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {4, "e"}, {5, "f"}, {6, "g"}, {7, "h"} };
		std::map<int, std::string> rowMap{ {-1, "-"}, {0, "8"}, {1, "7"}, {2, "6"}, {3, "5"}, {4, "4"}, {5, "3"}, {6, "2"}, {7, "1"} };
		return columnMap[this->column] + rowMap[this->row];
	}
	static std::string ToString(const Coordinates &coordinates) {
		std::map<int, std::string> columnMap{ {-1, "-"}, {0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {4, "e"}, {5, "f"}, {6, "g"}, {7, "h"} };
		std::map<int, std::string> rowMap{ {-1, "-"}, {0, "8"}, {1, "7"}, {2, "6"}, {3, "5"}, {4, "4"}, {5, "3"}, {6, "2"}, {7, "1"} };
		return columnMap[coordinates.column] + rowMap[coordinates.row];
	}
};