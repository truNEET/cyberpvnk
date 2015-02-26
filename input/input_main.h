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
#ifndef C_INPUT_H
	#define C_INPUT_H
	#define USE_SDL 1
	#include "../main.h"
	#include "signal.h"
	#include "cstdio"
	#include "algorithm"
	#include "string"
	#include "cassert"
	#include "../render/render_main.h"
	#ifdef __linux
		#include "SDL2/SDL.h"
	#elif _WIN32
		#include "SDL2.h"
	#endif
	extern bool terminate;
	class input_t{
	private:
		void input_find_key(long int*, const input_buffer_t*);
		void input_update_key(input_buffer_t*);
		void input_parse_key_up(SDL_Event);
		void input_parse_key_down(SDL_Event);
	public:
		render_t *render;
		client_t *self;
		bool is_used;
		void blank();
		input_t(int,char**);
		bool query_key(input_buffer_t* buffer = NULL,int sdl_key = 0,char key = '\0');
		int loop();
		void close();
	};
	namespace cursor{
		void set_location(input_t*, unsigned int, unsigned int);
		void get_location(unsigned int*, unsigned int*);
	}
	#ifdef __linux
	extern void basic_signal_handler(int);
	#endif
	#include "../class/class_main.h"
#endif
