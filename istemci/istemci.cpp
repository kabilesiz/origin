#include "istemci.h"
int main(int argc, char **argv)
{
	struct sockaddr_in sunucu_adresi, istemci_adresi;	struct stat st;	  struct istemci blok;	  struct timeval zaman_asimi = {0, 0};
	char gonderilen_komut[50], dosya_adi[20], komut[10], onayli_gonderim[4] = "ACK";
	ssize_t okunma_sayaci = 0, dosya_uzunlugu = 0;
	off_t dosya_boyutu = 0;
	long int gonderilen_sayisi = 0;
	int soket, ack_recv = 0, yasam = 1;
	memset(onayli_gonderim, 0, sizeof(onayli_gonderim));memset(&sunucu_adresi, 0, sizeof(sunucu_adresi));memset(&istemci_adresi, 0, sizeof(istemci_adresi));
	sunucu_adresi.sin_family = AF_INET;sunucu_adresi.sin_port = htons(atoi(argv[2]));sunucu_adresi.sin_addr.s_addr = inet_addr(argv[1]);
	soket = socket(AF_INET, SOCK_DGRAM, 0);
	FILE *dosya_islemcisi;
	while (yasam) 
	{
		memset(gonderilen_komut, 0, sizeof(gonderilen_komut));
		memset(komut, 0, sizeof(komut));
		memset(dosya_adi, 0, sizeof(dosya_adi));
		printf("\n Sunucudan alınmak istenen dosyayı alabilmek için komutu [cek dosyAdi.uzantisi] şeklinde veriniz. \n (Sunucudan cıkmak için cikis komutunu kullanınız.)\n \n --> ");		
		scanf(" %[^\n]%*c", gonderilen_komut);	
		sscanf(gonderilen_komut, "%s %s", komut, dosya_adi);
		sendto(soket, gonderilen_komut, sizeof(gonderilen_komut), 0, (struct sockaddr *) &sunucu_adresi, sizeof(sunucu_adresi));
		if ((strcmp(komut, "cek") == 0) && (dosya_adi[0] != '\0' ))
		{
			long int total_blok = 0;
			long int gonderim_takibi = 0, i = 0;
			zaman_asimi.tv_sec = 2;
			setsockopt(soket, SOL_SOCKET, SO_RCVTIMEO, (char *)&zaman_asimi, sizeof(struct timeval));
			recvfrom(soket, &(total_blok), sizeof(total_blok), 0, (struct sockaddr *) &istemci_adresi, (socklen_t *) &dosya_uzunlugu);
			zaman_asimi.tv_sec = 0;
            setsockopt(soket, SOL_SOCKET, SO_RCVTIMEO, (char *)&zaman_asimi, sizeof(struct timeval));
			if (total_blok > 0) 
			{
				sendto(soket, &(total_blok), sizeof(total_blok), 0, (struct sockaddr *) &sunucu_adresi, sizeof(sunucu_adresi));
				dosya_islemcisi = fopen(dosya_adi, "wb");
				for (i = 1; i <= total_blok; i++)
				{
					memset(&blok, 0, sizeof(blok));
					recvfrom(soket, &(blok), sizeof(blok), 0, (struct sockaddr *) &istemci_adresi, (socklen_t *) &dosya_uzunlugu);
					sendto(soket, &(blok.istemciId), sizeof(blok.istemciId), 0, (struct sockaddr *) &sunucu_adresi, sizeof(sunucu_adresi));
					if ((blok.istemciId < i) || (blok.istemciId > i)) i--;
					else 
					{
						fwrite(blok.dosya, 1, blok.dosya_boyutu, dosya_islemcisi);
						gonderim_takibi += blok.dosya_boyutu;
					}
					if (i == total_blok) printf("\n   Dosya sunucudan alindi. Dosyayı kullanılan (istemci) klasorun altında bulabilirsiniz.\n");
				}
				fclose(dosya_islemcisi);
			}
			else printf("\n   Boyle bir dosya bulunamadi.\n");
		}
		else if (strcmp(komut, "cikis") == 0) 
		{
			printf("\n   Server ile olan baglanti sonlandirildi.\n\n");
			close(soket);
			yasam = 0;
		}
		else
		{
			printf("\n   Tanimlanmayan sorgu!\n");
			printf("\n   Tanimli olan sorgular : \n");
			printf("\n   cek --> sunucudan istenen dosyaları almak için kullanılır. Kullanımı : ( cek abc.png)\n");
			printf("\n   cikis --> sunucuyla olan iletisimi koparmak için kullanılır. Kullanımı : (cikis))\n");
		}
		
	}	
	exit(EXIT_SUCCESS);
}