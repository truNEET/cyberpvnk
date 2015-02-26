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
#include "../class/class_main.h"
#include "../render/render_main.h"
#include "c_main.h"
#ifndef C_RENDER_ENGINE
	#define C_RENDER_ENGINE
	extern render_t *render;
	extern void render_init();
	extern void render_engine();
	extern void render_close();
#endif
