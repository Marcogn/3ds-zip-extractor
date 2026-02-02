# Problemi di Compilazione / Compilation Issues

## 🇮🇹 Italiano

### Problema Principale: Dipendenze di libarchive

La compilazione attualmente **fallisce** a causa di dipendenze mancanti di libarchive. 

#### Sintomi
```
undefined reference to `lzma_end'
undefined reference to `BZ2_bzDecompressEnd'
undefined reference to `ZSTD_freeDStream'
undefined reference to `umask'
undefined reference to `getpwnam'
undefined reference to `getgrnam'
```

#### Causa
La versione di `libarchive` nei portlibs devkitPro per 3DS è compilata con supporto per:
- LZMA/XZ (richiede `liblzma`)
- BZIP2 (richiede `libbz2`) 
- ZSTD (richiede `libzstd`)
- Funzioni POSIX (umask, getpwnam, getgrnam)

Tuttavia, queste librerie **non sono disponibili o non sono correttamente linkate** nel toolchain 3DS attuale.

#### Pacchetti Installati
```bash
sudo dkp-pacman -S 3ds-curl 3ds-libarchive 3ds-mbedtls 3ds-bzip2 3ds-liblzma 3ds-libzstd
```

Anche dopo l'installazione, il linker non trova le librerie.

#### Soluzioni Tentate

1. ✗ Aggiunta esplicita delle librerie nel Makefile: `-lzstd -llzma -lbz2`
2. ✗ Uso di `--start-group` / `--end-group` per dipendenze circolari
3. ✗ Rimozione di `archive_write_disk_set_standard_lookup()` (risolve umask/getpwnam/getgrnam)
4. ✗ Limitazione ai formati: `archive_read_support_format_zip()` invece di `_all()`

### Stato Attuale

**Il progetto NON compila** con la configurazione corrente di devkitPro/3DS.

### Possibili Soluzioni Future

#### Opzione 1: Ricompilare libarchive (Avanzato)
Ricompilare libarchive per 3DS **senza** supporto per LZMA, BZIP2, ZSTD:
```bash
./configure --disable-bzip2 --disable-lzma --disable-zstd --without-lzma --without-bz2lib --without-zstd
```

#### Opzione 2: Usare solo zlib (Semplificato)
Sostituire libarchive con implementazione manuale usando solo zlib (supporta solo ZIP deflate).

#### Opzione 3: Usare minizip
Libreria più semplice che supporta solo ZIP con deflate/store, senza dipendenze complesse.

### Raccomandazione

Per un progetto funzionante, si raccomanda di:
1. Sostituire libarchive con `minizip` (disponibile in portlibs)
2. Limitare il supporto a file ZIP standard (deflate/store)
3. Rimuovere supporto per 7Z, TAR.XZ, TAR.BZ2

---

## 🇬🇧 English

### Main Issue: libarchive Dependencies

Compilation currently **fails** due to missing libarchive dependencies.

#### Symptoms
```
undefined reference to `lzma_end'
undefined reference to `BZ2_bzDecompressEnd'
undefined reference to `ZSTD_freeDStream'
undefined reference to `umask'
undefined reference to `getpwnam'
undefined reference to `getgrnam'
```

#### Cause
The `libarchive` version in devkitPro portlibs for 3DS is compiled with support for:
- LZMA/XZ (requires `liblzma`)
- BZIP2 (requires `libbz2`)
- ZSTD (requires `libzstd`)
- POSIX functions (umask, getpwnam, getgrnam)

However, these libraries are **not available or not properly linked** in the current 3DS toolchain.

#### Packages Installed
```bash
sudo dkp-pacman -S 3ds-curl 3ds-libarchive 3ds-mbedtls 3ds-bzip2 3ds-liblzma 3ds-libzstd
```

Even after installation, the linker cannot find the libraries.

#### Attempted Solutions

1. ✗ Explicit library addition in Makefile: `-lzstd -llzma -lbz2`
2. ✗ Using `--start-group` / `--end-group` for circular dependencies
3. ✗ Removing `archive_write_disk_set_standard_lookup()` (fixes umask/getpwnam/getgrnam)
4. ✗ Limiting formats: `archive_read_support_format_zip()` instead of `_all()`

### Current Status

**The project DOES NOT compile** with the current devkitPro/3DS configuration.

### Possible Future Solutions

#### Option 1: Recompile libarchive (Advanced)
Recompile libarchive for 3DS **without** LZMA, BZIP2, ZSTD support:
```bash
./configure --disable-bzip2 --disable-lzma --disable-zstd --without-lzma --without-bz2lib --without-zstd
```

#### Option 2: Use zlib only (Simplified)
Replace libarchive with manual implementation using only zlib (supports only ZIP deflate).

#### Option 3: Use minizip
Simpler library that supports only ZIP with deflate/store, without complex dependencies.

### Recommendation

For a working project, it's recommended to:
1. Replace libarchive with `minizip` (available in portlibs)
2. Limit support to standard ZIP files (deflate/store)
3. Remove support for 7Z, TAR.XZ, TAR.BZ2

---

## Technical Details

### Missing Library Files

Expected locations:
- `/opt/devkitpro/portlibs/3ds/lib/liblzma.a`
- `/opt/devkitpro/portlibs/3ds/lib/libbz2.a`
- `/opt/devkitpro/portlibs/3ds/lib/libzstd.a`

These files either:
1. Don't exist after package installation
2. Have different names
3. Are in a different location
4. Are not being found by the linker

### Workaround for Testing

To test if the rest of the code works, you could:
1. Comment out all archive extraction code
2. Test only the download functionality
3. Save files without extraction

This would at least verify the networking and GUI components work correctly.
