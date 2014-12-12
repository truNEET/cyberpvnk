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
/*
TODO: Unify all of the controls in a file and load that into the program.
*/
#include "input_main.h"
#include "../render/render_main.h"
#include "../util/util_main.h"

char *event_key_to_key(SDL_Keycode);

extern render_t *render;
extern client_t *self;

extern void delete_array(array_t*);

input_buffer_t input_buffer_blank;

input_buffer_t::input_buffer_t(){
	array = new_array();
	array->int_array.push_back(&type);
	for(unsigned int i = 0;i < 8;i++){
		array->int_array.push_back(&int_data[i]);
	}
	type = 0;
	for(unsigned int i = 0;i < 8;i++){
		int_data[i] = 0;
	}
}

input_buffer_t::~input_buffer_t(){
	delete_array(array);
	array = nullptr;
}

void input_t::blank(){
	is_used = false;
	for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
		input_buffer[i] = nullptr;
	}
}

input_t::input_t(int argc,char** argv){
	SDL_Init(SDL_INIT_EVENTS);
	input_buffer = nullptr;
	input_buffer = new input_buffer_t*[INPUT_BUFFER_SIZE];
	blank();
	printf("%s\n",argv[0]);
	for(int i = 0;i < argc;i++){
		if((std::string)argv[i] == (std::string)"--input-mouse-sensitivity"){ // add yaw and pitch alongside this
	//		input_rules->mouse_yaw_sens = input_rules.mouse_pitch_sens = (float)atof((char*)argv[i+1]);
		}
	}
}

int input_t::loop(){
	int return_value = 0;
	int available_input = 0;
	for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
		if(input_buffer[i] != nullptr){
			if(input_buffer[i]->type != INPUT_TYPE_KEYBOARD){ // keyboard events are uninitialized when the key is let up.
				printf("%d\n",input_buffer[i]->type);
				delete input_buffer[i];
				input_buffer[i] = nullptr;
			}
		}
	}
	while(SDL_PollEvent(&event)){ // the only input that works without a screen would be SDL_QUIT (Ctrl-C).
		while(input_buffer[available_input] != nullptr){
			available_input++;
			if(available_input+1 > INPUT_BUFFER_SIZE){
				printf("Warning: reseting the input variable back to zero, this could lead to a lockup\n");
				available_input = 0;
			}
		}
		switch(event.type){
		case SDL_MOUSEMOTION:
			input_buffer[available_input] = new input_buffer_t;
			if(term_if_true(input_buffer[available_input] == nullptr,(char*)"input_buffer allocation") == TERMINATE){
				return_value = TERMINATE;
				break;
			}
			input_buffer[available_input]->type = INPUT_TYPE_MOUSE_MOTION;
			input_buffer[available_input]->int_data[INPUT_TYPE_MOUSE_MOTION_X] = event.motion.x;
			input_buffer[available_input]->int_data[INPUT_TYPE_MOUSE_MOTION_Y] = event.motion.y;
			break;
		case SDL_KEYUP:
			if(true){
				int key_entry = -1;
				if(event.key.keysym.sym < INPUT_BUFFER_SIZE && input_buffer[event.key.keysym.sym] != nullptr){
					if(input_buffer[event.key.keysym.sym]->type == INPUT_TYPE_KEYBOARD){
						if(input_buffer[event.key.keysym.sym]->int_data[INPUT_TYPE_KEYBOARD_KEY] == event.key.keysym.sym){
							key_entry = event.key.keysym.sym;
						}
					}
				}
				if(key_entry == -1) key_entry++;
				for(unsigned int i = (unsigned int)key_entry;i < INPUT_BUFFER_SIZE;i++){
					if(input_buffer[i] != nullptr){
						if(input_buffer[i]->type == INPUT_TYPE_KEYBOARD){
							if(input_buffer[i]->int_data[INPUT_TYPE_KEYBOARD_KEY] == event.key.keysym.sym){
								delete input_buffer[i];
								input_buffer[i] = nullptr;
								break;
							}
						}
					}
				}
			}
			break;
		case SDL_KEYDOWN:
			if(true){
				bool found_key = false;
				if(event.key.keysym.sym < INPUT_BUFFER_SIZE && input_buffer[event.key.keysym.sym] != nullptr){
					if(input_buffer[event.key.keysym.sym]->int_data[INPUT_TYPE_KEYBOARD_KEY] == event.key.keysym.sym){
						found_key = true;
					}
				}
				if(found_key == false){
					for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
						if(input_buffer[i] != nullptr){
							if(input_buffer[i]->type == INPUT_TYPE_KEYBOARD){
								if(input_buffer[i]->int_data[INPUT_TYPE_KEYBOARD_KEY] == event.key.keysym.sym){
									found_key = true;
									break;
								}
							}
						}
					}
				} // I think most of that code is redundant since there should be no matching key to begin with unless I push events to SDL manually
				if(found_key == false){
					input_buffer[available_input] = new input_buffer_t;
					if(term_if_true(input_buffer[available_input] == nullptr,(char*)"input_buffer allocation") == TERMINATE){
						return_value = TERMINATE;
						break;
					}
					input_buffer[available_input]->type = INPUT_TYPE_KEYBOARD;
					input_buffer[available_input]->int_data[INPUT_TYPE_KEYBOARD_KEY] = (int)event.key.keysym.sym;
					input_buffer[available_input]->int_data[INPUT_TYPE_KEYBOARD_CHAR] = (int)SDL_GetKeyName(event.key.keysym.sym)[0];
				}
				if(event.key.keysym.sym != SDLK_ESCAPE) break;
			}
		case SDL_QUIT:
			printf("Terminating per request via input layer\n");
			return_value = TERMINATE;
			break;
		default:
			//printf("The event you are creating is not supported (SDL ID:%d)\n",event.type);
			break;
		}
	}
	/*
	TODO: Add joystick support and map the buttons to keyboard keys
	*/
	return return_value;
}

