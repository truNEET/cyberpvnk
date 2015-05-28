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

array_t* array_vector[ARRAY_VECTOR_SIZE] = {nullptr};
std::mutex array_lock;

static array_id_t pull_id(std::string a){
	int_ return_value;
	const int_ start = a.find_first_of(ARRAY_ID_START);
	const int_ end = a.find_first_of(ARRAY_ID_END);
	std::string id_string = a.substr(start+1, end-start-1).c_str();
	return_value = atoi(id_string.c_str());
	return return_value;
}

array_id_t array_highest_id(){
	array_id_t id = 0;
	for(int_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
		if(likely(array_vector[i]->id > id)){
			id = array_vector[i]->id;
		}
	}
	return id;
}

void quick_vector_free();

void free_ram(){
	/*
	  There has to be something
	  I can put here to free
	  some RAM.
	 */
	quick_vector_free();
}

void quick_vector_free(){
	/*
	  Genius idea: Reserve a little area
	  inside the int_array for IDs. If there
	  is an array that has no references and
	  has been around for a while, put it on
	  a list of things to possibly delete. I 
	  am afraid that some of these items are
	  on the stack and arent' dynamically 
	  allocated. HOWEVER, I can set up an
	  initializing iteration variable in
	  the array that could be checked to see
	  how many iterations this item has been
	  around (time doesn't work well here since
	  some delays suck, but the same could be 
	  said about the iterations and the console
	  thread, but I could just ASSIGN A POINTER
	  to the int_vector. I have to write this as
	  a setter, so I have to make an immunity
	  function, which shouldn't be that hard.

	  How would this work over the internet?
	  The item would be deleted and constantly
	  updated from another source. Not a good 
	  idea for larger files at all. There should
	  be a boolean variable set either instead or
	  alongside the pointer. Better yet, we don't touch
	  models and we stick to things like coordinates, 
	  networking stuff, and other misc. stuff.

	  tl;dr iterator pointers immunize undeletable
	  items, networking is shit with larger data sets.
	 */
}

/*
  WARNING: Make sure everything I do
  to the arrays initially is accounted for
  in the corresponding ARRAY_RESERVE.
 */

bool array_t::get_send(){return send;}

int_ find_empty_array_entry(){
	int_ i;
	#ifdef RESERVE_ID_SIZE
	i = RESERVE_ID_SIZE;
	#else
	i = 0;
	#endif
	for(;i < ARRAY_VECTOR_SIZE;i++){
		if(array_vector[i] == nullptr){
			break;
		}
	}
	return i;
}

array_t::array_t(void* tmp_pointer, bool send_){
	pointer = tmp_pointer;
	write_protected = false;
	send = send_;
	last_update = get_time();
	int_array.push_back(&id);
	string_array.push_back(&data_type);
	quick_vector_free();
	array_lock.lock();
	int_ tmp_entry = find_empty_array_entry();
	array_vector[tmp_entry] = this;
	id = scramble_id(tmp_entry);
	array_lock.unlock();
}

void array_t::reset_values(){
	data_lock.lock();
	int_lock.lock();
	const int_ int_array_size = int_array.size();
	for(int_ i = 1;i < int_array_size;i++){ // first one is reserved for the array class
		*int_array[i] = DEFAULT_INT_VALUE;
	}
	int_lock.unlock();
	string_lock.lock();
	const int_ string_array_size = string_array.size();
	for(int_ i = 1;i < string_array_size;i++){ // first one is reserved for the array class
		*string_array[i] = DEFAULT_STRING_VALUE;
	}
	string_lock.unlock();
	long_double_lock.lock();
	const int_ long_double_array_size = long_double_array.size();
	for(int_ i = 0;i < long_double_array_size;i++){
		*long_double_array[i] = DEFAULT_LONG_DOUBLE_VALUE;
	}
	long_double_lock.unlock();
	data_lock.unlock();
}

bool array_t::id_match(array_id_t a){
	return id == a;
}

void array_t::new_id(array_id_t new_id_){
	data_lock.lock();
	array_vector[strip_id(new_id_)] = this;
	if(id != 0){ // 0 = wasn't added to the array at all
		array_vector[strip_id(id)] = nullptr;
	}
	printf_(gen_print_prefix() + "Changed the ID from '" + std::to_string(id) + "' to '" + std::to_string(new_id_) + "'\n", PRINTF_DEBUG);
	id = new_id_;
	data_lock.unlock();
}

