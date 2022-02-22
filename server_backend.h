#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include "packet.h"
#include "port.h"

#include <sys/mman.h>

#include "list_clients.h"
#include "list_groups.h"

#include <stdbool.h>

#define MAXN_FD 1021

bool init_server(list_t * _clients, list_t * _groups, size_t nsubroutines);