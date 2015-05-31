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
#include "c_render_engine.h"

extern int_ argc_;
extern char **argv_;

render_t *render = nullptr;

array_id_t background_coord[1024];

std::string background_filename[128][128]; // just guessing the numbers

static std::string get_tile_filename(uint_ x, uint_ y){
	return "../image_data/background.png";
	//x = (x-8) % 16;
	//y = (y-8) % 16;
	//if(x > 128 && y > 128){
	//	return background_filename[x][y];
	//}else{
	//	printf_("ERROR: get_tile_filename: requested tile is out of bounds\n", PRINTF_ERROR);
	//	return "";
	//}
}

void background_init(){
	uint_ current_x = 8;
	uint_ current_y = 8;
	screen_t *current_screen = (screen_t*)find_pointer(render->screen[0]);
	current_screen->array.data_lock.lock();
	for(uint_ i = 0;i < 1024;i++){
		std::string tile_filename = get_tile_filename(current_x, current_y);
		coord_t *coord = new coord_t;
		coord->array.data_lock.lock();
		tile_t *tile = new tile_t;
		tile->array.data_lock.lock();
		image_t *image = new image_t(tile_filename);
		image->array.data_lock.lock();
		for(uint_ i = 0;i < TILE_ANIMATION_SIZE;i++){
			for(uint_ c = 0;c < TILE_IMAGE_SIZE;c++){
				tile->set_image_id(i, c, image->array.id);
			}
		}
		background_coord[i] = coord->array.id;
		coord->tile_id = tile->array.id;
		coord->x = current_x;
		coord->y = current_y;
		if(current_x <= current_screen->x_res){
			current_x += 16;
		}else{
			current_x = 8;
			if(current_y <= current_screen->y_res){
				current_y += 16;
			}else{
				i = 1024; // don't get out of the loop
			}
		}
		coord->array.data_lock.unlock();
		tile->array.data_lock.unlock();
		image->array.data_lock.unlock();
	}
	current_screen->array.data_lock.unlock();
}

void render_init(){
	loop_add(&loop, "render_engine", render_engine);
	render = new render_t(argc_, argv_);
	screen_t *screen_ = new screen_t();
	screen_->array.data_lock.lock();
	screen_->x_res = 640;
	screen_->y_res = 480;
	screen_->title = "Cyberpvnk";
	screen_->new_screen();
	screen_->array.data_lock.unlock();
	render->screen.push_back(screen_->array.id);
	background_init();
}

void render_engine(){
	if(render != nullptr){
		render->loop();
	}
}

void render_close(){
	loop_del(&loop, render_engine);
}
