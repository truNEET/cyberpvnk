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
#ifndef CLASS_MAIN_H
	#include "class_array.h"
	#include "class_extra.h"
	#include "../util/util_main.h" // this houses the pull_items_data function since that is not specific to this
	#include "sstream"
	#include "vector"
	#include "string"
	#include "climits"
	#include "cstdio"
	#define CLASS_MAIN_H
	#define CLASS_DATA_UPDATE_EVERYTHING	UINT_MAX
	#define CLASS_DATA_COORD_BIT		0
	#define CLASS_DATA_MODEL_BIT		1
	#define CLASS_DATA_RENDER_BUFFER_BIT	2
	#define CLASS_DATA_INPUT_BUFFER_BIT	3
	#define CLASS_DATA_NET_IP_CONNECTION_INFO_BIT	4
	#define COORD_POINTER_SIZE 1024
	#define MODEL_POINTER_SIZE 1024
	#define INPUT_TYPE_KEYBOARD 0
		#define INPUT_TYPE_KEYBOARD_INT_SIZE 2
		#define INPUT_TYPE_KEYBOARD_KEY 0 // SDLK_*
		#define INPUT_TYPE_KEYBOARD_CHAR 1
	#define INPUT_TYPE_MOUSE_MOTION 1
		#define INPUT_TYPE_MOUSE_MOTION_INT_SIZE 2
		#define INPUT_TYPE_MOUSE_MOTION_X 0
		#define INPUT_TYPE_MOUSE_MOTION_Y 1
	#define INPUT_TYPE_MOUSE_PRESS 2 // type
		#define INPUT_TYPE_MOUSE_PRESS_INT_SIZE 1
		#define INPUT_TYPE_MOUSE_PRESS_BUTTON 0; // ID entry
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_LEFT 0 // values
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_MIDDLE 1
			#define INPUT_TYPE_MOUSE_PRESS_BUTTON_RIGHT 2
	#define INPUT_TYPE_MOUSE_SCROLL 3
		#define INPUT_TYPE_MOUSE_SCROLL_UP 0
		#define INPUT_TYPE_MOUSE_SCROLL_DOWN 1
	#define INPUT_MOTION_FORWARD 0
	#define INPUT_MOTION_BACKWARD 1
	#define INPUT_MOTION_LEFT 2
	#define INPUT_MOTION_RIGHT 3
	class net_ip_connection_info_t{
	public:
		net_ip_connection_info_t();
		~net_ip_connection_info_t();
		array_t array;
		int connection_type; // UDP or TCP
		std::string ip;
		int port;
		// TCP socket when that is implemented
	};
	class input_settings_t{
	public:
		input_settings_t();
		~input_settings_t();
		array_t array;
		int int_data[64][2];
	};
	class input_buffer_t{
	public:
		input_buffer_t();
		~input_buffer_t();
		array_t array;
		int client_id;
		int type;
		int int_data[8];
	};

	struct face{
		int facenum;
		bool four;
		int faces[4];
		int texcoord[4];
		int mat;
		face(int facen, int f1, int f2, int f3, int f4, int t2, int t3, int m);
		face(int facen, int f1, int f2, int f3, int f4, int t1, int t2, int t3, int t4, int m);
	}; 
	struct coordinate{
		float x,y,z;
		coordinate(float a,float b,float c);
	};
	struct material{
		std::string name;
		float alpha,ns,ni;
		float dif[3],amb[3],spec[3];
		int illum;
		int texture;
		material(const char*  na,float al,float n,float ni2,float* d,float* a,float* s,int i,int t);
	};
	struct texcoord{
		float u,v;
		texcoord(float a,float b);
	};
	class model_t{
	private:
		void load_parse_vector(std::string);
	public:
		array_t array;
		std::vector<std::string*> coord;
		std::vector<coordinate*> vertex;
		std::vector<face*> faces;
		std::vector<coordinate*> normals;
		std::vector<unsigned int> texture;
		std::vector<unsigned int> lists;
		std::vector<material*> materials;
		std::vector<texcoord*> texturecoordinate;
		bool ismaterial,isnormals,istexture;
		model_t();
		void get_size(long double*, long double*, long double*);
		void close();
		unsigned long long int collective_size;
		void update_array();
	};
	class coord_t{
	public:
		array_t array;
		long double x,y,z,x_angle,y_angle;
		long double x_vel,y_vel,z_vel;
		long double physics_time;
		long double old_time;
		bool mobile;
		coord_t();
		void update_array_pointers();
		void print();
		void set_x_angle(bool,long double);
		void set_y_angle(bool,long double);
		void apply_motion(int);
		void close();
		std::vector<unsigned long int> nearby_coord;
		int model_id;
	};
	class client_t{
	public:
		array_t array;
		int coord_id;
		int model_id;
		int connection_info_id;
		client_t();
		void update_array();
		void close();
	};
	class render_buffer_t{
	public:
		render_buffer_t();
		array_t array;
		int model_id;
		int coord_id;
	};
	#include "../input/input_main.h"
	class client_extra_t{
	public:
		std::vector<input_buffer_t> input_buffer;
		client_extra_t();
	};
	extern std::vector<input_buffer_t*> input_buffer_vector;
	extern std::vector<client_t*> client_vector;
	extern std::vector<coord_t*> coord_vector;
	extern std::vector<model_t*> model_vector;
	extern std::vector<render_buffer_t*> render_buffer_vector;
	extern render_buffer_t* new_render_buffer();
	extern coord_t* new_coord();
	extern model_t* new_model();
	extern client_t* new_client();
	extern net_ip_connection_info_t* new_net_ip_connection_info();
	extern void add_coord(coord_t*);
	extern void add_model(model_t*);
	extern void add_render_buffer(render_buffer_t*);
	extern void add_client(client_t*);
	extern void delete_client_id(int);
	extern void delete_client_entry(int);
	extern void delete_coord_id(int);
	extern void delete_coord_entry(int);
	extern client_t *find_client_pointer(int);
	extern coord_t *find_coord_pointer(int);
	extern model_t *find_model_pointer(int);
	extern net_ip_connection_info_t* find_net_ip_connection_info_pointer(int);
	extern render_buffer_t *find_render_buffer_pointer(int);
	extern input_buffer_t* new_input_buffer();
	extern void delete_input_buffer(input_buffer_t*);
	extern void delete_input_buffer_id(int);
#endif
