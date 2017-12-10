#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <exception>
#include <sstream>
#include <stdexcept>

using namespace std;

// Класс дата - отвечает за правильнось формирования даты события
class Date {
public:
	explicit Date() { // конструктор даты по умолчанию
		year = 0;
		month = 0;
		day = 0;
	};
	// Конструктор даты
	explicit Date(const int& year_, const int& month_, const int& day_) {
		year = year_;
		if (month_ < 1 || month_ > 12) { // проверка диапазона месяца
			string string_error;
			string_error = "Month value is invalid: " + to_string(month_);
			throw runtime_error(string_error);   // ловит исключение
		}
		else if (day_ < 1 || day_ > 31) { // проверка диапазона дней
			string string_error;
			string_error = "Day value is invalid: " + to_string(day_);
			throw runtime_error(string_error);   // ловит исключение
		}
		else {		
			month = month_;
			day = day_;
		}
	};
	// Методы вызова данных даты
	int GetYear() const { return year;}
	int GetMonth() const { return month; }
	int GetDay() const { return day; }

private:
	int year;
	int month;
	int day;
};

// перезагрузка оператора < для даты
bool operator < (const Date& lhs, const Date& rhs) {
	if ((lhs.GetYear() == rhs.GetYear()) && (lhs.GetMonth() == rhs.GetMonth())) {
		return lhs.GetDay() < rhs.GetDay();
	}
	else if ((lhs.GetYear() == rhs.GetYear()) && (lhs.GetMonth() != rhs.GetMonth())) {
		return lhs.GetMonth() < rhs.GetMonth();
	}
	else {
		return lhs.GetYear() < rhs.GetYear();
	}
}

// перезагрузка оператора << вывода в поток
ostream& operator << (ostream& stream, const Date& date) {
	stream << setw(4) << setfill('0') << date.GetYear() << '-'
		<< setw(2) << setfill('0') << date.GetMonth() << '-'
		<< setw(2) << setfill('0') << date.GetDay();
	return stream;
}
// функция проверки корректности разделителя '-' значений даты
void EnsureNextSymbolAndSkip(stringstream& stream, const string& data_str) {
	if (stream.peek() != '-') {  // что нужно найти
		stringstream ss;
		ss << "Wrong date format: " + data_str;
		throw runtime_error(ss.str());   // ловит исключение
	}                                    // str() - приводит данные к строке
	stream.ignore(1);
}
// перезагрузка оператора >> ввода из потока
istream& operator >> (istream& stream, Date& date) {
	int year = 10000;
	int month = 10000;
	int day = 10000;

	string data_str; // дата строкой
	string endError = ""; // ловим лишний символ в конце даты
	if (stream >> data_str) {
		stringstream d(data_str); // парсим дату на значения год, мес, день
		d >> year;
		EnsureNextSymbolAndSkip(d, data_str);
		d >> month;
		EnsureNextSymbolAndSkip(d, data_str);
		d >> day;
		d >> endError;
		// проверяем и передаем корректную дату для работы
		if (year > -1 && year < 10000 && month < 10000 && day < 10000 && endError == "") {
			date = Date(year, month, day);
			return stream;
		}
		else {
			// если пользователь ввел не корректную дату выбрасываем исключение
			stringstream string_error;
			string_error << "Wrong date format: " + data_str;
			throw runtime_error(string_error.str());   // ловит исключение
		}
	}
	else {
		// если пользователь не ввел дату выбрасываем исключение
		stringstream string_error;
		string_error << "Wrong date format: " + data_str;
		throw runtime_error(string_error.str());   // ловит исключение
	}

}	
// Класс база данных с парами вида: ключ - дата, значение - событие
class Database {
public:
	// метод добавления события в БД
	void AddEvent(const Date& date, const string& event) {
			DB[date].insert(event);	
	}
	// Удаление события
	bool DeleteEvent(const Date& date, const string& event) {
		if (DB.count(date)) {
			if (DB[date].count(event)) {
				DB[date].erase(event);
				return true;
			}
		}
		return false;
	}
	// Удаление даты вместе с событиями
	int  DeleteDate(const Date& date) {
		if (DB.count(date)) {
			int c = DB[date].size();
			DB.erase(date);
			return c;
		}
		return 0;
	}
	// Поиск события
	void Find(const Date& date) const {
		if (DB.count(date)) {
			for (auto& find : DB.at(date)) {
				cout << find << endl;
			}
		}
	}
	// Вывод всей базы
	void Print() const {
		for (auto& p : DB) {
			for (auto& s : p.second) {
				cout << p.first << ' ' << s << endl;
			}
		}
	}

private:
	map<Date, set<string>> DB;
};
// Парсим команды - даты - события
void ParseCommands(const string& command, Database& db) {
	stringstream stream(command);
	string com = "";
	Date date;
	string event = "";
	// Переключаемся по командам
	if (stream >> com) {
		if (com == "Add") {
			if (stream >> date) {
				if (stream >> event) {
		        	db.AddEvent(date, event);
				}
				else {
					stringstream ss; // Если нет события то исключение
					ss << "Wrong date format: " << date;
					throw runtime_error(ss.str());
				}
			}
		}
		else if (com == "Find") {
			if (stream >> date) {
				db.Find(date);
			}
		}
		else if (com == "Print") {
			db.Print();
		}
		else if (com == "Del") {
			if (stream >> date) {
				if (stream >> event) {
					if (db.DeleteEvent(date, event)) {
						// Удаление события прошло нормально
						cout << "Deleted successfully" << endl;
					}
					else {
						// Удаление события не прошло - нет события
						cout << "Event not found" << endl;
					}				
				}
				else {
					// Удаление даты c выводом ее в собщении
					cout << "Deleted " << db.DeleteDate(date) << " events" << endl;
				}
			}			
		}		
		else {
			// Выбрасываем исключение с сообщением что нет верной команды
			stringstream ss;
			ss << "Unknown command: " << com;
			throw runtime_error(ss.str());
		}
	}
}


int main() {
	Database db;
	string command;

	try { // в try помещаем опасный код	
		while (getline(cin, command)) {
			// Считываем команды с потока ввода и обрабатываем каждую парсером
			ParseCommands(command, db);
		}
	}
	//ловим все исключения и выводим на консоль
	catch (exception& ex) {
		cout << ex.what(); 
	}
	
	return 0;
}