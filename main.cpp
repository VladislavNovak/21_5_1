#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct character {
    string name = "unknown";
    int salary = 0;
};

bool readIntoPersonFromBinaryFile(std::ifstream &fileReader, character &person) {
    bool isItemReadSuccessfully = false;
    int bufferSize;

    // Читаем первую сущность. Это - размер для person.name
    fileReader.read((char*) &bufferSize, sizeof(int));
    // Если она существует в принципе, то продолжаем чтение
    if (!fileReader.eof()) {
        person.name.resize(bufferSize);
        fileReader.read((char*) person.name.c_str(), bufferSize);
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

    int nameSize = (int)person.name.length();
    file.write((char*) &nameSize, sizeof(nameSize));
    file.write(person.name.c_str(), nameSize);
    file.write((char*) &person.salary, sizeof(person.salary));

    file.close();
}

int main() {
    const char* path = R"(..\test.bin)";
    character persons[10];
    vector<character> results;

    for (int i = 0; i < 10; ++i) {
        persons[i].name = "person #" + std::to_string(i);
        persons[i].salary = (rand() % 90) + 10;
    }

    savePersonToBinaryFile(path, persons[0]);
    savePersonToBinaryFile(path, persons[1], true);
    savePersonToBinaryFile(path, persons[2], true);
    savePersonToBinaryFile(path, persons[3], true);
    savePersonToBinaryFile(path, persons[4], true);
    savePersonToBinaryFile(path, persons[5], true);
    savePersonToBinaryFile(path, persons[6], true);
    savePersonToBinaryFile(path, persons[7], true);
    savePersonToBinaryFile(path, persons[8], true);
    savePersonToBinaryFile(path, persons[9], true);

    auto isSuccess = loadIntoArrFromBinaryFile(path, results);

    if (isSuccess) {
        for (auto const &result : results)
            std::cout << result.name << ": " << result.salary << std::endl;
    } else {
        cout << "Error" << endl;
    }

}