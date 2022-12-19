#pragma once

#include <stdlib.h>

class Config final {

public:
	int load(std::string fileName) {

		// open config file
		std::ifstream file(fileName);

		if (file.is_open()) {

			// assume csv structure: name, value
			for (std::string line; std::getline(file, line, '\n');) {
				// ignore comments
				if (line.front() == toIgnore) {
					continue;
				}

				std::string name, value;
				std::stringstream ss(line);

				// get the name of the value
				std::getline(ss, name, ',');
				// get the value
				std::getline(ss, value, '\n');

				db.emplace(std::pair<std::string, std::string>{name, value});

				ss.clear();
			}

			// success
			return 1;
		}
		else {
			return 0;
		}
	}

	

	std::string getString(std::string name) {
		if (db.find(name) != db.end()) {
			return db[name];
		}
		else {
			std::cerr << "Could not find string " << name << std::endl;
			return "";
		}
	}

	int getInt(std::string name) {
		if (db.find(name) != db.end()) {
			return stoi(db[name]);
		}
		else {
			std::cerr << "Could not find int " << name << std::endl;
			return 0;
		}
	}

	float getFloat(std::string name) {
		if (db.find(name) != db.end()) {
			return stof(db[name]);
		}
		else {
			std::cerr << "Could not find float " << name << std::endl;
			return 0;
		}
	}

	void ignore(char name) {
		toIgnore = name;
	}

private:
	std::map<std::string, std::string> db;
	char toIgnore;
};
