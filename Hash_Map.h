#ifndef TASK3_HASH_MAP_H
#define TASK3_HASH_MAP_H

#include <list>
#include <functional>
#include <iterator>
#include <utility>
#include <math.h>

using namespace std;

template <typename Key_T, typename Val_T>
class Hash_Map {
    int elems;
    list<pair<Key_T, Val_T>>* hash_buff;
    int buff_size;
    int hash_list_limit;

public:

    class iterator {
        int list_ind;
        typename list<pair<Key_T, Val_T>> :: iterator cur_elem_iter;
        Hash_Map<Key_T, Val_T>* hash_table;

    public:
        iterator(Hash_Map<Key_T, Val_T>* hash_table, pair<Key_T, Val_T>* hash_table_elem);
        iterator(Hash_Map<Key_T, Val_T>* hash_table, typename list<pair<Key_T, Val_T>>::iterator iter);
        iterator(){}
        iterator(const iterator& that);
        ~iterator(){}

        friend  Hash_Map<Key_T, Val_T>::iterator Hash_Map<Key_T, Val_T>::end();  //внутри мы будем обращаться к полям итератора

        void copy(const iterator& that);
        iterator& operator=(const iterator& that);
        iterator operator++();
        pair<Key_T, Val_T>& operator*();
        pair<Key_T, Val_T>* operator->();
        bool is_end() {
            return this->list_ind == this->hash_table->buff_size;
        }
        bool operator==(const Hash_Map<Key_T, Val_T>::iterator& that);
        bool operator!=(const Hash_Map<Key_T, Val_T>::iterator& that) {
            return !(*this == that);
        }

    };

    Hash_Map(int sz, int list_limit);
    ~Hash_Map();

    void clean();
    void add(Key_T key, Val_T value);
    void remove(Key_T key);
    Hash_Map<Key_T, Val_T>::iterator get_value(Key_T key);
    Hash_Map<Key_T, Val_T>::iterator begin();
    Hash_Map<Key_T, Val_T>::iterator end() {
        Hash_Map<Key_T, Val_T>::iterator tmp;
        tmp.list_ind = this->buff_size;  //указвает на "несуществующую" ячейку
        tmp.hash_table = this;
        tmp.cur_elem_iter = this->hash_buff[0].begin();
        return tmp;
    }
    Hash_Map<Key_T, Val_T>& operator=(const Hash_Map<Key_T, Val_T>& that);
    void rehash();
    int count_unique();

    int get_elems() { return elems; }
};

// _________ ITERATOR _______________

template<typename Key_T, typename Val_T>
Hash_Map<Key_T, Val_T>::iterator::iterator(Hash_Map<Key_T, Val_T> *hash_table, pair<Key_T, Val_T> *hash_table_elem) {
    hash<Key_T> raw_elem_hash; //создали функцию хеширования
    int elem_hash = raw_elem_hash(hash_table_elem->first) % hash_table->buff_size; //хешируем ключ для доступа к
                                                                                    //нужному списку
    list<pair<Key_T, Val_T>>* cur_hash_map_buff = hash_table->hash_buff;
    typename list<pair<Key_T, Val_T>> :: iterator iter = cur_hash_map_buff[elem_hash].begin(), end_iter = cur_hash_map_buff[elem_hash].end();

    //Проходим по списку до нужного значения
    while(*iter != *hash_table_elem && iter != end_iter) {
        iter++;
    }

    this->cur_elem_iter = iter;
    this->list_ind = elem_hash;
    this->hash_table = hash_table;
}

template<typename Key_T, typename Val_T>
void Hash_Map<Key_T, Val_T>::iterator::copy(const iterator& that) {
    this->hash_table = that.hash_table;
    this->list_ind = that.list_ind;
    this->cur_elem_iter = that.cur_elem_iter;
}

template<typename Key_T, typename Val_T>
Hash_Map<Key_T, Val_T>::iterator::iterator(Hash_Map<Key_T, Val_T> *hash_table, typename list<pair<Key_T, Val_T>>::iterator iter) {
    //используем конструктор с передачей указателя на элемент таблицы
    Hash_Map<Key_T, Val_T>::iterator n_iter = Hash_Map<Key_T, Val_T>::iterator(hash_table, &(*iter));
    this->copy(n_iter);
}

