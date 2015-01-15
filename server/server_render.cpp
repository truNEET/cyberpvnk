#include "server_render.h"
#include "server_main.h"

static bool render_init_bool = false;
extern server_info_t *server_info;
extern coord_t *map;

void render_init(){
	render_init_bool = true;
	if(map == nullptr){
		printf("WARNING: map == nullptr, make sure that the physics_init function ran BEFORE the render_init function\n");
		map = new_coord();
	}
	model_t *tmp_model = new_model();
	map->model_id = tmp_model->array.id;
	assert(server_info != nullptr);
	model_load(tmp_model, server_info->map_name);
	loop_code.push_back(render_engine);
}

void render_engine(){
	if(unlikely(render_init_bool == false)){
		render_init();
	}
	const unsigned long int render_buffer_size = render_buffer_vector.size();
	const unsigned long int client_size = client_vector.size();
	for(unsigned long int i = 0;i < client_size;i++){
		const long int coord_id = client_vector[i]->coord_id;
		long int model_id;
		if(client_vector[i]->model_id == 0){
			model_id = find_coord_pointer(client_vector[i]->coord_id)->model_id;
		}else{
			model_id = client_vector[i]->model_id;
		}
		bool found_client = false;
		for(unsigned long int n = 0;n < render_buffer_size;n++){
			const bool condition_a = render_buffer_vector[n]->coord_id == coord_id;
			const bool condition_b = render_buffer_vector[n]->model_id == model_id;
			if(condition_a && condition_b){
				found_client = true;
				break;
			}
		}
		if(found_client == false){
			render_buffer_t *tmp_render_buffer = new_render_buffer();
			tmp_render_buffer->coord_id = coord_id;
			tmp_render_buffer->model_id = model_id;
		}
	}
}

void render_close(){
	if(render_init_bool == false){
		return;
	}
	const unsigned long int render_buffer_size = render_buffer_vector.size();
	for(unsigned long int i = 0;i < render_buffer_size;i++){
		delete render_buffer_vector[i];
		render_buffer_vector[i] = nullptr;
	}
	render_buffer_vector.clear();
}
