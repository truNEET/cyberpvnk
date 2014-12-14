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
#include "c_main.h"
#include "signal.h"

std::vector<coord_t*> coord;
std::vector<coord_extra_t> coord_extra;
std::vector<model_t*> model;
std::vector<model_extra_t> model_extra;
std::vector<client_t*> client;

client_t *self = nullptr;

render_t *render = nullptr;
input_t *input = nullptr;
net_t *net = nullptr;

int argc_;
char **argv_;

unsigned long int tick = 0;
bool terminate = false;

static bool check_for_parameter(const std::string a){
	std::string argv__;
	for(int i = 0;i < argc_;i++){
		argv__ += argv_[i];
	}
	return argv__.find_first_of(a) == std::string::npos;
}

static void init(){
	printf("Allocating & initializing self\n");
	self = new client_t();
	if(check_for_parameter("--render-disable")){
		printf("Allocating & initializing render\n");
		render = new render_t(argc_,argv_);
		printf("Initializing the render engine\n");
		render_init();
	}
	if(check_for_parameter("--input-disable")){
		printf("Allocating & initializing input\n");
		input = new input_t(argc_,argv_);
		printf("Initializing the input engine\n");
		input_init();
	}
	if(check_for_parameter("--net-disable")){
		printf("Allocating & initializing net\n");
		net = new net_t(argc_,argv_);
		printf("Initializing the net engine\n");
		net_init();
	}
}

static void close(){
	if(render != nullptr){
		render_close();
		render->close();
		delete render;
		render = nullptr;
	}
	if(input != nullptr){
		input_close();
		input->close();
		delete input;
		input = nullptr;
	}
	if(net != nullptr){
		net_close();
		net->close();
		delete net;
		net = nullptr;
	}
	self->close();
	delete self;
	self = nullptr;
}

static void engine_loop(){
	if(net != nullptr) net_engine();
	if(input != nullptr) input_engine();
	if(render != nullptr) render_engine();
}

static void module_loop(){
	if(net != nullptr) net->loop();
	if(input != nullptr && input->loop() == TERMINATE) terminate = true;
	if(render != nullptr) render->loop();
}

static void loop(){
	engine_loop();
	module_loop();
}

#ifdef __linux
void signal_handler(int signal){
	if(signal == SIGINT){
		printf("Received SIGINT\n");
		terminate = true;
	}
}
#endif

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	printf("Initializing the modules and other data\n");
	init();
	#ifdef __linux
	printf("Setting up the signal handler\n");
	signal(SIGINT,signal_handler);
	#endif
	while(terminate == false){
		loop();
		coord_t a;
		a.x = 3;
		coord_t b;
		std::vector<std::vector<std::string>> c = a.array->gen_string_vector();
		for(unsigned long int i = 0;i < c.size();i++){
			for(unsigned long int n = 0;n < c[i].size();n++){
				printf("\tc[%lu][%lu]:%s\n",i,n,c[i][n].c_str());
			}
		}
		b.array->parse_string_vector(c);
		b.print();
		tick++;
	}
	close();
}
