# Documentazione Tecnica - 3DS Archive Extractor

## Panoramica

3DS Archive Extractor è un'applicazione homebrew per Nintendo 3DS che permette di scaricare ed estrarre archivi compressi in molteplici formati direttamente sulla console.

---

## Formati Supportati

L'applicazione supporta **11+ formati di archivio** tramite libarchive:

### Archivi Completi
- **ZIP** (.zip) - Formato standard, supporto completo
- **TAR** (.tar) - Archivi tape, non compressi
- **7-Zip** (.7z) - Alta compressione
- **RAR** (.rar) - Solo lettura (estrazione)

### Archivi TAR Compressi
- **TAR.GZ** (.tar.gz, .tgz) - Compressione GZIP
- **TAR.BZ2** (.tar.bz2, .tbz2) - Compressione BZIP2
- **TAR.XZ** (.tar.xz, .txz) - Compressione XZ/LZMA
- **TAR.ZSTD** (.tar.zst) - Compressione Zstandard

### File Singoli Compressi
- **GZIP** (.gz) - Compressione DEFLATE
- **BZIP2** (.bz2) - Compressione Burrows-Wheeler
- **XZ** (.xz) - Compressione LZMA2
- **ZSTD** (.zst) - Compressione Zstandard

### Non Supportato
- **LZ4** (.lz4) - Libreria non disponibile su 3DS portlibs

---

## Architettura

```
┌─────────────────────────────────────────┐
│    3DS Archive Extractor (main.c)      │
│  - Download Manager (libcurl)          │
│  - Queue System                         │
│  - GUI (citro2d/citro3d)               │
├─────────────────────────────────────────┤
│   Archive Extractor Module              │
│   (archive_extractor.c)                 │
│  - Format Detection (magic bytes)       │
│  - Multi-format Extraction              │
├─────────────────────────────────────────┤
│   Libraries                             │
│  ├─ libarchive (multi-format)          │
│  ├─ libcurl (HTTP/HTTPS)               │
│  ├─ zlib (gzip/deflate)                │
│  ├─ libbz2 (bzip2)                     │
│  ├─ liblzma (xz/lzma)                  │
│  ├─ libzstd (zstandard)                │
│  └─ mbedtls (SSL/TLS)                  │
├─────────────────────────────────────────┤
│   3DS System (libctru)                  │
└─────────────────────────────────────────┘
```

---

## Rilevamento Formato

Il rilevamento del formato avviene tramite analisi dei "magic bytes" (signature del file):

```c
// Esempi di signature
ZIP:    PK\x03\x04
GZIP:   0x1f 0x8b
BZIP2:  BZ
7Z:     37 7A BC AF 27 1C
RAR:    Rar!\x1a\x07
TAR:    "ustar" @ offset 257
XZ:     FD 37 7A 58 5A 00
ZSTD:   28 B5 2F FD
```

Il file `archive_extractor.c` implementa la funzione:
```c
ArchiveType detect_archive_type(const char* file_path);
```

---

## Flusso Operativo

1. **Inizializzazione**
   - Lettura `config.txt`
   - Inizializzazione GUI
   - Setup networking

2. **Download**
   - Richiesta HTTP/HTTPS via libcurl
   - Supporto resume per download interrotti
   - Conversione URL Google Drive automatica
   - Progress tracking

3. **Estrazione**
   - Rilevamento automatico formato
   - Estrazione via libarchive
   - Callback per progress updates
   - Gestione errori robusta

4. **Cleanup**
   - Rimozione file temporanei
   - Report risultato
   - Aggiornamento coda

---

## Compatibilità Hardware

### Console Supportate
- Nintendo 3DS (Old) - 64MB RAM, 268MHz CPU
- Nintendo 3DS XL (Old)
- Nintendo 2DS
- New Nintendo 3DS - 128MB RAM, 804MHz CPU ⚡
- New Nintendo 3DS XL
- New Nintendo 2DS XL

### Requisiti Memoria
```
Buffer Download:      128KB
Buffer Estrazione:    128KB
Stack App:            ~4MB
Heap libarchive:      2-8MB (dinamico)
────────────────────────────
Totale Tipico:        6-12MB

✅ Compatibile con 3DS Old (64MB disponibili)
✅ Ottimale su New 3DS (128MB disponibili)
```

### Performance

| Operazione | 3DS Old (268MHz) | New 3DS (804MHz) |
|-----------|------------------|------------------|
| Download 10MB | ~30-60s | ~30-60s (I/O bound) |
| Estrai ZIP 10MB | ~10-20s | ~3-7s |
| Estrai TAR.GZ 10MB | ~15-30s | ~5-10s |
| Estrai 7Z 10MB | ~20-40s | ~7-15s |

---

## Sicurezza

### Protezioni Implementate

1. **Buffer Overflow Protection**
   - Buffer size fissi e validati
   - Uso di `strncpy` invece di `strcpy`
   - Controlli limiti array

2. **Path Traversal Protection**
   - Flag `ARCHIVE_EXTRACT_SECURE_NODOTDOT`
   - Impedisce estrazione fuori directory target
   - Sanitizzazione percorsi

3. **Gestione Memoria**
   - No `malloc` esplicito non gestito
   - libarchive gestisce memoria internamente
   - Cleanup automatico in caso errore

### Limitazioni Note