void array_t::parse_string_entry(std::string tmp_string){
	const array_id_t old_id = id;
	data_lock.lock();
	#ifdef CLASS_DEBUG_OUTPUT
	printf("tmp_string FROM parse_string_entry: %s\n", tmp_string.c_str());
	#endif
	last_update = get_time();
	if(tmp_string.find_first_of(ARRAY_INT_SEPERATOR_START) != std::string::npos){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("Parsing int_ from string\n");
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
	if(old_id != id){
		array_id_t new_id_ = id;
		id = old_id;
		// the NEW ID needs to be passed as the parameter
		new_id(new_id_);
	}
}

std::string array_t::gen_updated_string(int_ what_to_update){
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

uint_ array_t::pull_starting_number(std::string a){
	const int_ start = a.find_first_of(ARRAY_STARTING_START);
	const int_ end = a.find_first_of(ARRAY_STARTING_END);
	if(start == end){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("pull_starting_number start==end %s\n", a.c_str());
		#endif
	}
	return std::atoll(a.substr(start+1, end-start-1).c_str());
}

std::vector<std::string> pull_items_data(char *a, std::string b, char *c){
	std::vector<std::string> return_value;
	while(b.find_first_of(a) != std::string::npos || b.find_first_of(c) != std::string::npos){
		const int_ start = b.find_first_of(a);
		const int_ end = b.find_first_of(c);
		return_value.push_back(b.substr(start+1, end-start-1));
		b = b.substr(end+1, b.size());
	}
	return return_value;
}

std::vector<std::string> array_t::pull_items(char *x, std::string a, char *y, std::vector<int> *entries_for_data){
	assert(a.find_first_of(x) != a.find_first_of(y));
	std::vector<std::string> b;
	uint_ start;
	while((start = a.find_first_of(x)) != std::string::npos){
		const int_ end = a.find_first_of(y);
		std::string tmp_string_whole = a.substr(start+1, end-start-1);
		const int_ start_number = pull_starting_number(tmp_string_whole);
		#ifdef CLASS_DEBUG_OUTPUT
		printf("tmp_string_whole: %s\n", tmp_string_whole.c_str());
		#endif
		const std::vector<std::string> tmp_vector = pull_items_data(ARRAY_ITEM_SEPERATOR_START, tmp_string_whole, ARRAY_ITEM_SEPERATOR_END);
		for(uint_ i = 0;i < tmp_vector.size();i++){
			entries_for_data->push_back((int_)i+(int_)start_number);
		}
		b.insert(b.end(), tmp_vector.begin(), tmp_vector.end());
		a = a.substr(a.find_first_of(y)+1, a.size());
	}
	return b;
}

void array_t::parse_int_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> int_data = pull_items(ARRAY_INT_SEPERATOR_START, a, ARRAY_INT_SEPERATOR_END, &entries_for_data);
	const int_ int_data_size = int_data.size();
	while(int_array.size() < int_data.size()){
		int_array.push_back(new int_);
	}
	int_lock.lock();
	for(int_ i = 0;i < int_data_size;i++){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("int_data[%d]: %s\n", entries_for_data[i], int_data[i].c_str());
		#endif
		*int_array[entries_for_data[i]] = std::atoll(int_data[i].c_str());
	}
	int_lock.unlock();
}

void array_t::parse_long_double_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> long_double_data = pull_items(ARRAY_LONG_DOUBLE_SEPERATOR_START, a, ARRAY_LONG_DOUBLE_SEPERATOR_END, &entries_for_data);
	const int_ long_double_data_size = long_double_data.size();
	long_double_lock.lock();
	while(long_double_array.size() < long_double_data_size){
		long_double_array.push_back(new long double);
	}
	for(int_ i = 0;i < long_double_data_size;i++){
		*long_double_array[entries_for_data[i]] = strtold(long_double_data[i].c_str(), nullptr);
	}
	long_double_lock.unlock();
}

void array_t::parse_string_from_string(std::string a){
	std::vector<int> entries_for_data;
	const std::vector<std::string> string_data = pull_items(ARRAY_STRING_SEPERATOR_START, a, ARRAY_STRING_SEPERATOR_END, &entries_for_data);
	const int_ string_data_size = string_data.size();
	string_lock.lock();
	while(string_array.size() < string_data_size){
		string_array.push_back(new std::string);
	}
	for(int_ i = 0;i < string_data_size;i++){
		#ifdef CLASS_DEBUG_OUTPUT
		printf("string_array[%d]: %s\n",entries_for_data[i], string_data[i].c_str());
		#endif
		*string_array[entries_for_data[i]] = string_data[i];
	}
	string_lock.unlock();
}

