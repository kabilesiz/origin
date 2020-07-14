#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#define BLOK_BOYUTU 2048	
struct istemci {
	int istemciId;
	long int dosya_boyutu;
	char dosya[BLOK_BOYUTU];
};