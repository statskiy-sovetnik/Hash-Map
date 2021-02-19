#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "Hash_Map.h"

string INPUT_FILE_PATH(R"(D:\Programming\ClionProjects\Task3\input.txt)");
string OUTPUT_FILE_PATH(R"(D:\Programming\ClionProjects\Task3\output.txt)");

using namespace std;

template <typename Key_T, typename Val_T>
Hash_Map<Key_T, Val_T>* execute_commands(ifstream* inp);

template <typename Key_T, typename Val_T>
void type_elem_nums(ofstream* outp, ifstream* inp);

template <typename Key_T>
void execute_val_t(char val_t, ofstream* outp, ifstream* inp);

void execute_types(char key_t, char val_t, ofstream* outp, ifstream* inp);


int main() {

    char key_type, val_type;
    ifstream inp(INPUT_FILE_PATH);
    ofstream outp(OUTPUT_FILE_PATH);

    if(!inp.is_open() || !outp.is_open()) {
        throw "Unable to create file stream objects";
    }

    inp >> key_type >> val_type;
    execute_types(key_type, val_type, &outp, &inp);

    inp.close();
    outp.close();
}

//Переход от входных данных с типом ключа к шаблонам
void execute_types(char key_t, char val_t, ofstream* outp, ifstream* inp) {
    if(key_t == 'I') {
        execute_val_t<int>(val_t, outp, inp);
    }
    else if(key_t == 'D') {
        execute_val_t<double>(val_t, outp, inp);
    }
    else if(key_t == 'S') {
        execute_val_t<string>(val_t, outp, inp);
    }
}

//Передача типа значения в шаблонную функцию
template <typename Key_T>
void execute_val_t(char val_t, ofstream* outp, ifstream* inp) {
    if(val_t == 'I') {
        type_elem_nums<Key_T, int>(outp, inp);
    }
    else if(val_t == 'D') {
        type_elem_nums<Key_T, double>(outp, inp);
    }
    else if(val_t == 'S') {
        type_elem_nums<Key_T, string>(outp, inp);
    }
}

template <typename Key_T, typename Val_T>
Hash_Map<Key_T, Val_T>* execute_commands(ifstream* inp) {

    char cur_cmd;
    int cmd_num = 0;
    Key_T cur_key;
    Val_T cur_value;
    Hash_Map<Key_T, Val_T>* hashMap = new Hash_Map<Key_T, Val_T>(255, log(255) / 1);

    *inp >> cmd_num;

    for(int i = 0; i < cmd_num; i++) {
        *inp >> cur_cmd;

        switch (cur_cmd) {
            case 'A':
                *inp >> cur_key >> cur_value;
                hashMap->add(cur_key, cur_value);
                break;
            case 'R':
                *inp >> cur_key;
                hashMap->remove(cur_key);
                break;
        }
    }

    return hashMap;
}

template <typename Key_T, typename Val_T>
void type_elem_nums(ofstream* outp, ifstream* inp) {
    Hash_Map<Key_T, Val_T>* hash_table = execute_commands<Key_T, Val_T>(inp);
    *outp << hash_table->get_elems() << " "; //возвращает число элементов хеш-таблицы
    *outp << hash_table->count_unique(); //число уникальных элементов хеш-таблицы (с неповторяющимися значениями)

    hash_table->~Hash_Map();
}