static std::string array_gen_data_vector_entry(std::string data, int_ start){
	return wrap(ARRAY_STARTING_START, std::to_string(start), ARRAY_STARTING_END) + wrap(ARRAY_ITEM_SEPERATOR_START, data, ARRAY_ITEM_SEPERATOR_END);
}

bool array_t::updated( int_ *what_to_update){
	data_lock.lock();
	assert(what_to_update != nullptr);
	std::hash<std::string> hash_function;
	int_ tmp_string_hash = hash_function(gen_string_string());
	int_ tmp_long_double_hash = hash_function(gen_long_double_string());
	int_ tmp_int_hash = hash_function(gen_int_string());
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
	const int_ string_size = string_array.size();
	for(int_ i = 0;i < string_size;i++){
		if(string_array[i] != nullptr) return_value += array_gen_data_vector_entry((std::string)*string_array[i], (int_)i);
	}
	//data_lock.unlock();
	return_value = wrap(ARRAY_STRING_SEPERATOR_START, return_value, ARRAY_STRING_SEPERATOR_END);
	return return_value;
}

std::string array_t::gen_int_string(){
	std::string return_value;
	//data_lock.lock();
	const int_ int_size = int_array.size();
	for(int_ i = 0;i < int_size;i++){
		if(int_array[i] != nullptr) return_value += array_gen_data_vector_entry(std::to_string((int_)*int_array[i]), (int_)i);
	}
	//data_lock.unlock();
	return_value = wrap(ARRAY_INT_SEPERATOR_START, return_value, ARRAY_INT_SEPERATOR_END);
	return return_value;
}

std::string array_t::gen_long_double_string(){
	std::string return_value;
	//data_lock.lock();
	const int_ long_double_size = long_double_array.size();
	for(int_ i = 0;i < long_double_size;i++){
		if(long_double_array[i] != nullptr) return_value += array_gen_data_vector_entry(std::to_string((long double)*long_double_array[i]), (int_)i);
	}
	//data_lock.unlock();
	return_value = wrap(ARRAY_LONG_DOUBLE_SEPERATOR_START, return_value, ARRAY_LONG_DOUBLE_SEPERATOR_END);
	return return_value;
}

std::vector<std::string> generate_outbound_class_data(){
	std::vector<std::string> return_value;
	for(uint_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
		if(array_vector[i] != nullptr && array_vector[i]->get_send()){
			array_vector[i]->data_lock.lock();
			const std::string tmp = array_vector[i]->gen_updated_string(~0);
			array_vector[i]->data_lock.unlock();
			return_value.push_back(tmp);
		}
	}
	return return_value;
}

void update_class_data(std::string a, int_ what_to_update){
	array_id_t id = pull_id(a);
	array_t *tmp = (array_t*)find_array_pointer(id);
	if(tmp == nullptr){
		std::string type = a.substr(a.find_first_of(ARRAY_TYPE_SEPERATOR_START)+1, a.find_first_of(ARRAY_TYPE_SEPERATOR_END)-a.find_first_of(ARRAY_TYPE_SEPERATOR_START)-1);
		try{
			if(type == "coord_t"){
				tmp = &((new coord_t)->array);
			}else if(type == "model_t"){
				tmp = &((new model_t)->array);
			}else if(type == "client_t"){
				tmp = &((new client_t)->array);
			}else if(type == "net_ip_connection_info_t"){
				tmp = &((new net_ip_connection_info_t)->array);
			}else{
				printf("TODO: make a special allocator for '%s'\n", type.c_str());
				tmp = new array_t(nullptr, true);
			}
		}catch(std::bad_alloc &a){
			printf_("update_class_data: couldn't allocate data for a new item\n", PRINTF_VITAL);
			free_ram();
		}
	}
	if(tmp->get_write_protected() == false){
		tmp->parse_string_entry(a);
	}
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
	//printf_(print() + "DELETING AN ARRAY\n", PRINTF_STATUS);
	array_lock.lock();
	int_ array_pos_in_vector = strip_id(id);
	//assert(array_vector[array_pos_in_vector] == this);
	printf_(print(), PRINTF_DEBUG);
	array_vector[array_pos_in_vector] = nullptr;
	array_lock.unlock();
}

bool lower_id(array_t* a, array_t* y){
	return likely(a->id < y->id);
}

static array_id_t match_id_;

bool match_id(array_t* a){
	return match_id_ == a->id;
}

