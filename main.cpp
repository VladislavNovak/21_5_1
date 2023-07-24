#include <windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct character {
    string firstName = "unknown";
    string secondName = "unknown";
    string payDay = "unknown";
    int salary = 0;
};

bool isNumeric(std::string const &str) {
    auto it = std::find_if(str.begin(), str.end(), [](const char &c) { return !std::isdigit(c); });

    return (!str.empty() && it == str.end());
}

bool isContainsOnlyLetters(std::string const &str) {
    auto it = std::find_if(str.begin(), str.end(), [](const char &c) { return !std::isalpha(c); });

    return it == str.end();
}

std::string getTrimmedString(std::string str, std::string const &whiteSpaces = " \r\n\t\v\f") {
    auto start = str.find_first_not_of(whiteSpaces);
    str.erase(0, start);
    auto end = str.find_last_not_of(whiteSpaces);
    str.erase(end + 1);

    return str;
}

std::string getUserLineString(const std::string &msg) {
    while (true) {
        std::string userLineString;
        printf("%s: ", msg.c_str());
        std::getline(std::cin, userLineString);

        userLineString = getTrimmedString(userLineString);
        if (userLineString.empty()) {
            std::cout << "Строка не может быть пустой. Попробуйте снова!" << std::endl;
            continue;
        }

        return userLineString;
    }
}

std::vector<std::string> splitStringIntoList(std::string const &source, const char delim = ',', bool isEmptyRemove = true) {
    std::vector<std::string> list;
    std::stringstream ss(source);
    std::string rawString;

    while(std::getline(ss, rawString, delim)) {
        std::string record = getTrimmedString(rawString);

        if (record.empty() && isEmptyRemove) continue;

        list.push_back(record);
    }

    return list;
}

bool isStringADate(std::string const &str, std::string &cause) {
    bool isValid = true;
    std::vector<std::vector<int>> ranges = { { 1, 31 }, { 1, 12 }, { 1920, 2030 } };
    std::vector<std::string> parts = splitStringIntoList(str, '.');

    if (parts.size() != 3) {
        cause += "Формат ввода: ДД.ММ.ГГГГ\n";
        return false;
    }

    for (int i = 0; i < parts.size(); ++i) {
        std::string current = parts[i];

        if (!isNumeric(current)) {
            char warning[100];
            sprintf(warning, "%i часть (%s) не является цифрой\n", (i + 1), current.c_str());
            cause += warning;
            isValid = false;
            continue;
        }

        int part = std::stoi(current);
        auto range = ranges[i];

        if (part < range[0] || part > range[1]) {
            char warning[100];
            sprintf(warning, "%i часть (%i) должна быть в диапазоне %i - %i\n", (i + 1), part, range[0], range[1]);
            cause += warning;
            isValid = false;
        }
    }

    return isValid;
}

std::string getUserWord(std::string const &msg) {
    return splitStringIntoList(getUserLineString((msg)), ' ')[0];
}

character addNewPerson() {
    character person;
    const char* proposes[] = {
            "имя (буквы латиницей)",
            "фамилию (буквы латиницей)",
            "дата (в формате dd.mm.yyyy)",
            "сумму (целое число)" };

    for (int i = 0; i < sizeof(proposes) / sizeof(const char*); ++i) {
        while (true) {
            std::string warning;
            std::string msg = "Введите ";
            auto userInput = getUserWord(msg += proposes[i]);

            if (i == 0 && isContainsOnlyLetters(userInput)) {
                person.firstName = userInput;
                break;
            }
            else if (i == 1 && isContainsOnlyLetters(userInput)) {
                person.secondName = userInput;
                break;
            }
            else if (i == 2 && isStringADate(userInput, warning)) {
                person.payDay = userInput;
                break;
            }
            else if (i == 3 && isNumeric(userInput)) {
                person.salary = stoi(userInput);
                break;
            }

            std::cout << (warning.length() ? warning : "Попробуйте снова") << std::endl;
        }
    }

    return person;
}

// ---