template<typename Key_T, typename Val_T>
Hash_Map<Key_T, Val_T>::iterator::iterator(const Hash_Map::iterator &that){
    this->copy(that);
}

template<typename Key_T, typename Val_T>
typename Hash_Map<Key_T, Val_T>::iterator& Hash_Map<Key_T, Val_T>::iterator::operator=(const Hash_Map<Key_T, Val_T>::iterator &that) {
    this->copy(that);
    return *this;
}

template<typename Key_T, typename Val_T>
typename Hash_Map<Key_T, Val_T>::iterator Hash_Map<Key_T, Val_T>::iterator::operator++() {
    if (this->is_end()) {
        return *this;
    }

    Key_T elem_key = (*this)->first;
    Hash_Map<Key_T, Val_T>* iterable_hash_table = this->hash_table;
    typename list<pair<Key_T, Val_T>>::iterator tmp = this->cur_elem_iter;
    int cur_list_ind = this->list_ind;

    if( ++tmp != iterable_hash_table->hash_buff[cur_list_ind].end()) {  //если это не последний элемент списка
        this->cur_elem_iter++;
    }
    else {
        cur_list_ind++;
        while(cur_list_ind < iterable_hash_table->buff_size){  //перебираем списки
            if(iterable_hash_table->hash_buff[cur_list_ind].size() != 0) { //нашли непустой
                this->list_ind = cur_list_ind;
                this->cur_elem_iter = iterable_hash_table->hash_buff[cur_list_ind].begin();
                return *this;
            }
            cur_list_ind++;
        }
        *this = iterable_hash_table->end();
    }
    return *this;
}

template<typename Key_T, typename Val_T>
pair<Key_T, Val_T> &Hash_Map<Key_T, Val_T>::iterator::operator*() {
    return *(this->cur_elem_iter);  //возвращаем указатель на элемент (пару ключ-значение)
}

template<typename Key_T, typename Val_T>
pair<Key_T, Val_T> *Hash_Map<Key_T, Val_T>::iterator::operator->() {
    return &(*(this->cur_elem_iter));
}

template<typename Key_T, typename Val_T>
bool Hash_Map<Key_T, Val_T>::iterator::operator==(const Hash_Map<Key_T, Val_T>::iterator & that) {
    return this->list_ind == that.list_ind && this->cur_elem_iter == that.cur_elem_iter && this->hash_table == that.hash_table;
}


/*________ HASH MAP _____________________*/

template <typename Key_T, typename Val_T>
Hash_Map<Key_T, Val_T>::Hash_Map(int sz, int list_limit) {
    if(sz < 0 || list_limit < 0) {
        throw "Incorrect size or limit";
    }

    this->hash_buff = new list<pair<Key_T, Val_T>>[sz];
    /*list<pair<Key_T, Val_T>>* tmp_buff = (list<pair<Key_T, Val_T>>*)operator new[](sizeof(list<pair<Key_T, Val_T>>) * sz), *p = tmp_buff;
    for (int i = 0; i < sz; i++) {
        new (p) list<pair<Key_T, Val_T>>();
        p++;
    }*/

    this->buff_size = sz;
    this->elems = 0;
    this->hash_list_limit = list_limit;
}

template <typename Key_T, typename Val_T>
Hash_Map<Key_T, Val_T>::~Hash_Map() {
    this->clean();
}

template<typename Key_T, typename Val_T>
void Hash_Map<Key_T, Val_T>::clean() {
    delete[] this->hash_buff;
}

template<typename Key_T, typename Val_T>
typename Hash_Map<Key_T, Val_T>::iterator Hash_Map<Key_T, Val_T>::begin() {
    int hash_ind = 0, sz = this->buff_size;

    while(hash_ind < sz){
        if(this->hash_buff[hash_ind].size() != 0) {
            typename list<pair<Key_T, Val_T>> :: iterator head = this->hash_buff[hash_ind].begin();
            return typename Hash_Map<Key_T, Val_T>::iterator(this, head); //до этого был ПРЯМОЙ вызов конструктора
                                                                            //(дописать ::iterator)
        }
        hash_ind++;
    }

    return this->end();

}


