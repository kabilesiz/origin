#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#define BLOK_BOYUTU 2048	
struct sunucu {
	long int sunucuId;
	long int dosya_boyutu;
	char dosya[BLOK_BOYUTU];
};