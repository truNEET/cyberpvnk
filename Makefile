CC=clang++
MACROS=
CFLAGS=-std=c++11 -g -Wall -Werror -Wextra -Wno-error=unused-parameter $(MACROS)
LINKER=-lSDL2 -lm -lGL -lSDL2_net -pthread

$(shell rm -r bin)
$(shell mkdir bin)

all: client server

server_delete:
	touch bin/server.$(shell uname -m)
	rm -r bin/server.$(shell uname -m)

client_delete:
	touch bin/client.$(shell uname -m)
	rm -r bin/client.$(shell uname -m)

server: server_delete class net thread math util
	$(CC) -c $(CFLAGS) server/server_main.cpp -o server/obj/server_main.o
	$(CC) -c $(CFLAGS) server/server_physics.cpp -o server/obj/server_physics.o
	$(CC) -c $(CFLAGS) server/server_net.cpp -o server/obj/server_net.o
	ld -r server/obj/server_main.o  server/obj/server_physics.o server/obj/server_net.o -o server/obj/server.o
	$(CC) $(CFLAGS) math/obj/math.o class/obj/class.o net/obj/net.o thread/obj/thread.o util/obj/util.o server/obj/server.o -o bin/server.$(shell uname -m) $(LINKER)

c_input_engine:
	$(CC) -c $(CFLAGS) client/c_input_engine.cpp -o client/obj/c_input_engine.o

c_net_engine:
	$(CC) -c $(CFLAGS) client/c_net_engine.cpp -o client/obj/c_net_engine.o

c_render_engine:
	$(CC) -c $(CFLAGS) client/c_render_engine.cpp -o client/obj/c_render_engine.o

c_engine: c_input_engine c_net_engine c_render_engine
	$(CC) -c $(CFLAGS) client/c_engine.cpp -o client/obj/c_engine.o
	$(CC) -c $(CFLAGS) client/c_menu_engine.cpp -o client/obj/c_menu_engine.o
	cp -r client/obj/c_engine.o client/obj/c_engine.o.o
	ld -r client/obj/c_net_engine.o client/obj/c_menu_engine.o client/obj/c_render_engine.o client/obj/c_input_engine.o client/obj/c_engine.o.o -o client/obj/c_engine.o


client: client_delete class net render input util math thread c_engine
	$(CC) -c $(CFLAGS) client/c_main.cpp -o client/obj/c_main.o
	ld -r client/obj/c_main.o client/obj/c_engine.o -o client/obj/c.o
	$(CC) $(CFLAGS) util/obj/util.o math/obj/math.o class/obj/class.o net/obj/net.o render/obj/render.o input/obj/input.o thread/obj/thread.o client/obj/c.o -o bin/client.$(shell uname -m) $(LINKER)

thread:
	$(CC) -c $(CFLAGS) thread/thread_main.cpp -o thread/obj/thread.o 

console: util net
	$(CC) $(CFLAGS) net/obj/net.o console/console_main.cpp util/obj/util_main.o -o bin/console.$(shell uname -m) $(LINKER)

class:
	$(CC) -c $(CFLAGS) class/class_main.cpp -o class/obj/class_main.o
	$(CC) -c $(CFLAGS) class/class_array.cpp -o class/obj/class_array.o
	$(CC) -c $(CFLAGS) class/class_extra.cpp -o class/obj/class_extra.o
	ld -r class/obj/class_main.o class/obj/class_array.o class/obj/class_extra.o -o class/obj/class.o

input:
	$(CC) -c $(CFLAGS) input/input_main.cpp -o input/obj/input.o

net:
	$(CC) -c $(CFLAGS) net/net_main.cpp -o net/obj/net_main.o
	$(CC) -c $(CFLAGS) net/net_ip.cpp -o net/obj/net_ip.o
	$(CC) -c $(CFLAGS) net/net_store.cpp -o net/obj/net_store.o
	ld -r net/obj/net_main.o net/obj/net_ip.o net/obj/net_store.o -o net/obj/net.o

render:
	$(CC) -c $(CFLAGS) render/render_main.cpp -o render/obj/render_main.o
	$(CC) -c $(CFLAGS) render/render_obj_loader.cpp -o render/obj/render_obj_loader.o
	ld -r render/obj/render_obj_loader.o render/obj/render_main.o -o render/obj/render.o

util:
	$(CC) -c $(CFLAGS) util/util_main.cpp -o util/obj/util.o

math:
	$(CC) -c $(CFLAGS) math/math_main.cpp -o math/obj/math.o
