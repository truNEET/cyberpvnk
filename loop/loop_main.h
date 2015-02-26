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

#ifndef LOOP_MAIN_H
	#define LOOP_MAIN_H
	#define LOOP_CODE_MT 0
	#include "../util/util_main.h"
	#include "vector"
	#include "thread"
	#define LOOP_PRINT_THIS_TIME 1
	struct loop_entry_t{
		void(*code)();
		std::string name;
	};
	struct loop_t{
		loop_t();
		long double target_rate;
		long double average_rate;
		std::string name;
		std::vector<loop_entry_t> code;
		int settings;
	};
	extern void loop_add(loop_t*, std::string, void(*)());
	extern void loop_run(loop_t*, int*);
	extern void loop_del(loop_t*, void(*)());
#endif
