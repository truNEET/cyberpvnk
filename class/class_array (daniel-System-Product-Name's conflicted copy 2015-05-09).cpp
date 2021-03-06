/*
Czech_mate by Daniel
This file is part of Czech_mate.

Czech_mate is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 2 as published by
the Free Software Foundation, 

Czech_mate is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Czech_mate.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "class_main.h"
#include "class_array.h"

//#define CLASS_DEBUG_OUTPUT 1

std::vector<array_t*> array_vector;
std::mutex array_lock;

static array_id_t pull_id(std::string a){
	int64_t return_value;
	const unsigned long int64_t start = a.find_first_of(ARRAY_ID_START);
	const unsigned long int64_t end = a.find_first_of(ARRAY_ID_END);
	std::string id_string = a.substr(start+1, end-start-1).c_str();
	return_value = atoi(id_string.c_str());
	return return_value;
}

array_id_t array_highest_id(){
	array_id_t id = 0;
	const unsigned long int64_t array_vector_size = array_vector.size();
	for(unsigned long int64_t i = 0;i < array_vector_size;i++){
		if(likely(array_vector[i]->id > id)){
			id = array_vector[i]->id;
		}
	}
	return id;
}

array_t::array_t(void* tmp_pointer, bool add_array_to_vector){
	pointer = tmp_pointer;
	id = (array_id_t)gen_rand();
	if(id == 0){
		id = (array_id_t)gen_rand();
	}
	update_pointers();
	if(add_array_to_vector){
		array_lock.lock();
		array_vector.push_back(this);
		array_lock.unlock();
	}
	last_update = get_time();
}

void array_t::reset_values(){
	data_lock.lock();
	int_lock.lock();
	const unsigned long int64_t int_array_size = int_array.size();
	for(unsigned long int64_t i = 1;i < int_array_size;i++){
		*int_array[i] = DEFAULT_INT_VALUE;
	}
	int_lock.unlock();
	string_lock.lock();
	const unsigned long int64_t string_array_size = string_array.size();
	for(unsigned long int64_t i = 1;i < string_array_size;i++){
		*string_array[i] = DEFAULT_STRING_VALUE;
	}
	string_lock.unlock();
	long_double_lock.lock();
	const unsigned long int64_t long_double_array_size = long_double_array.size();
	for(unsigned long int64_t i = 0;i < long_double_array_size;i++){
		*long_double_array[i] = DEFAULT_LONG_DOUBLE_VALUE;
	}
	long_double_lock.unlock();
	data_lock.unlock();
}

bool array_t::id_match(array_id_t a){
	return id == a;
}

void array_t::parse_string_entry(std::string tmp_string){
	data_lock.lock();
	#ifdef CLASS_DEBUG_OUTPUT
	printf("tmp_string FROM parse_string_entry: %s\n", tmp_string.c_str());
	#endif
	last_update = get_time();
	if(tmp_string.find_first_of(ARRAY_INT_SEPERATOR_START) != std::string::npos){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("Parsing int64_t from string\n");
		#endif
		parse_int_from_string(tmp_string);
	}
	if(tmp_string.find_first_of(ARRAY_LONG_DOUBLE_SEPERATOR_START) != std::string::npos){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("Parsing long_double from string\n");
		#endif
		parse_long_double_from_string(tmp_string);
	}
	if(tmp_string.find_first_of(ARRAY_STRING_SEPERATOR_START) != std::string::npos){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("Parsing string from string\n");
		#endif
		parse_string_from_string(tmp_string);
	}
	data_lock.unlock();
}

std::string array_t::gen_updated_string(int64_t what_to_update){
	data_lock.lock();
	std::string return_value = wrap(ARRAY_TYPE_SEPERATOR_START, data_type, ARRAY_TYPE_SEPERATOR_END) + wrap(ARRAY_ID_START, std::to_string(id), ARRAY_ID_END);
	SET_BIT(what_to_update, ARRAY_STRING_HASH_BIT, 1);
	SET_BIT(what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT, 1);
	SET_BIT(what_to_update, ARRAY_INT_HASH_BIT, 1);
	if(CHECK_BIT(what_to_update, ARRAY_STRING_HASH_BIT) == 1){
		return_value += gen_string_string();
	}
	if(CHECK_BIT(what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT) == 1){
		return_value += gen_long_double_string();
	}
	if(CHECK_BIT(what_to_update, ARRAY_INT_HASH_BIT) == 1){
		return_value += gen_int_string();
	}
	data_lock.unlock();
	return return_value;
}

void array_t::update_pointers(){
	data_lock.lock();
	int_lock.lock();
	int_array.clear();
	int_array.push_back(&id);
	int_lock.unlock();
	string_lock.lock();
	string_array.push_back(&data_type); // is this needed?
	string_lock.unlock();
	data_lock.unlock();
}

unsigned int64_t array_t::pull_starting_number(std::string a){
	const unsigned long int64_t start = a.find_first_of(ARRAY_STARTING_START);
	const unsigned long int64_t end = a.find_first_of(ARRAY_STARTING_END);
	if(start == end){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("pull_starting_number start==end %s\n", a.c_str());
		#endif
	}
	return atoi(a.substr(start+1, end-start-1).c_str());
}

std::vector<std::string> pull_items_data(char *a, std::string b, char *c){
	std::vector<std::string> return_value;
	while(b.find_first_of(a) != std::string::npos || b.find_first_of(c) != std::string::npos){
		const unsigned long int64_t start = b.find_first_of(a);
		const unsigned long int64_t end = b.find_first_of(c);
		return_value.push_back(b.substr(start+1, end-start-1));
		b = b.substr(end+1, b.size());
	}
	return return_value;
}

std::vector<std::string> array_t::pull_items(char *x, std::string a, char *y, std::vector<int> *entries_for_data){
	assert(a.find_first_of(x) != a.find_first_of(y));
	std::vector<std::string> b;
	unsigned long int64_t start;
	while((start = a.find_first_of(x)) != std::string::npos){
		const unsigned long int64_t end = a.find_first_of(y);
		std::string tmp_string_whole = a.substr(start+1, end-start-1);
		const unsigned long int64_t start_number = pull_starting_number(tmp_string_whole);
		#ifdef CLASS_DEBUG_OUTPUT
		printf("tmp_string_whole: %s\n", tmp_string_whole.c_str());
		#endif
		const std::vector<std::string> tmp_vector = pull_items_data(ARRAY_ITEM_SEPERATOR_START, tmp_string_whole, ARRAY_ITEM_SEPERATOR_END);
		for(unsigned long int64_t i = 0;i < tmp_vector.size();i++){
			entries_for_data->push_back((int)i+(int)start_number);
		}
		b.insert(b.end(), tmp_vector.begin(), tmp_vector.end());
		a = a.substr(a.find_first_of(y)+1, a.size());
	}
	return b;
}

void array_t::parse_int_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> int_data = pull_items(ARRAY_INT_SEPERATOR_START, a, ARRAY_INT_SEPERATOR_END, &entries_for_data);
	const unsigned long int64_t int_data_size = int_data.size();
	int_lock.lock();
	for(unsigned long int64_t i = 0;i < int_data_size;i++){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("int_data[%d]: %s\n", entries_for_data[i], int_data[i].c_str());
		#endif
		*int_array[entries_for_data[i]] = atoi(int_data[i].c_str());
	}
	int_lock.unlock();
}

void array_t::parse_long_double_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> long_double_data = pull_items(ARRAY_LONG_DOUBLE_SEPERATOR_START, a, ARRAY_LONG_DOUBLE_SEPERATOR_END, &entries_for_data);
	const unsigned long int64_t long_double_data_size = long_double_data.size();
	long_double_lock.lock();
	for(unsigned long int64_t i = 0;i < long_double_data_size;i++){
		*long_double_array[entries_for_data[i]] = strtold(long_double_data[i].c_str(), nullptr);
	}
	long_double_lock.unlock();
}

void array_t::parse_string_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> string_data = pull_items(ARRAY_STRING_SEPERATOR_START, a, ARRAY_STRING_SEPERATOR_END, &entries_for_data);
	const unsigned long int64_t string_data_size = string_data.size();
	string_lock.lock();
	for(unsigned long int64_t i = 0;i < string_data_size;i++){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("string_array[%d]: %s\n",entries_for_data[i], string_data[i].c_str());
		#endif
		*string_array[entries_for_data[i]] = string_data[i];
	}
	string_lock.unlock();
}

static std::string array_gen_data_vector_entry(std::string data, unsigned int64_t start){
	return wrap(ARRAY_STARTING_START, std::to_string(start), ARRAY_STARTING_END) + wrap(ARRAY_ITEM_SEPERATOR_START, data, ARRAY_ITEM_SEPERATOR_END);
}

bool array_t::updated(int64_t *what_to_update){
	data_lock.lock();
	assert(what_to_update != nullptr);
	std::hash<std::string> hash_function;
	long int64_t tmp_string_hash = hash_function(gen_string_string());
	long int64_t tmp_long_double_hash = hash_function(gen_long_double_string());
	long int64_t tmp_int_hash = hash_function(gen_int_string());
	bool update = false;
	/*
	Chunking this up into smaller groups would make bigger arrays run faster.
	*/
	if(tmp_string_hash != string_hash){
		update = true;
		SET_BIT(*what_to_update, ARRAY_STRING_HASH_BIT, 1);
		string_hash = tmp_string_hash;
	}
	if(tmp_long_double_hash != long_double_hash){
		update = true;
		SET_BIT(*what_to_update, ARRAY_LONG_DOUBLE_HASH_BIT, 1);
		long_double_hash = tmp_long_double_hash;
	}
	if(tmp_int_hash != int_hash){
		update = true;
		SET_BIT(*what_to_update, ARRAY_INT_HASH_BIT, 1);
		int_hash = tmp_int_hash;
	}
	data_lock.unlock();
	return update;
}

