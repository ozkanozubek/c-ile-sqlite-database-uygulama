#include <stdio.h>
#include <stdlib.h>
#include "veritabani.h"

int main() {
    int secim;

    do {
        printf("\n=================================");
        printf("\n  PERSONEL VERITABANI SISTEMI    ");
        printf("\n=================================");
        printf("\n1 - Yeni Personel Kaydet");
        printf("\n2 - T.C. ile Personel Ara");
        printf("\n3 - T.C. ile Personel Sil");
        printf("\n4 - T.C. ile Personel Guncelle");
        printf("\n0 - Programdan Cikis");
        printf("\n=================================");
        printf("\nSeciminiz: ");
        
        if (scanf("%d", &secim) != 1) {
            while (getchar() != '\n');
            secim = -1;
        }

        switch (secim) {
            case 1:
                personel_kaydet();
                break;
            case 2:
                personel_ara_tc();
                break;
            case 3:
                personel_sil_tc();
                break;
            case 4:
                personel_guncelle_tc();
                break;
            case 0:
                printf("\nProgram sonlandiriliyor. Iyi gunler!\n");
                break;
            default:
                printf("\n[UYARI] Gecersiz secim! Lutfen tekrar deneyin.\n");
        }
        
    } while (secim != 0);

    system("PAUSE");
    return 0;
}

