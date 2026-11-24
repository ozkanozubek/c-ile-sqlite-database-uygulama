#ifndef VERITABANI_H
#define VERITABANI_H

#include "sqlite3.h"

// Veritabaný dosya yolu sabiti
#define DB_YOLU "C:\\Users\\dem\\Desktop\\cprojelerim\\sqliteornekpro\\personel.db"

// Fonksiyon Prototipleri
int veritabani_baglan(sqlite3 **db);
void personel_kaydet();
void personel_ara_tc();
void personel_sil_tc();
void personel_guncelle_tc();

#endif

