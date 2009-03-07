// Copyright 2007 Bret Taylor (btaylor@gmail.com)
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static void SystemFatal(const char* message);
static bool ReadFromFile(int fd);

static const unsigned short kPort = 8080;

// A simple example application to demonstrate the use of epoll. I found the
// examples lacking when I was going through the epoll documentation available
// on the web, so hopefully this will be useful to someone else in the future.
//
// This runs a simple, single-threaded server with non-blocking, edge-triggered
// I/O to handle simultaneous connections. We simply echo back what is sent to
// us from clients, and we never terminate (i.e., use Ctrl-C to kill the
// server).
int main(int argc, char* argv[]) {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
	SystemFatal("socket");
    }

    // SO_REUSEADDR ensures we can reuse the port instantly if the process
    // is killed while the port is in use
    int arg = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &arg,
		   sizeof(arg)) == -1) {
	SystemFatal("setsockopt");
    }

    // Make our server non-blocking
    if (fcntl(server, F_SETFL, O_NONBLOCK | fcntl(server, F_GETFL, 0)) == -1) {
	SystemFatal("fcntl");
    }

    // Bind to our specified port
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(kPort);
    if (bind(server, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
	SystemFatal("bind");
    }

    // Listen for connections
    if (listen(server, SOMAXCONN) == -1) {
	SystemFatal("listen");
    }

    // Create our epoll file descriptor
    const int max_events = 16;
    int epoll_fd = epoll_create(max_events);
    if (epoll_fd == -1) {
	SystemFatal("epoll_create");
    }

    // Add our server fd to the epoll event loop
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
    event.data.fd = server;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server, &event) == -1) {
	SystemFatal("epoll_ctl");
    }

    // Execute the epoll event loop
    while (true) {
	struct epoll_event events[max_events];
	int num_fds = epoll_wait(epoll_fd, events, max_events, -1);

	for (int i = 0; i < num_fds; i++) {
	    // Case 1: Error condition
	    if (events[i].events & (EPOLLHUP | EPOLLERR)) {
		fputs("epoll: EPOLLERR", stderr);
		close(events[i].data.fd);
		continue;
	    }
	    assert(events[i].events & EPOLLIN);

	    // Case 2: Our server is receiving a connection
	    if (events[i].data.fd == server) {
		struct sockaddr remote_addr;
		socklen_t addr_size = sizeof(remote_addr);
		int connection = accept(server, &remote_addr, &addr_size);
		if (connection == -1) {
		    if (errno != EAGAIN && errno != EWOULDBLOCK) {
			perror("accept");
		    }
		    continue;
		}

		// Make the connection non-blocking
		if (fcntl(connection, F_SETFL,
			  O_NONBLOCK | fcntl(connection, F_GETFL, 0)) == -1) {
		    SystemFatal("fcntl");
		}

		// Add the connection to our epoll loop
		event.data.fd = connection;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection,
			      &event) == -1) {
		    SystemFatal("epoll_ctl");
		}
		continue;
	    }

	    // Case 3: One of our connections has read data
	    if (!ReadFromFile(events[i].data.fd)) {
		// epoll will remove the fd from its set
		// automatically when the fd is closed
		close(events[i].data.fd);
	    }
	}
    }

    close(server);
}

// Reads data from the given file descriptor until we would block (EAGAIN)
// or we hit the end of the file. We return true if we hit EAGAIN, and we
// return false if we hit the end of the file or encounter an unexpected
// error.
static bool ReadFromFile(int fd) {
    while (true) {
	char buffer[1024];
	int bytes = read(fd, buffer, sizeof(buffer));
	if (bytes == -1) {
	    if (errno == EAGAIN) {
		return true;
	    } else {
		perror("read");
		return false;
	    }
	}
	if (bytes == 0) {
	    return false;
	}

	// Write the data to stdout
	if (write(1, buffer, bytes) == -1) {
	    SystemFatal("write");
	}
    }
}

// Prints the error stored in errno and aborts the program.
static void SystemFatal(const char* message) {
    perror(message);
    abort();
}

