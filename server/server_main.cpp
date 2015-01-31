#include "server_main.h"
#include "server_physics.h"
#include "server_net.h"
#include "server_render.h"

std::vector<void(*)()> loop_code;

server_info_t *server_info = nullptr;
net_ip_connection_info_t *self_info = nullptr; // TODO: rename this to prevent confusion with server_info
thread_t *thread = nullptr;

bool terminate = false;
loop_t server_loop_code;

int argc_;
char **argv_;

coord_t *map = nullptr;
int sleep_time = 1;
bool fixed_sleep_time = false;

server_info_t::server_info_t(){}

server_info_t::~server_info_t(){}

/*static void write_vector_to_net(std::vector<std::vector<std::string>> *a){
	const unsigned long int a_size = a->size();
	for(unsigned long int i = 0;i < a_size;i++){
		const unsigned long int a_i_size = (*a)[i].size();
		for(unsigned long int n = 0;n < a_i_size;n++){
			const unsigned long int client_size = client_vector.size();
			for(unsigned long int c = 0;c < client_size;c++){
				net->write((*a)[i][n], client_vector[c]->connection_info_id, gen_rand());
			}
		}
	}
}*/

void server_info_init(){
	server_info = new server_info_t; // nothing special here until I decide to send this over to the client
	for(int i = 0;i < argc_;i++){
		char *next_item;
		if(i+i == argc_){
			next_item = (char*)"";
		}else{
			next_item = argv_[i+1];
		}
		if(strcmp(argv_[i],"--server-info-map-path") == 0){
			server_info->map_path = next_item;
		}else if(strcmp(argv_[i], "--server-info-map-name") == 0){
			server_info->map_name = next_item;
		}
	}
}

void test_logic_init(){}
void test_logic_engine(){}
void test_logic_close(){}

void init(int choice){
	for(int i = 0;i < argc_;i++){
		char *next_item;
		if(i+i == argc_){
			next_item = (char*)"";
		}else{
			next_item = argv_[i+1];
		}
		if(strcmp(argv_[i], "--fix-ms-sleep") == 0){
			sleep_time = atoi(next_item);
			fixed_sleep_time = true;
		}
	}
	switch(choice){
	case 1:
		server_info_init();
		net_init();
		physics_init("gametype/default");
		render_init();
		break;
	case 2:
		test_logic_init();
		break;
	case 3:
		terminate = true;
		break;
	default:
		printf("WARNING: This was NOT one of the options, terminating\n");
		terminate = true;
		break;
	}
}

void close(){
	net_close();
	physics_close();
	render_close();
	test_logic_close();
	// the functions are smart enough to not close if they never initialized
}

int menu(){
	int return_value;
	printf("(1) Start the server normally\n");
	printf("(2) Start the test_logic\n");
	printf("(3) Exit\n");
	scanf("%d", &return_value);
	return return_value;
}

int main(int argc, char **argv){
	argc_ = argc;
	argv_ = argv;
	init(menu());
	printf("Starting the main loop\n");
	int loop_code = INT_MIN;
	SET_BIT(&loop_code, LOOP_CODE_MT, 0);
	while(terminate == false){
		loop_run(&server_loop_code, loop_code);
		ms_sleep(sleep_time);
	}
	close();
	return 0;
}
