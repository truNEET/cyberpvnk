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

#include "c_engine.h"
#include "c_render_engine.h"

extern render_t *render;
extern int self_id;

static int old_self_id = 0;
static int old_coord_id = 0;
static client_t *self = nullptr;
static coord_t *coord = nullptr;

static void update_pointers(){
	if(unlikely(self_id != old_self_id)){
		self = find_client_pointer(self_id);
		old_self_id = self_id;
		if(likely(self->coord_id != old_coord_id)){
			coord = find_coord_pointer(self->coord_id);
			old_coord_id = self->coord_id;
		}
	}
}

void render_init(){
	update_pointers();
}

void render_engine(){
	update_pointers();
	/*
	The server will send over all of the render_buffer entries. We will not do anything to them.
	*/
	render->loop(coord);
}

void render_close(){
	update_pointers(); // why not?
}
