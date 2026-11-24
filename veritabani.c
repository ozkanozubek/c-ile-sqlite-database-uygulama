#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veritabani.h"

// Windows API prototipleri (Derleyici hatasý almamak için)
__declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int CP, unsigned long f, const char *s, int cb, wchar_t *w, int cch);
__declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int CP, unsigned long f, const wchar_t *w, int cch, char *s, int cb, const char *d, int *u);

// Arabellek temizleme aracý
void arabellegi_temizle() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Konsoldan (857) SQLite'a (65001 - UTF-8) çevirici
void utf8_e_cevir(const char *kaynak, char *hedef, int hedef_boyut) {
    wchar_t gecici[256];
    MultiByteToWideChar(857, 0, kaynak, -1, gecici, 256);
    WideCharToMultiByte(65001, 0, gecici, -1, hedef, hedef_boyut, NULL, NULL);
}

// SQLite'tan (65001 - UTF-8) Konsola (857) geri çevirici (Ekranda düzgün görmek için)
void konsola_cevir(const char *kaynak, char *hedef, int hedef_boyut) {
    wchar_t gecici[256];
    MultiByteToWideChar(65001, 0, kaynak, -1, gecici, 256);
    WideCharToMultiByte(857, 0, gecici, -1, hedef, hedef_boyut, NULL, NULL);
}

// Ortak Veritabaný Baðlantý Fonksiyonu
int veritabani_baglan(sqlite3 **db) {
    if (sqlite3_open(DB_YOLU, db) != SQLITE_OK) {
        fprintf(stderr, "\n[HATA] Veritabani acilamadi: %s\n", sqlite3_errmsg(*db));
        return 0;
    }
    return 1;
}

