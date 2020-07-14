#include "sunucu.h"
int main(int argc, char **argv)
{
	struct sockaddr_in sunucu_adresi, istemci_adresi;	struct stat st;	struct sunucu blok;	struct timeval zaman_asimi = {0, 0};
	char istemci_mesaji[BLOK_BOYUTU], istenilen_dosyaAdi[20], girilen_komut[10];
	ssize_t okunma_sayaci, dosya_uzunlugu;
	off_t dosya_boyutu; 	
	long int gonderilen_sayisi = 0;  
	int soket, yasam = 1, onaylanan_gonderim = 0;
	memset(&sunucu_adresi, 0, sizeof(sunucu_adresi));
	sunucu_adresi.sin_family = AF_INET;	sunucu_adresi.sin_port = htons(atoi(argv[1]));	sunucu_adresi.sin_addr.s_addr = INADDR_ANY;
	soket = socket(AF_INET, SOCK_DGRAM, 0);
	bind(soket, (struct sockaddr *) &sunucu_adresi, sizeof(sunucu_adresi));
	FILE *dosya_islemi;
	while(yasam) {
		printf("\nSunucu: istemci baglantisi icin bekleniyor.\n");
		memset(istemci_mesaji, 0, sizeof(istemci_mesaji));
		memset(girilen_komut, 0, sizeof(girilen_komut));
		memset(istenilen_dosyaAdi, 0, sizeof(istenilen_dosyaAdi));
		dosya_uzunlugu = sizeof(istemci_adresi);
		okunma_sayaci = recvfrom(soket, istemci_mesaji, BLOK_BOYUTU, 0, (struct sockaddr *) &istemci_adresi, (socklen_t *) &dosya_uzunlugu);
		printf("\n   Sunucu: Istenilen sorgu --> %s\n", istemci_mesaji);
		sscanf(istemci_mesaji, "%s %s", girilen_komut, istenilen_dosyaAdi);
		if ((strcmp(girilen_komut, "cek") == 0) && (istenilen_dosyaAdi[0] != '\0')) {
			printf("\n   Sunucu: cek sorgusu [ '%s' ] ismi ile calistirildi.\n", istenilen_dosyaAdi);
			if (access(istenilen_dosyaAdi, F_OK) == 0) {			//Check if file exist
				int total_blok = 0, gonderilen_blok = 0, tasinamayan_blok = 0, zaman_asimi_flag = 0;
				long int i = 0;
				stat(istenilen_dosyaAdi, &st);
				dosya_boyutu = st.st_size;
				zaman_asimi.tv_sec = 2;			
				zaman_asimi.tv_usec = 0;
				setsockopt(soket, SOL_SOCKET, SO_RCVTIMEO, (char *)&zaman_asimi, sizeof(struct timeval));  
				dosya_islemi = fopen(istenilen_dosyaAdi, "rb");       
				if ((dosya_boyutu % BLOK_BOYUTU) != 0) total_blok = (dosya_boyutu / BLOK_BOYUTU) + 1;				
				else total_blok = (dosya_boyutu / BLOK_BOYUTU);	
				dosya_uzunlugu = sizeof(istemci_adresi);
				sendto(soket, &(total_blok), sizeof(total_blok), 0, (struct sockaddr *) &istemci_adresi, sizeof(istemci_adresi));	
				recvfrom(soket, &(gonderilen_sayisi), sizeof(gonderilen_sayisi), 0, (struct sockaddr *) &istemci_adresi, (socklen_t *) &dosya_uzunlugu);
				while (gonderilen_sayisi != total_blok)
				{
					sendto(soket, &(total_blok), sizeof(total_blok), 0, (struct sockaddr *) &istemci_adresi, sizeof(istemci_adresi)); 
					recvfrom(soket, &(gonderilen_sayisi), sizeof(gonderilen_sayisi), 0, (struct sockaddr *) &istemci_adresi, (socklen_t *) &dosya_uzunlugu);
					gonderilen_blok++;
					if (gonderilen_blok == 20) 
					{
						zaman_asimi_flag = 1;
						break;
					}
				}
				for (i = 1; i <= total_blok; i++)
				{
					memset(&blok, 0, sizeof(blok));
					gonderilen_sayisi = 0;
					blok.sunucuId = i;
					blok.dosya_boyutu = fread(blok.dosya, 1, BLOK_BOYUTU, dosya_islemi);
					sendto(soket, &(blok), sizeof(blok), 0, (struct sockaddr *) &istemci_adresi, sizeof(istemci_adresi));		
					recvfrom(soket, &(gonderilen_sayisi), sizeof(gonderilen_sayisi), 0, (struct sockaddr *) &istemci_adresi, (socklen_t *) &dosya_uzunlugu);
					while (gonderilen_sayisi != blok.sunucuId)
					{
						sendto(soket, &(blok), sizeof(blok), 0, (struct sockaddr *) &istemci_adresi, sizeof(istemci_adresi));
						recvfrom(soket, &(gonderilen_sayisi), sizeof(gonderilen_sayisi), 0, (struct sockaddr *) &istemci_adresi, (socklen_t *) &dosya_uzunlugu);
						gonderilen_blok++;
						if (gonderilen_blok == 200) 
						{
							zaman_asimi_flag = 1;
							break;
						}
					}
					gonderilen_blok = 0;
					tasinamayan_blok = 0;
					if (zaman_asimi_flag == 1) 
					{
						printf("\n   Dosya gonderilemedi !\n");
						break;
					}
					if (total_blok == gonderilen_sayisi) printf("\n   Dosya Aktarımı [ '%s' ] başarılı oldu.\n",istenilen_dosyaAdi);
				}
				fclose(dosya_islemi);
				zaman_asimi.tv_sec = 0;
				zaman_asimi.tv_usec = 0;
				setsockopt(soket, SOL_SOCKET, SO_RCVTIMEO, (char *)&zaman_asimi, sizeof(struct timeval));
			}
			else printf("\n   %s isimli bir dosya bulunamadi.\n",istenilen_dosyaAdi);
		}
		else if (strcmp(girilen_komut, "cikis") == 0) 
		{
			printf("\n   Sunucu oturumu sonlandırıldı.\n\n");
			close(soket);
			yasam = 0;
		}
		else printf("\n   Sunucu: Bilinmeyen sorgu, lutfen sistem için tanımlanmis olan sorgular ile tekrar istek yollayiniz.\n");
	}
	exit(EXIT_SUCCESS);
}