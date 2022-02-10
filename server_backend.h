#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "list_gropus.h"

#define MAXN_FD 1021

bool init_server(list_t * _clients, list_t * _groups, size_t nsubroutines);