// 1. KAYIT EKLEME FONKSÝYONU (INSERT)
void personel_kaydet() {
    sqlite3 *db;
    sqlite3_stmt *res;
    char g_tc[20], g_ad[50], g_soyad[50], g_egitim[50];
    char tc[20], ad[50], soyad[50], egitimdurumu[50];

    printf("\n--- YENI PERSONEL KAYDI ---\n");
    printf("T.C. Kimlik No: "); scanf("%19s", g_tc); arabellegi_temizle();
    printf("Adiniz: ");         scanf("%49[^\n]", g_ad); arabellegi_temizle();
    printf("Soyadiniz: ");      scanf("%49[^\n]", g_soyad); arabellegi_temizle();
    printf("Egitim Durumu: ");  scanf("%49[^\n]", g_egitim); arabellegi_temizle();

    utf8_e_cevir(g_tc, tc, 20);
    utf8_e_cevir(g_ad, ad, 50);
    utf8_e_cevir(g_soyad, soyad, 50);
    utf8_e_cevir(g_egitim, egitimdurumu, 50);

    if (!veritabani_baglan(&db)) return;

    const char *sql = "INSERT INTO temelbilgiler (tc, ad, soyad, egitimdurumu) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        fprintf(stderr, "\n[HATA] Sorgu hazirlama hatasi: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(res, 1, tc, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(res, 2, ad, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(res, 3, soyad, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(res, 4, egitimdurumu, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(res) == SQLITE_DONE) {
        printf("\n[BASARILI] Personel veritabanina kaydedildi!\n");
    } else {
        fprintf(stderr, "\n[HATA] Kayit eklenemedi: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
}

// 2. T.C. ÝLE GÜVENLÝ PERSONEL ARAMA FONKSÝYONU (SELECT)
void personel_ara_tc() {
    sqlite3 *db;
    sqlite3_stmt *res;
    char g_aranan_tc[20], aranan_tc[20];
    char e_tc[20], e_ad[50], e_soyad[50], e_egitim[50];

    printf("\n--- T.C. ILE PERSONEL ARAMA ---\n");
    printf("Aranacak T.C. Kimlik No: ");
    scanf("%19s", g_aranan_tc);
    arabellegi_temizle();

    utf8_e_cevir(g_aranan_tc, aranan_tc, 20);

    if (!veritabani_baglan(&db)) return;

    const char *sql = "SELECT tc, ad, soyad, egitimdurumu FROM temelbilgiler WHERE tc = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        fprintf(stderr, "\n[HATA] Sorgu hatasi: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(res, 1, aranan_tc, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(res) == SQLITE_ROW) {
        printf("\n--- PERSONEL BILGILERI ---\n");
        
        const char *db_tc    = (const char*)sqlite3_column_text(res, 0);
        const char *db_ad    = (const char*)sqlite3_column_text(res, 1);
        const char *db_soyad = (const char*)sqlite3_column_text(res, 2);
        const char *db_egitim = (const char*)sqlite3_column_text(res, 3);

        konsola_cevir(db_tc, e_tc, 20);
        konsola_cevir(db_ad, e_ad, 50);
        konsola_cevir(db_soyad, e_soyad, 50);
        konsola_cevir(db_egitim, e_egitim, 50);

        printf("T.C. No       : %s\n", e_tc);
        printf("Adý           : %s\n", e_ad);
        printf("Soyadý        : %s\n", e_soyad);
        printf("Eðitim Durumu : %s\n", e_egitim);
    } else {
        printf("\n[UYARI] Bu T.C. numarasina ait kayit bulunamadi.\n");
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
}

// 3. T.C. ÝLE GÜVENLÝ PERSONEL SÝLME FONKSÝYONU (DELETE)
void personel_sil_tc() {
    sqlite3 *db;
    sqlite3_stmt *res;
    char g_aranan_tc[20], aranan_tc[20];

    printf("\n--- T.C. ILE PERSONEL SILME ---\n");
    printf("Silinecek Personelin T.C. Kimlik No: ");
    scanf("%19s", g_aranan_tc);
    arabellegi_temizle();

    utf8_e_cevir(g_aranan_tc, aranan_tc, 20);

    if (!veritabani_baglan(&db)) return;

    const char *sql = "DELETE FROM temelbilgiler WHERE tc = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        fprintf(stderr, "\n[HATA] Sorgu hatasi: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(res, 1, aranan_tc, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(res) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0) {
            printf("\n[BASARILI] Personel kaydi veritabanindan tamamen silindi!\n");
        } else {
            printf("\n[UYARI] Bu T.C. numarasina ait silinecek bir kayit bulunamadi.\n");
        }
    } else {
        fprintf(stderr, "\n[HATA] Silme islemi basarisiz: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
}

// 4. T.C. ÝLE GÜVENLÝ PERSONEL GÜNCELLEME FONKSÝYONU (UPDATE)
void personel_guncelle_tc() {
    sqlite3 *db;
    sqlite3_stmt *res;
    char g_aranan_tc[20], aranan_tc[20];
    char g_yeni_ad[50], g_yeni_soyad[50], g_yeni_egitim[50];
    char yeni_ad[50], yeni_soyad[50], yeni_egitim[50];

    printf("\n--- T.C. ILE PERSONEL GUNCELLEME ---\n");
    printf("Güncellenecek Personelin T.C. Kimlik No: ");
    scanf("%19s", g_aranan_tc);
    arabellegi_temizle();

    utf8_e_cevir(g_aranan_tc, aranan_tc, 20);

    if (!veritabani_baglan(&db)) return;

    // Önce kaydýn varlýðýný kontrol edelim
    const char *sql_kontrol = "SELECT tc FROM temelbilgiler WHERE tc = ?;";
    if (sqlite3_prepare_v2(db, sql_kontrol, -1, &res, 0) == SQLITE_OK) {
        sqlite3_bind_text(res, 1, aranan_tc, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(res) != SQLITE_ROW) {
            printf("\n[UYARI] Bu T.C. numarasina ait güncellenecek bir kayit bulunamadi.\n");
            sqlite3_finalize(res);
            sqlite3_close(db);
            return;
        }
        sqlite3_finalize(res);
    }

    printf("\nYeni Adý: ");           scanf("%49[^\n]", g_yeni_ad); arabellegi_temizle();
    printf("Yeni Soyadý: ");        scanf("%49[^\n]", g_yeni_soyad); arabellegi_temizle();
    printf("Yeni Eðitim Durumu: ");  scanf("%49[^\n]", g_yeni_egitim); arabellegi_temizle();

    utf8_e_cevir(g_yeni_ad, yeni_ad, 50);
    utf8_e_cevir(g_yeni_soyad, yeni_soyad, 50);
    utf8_e_cevir(g_yeni_egitim, yeni_egitim, 50);

    const char *sql_update = "UPDATE temelbilgiler SET ad = ?, soyad = ?, egitimdurumu = ? WHERE tc = ?;";
    if (sqlite3_prepare_v2(db, sql_update, -1, &res, 0) != SQLITE_OK) {
        fprintf(stderr, "\n[HATA] Sorgu hatasi: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(res, 1, yeni_ad, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(res, 2, yeni_soyad, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(res, 3, yeni_egitim, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(res, 4, aranan_tc, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(res) == SQLITE_DONE) {
        printf("\n[BASARILI] Personel bilgileri baþarýyla güncellendi!\n");
    } else {
        fprintf(stderr, "\n[HATA] Güncelleme basarisiz: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
}

