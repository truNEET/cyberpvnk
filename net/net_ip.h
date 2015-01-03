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
#include "../util/util_main.h"
#include "net_const.h"
#include "../class/class_array.h"
#include "cstdio"
#include "cstdlib"
#include "vector"
#include "mutex"
#include "string"
#include "thread"
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#ifndef NET_IP_H
	#define NET_IP_H
	#define NET_IP_MTU 256
	#define NET_IP_CONNECTION_TYPE_UNDEFINED	0
	#define NET_IP_CONNECTION_TYPE_UDP		1
	#define NET_IP_CONNECTION_TYPE_TCP		2
	#define NET_IP_SERVER_RECEIVE_PORT		50000
	#define NET_IP_CLIENT_RECEIVE_PORT		50001
	// refer to net_const.h for how the special characters are delegated
	#define NET_IP_RETURN_ADDRESS_START		(char*)"\1e"
	#define NET_IP_RETURN_ADDRESS_END		(char*)"\1f"
	class net_ip_write_buffer_t{
	public:
		bool sent;
		unsigned long int position;
		int connection_info_id;
		net_ip_write_buffer_t(std::string, unsigned long int, int);
		std::vector<std::string> gen_string_vector();
		unsigned long int packet_id;
		std::string data;
	};
	class net_ip_read_buffer_t{
	public:
		net_ip_read_buffer_t(int);
		~net_ip_read_buffer_t();
		void parse_packet_segment(std::string);
		bool finished();
		std::string gen_string();
		unsigned long int packet_id;
		std::vector<std::string> data_vector;
	};
	class net_ip_t{
	private:
		unsigned long int total_sent_bytes;
		std::vector<net_ip_read_buffer_t> read_buffer;
		std::vector<net_ip_write_buffer_t> write_buffer;
		UDPsocket outbound;
		UDPpacket *outbound_packet;
		UDPsocket inbound; // no TCP yet
		UDPpacket *inbound_packet;
		bool receive_check_read_array(std::string, unsigned long int);
		int connection_info_id;
	public:
		int loop_send_mt();
		int loop_receive_mt();
		net_ip_connection_info_t self_info;
		int init(int,char**, int);
		std::string read(std::string);
		void write(std::string, int, unsigned long int);
		void write_array_vector(std::vector<std::string>, int); // net_ip_connection_info ID
		int send_now(net_ip_write_buffer_t*);
		std::string receive_now();
		void loop();
		void loop_send();
		void loop_receive();
		void close();
	};
#endif