std::string array_t::gen_string_string(){
	std::string return_value;
	//data_lock.lock();
	const unsigned long int64_t string_size = string_array.size();
	for(unsigned long int64_t i = 0;i < string_size;i++){
		return_value += array_gen_data_vector_entry((std::string)*string_array[i], (unsigned int)i);
	}
	//data_lock.unlock();
	return_value = wrap(ARRAY_STRING_SEPERATOR_START, return_value, ARRAY_STRING_SEPERATOR_END);
	return return_value;
}

std::string array_t::gen_int_string(){
	std::string return_value;
	//data_lock.lock();
	const unsigned long int64_t int_size = int_array.size();
	for(unsigned long int64_t i = 0;i < int_size;i++){
		return_value += array_gen_data_vector_entry(std::to_string((int)*int_array[i]), (unsigned int)i);
	}
	//data_lock.unlock();
	return_value = wrap(ARRAY_INT_SEPERATOR_START, return_value, ARRAY_INT_SEPERATOR_END);
	return return_value;
}

std::string array_t::gen_long_double_string(){
	std::string return_value;
	//data_lock.lock();
	const unsigned long int64_t long_double_size = long_double_array.size();
	for(unsigned long int64_t i = 0;i < long_double_size;i++){
		return_value += array_gen_data_vector_entry(std::to_string((long double)*long_double_array[i]), (unsigned int)i);
	}
	//data_lock.unlock();
	return_value = wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, return_value, ARRAY_LONG_DOUBLE_SEPERATOR_END);
	return return_value;
}

