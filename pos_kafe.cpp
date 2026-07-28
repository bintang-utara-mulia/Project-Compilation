/*
============================================================
  KAFE KODING SEJAHTERA - Point of Sales System
  Version 2.0
  
  Konsep C++ yang digunakan:
  - BAB 1 : Variabel, Tipe Data, Operator, Input/Output
  - BAB 2 : Percabangan (if/else, switch-case)
  - BAB 3 : Perulangan (for, while, do-while)
  - EXTRA  : Struct, Vector, Function, File I/O (fstream)
============================================================
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <limits>

using namespace std;

// ==============================================================
// SECTION 1: STRUKTUR DATA (STRUCT)
// Mendefinisikan blueprint untuk data Menu dan Transaksi
// ==============================================================

struct MenuItem {
    int id;
    string kategori;
    string nama;
    double harga;
    bool tersedia;
};

struct ItemKeranjang {
    MenuItem menu;
    int jumlah;
    double subTotal;
};

struct Member {
    string nama;
    string kode;
    int poin;
    bool aktif;
};

struct Transaksi {
    string idTransaksi;
    string tanggal;
    string namaCashier;
    vector<ItemKeranjang> keranjang;
    double totalBelanja;
    double diskon;
    double totalBayar;
    double uangBayar;
    double kembalian;
    bool adaMember;
    string namaMember;
};

// ==============================================================
// SECTION 2: DATA GLOBAL
// ==============================================================

// Daftar Menu Kafe
vector<MenuItem> daftarMenu = {
    // ID, Kategori, Nama, Harga, Tersedia
    {1,  "Kopi",    "Kopi Hitam",       15000, true},
    {2,  "Kopi",    "Cappuccino",        28000, true},
    {3,  "Kopi",    "Caffe Latte",       30000, true},
    {4,  "Kopi",    "Espresso",          20000, true},
    {5,  "Kopi",    "Cold Brew",         35000, true},
    {6,  "Non-Kopi","Matcha Latte",      32000, true},
    {7,  "Non-Kopi","Teh Manis Dingin",  12000, true},
    {8,  "Non-Kopi","Coklat Panas",      25000, true},
    {9,  "Non-Kopi","Jus Alpukat",       28000, true},
    {10, "Non-Kopi","Lemon Tea",         18000, true},
    {11, "Makanan", "Croissant",         22000, true},
    {12, "Makanan", "Sandwich Tuna",     35000, true},
    {13, "Makanan", "Waffle Nutella",    38000, true},
    {14, "Makanan", "Kentang Goreng",    20000, true},
    {15, "Makanan", "Cheesecake Slice",  32000, true},
};

// Daftar Member
vector<Member> daftarMember = {
    {"Budi Santoso",   "MBR001", 150, true},
    {"Siti Rahayu",    "MBR002", 320, true},
    {"Ahmad Fauzi",    "MBR003",  80, true},
    {"Dewi Lestari",   "MBR004", 500, true},
};

// Keranjang Belanja Aktif
vector<ItemKeranjang> keranjang;

// Riwayat transaksi hari ini (untuk laporan)
vector<Transaksi> riwayatTransaksi;

// Counter nomor transaksi
int counterTransaksi = 1;
string namaCashier = "";

// ==============================================================
// SECTION 3: FUNGSI UTILITAS
// ==============================================================

// Mendapatkan timestamp saat ini
string getTanggal() {
    time_t now = time(0);
    tm* timeinfo = localtime(&now);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    return string(buffer);
}

// Mendapatkan ID transaksi unik
string getIDTransaksi() {
    time_t now = time(0);
    tm* t = localtime(&now);
    char buf[25];
    sprintf(buf, "TRX%02d%02d%04d-%03d",
        t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, counterTransaksi);
    counterTransaksi++;
    return string(buf);
}

// Membersihkan layar (cross-platform)
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Format angka menjadi format Rupiah
string formatRupiah(double angka) {
    long long nilai = (long long)angka;
    string s = to_string(nilai);
    int len = s.length();
    string hasil = "";
    int count = 0;
    for (int i = len - 1; i >= 0; i--) {
        hasil = s[i] + hasil;
        count++;
        if (count % 3 == 0 && i != 0) {
            hasil = "." + hasil;
        }
    }
    return "Rp " + hasil;
}

// Mencetak garis pemisah
void printGaris(char c = '=', int panjang = 54) {
    for (int i = 0; i < panjang; i++) cout << c;
    cout << "\n";
}

// Mencetak header utama
void printHeader() {
    clearScreen();
    cout << "\n";
    printGaris('=');
    cout << left;
    cout << "||" << setw(50) << "        KAFE KODING SEJAHTERA" << "||\n";
    cout << "||" << setw(50) << "       Point of Sales System v2.0" << "||\n";
    printGaris('=');
    cout << "  Cashier  : " << namaCashier << "\n";
    cout << "  Waktu    : " << getTanggal() << "\n";
    printGaris('-');
}

// Input integer dengan validasi
int inputInt(const string& prompt, int min, int max) {
    int nilai;
    while (true) {
        cout << prompt;
        if (cin >> nilai && nilai >= min && nilai <= max) {
            return nilai;
        }
        cout << "  [!] Input tidak valid. Masukkan angka " << min << "-" << max << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ==============================================================
// SECTION 4: FUNGSI TAMPILAN MENU
// ==============================================================

void tampilkanDaftarMenu() {
    printGaris('=');
    cout << setw(54) << left << "  DAFTAR MENU" << "\n";
    printGaris('=');

    // BAB 3: Perulangan For untuk menampilkan setiap item menu
    vector<string> kategoriList = {"Kopi", "Non-Kopi", "Makanan"};
    for (const string& kat : kategoriList) {
        cout << "\n  [ " << kat << " ]\n";
        printGaris('-');
        cout << "  " << left << setw(4) << "ID"
             << setw(24) << "Nama Item"
             << setw(16) << "Harga"
             << "Status\n";
        printGaris('-');
        for (const MenuItem& item : daftarMenu) {
            if (item.kategori == kat) {
                string status = item.tersedia ? "Tersedia" : "Habis";
                cout << "  " << setw(4) << item.id
                     << setw(24) << item.nama
                     << setw(16) << formatRupiah(item.harga)
                     << status << "\n";
            }
        }
    }
    printGaris('=');
}

// ==============================================================
// SECTION 5: FUNGSI KERANJANG
// ==============================================================

void tampilkanKeranjang() {
    if (keranjang.empty()) {
        cout << "\n  [ Keranjang masih kosong ]\n";
        return;
    }

    printGaris('=');
    cout << "  KERANJANG PESANAN\n";
    printGaris('=');
    cout << left << setw(4) << "No"
         << setw(24) << "Item"
         << setw(6)  << "Qty"
         << setw(16) << "Harga"
         << "Subtotal\n";
    printGaris('-');

    double total = 0;
    // BAB 3: For loop untuk menampilkan item di keranjang
    for (int i = 0; i < (int)keranjang.size(); i++) {
        const ItemKeranjang& ik = keranjang[i];
        cout << left << setw(4) << (i + 1)
             << setw(24) << ik.menu.nama
             << setw(6)  << ik.jumlah
             << setw(16) << formatRupiah(ik.menu.harga)
             << formatRupiah(ik.subTotal) << "\n";
        total += ik.subTotal;
    }

    printGaris('-');
    cout << right << setw(50) << "TOTAL : " << formatRupiah(total) << "\n";
    printGaris('=');
}

void tambahKeKeranjang() {
    tampilkanDaftarMenu();
    tampilkanKeranjang();

    cout << "\n";
    int idPilih = inputInt("  Masukkan ID Menu (0 = Batal): ", 0, 15);
    if (idPilih == 0) return;

    // BAB 2: Mencari menu berdasarkan ID menggunakan loop dan if
    MenuItem* itemDitemukan = nullptr;
    for (MenuItem& item : daftarMenu) {
        if (item.id == idPilih) {
            itemDitemukan = &item;
            break;
        }
    }

    if (itemDitemukan == nullptr) {
        cout << "  [!] Menu tidak ditemukan!\n";
        return;
    }

    if (!itemDitemukan->tersedia) {
        cout << "  [!] Maaf, item ini sedang habis.\n";
        return;
    }

    int jumlah = inputInt("  Jumlah (" + itemDitemukan->nama + "): ", 1, 50);

    // BAB 2: Cek apakah item sudah ada di keranjang
    bool sudahAda = false;
    for (ItemKeranjang& ik : keranjang) {
        if (ik.menu.id == idPilih) {
            ik.jumlah += jumlah;
            ik.subTotal = ik.menu.harga * ik.jumlah;
            sudahAda = true;
            cout << "  [v] Kuantitas " << itemDitemukan->nama << " diperbarui menjadi " << ik.jumlah << ".\n";
            break;
        }
    }

    // Jika belum ada, tambahkan sebagai item baru
    if (!sudahAda) {
        ItemKeranjang baru;
        baru.menu = *itemDitemukan;
        baru.jumlah = jumlah;
        baru.subTotal = itemDitemukan->harga * jumlah;
        keranjang.push_back(baru);
        cout << "  [v] " << jumlah << "x " << itemDitemukan->nama << " ditambahkan!\n";
    }

    cout << "\n  Tekan ENTER untuk melanjutkan...";
    cin.ignore();
    cin.get();
}

void hapusDariKeranjang() {
    tampilkanKeranjang();
    if (keranjang.empty()) {
        cout << "\n  Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    int noPilih = inputInt("\n  Pilih nomor item yang dihapus (0 = Batal): ", 0, (int)keranjang.size());
    if (noPilih == 0) return;

    cout << "  [v] " << keranjang[noPilih - 1].menu.nama << " dihapus dari keranjang.\n";
    keranjang.erase(keranjang.begin() + noPilih - 1);

    cout << "\n  Tekan ENTER untuk melanjutkan...";
    cin.ignore();
    cin.get();
}

// ==============================================================
// SECTION 6: FUNGSI PEMBAYARAN & STRUK
// ==============================================================

double hitungTotalBelanja() {
    double total = 0;
    for (const ItemKeranjang& ik : keranjang) {
        total += ik.subTotal;
    }
    return total;
}

// Simpan struk ke file .txt
void simpanStrukKeFile(const Transaksi& trx) {
    string namaFile = "struk_" + trx.idTransaksi + ".txt";
    ofstream file(namaFile);
    if (!file.is_open()) return;

    file << "========================================\n";
    file << "        KAFE KODING SEJAHTERA           \n";
    file << "      Jl. A.Yani No. 42, Kota Surabaya  \n";
    file << "       Telp: 0812-3456-7890             \n";
    file << "========================================\n";
    file << "ID Transaksi : " << trx.idTransaksi << "\n";
    file << "Tanggal      : " << trx.tanggal << "\n";
    file << "Cashier      : " << trx.namaCashier << "\n";
    if (trx.adaMember) {
        file << "Member       : " << trx.namaMember << "\n";
    }
    file << "----------------------------------------\n";
    file << left << setw(22) << "Item"
         << setw(6) << "Qty"
         << setw(12) << "Harga"
         << "Subtotal\n";
    file << "----------------------------------------\n";
    for (const ItemKeranjang& ik : trx.keranjang) {
        file << left << setw(22) << ik.menu.nama
             << setw(6) << ik.jumlah
             << setw(12) << formatRupiah(ik.menu.harga)
             << formatRupiah(ik.subTotal) << "\n";
    }
    file << "----------------------------------------\n";
    file << right << setw(40) << "Total Belanja : " << formatRupiah(trx.totalBelanja) << "\n";
    if (trx.diskon > 0) {
        file << right << setw(40) << "Diskon        : -" << formatRupiah(trx.diskon) << "\n";
    }
    file << right << setw(40) << "Total Bayar   : " << formatRupiah(trx.totalBayar) << "\n";
    file << right << setw(40) << "Uang Bayar    : " << formatRupiah(trx.uangBayar) << "\n";
    file << right << setw(40) << "Kembalian     : " << formatRupiah(trx.kembalian) << "\n";
    file << "========================================\n";
    file << "       Terima Kasih Atas Kunjungan!     \n";
    file << "        Selamat Menikmati Minuman :)     \n";
    file << "========================================\n";
    file.close();

    cout << "  [v] Struk disimpan sebagai: " << namaFile << "\n";
}

void prosesTransaksi(bool adaMember, const string& namaMember) {
    if (keranjang.empty()) {
        cout << "\n  [!] Keranjang kosong! Tambahkan pesanan dulu.\n";
        cout << "\n  Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    double totalBelanja = hitungTotalBelanja();
    double diskon = 0;
    string alasanDiskon = "";

    // BAB 2: Logika Diskon Bertingkat (Nested If)
    if (adaMember && totalBelanja >= 100000) {
        diskon = totalBelanja * 0.15;
        alasanDiskon = "Member + Belanja >= Rp100.000 (15%)";
    } else if (adaMember) {
        diskon = totalBelanja * 0.05;
        alasanDiskon = "Member (5%)";
    } else if (totalBelanja >= 100000) {
        diskon = totalBelanja * 0.10;
        alasanDiskon = "Belanja >= Rp100.000 (10%)";
    }

    double totalBayar = totalBelanja - diskon;

    printHeader();
    tampilkanKeranjang();
    cout << "\n";
    printGaris('=');
    cout << "  RINGKASAN PEMBAYARAN\n";
    printGaris('=');
    if (adaMember) cout << "  Member       : " << namaMember << "\n";
    cout << "  Total Belanja: " << formatRupiah(totalBelanja) << "\n";
    if (diskon > 0) {
        cout << "  Diskon       : -" << formatRupiah(diskon) << " (" << alasanDiskon << ")\n";
    }
    printGaris('-');
    cout << "  TOTAL BAYAR  : " << formatRupiah(totalBayar) << "\n";
    printGaris('=');

    // BAB 3: Perulangan do-while untuk validasi uang bayar
    double uangBayar = 0;
    do {
        cout << "  Uang Bayar (Rp): ";
        cin >> uangBayar;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            uangBayar = 0;
        }
        if (uangBayar < totalBayar) {
            cout << "  [!] Uang kurang " << formatRupiah(totalBayar - uangBayar) << "! Masukkan lagi.\n";
        }
    } while (uangBayar < totalBayar);

    double kembalian = uangBayar - totalBayar;

    // Buat Transaksi
    Transaksi trx;
    trx.idTransaksi  = getIDTransaksi();
    trx.tanggal      = getTanggal();
    trx.namaCashier  = namaCashier;
    trx.keranjang    = keranjang;
    trx.totalBelanja = totalBelanja;
    trx.diskon       = diskon;
    trx.totalBayar   = totalBayar;
    trx.uangBayar    = uangBayar;
    trx.kembalian    = kembalian;
    trx.adaMember    = adaMember;
    trx.namaMember   = namaMember;
    riwayatTransaksi.push_back(trx);

    // Cetak Struk ke Layar
    cout << "\n";
    printGaris('=');
    cout << "          *** STRUK PEMBAYARAN ***\n";
    printGaris('=');
    cout << "  ID Transaksi : " << trx.idTransaksi << "\n";
    cout << "  Tanggal      : " << trx.tanggal << "\n";
    cout << "  Cashier      : " << namaCashier << "\n";
    if (adaMember) cout << "  Member       : " << namaMember << "\n";
    printGaris('-');
    for (const ItemKeranjang& ik : keranjang) {
        cout << "  " << left << setw(22) << ik.menu.nama
             << setw(4) << ik.jumlah << "x "
             << setw(14) << formatRupiah(ik.menu.harga)
             << formatRupiah(ik.subTotal) << "\n";
    }
    printGaris('-');
    cout << right << setw(44) << "Subtotal  : " << formatRupiah(totalBelanja) << "\n";
    if (diskon > 0) {
        cout << right << setw(44) << "Diskon    : -" << formatRupiah(diskon) << "\n";
    }
    cout << right << setw(44) << "TOTAL     : " << formatRupiah(totalBayar) << "\n";
    cout << right << setw(44) << "Bayar     : " << formatRupiah(uangBayar) << "\n";
    cout << right << setw(44) << "Kembali   : " << formatRupiah(kembalian) << "\n";
    printGaris('=');
    cout << "     Terima kasih! Selamat menikmati :)\n";
    printGaris('=');

    // Simpan struk ke file
    simpanStrukKeFile(trx);

    // Reset keranjang
    keranjang.clear();
    cout << "\n  Tekan ENTER untuk kembali ke menu...";
    cin.ignore();
    cin.get();
}

// ==============================================================
// SECTION 7: FUNGSI MEMBER
// ==============================================================

Member* cariMember(const string& kode) {
    for (Member& m : daftarMember) {
        if (m.kode == kode && m.aktif) {
            return &m;
        }
    }
    return nullptr;
}

void menuPembayaran() {
    printHeader();
    tampilkanKeranjang();

    if (keranjang.empty()) {
        cout << "\n  [!] Keranjang kosong!\n";
        cout << "\n  Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    cout << "\n";
    printGaris('=');
    cout << "  CEK MEMBER\n";
    printGaris('=');
    cout << "  1. Pelanggan punya kartu member\n";
    cout << "  2. Tidak (lewati)\n";
    printGaris('-');
    int pilihanMember = inputInt("  Pilihan: ", 1, 2);

    bool adaMember = false;
    string namaMember = "";

    // BAB 2: Percabangan untuk cek member
    if (pilihanMember == 1) {
        cout << "  Masukkan Kode Member (contoh: MBR001): ";
        string kodeMember;
        cin >> kodeMember;

        // Ubah ke huruf besar (uppercase) untuk validasi tidak case-sensitive
        for (char& c : kodeMember) c = toupper(c);

        Member* m = cariMember(kodeMember);
        if (m != nullptr) {
            adaMember = true;
            namaMember = m->nama;
            m->poin += (int)(hitungTotalBelanja() / 10000); // 1 poin per Rp10.000
            cout << "  [v] Selamat datang, " << m->nama << "! Poin Anda saat ini: " << m->poin << "\n";
        } else {
            cout << "  [!] Kode member tidak ditemukan atau tidak aktif. Lanjut tanpa member.\n";
        }
    }

    prosesTransaksi(adaMember, namaMember);
}

// ==============================================================
// SECTION 8: FUNGSI LAPORAN HARIAN
// ==============================================================

void tampilkanLaporan() {
    printHeader();
    cout << "  LAPORAN TRANSAKSI HARI INI\n";
    printGaris('=');

    if (riwayatTransaksi.empty()) {
        cout << "\n  Belum ada transaksi hari ini.\n";
        cout << "\n  Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    double totalPendapatan = 0;
    double totalDiskon = 0;

    // BAB 3: For loop untuk iterasi semua transaksi
    for (int i = 0; i < (int)riwayatTransaksi.size(); i++) {
        const Transaksi& trx = riwayatTransaksi[i];
        cout << "\n  Transaksi #" << (i + 1) << "\n";
        cout << "  ID           : " << trx.idTransaksi << "\n";
        cout << "  Waktu        : " << trx.tanggal << "\n";
        if (trx.adaMember) {
            cout << "  Member       : " << trx.namaMember << "\n";
        }
        cout << "  Total Bayar  : " << formatRupiah(trx.totalBayar) << "\n";
        cout << "  Diskon       : " << formatRupiah(trx.diskon) << "\n";
        printGaris('-');

        totalPendapatan += trx.totalBayar;
        totalDiskon     += trx.diskon;
    }

    // Menghitung rata-rata pendapatan per transaksi
    double rata2 = totalPendapatan / riwayatTransaksi.size();

    cout << "\n";
    printGaris('=');
    cout << "  RINGKASAN LAPORAN\n";
    printGaris('=');
    cout << "  Jumlah Transaksi       : " << riwayatTransaksi.size() << " transaksi\n";
    cout << "  Total Pendapatan       : " << formatRupiah(totalPendapatan) << "\n";
    cout << "  Total Diskon Diberikan : " << formatRupiah(totalDiskon) << "\n";
    cout << "  Rata-rata per Transaksi: " << formatRupiah(rata2) << "\n";
    printGaris('=');

    // Simpan laporan ke file
    ofstream laporanFile("laporan_harian.txt");
    if (laporanFile.is_open()) {
        laporanFile << "LAPORAN HARIAN - KAFE KODING SEJAHTERA\n";
        laporanFile << "Dicetak pada: " << getTanggal() << "\n\n";
        laporanFile << "Jumlah Transaksi       : " << riwayatTransaksi.size() << "\n";
        laporanFile << "Total Pendapatan       : " << formatRupiah(totalPendapatan) << "\n";
        laporanFile << "Total Diskon           : " << formatRupiah(totalDiskon) << "\n";
        laporanFile << "Rata-rata per Transaksi: " << formatRupiah(rata2) << "\n";
        laporanFile.close();
        cout << "  [v] Laporan disimpan ke: laporan_harian.txt\n";
    }

    cout << "\n  Tekan ENTER untuk kembali...";
    cin.ignore();
    cin.get();
}

// ==============================================================
// SECTION 9: MENU MANAJEMEN STOK (Fitur Bonus)
// ==============================================================

void manajemenStok() {
    printHeader();
    cout << "  MANAJEMEN KETERSEDIAAN MENU\n";
    printGaris('=');
    for (const MenuItem& item : daftarMenu) {
        string status = item.tersedia ? "[TERSEDIA]" : "[HABIS]   ";
        cout << "  " << setw(4) << item.id
             << status << " "
             << item.nama << "\n";
    }
    printGaris('=');

    int idPilih = inputInt("  Masukkan ID untuk toggle status (0 = Batal): ", 0, 15);
    if (idPilih == 0) return;

    // BAB 2 & 3: Loop mencari item dan toggle status
    for (MenuItem& item : daftarMenu) {
        if (item.id == idPilih) {
            item.tersedia = !item.tersedia; // Toggle
            string statusBaru = item.tersedia ? "TERSEDIA" : "HABIS";
            cout << "  [v] " << item.nama << " diubah menjadi: " << statusBaru << "\n";
            break;
        }
    }

    cout << "\n  Tekan ENTER untuk kembali...";
    cin.ignore();
    cin.get();
}

// ==============================================================
// SECTION 10: MAIN FUNCTION - Menu Utama
// ==============================================================

void loginCashier() {
    clearScreen();
    printGaris('=');
    cout << "||" << setw(50) << "       KAFE KODING SEJAHTERA" << "||\n";
    cout << "||" << setw(50) << "         Point of Sales v2.0" << "||\n";
    printGaris('=');
    cout << "\n  Selamat Datang, Kasir!\n";
    cout << "  Masukkan nama Anda: ";
    cin.ignore();
    getline(cin, namaCashier);
    if (namaCashier.empty()) namaCashier = "Kasir";
    cout << "\n  Halo, " << namaCashier << "! Selamat bekerja.\n";
    cout << "\n  Tekan ENTER untuk mulai...";
    cin.get();
}

int main() {
    loginCashier();

    // BAB 3: Perulangan utama program (while loop)
    bool running = true;
    while (running) {
        printHeader();

        // Tampilkan indikator keranjang jika ada item
        if (!keranjang.empty()) {
            cout << "  [KERANJANG AKTIF: " << keranjang.size() << " item | Total: "
                 << formatRupiah(hitungTotalBelanja()) << "]\n";
            printGaris('-');
        }

        cout << "\n  MENU UTAMA\n";
        printGaris('=');
        cout << "  1. Tambah Pesanan\n";
        cout << "  2. Lihat & Hapus Keranjang\n";
        cout << "  3. Proses Pembayaran\n";
        cout << "  4. Laporan Transaksi Harian\n";
        cout << "  5. Manajemen Stok Menu\n";
        cout << "  6. Keluar\n";
        printGaris('=');

        // BAB 2: Switch-Case untuk routing menu utama
        int pilihan = inputInt("  Pilih Menu: ", 1, 6);
        switch (pilihan) {
            case 1:
                tambahKeKeranjang();
                break;
            case 2:
                printHeader();
                tampilkanKeranjang();
                if (!keranjang.empty()) {
                    cout << "\n  Apakah Anda ingin menghapus item? (y/n): ";
                    char hapus;
                    cin >> hapus;
                    if (hapus == 'y' || hapus == 'Y') {
                        hapusDariKeranjang();
                    } else {
                        cout << "  Tekan ENTER untuk kembali...";
                        cin.ignore();
                        cin.get();
                    }
                } else {
                    cout << "\n  Tekan ENTER untuk kembali...";
                    cin.ignore();
                    cin.get();
                }
                break;
            case 3:
                menuPembayaran();
                break;
            case 4:
                tampilkanLaporan();
                break;
            case 5:
                manajemenStok();
                break;
            case 6:
                running = false;
                break;
        }
    }

    // Pesan keluar
    clearScreen();
    printGaris('=');
    cout << "||" << setw(50) << "       TERIMA KASIH, " + namaCashier + "!" << "||\n";
    cout << "||" << setw(50) << "       Semoga hari Anda menyenangkan :)" << "||\n";
    printGaris('=');
    cout << "\n";

    return 0;
}
