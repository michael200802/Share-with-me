#pragma once

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include <errno.h>

typedef unsigned char byte_t;

//size_t for names
typedef unsigned char namesize_t;
#define NAMESIZE_MAX UCHAR_MAX

//portable size_t
typedef int64_t psize_t;

//type of the packet sent by the client
enum packet_type_t {PT_LOGIN, PT_REQUEST_FOR_UPDATE, PT_LOGOUT};
typedef char packet_type_t;

//packsize packet
typedef struct
{
    psize_t file_size;
    psize_t name_len;
}packetsize_t;

#define FILE_BLOCK_SIZE 100

bool sendpack(const int sockfd, void * _pack, size_t size);

bool recvpack(const int sockfd, void * _pack, size_t size);