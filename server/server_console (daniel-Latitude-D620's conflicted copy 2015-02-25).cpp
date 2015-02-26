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
#include "../render/render_main.h"
#include "server_console.h"
#include "server_net.h" // net_loop_settings

static void settings_engine();
static void class_engine();
static void terminate_engine();
static void render_object_engine();
static void input_render_object_engine(input_t*, coord_t*, float*);

static void int_modifier_engine(int*);
static void double_modifier_engine(long double*);
static void string_modifier_engine(std::string*);

static std::thread *console_thread;

void console_init(){
	console_thread = new std::thread(console_engine);
}

void console_engine(){
	while(true){
		char data[512];
		printf("]");
		scanf("%511s", data);
		if((std::string)data == "settings"){
			settings_engine();
		}else if((std::string)data == "class"){
			class_engine();
		}else if((std::string)data == "terminate"){
			terminate_engine();
		}else if((std::string)data == "render" || (std::string)data == "render_object"){
			render_object_engine();
		}
	}
}

void console_close(){}

#define INFO_INT_BIT 0
#define INFO_STRING_BIT 1
#define INFO_DOUBLE_BIT 2

static void input_render_object_engine(input_t *input, coord_t *coord, float *time){
	if(input == nullptr){
		printf("Input render engine called with a null input engine\n");
		return;
	}
	if(input->query_key(nullptr, SDLK_w)){
		coord->y_vel += 1;
	}
	if(input->query_key(nullptr, SDLK_a)){
		coord->x_vel -= 1;
	}
	if(input->query_key(nullptr, SDLK_s)){
		coord->y_vel -= 1;
	}
	if(input->query_key(nullptr, SDLK_d)){
		coord->x_vel += 1;
	}
	int coordinate_to_use = 0;
	if(input->query_key(nullptr, SDLK_x)){
		SET_BIT(coordinate_to_use, 0, 1);
	}
	if(input->query_key(nullptr, SDLK_y)){
		SET_BIT(coordinate_to_use, 1, 1);
	}
	if(input->query_key(nullptr, SDLK_z)){
		SET_BIT(coordinate_to_use, 2, 1);
	}
	if(input->query_key(nullptr, SDLK_f)){
		if(CHECK_BIT(coordinate_to_use, 0)){
			coord->x += 1;
		}
		if(CHECK_BIT(coordinate_to_use, 1)){
			coord->y += 1;
		}
		if(CHECK_BIT(coordinate_to_use, 2)){
			coord->z += 1;
		}
	}
	if(input->query_key(nullptr, SDLK_c)){
		if(CHECK_BIT(coordinate_to_use, 0) == 1){
			coord->x -= 1;
		}
		if(CHECK_BIT(coordinate_to_use, 1) == 1){
			coord->y -= 1;
		}
		if(CHECK_BIT(coordinate_to_use, 2) == 1){
			coord->z -= 1;
		}
	}

}

static void render_object_engine(){
	char data[512];
	printf("ID\n");
	scanf("%511s", data);
	render_t *render = new render_t(argc_, argv_);
	input_t *input = new input_t(argc_, argv_);
	input->render = render;
	//coord_t *coord = new coord_t;
	coord_t coord;
	coord.mobile = false;
	coord.model_id = atoi(data);
	bool rendering = true;
	float old_time = 0;
	while(rendering == true && terminate == false){
		float time = 0;
		if(old_time != 0){
			time = get_time()-old_time;
		}else{
			time = 0;
		}
		input_render_object_engine(input, &coord, &time);
		input->loop();
		render->loop(&coord);
		// I don't think the renderer needs an engine
		// would there be trouble if this outruns the main engine thread?
	}
	delete render;
	delete input;
	render = nullptr;
	input = nullptr;
}

static void restart_terminate_engine(){
	util_shell(UTIL_SHELL_DELETE, "server_state.save");
	std::ofstream out("server_state.save");
	if(out.is_open()){
		const unsigned long int array_size = array_vector.size();
		array_lock.lock();
		for(unsigned long int i = 0;i < array_size;i++){
			out << array_vector[i]->gen_updated_string(UINT_MAX) << "\n";
		}
		array_lock.unlock();
		out.close();
		terminate = true;
	}else{
		printf("Could not write to the server_save file, not restarting\n");
	}
}

static void terminate_engine(){
	printf("'restart' or 'quit'\n");
	char data[512];
	scanf("%511s", data);
	if((std::string)data == "restart"){
		restart_terminate_engine();
	}else if((std::string)data == "quit"){
		terminate = true;
	}
}

static void new_class_engine(){
	array_t *array_of_new_item = nullptr;
	printf("'coord_t' or 'model_t' (other types will be supported soon)\n");
	char data[512];
	scanf("%511s", data);
	if((std::string)data == "coord_t"){
		double a[5];
		int b[1];
		printf("X Y Z Angle_X Angle_Y Model_ID\n");
		scanf("%lf %lf %lf %lf %lf %d", &a[0], &a[1], &a[2], &a[3], &a[4], &b[0]);
		coord_t *coord = new coord_t;
		coord->x = a[0];
		coord->y = a[1];
		coord->z = a[2];
		coord->x_angle = a[3];
		coord->y_angle = a[4];
		coord->model_id = b[0];
		array_of_new_item = &coord->array;
	}else if((std::string)data == "model_t"){
		printf("OBJ_path\n");
		char data[512];
		scanf("%511s", data);
		model_t *tmp = new model_t;
		model_load(tmp, data);
		array_of_new_item = &tmp->array;
	}
	printf("Generated a %s with an ID of %d\n", array_of_new_item->data_type.c_str(), array_of_new_item->id);
}

static void mod_class_engine(){
	char data[512];
	array_t *selected = nullptr;
	printf("ID\n");
	scanf("%511s", data);
	selected = find_array_pointer(atoi(data));
	if(selected == nullptr){
		printf("ID to modify does not exist\n");
		return;
	}
	selected->print();
	printf("VECTOR ('int_array', 'long_double_array', 'string_array')\n");
	scanf("%511s", data);
	printf("ENTRY\n");
	int position = 0;
	scanf("%d", &position);
	if((std::string)data == "int_array"){
		selected->int_lock.lock();
		int *tmp = selected->int_array[position];
		selected->int_lock.unlock();
		int_modifier_engine(tmp);
	}else if((std::string)data == "long_double_array"){
		selected->long_double_lock.lock();
		long double *tmp = selected->long_double_array[position];
		selected->long_double_lock.unlock();
		double_modifier_engine(tmp);
	}else if((std::string)data == "string_array"){
		selected->string_lock.lock();
		std::string *tmp = selected->string_array[position];
		selected->string_lock.unlock();
		string_modifier_engine(tmp);
	}
}

static void del_class_engine(){
	char data[512];
	printf("ID\n");
	scanf("%511s", data);
	delete_array_and_pointer((array_t*)find_array_pointer(atoi(data)));
}

static void list_ext_class_engine(){
	char data[512];
	printf("'type', 'id', or 'all' search\n");
	scanf("%511s", data);
	std::vector<array_t*> tmp_array_vector;
	if((std::string)data == "type"){
		all_entries_of_type(data);
	}else if((std::string)data == "id"){
		tmp_array_vector.push_back(find_array_pointer(atoi(data)));
	}else if((std::string)data == "all"){
		tmp_array_vector = array_vector;
	}
	const unsigned long int tmp_array_vector_size = tmp_array_vector.size();
	for(unsigned long int i = 0;i < tmp_array_vector_size;i++){
		tmp_array_vector[i]->print();
	}
}

static void patch_ext_class_engine(){
	char data[16*MEGABYTES_TO_BYTES];
	printf("PATCH_PATH\n");
	scanf("%511s", data);
	std::ifstream in((std::string)data);
	std::vector<std::string> tmp_vector;
	if(in.is_open()){
		while(in.getline(data, 65536)){
			tmp_vector.push_back(data);
		}
		const unsigned long int tmp_vector_size = tmp_vector.size();
		for(unsigned long int i = 0;i < tmp_vector_size;i++){
			update_class_data(tmp_vector[i], UINT_MAX);
			update_progress_bar((long double)((long double)i/(long double)tmp_vector_size)*(long double)100);
		}
		printf("Finished with the patch. If there was an error, make sure the line length is below 65535.\n");
	}else{
		printf("Cannot open the patch\n");
	}
}

static void class_engine(){
	printf("'new', 'mod', 'del', or 'ext'?\n");
	char data[512];
	scanf("%511s", data);
	if((std::string)data == "new"){
		new_class_engine();
	}else if((std::string)data == "mod"){
		mod_class_engine();
	}else if((std::string)data == "del"){
		del_class_engine();
	}else if((std::string)data == "list"){
		list_ext_class_engine();
	}else if((std::string)data == "patch"){
		patch_ext_class_engine();
	}

}

static void int_modifier_engine(int* pointer){
	printf("'bit', 'constant', or 'math' modification?\n");
	char bit_or_constant[512];
	scanf("%511s", bit_or_constant);
	if((std::string)bit_or_constant == "bit"){
		printf("BIT NEW_VALUE\n");
		int bit, value;
		scanf("%d %d", &bit, &value);
		SET_BIT(*pointer, bit, value);
		printf("pointer's new value: %d\n", *pointer);
		return;
	}else if((std::string)bit_or_constant == "constant"){
		printf("NEW_VALUE\n");
		int new_value;
		scanf("%d", &new_value);
		*pointer = new_value;
	}else if((std::string)bit_or_constant == "math"){
		printf("'add', 'sub', 'mul' or 'div' is the first keyword, the value is the second one\n");
		printf("KEYWORD VALUE\n");
		char new_data[4];
		float modifier;
		scanf("%3s %f", new_data, &modifier);
		if((std::string)new_data == "add"){
			*pointer += modifier;
		}else if((std::string)new_data == "sub"){
			*pointer -= modifier;
		}else if((std::string)new_data == "mul"){
			*pointer *= modifier;
		}else if((std::string)new_data == "div"){
			if(modifier == 0){
				modifier += 0.00000001;
			}
			*pointer /= modifier;
		}
	}
}

static void string_modifier_engine(std::string *pointer){
	printf("constant modification is the only available one here\n");
	printf("NEW_CONSTANT\n");
	char new_data[512];
	scanf("%511s", new_data);
	*pointer = new_data;
}

static void double_modifier_engine(long double *pointer){
	char data[512];
	printf("'constant' or 'math'\n");
	scanf("%511s", data);
	if((std::string)data == "constant"){
		printf("NEW_VALUE\n");
		scanf("%511s", data);
		// check to see that the input is valid so we don't blank the variable on accident
		*pointer = strtold(data, nullptr); // WARNING: major loss of precision
	}else if((std::string)data == "math"){
		printf("'add', 'sub', 'mul', or 'div' is the first keyboard, the value is the second one\n");
		float modifier;
		scanf("%3s %f",data,&modifier);
		if((std::string)data == "add"){
			*pointer += modifier;
		}else if((std::string)data == "sub"){
			*pointer -= modifier;
		}else if((std::string)data == "mul"){
			*pointer *= modifier;
		}else if((std::string)data == "div"){
			*pointer /= modifier;
		}
	}
}

static void settings_engine(){
	void *pointer_to_data;
	int info_about_pointer = 0;
	printf("Variable to modify: ");
	char data[512];
	scanf("%511s", data);
	if((std::string)data == "loop_settings"){
		pointer_to_data = &loop_settings;
		SET_BIT(info_about_pointer, INFO_INT_BIT, 1);
	}else if((std::string)data == "net_loop_settings"){
		pointer_to_data = &net_loop_settings;
		SET_BIT(info_about_pointer, INFO_INT_BIT, 1);
	}
	if(CHECK_BIT(info_about_pointer, INFO_INT_BIT) == 1){
		int_modifier_engine((int*)pointer_to_data);
	}else if(CHECK_BIT(info_about_pointer, INFO_STRING_BIT) == 1){
		string_modifier_engine((std::string*)pointer_to_data);
	}else if(CHECK_BIT(info_about_pointer, INFO_DOUBLE_BIT) == 1){
		double_modifier_engine((long double*)pointer_to_data);
	}
}
