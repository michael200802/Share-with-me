#include "packet.h"

bool sendpack(const int sockfd, void * _pack, size_t size)
{
    byte_t * pack = _pack;
    ssize_t sent_bytes;

    while(size != 0)
    {
        sent_bytes = send(sockfd, pack, size, 0);
        if(sent_bytes == -1)
        {
            if(errno != EINTR)
            {
                return false;
            }
        }
        else
        {
            pack += sent_bytes;
            size -= sent_bytes;
        }
    }
    return true;   
}

bool recvpack(const int sockfd, void * _pack, size_t size)
{
    byte_t * pack = _pack;
    ssize_t recv_bytes;

    while(size != 0)
    {
        recv_bytes = recv(sockfd, pack, size, 0);
        if(recv_bytes == -1)
        {
            if(errno != EINTR)
            {
                return false;
            }
        }
        else
        {
            pack += recv_bytes;
            size -= recv_bytes;
        }
    }
    return true;
}