template<typename Key_T, typename Val_T>
Hash_Map<Key_T, Val_T>& Hash_Map<Key_T, Val_T>::operator=(const Hash_Map<Key_T, Val_T>& that) {
    this->buff_size = that.buff_size;
    this->hash_list_limit = that.hash_list_limit;
    this->elems = that.elems;

    list<pair<Key_T, Val_T>>* n_hash_buff = new list<pair<Key_T, Val_T>>[that.buff_size];
    for(int i = 0; i < that.buff_size; i++) {
        n_hash_buff[i] = that.hash_buff[i];
    }

    this->clean(); //удаляем предыдущий буффер
    this->hash_buff = n_hash_buff;

    return *this;
}

//_______ REHASH, ADD, REMOVE, GET_ELEM _______

template <typename Key_T, typename Val_T>
void Hash_Map<Key_T, Val_T>::rehash() {
    //Меняется как число ячеек, так и длины списков
    Hash_Map<Key_T, Val_T> n_hash_map = Hash_Map(this->buff_size * 2, log(this->buff_size) / 1);

    Hash_Map<Key_T, Val_T>::iterator iter;
    for(iter = this->begin(); iter != this->end(); ++iter) {
        n_hash_map.add(iter->first, iter->second);
    }
    n_hash_map.elems = this->get_elems();

    *this = n_hash_map;
}

template<typename Key_T, typename Val_T>
void Hash_Map<Key_T, Val_T>::add(Key_T key, Val_T value) {
    
	typename Hash_Map<Key_T, Val_T>::iterator key_elem_it = this->get_value(key);
	if (key_elem_it.is_end()) { //если элемента с таким ключом нет
		hash<Key_T> key_raw_hash;
		int key_hash = key_raw_hash(key) % this->buff_size;

		//Если список с этим ключом уже заполнен
		if (this->hash_buff[key_hash].size() == this->hash_list_limit) {
			this->rehash();
			this->add(key, value);
			return;
		}
		else {
			this->hash_buff[key_hash].push_back(pair<Key_T, Val_T>(key, value));
		}

        this->elems++;
    }
	else { //ключ повторяется => заменяем значение
		key_elem_it->second = value;
	}
    
}

template <typename Key_T, typename Val_T>
void Hash_Map<Key_T, Val_T>::remove(Key_T key) {
    hash<Key_T> key_raw_hash;
    int key_hash = key_raw_hash(key) % this->buff_size;

    list<pair<Key_T, Val_T>>& cur_hash_list = this->hash_buff[key_hash];
    typename list<pair<Key_T, Val_T>>::iterator iter = cur_hash_list.begin(), end_it = cur_hash_list.end();

    for(iter; iter != end_it; ++iter) {
        if(iter->first == key) {
            pair<Key_T, Val_T> to_remove(key, iter->second);
            cur_hash_list.remove(to_remove);
            this->elems--;
            break;
        }
    }

}

template <typename Key_T, typename Val_T>
typename Hash_Map<Key_T, Val_T>::iterator Hash_Map<Key_T, Val_T>::get_value(Key_T key) {
    hash<Key_T> key_raw_hash;
    int key_hash = key_raw_hash(key) % this->buff_size;
    list<pair<Key_T, Val_T>>& cur_hash_list = this->hash_buff[key_hash];

    //int cur_list_size = cur_hash_list.size();
    typename list<pair<Key_T, Val_T>>::iterator iter = cur_hash_list.begin(), end_it = cur_hash_list.end();

    while (end_it != iter) {
        if (iter->first == key) {
            return typename Hash_Map<Key_T, Val_T>::iterator(this, iter); //до этого бы прямой вызов конструктора
        }
        ++iter;
    }

    return this->end();

}

template<typename Key_T, typename Val_T>
int Hash_Map<Key_T, Val_T>::count_unique() {
    Hash_Map<Key_T, Val_T>::iterator iter = this->begin(), end_iter = this->end();
    int unique_counter = 0;
    Val_T* unique_elems = new Val_T[this->elems];

    for(iter; iter != end_iter; ++iter) {
        bool is_unique = true;
        Val_T cur_value = iter->second;
        for(int i = 0; i < unique_counter; i++) {
            if(unique_elems[i] == cur_value) {
                is_unique = false;
                break;
            }
        }

        if(is_unique) {
            unique_elems[unique_counter++] = cur_value;
        }
    }
    delete[] unique_elems;

    return unique_counter;
}


#endif //TASK3_HASH_MAP_H