void update_class_data(std::string a, int64_t what_to_update){
	array_t *tmp = nullptr;
	array_id_t id = pull_id(a);
	printf("ID as I received it from pull_id(a): %d\n", id);
	array_lock.lock();
	const unsigned long int64_t array_vector_size = array_vector.size();
	for(unsigned long int64_t i = 0;i < array_vector_size;i++){
		if(array_vector[i] != nullptr && unlikely(array_vector[i]->id == id)){
			tmp = array_vector[i];
			break;
		}
	}
	array_lock.unlock();
	if(tmp == nullptr){
		std::string type = a.substr(a.find_first_of(ARRAY_TYPE_SEPERATOR_START)+1, a.find_first_of(ARRAY_TYPE_SEPERATOR_END)-a.find_first_of(ARRAY_TYPE_SEPERATOR_START)-1);
		if(type == "coord_t"){
			tmp = &((new coord_t)->array);
		}else if(type == "model_t"){
			tmp = &((new model_t)->array);
		}else if(type == "client_t"){
			tmp = &((new client_t)->array);
		}else if(type == "net_ip_connection_info_t"){
			tmp = &((new net_ip_connection_info_t)->array);
		}else{
			assert(false);
		}
	}
	tmp->parse_string_entry(a);
}

void add_two_arrays(array_t *a, array_t *b){
	a->data_lock.lock();
	b->data_lock.lock();
	a->int_array.insert(a->int_array.end(), b->int_array.begin(), b->int_array.end());
	a->long_double_array.insert(a->long_double_array.end(), b->long_double_array.begin(), b->long_double_array.end());
	a->string_array.insert(a->string_array.end(), b->string_array.begin(), b->string_array.end());
	a->data_lock.unlock();
	b->data_lock.unlock();
}

array_t::~array_t(){
	array_lock.lock();
	std::vector<array_t*>::iterator array_pos_in_vector = std::find_if(array_vector.begin(), array_vector.end(), pointer_device_t(this));
	if(likely(array_pos_in_vector != array_vector.end())){
		array_vector.erase(array_pos_in_vector);
	}else{
		printf("This vector does not exist.\n");
	}
	array_lock.unlock();
}

void *find_pointer(array_id_t id, std::string type){
	void* return_value = nullptr;
	array_lock.lock();
	const unsigned long int64_t array_size = array_vector.size();
	for(unsigned long int64_t i = 0;i < array_size;i++){
		if(array_vector[i] != nullptr && unlikely(array_vector[i]->id == id)){
			if(unlikely(type != array_vector[i]->data_type)){
				printf("The supplied data type doesn't match the ID\n");
				assert(false);
			}
			return_value = array_vector[i]->pointer;
			break;
		}
	}
	array_lock.unlock();
	return return_value;
}