bool readIntoPersonFromBinaryFile(std::ifstream &fileReader, character &person) {
    bool isItemReadSuccessfully = false;
    int bufferSize;

    // Читаем первую сущность. Это - размер для person.name
    fileReader.read((char*) &bufferSize, sizeof(int));
    // Если она существует в принципе, то продолжаем чтение
    if (!fileReader.eof()) {
        person.firstName.resize(bufferSize);
        fileReader.read((char*) person.firstName.c_str(), bufferSize);

        fileReader.read((char*) &bufferSize, sizeof(int));
        person.secondName.resize(bufferSize);
        fileReader.read((char*) person.secondName.c_str(), bufferSize);

        fileReader.read((char*) &bufferSize, sizeof(int));
        person.payDay.resize(bufferSize);
        fileReader.read((char*) person.payDay.c_str(), bufferSize);

        fileReader.read((char*) &person.salary, sizeof(int));

        isItemReadSuccessfully = true;
    }

    return isItemReadSuccessfully;
}

template<typename T>
bool loadIntoArrFromBinaryFile(const char* path, vector<T> &arr) {
    std::ifstream fileReader(path, std::ios::binary);
    bool isFileFound = (fileReader.is_open() && !fileReader.bad());

    if (isFileFound) {
        while(!fileReader.eof()) {
            T item;
            bool isItemReadSuccessfully = readIntoPersonFromBinaryFile(fileReader, item);

            if (isItemReadSuccessfully)
                arr.push_back(item);
        }
    }

    fileReader.close();

    return isFileFound;
}

void savePersonToBinaryFile(const char* path, character const &person, bool isAppMode = false) {
    std::ofstream file(path, std::ios::binary | (isAppMode ? std::ios::app : std::ios::out));

    int firstNameLineSize = (int)person.firstName.length();
    file.write((char*) &firstNameLineSize, sizeof(firstNameLineSize));
    file.write(person.firstName.c_str(), firstNameLineSize);

    int secondNameLineSize = (int)person.secondName.length();
    file.write((char*) &secondNameLineSize, sizeof(secondNameLineSize));
    file.write(person.secondName.c_str(), secondNameLineSize);

    int payDayLineSize = (int)person.payDay.length();
    file.write((char*) &payDayLineSize, sizeof(payDayLineSize));
    file.write(person.payDay.c_str(), payDayLineSize);

    file.write((char*) &person.salary, sizeof(person.salary));

    file.close();
}

void printPersons(vector<character> const &persons) {
    for (auto const &result : persons) {
        std::cout << result.firstName << std::endl;
        std::cout << result.secondName << std::endl;
        std::cout << result.payDay << std::endl;
        std::cout << result.salary << std::endl;
        std::cout << std::endl;
    }

    system("pause");
    system("cls");
}

int getSelectedMenuItem() {
    const char* menu[] = {
            "exit   :чтобы выйти из программы",
            "add    :чтобы добавить новую запись",
            "list   :чтобы вывести ранее созданный список"
    };

    while (true) {
        std::cout << "------------МЕНЮ------------" << std::endl;
        // находим длину массива с указателями на массивы char
        auto sizeOfMenu = std::end(menu) - std::begin(menu);

        for (int i = 0; i < sizeOfMenu; ++i) {
            std::cout << menu[i] << std::endl;
        }

        std::string result = getUserWord("Для дальнейшей работы введите одну из команд");

        for (int i = 0; i < sizeOfMenu; ++i) {
            // получаем из меню одно из ключевых слов (exit, add, list)
            auto firstKeyword = splitStringIntoList(menu[i], ' ')[0];
            if (result == firstKeyword) {
                system("cls");
                return i;
            }
        }

        std::cout << "Введена неверная команда. Попробуйте снова!" << std::endl;
        system("pause");
        system("cls");
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    const char* path = R"(..\test.bin)";

    while (true) {
        string info;
        auto selectedMenuItem = getSelectedMenuItem();

        if (selectedMenuItem == 0) {
            cout << "Программа закончила работу. До новых встреч" << endl;
            break;
        }
        else if (selectedMenuItem == 1) {
            savePersonToBinaryFile(path, addNewPerson(), true);
            info = "Добавлена новая запись.\nДля того, чтобы увидеть все записи в базе данных: введите команду list";
        }
        else if (selectedMenuItem == 2) {
            vector<character> results;
            auto isSuccess = loadIntoArrFromBinaryFile(path, results);

            if (isSuccess) {
                cout << "Полный список сохраненных записей:" << endl;
                printPersons(results);
            }
            else {
                info = "Ошибка чтения. Возможно, файл отсутствует.\nПопробуйте ввести новую запись, набрав команду add";
            }
        }

        if (!info.empty()) {
            cout << info << endl;
            system("pause");
            system("cls");
        }
    }
}
