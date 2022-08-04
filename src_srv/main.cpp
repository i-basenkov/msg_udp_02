
#include <iostream>
#include <fstream>

#include <tuple>

#include <unistd.h>
#include <charconv>
#include <atomic>
#include <future>

#include <signal.h>

#include "../lib_msg/msgthreads.h"

#include <ctime>
#include <cstddef>

#include "srv_net.h"

using namespace msg;

static std::atomic<int> stop_prog{0};
static void sighandler(int){ stop_prog = 1; }


int main()
{

	signal(SIGINT, sighandler);


	net::udp_interface_t srv_net(ip_addr(127, 0, 0, 1), port(7002), port(7001));

	srv_net.thread = std::thread(worker_t<file_send::SrvNet>(srv_net));

	while (!stop_prog)
	{

		usleep(1000 * 1000);

	}

	std::cout << "-------- Сервер завершает работу --------" << std::endl;

	usleep(2000 * 1000);

	srv_net.stop |= 0x01;
	srv_net.join();

	return 0;
}