__attribute__((always_inline)) void *find_pointer(array_id_t id, std::string type){
	const array_id_t truncated_id = strip_id(id);
	if(array_vector[truncated_id] == nullptr){
		return nullptr;
	}
	if(array_vector[truncated_id]->id == id){
		return array_vector[truncated_id]->pointer;
	}
	return nullptr;
}

__attribute__((always_inline)) array_t* find_array_pointer(array_id_t id){
	return array_vector[strip_id(id)];
}

void delete_array_and_pointer(array_t *array){
	if(array == nullptr){
		return;
	}
	array->data_lock.lock();
	void *ptr = array->pointer;
	if(ptr == nullptr){
		delete array;
		return;
	}
	const std::string type = array->data_type;
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
	}else{
		printf_("delete_array_and_pointer: cannot delete the unknown data type '" + type + "'\n" + array->print(), PRINTF_ERROR);
	}
}

void delete_all_data(){
	for(int_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
		delete_array_and_pointer(array_vector[i]);
		array_vector[i] = nullptr;
	}
}

std::string array_t::gen_print_prefix(){
	std::stringstream ss;
	ss << this;
	return "array_t::array_t (this = " + ss.str() + "): ";
}

std::string array_t::print(){
	std::string output;
	std::stringstream ss_;
	ss_ << this;
	output += "Type: " + data_type + "\t";
	output += "Pointer: " + ss_.str() + "\t";
	output += "ID: " + std::to_string(id) + "\n";
	int_lock.lock();
	const int_ int_array_size = int_array.size();
	for(int_ i = 0;i < int_array_size;i++){
		output += "\tint_array[" + std::to_string(i) + "]: " + std::to_string(*int_array[i]) + "\n";
	}
	int_lock.unlock();
	string_lock.lock();
	const int_ max_print_string_length = 8192;
	const int_ string_array_size = string_array.size();
	for(int_ i = 0;i < string_array_size;i++){
		if(string_array[i]->size() < max_print_string_length){
			output += "\tstring_array[" + std::to_string(i) + "]: " + *string_array[i] + "\n";
		}else{
			output += "\tstring_array[" + std::to_string(i) + "] is too large to print\n";
		}
	}
	string_lock.unlock();
	long_double_lock.lock();
	const int_ long_double_array_size = long_double_array.size();
	for(int_ i = 0;i < long_double_array_size;i++){
		output += "\tlong_double_array[" + std::to_string(i) + "]: " + std::to_string(*long_double_array[i]) + "\n";
	}
	long_double_lock.unlock();
	output += "\n";
	std::stringstream ss;
	ss << &data_lock;
	output += "\tdata_lock address: " + ss.str() + "\n";
	return output;
}

std::vector<array_id_t> all_ids_of_type(std::string type){
	std::vector<array_id_t> return_value;
	array_lock.lock();
	for(int_ i = 0;i < ARRAY_VECTOR_SIZE;i++){
		if(array_vector[i] != nullptr){
			if(array_vector[i]->data_type == type){
				return_value.push_back(array_vector[i]->id);
			}
		}
	}
	array_lock.unlock();
	return return_value;
}

bool valid_id(array_id_t a){
	return valid_int(a) && a != 0 && a != -1;
}

int_ array_t::new_int(){
	int_array.push_back(new int_);
	return int_array.size()-1;
}

void array_t::delete_int(int_ position_in_vector){
	delete int_array[position_in_vector];
	int_array[position_in_vector] = nullptr;
}

int_ array_t::new_long_double(){
	long_double_array.push_back(new long double);
	return long_double_array.size()-1;
}

void array_t::delete_long_double(int_ position_in_vector){
	delete long_double_array[position_in_vector];
	long_double_array[position_in_vector] = nullptr;
}

int_ array_t::new_string(){
	string_array.push_back(new std::string);
	return string_array.size()-1;
}

void array_t::delete_string(int_ position_in_vector){
	delete string_array[position_in_vector];
	string_array[position_in_vector] = nullptr;
}

void array_t::immunity(bool a){
	// If I get an actual track of new variables, ditch the (possibly) un-needed find_array_pointer
	//array_t *iterator = find_array_pointer(RESERVE_ID_ITERATOR);
	//if(iterator == nullptr){
	//	printf_(gen_print_prefix() + "Couldn't find the iterator array, this is bad\n", PRINTF_VITAL);
	//	assert(false);
	//}
}

bool array_t::get_write_protected(){return write_protected;}