bool input_t::query_key(input_buffer_t *buffer,int sdl_key,char key){ // This is used for debugging more than anything (all of the other code would be put into the main function here and not stranded in the classes in the code at random spots).
	bool return_value = false;
	if(sdl_key == -1 && key == '\0' && buffer != nullptr){
		for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
			if(input_buffer[i] != nullptr){
				if(input_buffer[i]->int_data == buffer->int_data){
					return_value = true;
					break;
				}
			}
		}
	}else if(sdl_key != -1){
		if(sdl_key < INPUT_BUFFER_SIZE && input_buffer[sdl_key] != nullptr){
			if(input_buffer[sdl_key]->type == INPUT_TYPE_KEYBOARD){
				if(input_buffer[sdl_key]->int_data[INPUT_TYPE_KEYBOARD_KEY] == sdl_key) return_value = sdl_key;
			}
		}else{
			for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
				if(input_buffer[i] != nullptr){
					if(input_buffer[i]->type == INPUT_TYPE_KEYBOARD){
						if(input_buffer[i]->int_data[INPUT_TYPE_KEYBOARD_KEY] == sdl_key){
							return_value = true;
							break;
						}
					}
				}
			}
		}
	}else if(key != '\0'){
		for(unsigned int i = 0;i < INPUT_BUFFER_SIZE;i++){
			if(input_buffer[i] != nullptr){
				if(input_buffer[i]->type == INPUT_TYPE_KEYBOARD){
					if(input_buffer[i]->int_data[INPUT_TYPE_KEYBOARD_CHAR] == (int)key){
						return_value = true;
						break;
					}
				}
			}
		}
	} // clean this up a bit
	return return_value;
}

void input_t::close(){
	blank();
	delete[] input_buffer;
	input_buffer = nullptr;
}

void input_settings_t::blank(){
	is_used = false;
}

int input_settings_t::init(){
	int return_value = 0;
	blank();
	is_used = true;
	return return_value;
}
void input_settings_t::close(){
	blank();
}

void input_settings_mouse_t::blank(){
	x_sens = 1;
	y_sens = 1;
	slow_key = 'q';
	slow_multiplier = .5; // slow down the mouse movement by 1/2 when Q is pressed
}
int input_settings_mouse_t::init(){
	int return_value = 0;
	blank();
	return return_value;
}

void input_settings_mouse_t::close(){
	blank();
}