std::vector<void*> all_entries_of_type(std::string type, bool lock){
	std::vector<void*> return_value;
	const unsigned long int64_t array_vector_size = array_vector.size();
	for(unsigned long int64_t i = 0;i < array_vector_size;i++){
		if(unlikely(array_vector[i]->data_type == type)){
			if(lock) array_vector[i]->data_lock.lock();
			return_value.push_back((void*)array_vector[i]);
		}
	}
	return return_value;
}

std::vector<void*> all_pointers_of_type(std::string type, bool lock){
	std::vector<void*> return_value;
	array_lock.lock();
	const unsigned long int64_t array_vector_size = array_vector.size();
	for(unsigned long int64_t i = 0;i < array_vector_size;i++){
		if(likely(array_vector[i] != nullptr)){
			if(array_vector[i]->data_type == type){
				if(lock) array_vector[i]->data_lock.lock();
				return_value.push_back((void*)array_vector[i]->pointer);
			}
		}
	}
	array_lock.unlock();
	return return_value;
}

array_t* find_array_pointer(int64_t id){
	const unsigned long int64_t array_vector_size = array_vector.size();
	array_lock.lock();
	for(unsigned long int64_t i = 0;i < array_vector_size;i++){
		if(array_vector[i] != nullptr && array_vector[i]->id == id){
			return array_vector[i];
		}
	}
	array_lock.unlock();
	return nullptr;
}

void delete_array_and_pointer(array_t *array){
	if(array == nullptr){
		return;
	}
	array->data_lock.lock();
	void *ptr = array->pointer;
	if(ptr == nullptr){
		return;
	}
	const std::string type = array->data_type;
	array->print();
	if(type == "coord_t"){
		delete (coord_t*)ptr;
	}else if(type == "model_t"){
		delete (model_t*)ptr;
	}else if(type == "input_settings_t"){
		delete (input_settings_t*)ptr;
	}else if(type == "client_t"){
		delete (client_t*)ptr;
	}else if(type == "net_ip_connection_info_t"){
		delete (net_ip_connection_info_t*)ptr;
	}else if(type == "input_keyboard_map_t"){
		delete (input_keyboard_map_t*)ptr;
	}else{
		printf("cannot delete the unknown data type %s\n", type.c_str());
	}
}

void delete_all_data(){
	std::vector<array_t*> array_vector_ = array_vector; // make a copy of it, something breaks when I don't make a copy
	const unsigned long int64_t array_vector__size = array_vector_.size();
	for(unsigned long int64_t i = 0;i < array_vector__size;i++){
		delete_array_and_pointer(array_vector_[i]);
	}
}

std::string array_t::print(){
	std::string output;
	output += "Type: " + data_type + "\t";
	output += "ID: " + std::to_string(id) + "\n";
	int_lock.lock();
	const unsigned long int64_t int_array_size = int_array.size();
	for(unsigned long int64_t i = 0;i < int_array_size;i++){
		output += "\tint_array[" + std::to_string(i) + "]: " + std::to_string(*int_array[i]) + "\n";
	}
	int_lock.unlock();
	string_lock.lock();
	const unsigned long int64_t max_print_string_length = 8192;
	const unsigned long int64_t string_array_size = string_array.size();
	for(unsigned long int64_t i = 0;i < string_array_size;i++){
		if(string_array[i]->size() < max_print_string_length){
			output += "\tstring_array[" + std::to_string(i) + "]: " + *string_array[i] + "\n";
		}else{
			output += "\tstring_array[" + std::to_string(i) + "] is too large to print\n";
		}
	}
	string_lock.unlock();
	long_double_lock.lock();
	const unsigned long int64_t long_double_array_size = long_double_array.size();
	for(unsigned long int64_t i = 0;i < long_double_array_size;i++){
		output += "\tlong_double_array[" + std::to_string(i) + "]: " + std::to_string(*long_double_array[i]) + "\n";
	}
	long_double_lock.unlock();
	output += "\n";
	std::stringstream ss;
	ss << &data_lock;
	output += "\tdata_lock address: " + ss.str() + "\n";
	std::cout << output;
	return output;
}

void unlock_array_of_data_type(std::vector<void*> a, void *pointer_offset){
	for(unsigned long int64_t i = 0;i < a.size();i++){
		bool *tmp = (bool*)a[i];
		((array_t*)tmp[(long int)pointer_offset])->data_lock.unlock();
	}
}
