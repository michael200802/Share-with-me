#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "port.h"
#include "packet.h"

int main(int argc, char ** argv)
{ 
    int sockfd;
    struct sockaddr_in sockaddr;
    struct hostent * hostentry;
    struct in_addr ** addr_list;
    char ipaddr[INET_ADDRSTRLEN];

    char * client_name;
    namesize_t namesize;

    psize_t nfiles;
    packet_type_t packtype;
    packetsize_t packsize;
    char * path;
    char * filename;
    FILE * file;
    char buffer[FILE_BLOCK_SIZE];

    //get name from console
    if(argc == 1)
    {
        puts("User name wasn't given.");
        return -1;
    }
    client_name = argv[1];
    {
        size_t size;

        for(size = 1; client_name[size] != '\0'; size++);

        if(size > NAMESIZE_MAX)
        {
            puts("Name too long.");
            return -1;
        }

        namesize = size;
    }

    hostentry = gethostbyname("adachitoshimamura.ddns.net");

    addr_list = (struct in_addr **)hostentry->h_addr_list;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        puts("Couldn't create new socket.");
        return -1;
    }

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(PORT);
    sockaddr.sin_addr.s_addr = (*addr_list)->s_addr;
    for(size_t i = 0; i < sizeof(sockaddr.sin_zero)/sizeof(sockaddr.sin_zero[0]); i++)
    {
        sockaddr.sin_zero[i] = 0;
    }

    if(connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1)
    {
        puts("Couldn't connect.");
        close(sockfd);
        return -1;
    }

    if(sendpack(sockfd, &namesize, sizeof(namesize)) == false)
    {
        puts("Error at login: Couldn't send the namesize.");
        close(sockfd);
        return -1;
    }

    if(sendpack(sockfd, client_name, namesize) == false)
    {
        puts("Error at login: Couldn't send the client's name");
        close(sockfd);
        return -1;
    }

    inet_ntop(AF_INET, &(*addr_list)->s_addr, ipaddr, INET_ADDRSTRLEN);

    printf("server IP: %s\n",ipaddr);


    while(1)
    {

        packtype = PT_REQUEST_FOR_UPDATE;
        if(sendpack(sockfd, &packtype, sizeof(packtype)) == false)
        {
            puts("Error at requesting an update: Couldn't request for an update.");
            close(sockfd);
            return -1;
        }

        if(recvpack(sockfd, &nfiles, sizeof(nfiles)) == false)
        {
            puts("Error at updating: Couldn't get number of files to be recieved.");
            close(sockfd);
            return -1;
        }

        for(psize_t i = 0; i < nfiles; i++)
        {
            if(recvpack(sockfd, &packsize, sizeof(packsize)) == false)
            {
                puts("Error at updating: Couldn't get the size of the packet/file to be recieved.");
                close(sockfd);
                return -1;
            }

            if(packsize.name_len == 0)
            {
                continue;
            }

            filename = (char*)malloc(packsize.name_len);

            if(recvpack(sockfd, filename, packsize.name_len) == false)
            {
                puts("Error at updating: Couldn't receive the name of the file.");
                close(sockfd);
                return -1;
            }

            mkdir(client_name,0777);

            path = (char*)malloc(namesize + sizeof("/")-1 + packsize.name_len);
            strcpy(path, client_name);
            strcat(path, "/");
            strcat(path, filename);

            file = fopen(path, "w");
            if(file == NULL)
            {
                puts("Error at writing: Couldn't open a file");
                close(sockfd);
                fclose(file);
                return -1;
            }

            if(packsize.file_size != FILE_BLOCK_SIZE)
            {
                if(recvpack(sockfd, buffer, packsize.file_size%FILE_BLOCK_SIZE) == false)
                {
                    puts("Error at updating: Couldn't get the first block of the file.");
                    close(sockfd);
                    fclose(file);
                    return -1;
                }
                if(fwrite(buffer, sizeof(char), packsize.file_size%FILE_BLOCK_SIZE, file) != packsize.file_size%FILE_BLOCK_SIZE)
                {
                    puts("Error at writing: Couldn't write in a file");
                    close(sockfd);
                    fclose(file);
                    return -1;
                }
                packsize.file_size -= packsize.file_size%FILE_BLOCK_SIZE;
            }

            while(packsize.file_size != 0)
            {
                if(recvpack(sockfd, buffer, FILE_BLOCK_SIZE) == false)
                {
                    puts("Error at updating: Couldn't get another block of the file.");
                    close(sockfd);
                    fclose(file);
                    return -1;
                }
                if(fwrite(buffer, sizeof(char), FILE_BLOCK_SIZE, file) != FILE_BLOCK_SIZE)
                {
                    puts("Error at writing: Couldn't write in a file");
                    close(sockfd);
                    fclose(file);
                    return -1;
                }
                packsize.file_size -= FILE_BLOCK_SIZE;
            }

            fclose(file);

            free(filename);
            free(path);
        }

        sleep(1);
    }

    close(sockfd);

    return 0;
}