1. **SSL Certificate Verification: DISABILITATA**
   - Motivo: 3DS non ha certificate store
   - Impatto: Possibili MITM attacks
   - Nota: Limitazione comune app homebrew 3DS

2. **File Permissions: IGNORATE**
   - Motivo: SD card usa FAT32
   - Impatto: Permessi non preservati
   - Normale su 3DS

---

## Stub POSIX

Il 3DS non implementa alcune funzioni POSIX. Sono stati creati stub sicuri:

```c
// Stub per funzioni non disponibili
mode_t umask(mode_t mask) {
    return 0022;  // Default umask
}

struct passwd *getpwnam(const char *name) {
    return NULL;  // No user database
}

struct group *getgrnam(const char *name) {
    return NULL;  // No group database
}

// LZ4 non supportato
int LZ4_decompress_safe(...) {
    return -1;  // Not supported
}
```

---

## Compilazione

### Prerequisiti
```bash
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-citro3d 3ds-citro2d \
                   3ds-libarchive 3ds-zlib 3ds-bzip2 3ds-xz 3ds-zstd
```

### Build
```bash
make clean
make
```

### Output
- `3ds-zip-extractor.3dsx` - Eseguibile per Homebrew Launcher
- `3ds-zip-extractor.elf` - File debug con simboli

### Librerie Linkate
```makefile
LIBS := -lcitro2d -lcitro3d -lcurl -lmbedtls -lmbedx509 -lmbedcrypto \
        -larchive -llzma -lbz2 -lzstd -lz -lctru -lm
```

---

## API Principale

### Archive Extractor

```c
// Rileva tipo archivio
ArchiveType detect_archive_type(const char* file_path);

// Ottieni nome tipo
const char* get_archive_type_name(ArchiveType type);

// Estrai archivio
int extract_archive_libarchive(
    const char* archive_path,
    const char* output_dir,
    ExtractCallback callback,
    void* user_data
);

// Callback per progress
typedef bool (*ExtractCallback)(
    int file_count,
    const char* current_file,
    void* user_data
);
```

---

## Struttura File

```
3ds-zip-extractor/
├── source/
│   ├── main.c              # Logic principale
│   ├── archive_extractor.c # Estrazione multi-formato
│   └── gui.c               # Rendering grafico
├── include/
│   ├── gui.h
│   └── archive_extractor.h
├── docs/
│   ├── TECHNICAL.md        # Questo file
│   ├── USER_GUIDE.md       # Guida utente
│   ├── DEVELOPMENT.md      # Note sviluppatori
│   └── archive/            # File storici
├── build/                  # File compilati
├── Makefile
├── config.txt              # Esempio config
├── README.md               # Documentazione principale
├── CONTRIBUTING.md         # Guide contribuzione
├── SECURITY.md             # Policy sicurezza
└── LICENSE                 # Licenza MIT
```

---

## Debug e Testing

### Compilazione Debug
```bash
make clean
make V=1  # Verbose output
```

### Check Errori
```bash
# Cerca memory leaks (se disponibile su 3DS)
# Verifica log console durante esecuzione
```

### Test su Citra Emulator
```bash
# L'app può essere testata su Citra 3DS emulator
citra-qt 3ds-zip-extractor.3dsx
```

**Nota**: Alcune funzioni di rete potrebbero non funzionare su emulatore.

---

## Troubleshooting

### Build Errors

**Errore: `libarchive not found`**
```bash
sudo dkp-pacman -S 3ds-libarchive
```

**Errore: `undefined reference to LZ4_*`**
- Normale, stub implementato
- Se persiste: verifica che `archive_extractor.c` sia compilato

### Runtime Errors

**App crash all'avvio**
- Verifica `config.txt` sia presente
- Controlla formato config file

**Download fallisce**
- Verifica connessione WiFi 3DS
- Testa URL su browser prima

**Estrazione fallisce**
- Verifica spazio su SD card
- File potrebbe essere corrotto
- Formato potrebbe non essere supportato

---

## Ottimizzazioni Future

### Performance
- [ ] Decompressione multi-thread su New 3DS
- [ ] Cache per file piccoli
- [ ] Streaming diretto (download + estrai)

### Funzionalità
- [ ] Supporto LZ4 (se libreria diventa disponibile)
- [ ] Anteprima contenuto archivio prima estrazione
- [ ] Estrazione selettiva file
- [ ] Creazione archivi (non solo estrazione)

### UI/UX
- [ ] Grafica completamente GPU-based
- [ ] Temi personalizzabili
- [ ] Supporto touch screen
- [ ] Integrazione file browser 3DS

---

## Riferimenti

### Librerie
- [libarchive](https://www.libarchive.org/) - Multi-format archive library
- [libcurl](https://curl.se/libcurl/) - HTTP/HTTPS client
- [mbedTLS](https://www.trustedfirmware.org/projects/mbed-tls/) - SSL/TLS
- [zlib](https://www.zlib.net/) - DEFLATE compression

### 3DS Development
- [devkitPRO](https://devkitpro.org/) - 3DS toolchain
- [libctru](https://github.com/devkitPro/libctru) - 3DS system library
- [citro3d](https://github.com/devkitPro/citro3d) - GPU library

### Documentazione
- [3DS Brew Wiki](https://www.3dbrew.org/)
- [libarchive Manual](https://github.com/libarchive/libarchive/wiki)

---

**Versione Documento**: 1.0  
**Data**: 2 Febbraio 2026  
**Autore**: Marcogn
