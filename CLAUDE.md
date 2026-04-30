> **Stato attuale (2026-04-30):** il refactor modulare è stato completato.
> Tutti i moduli elencati nella sezione 3 esistono in `source/` e in
> `include/`, la build con `make` è verde su `devkitpro/devkitarm:latest`
> e la suite host test in `tests/` passa con ASan/UBSan. Aggiornare questo
> file quando si introducono nuovi moduli o si cambiano comandi di build.
>
> # CLAUDE.md

> Guida per agenti AI (Claude, Copilot coding agent, Cursor, ecc.) che lavorano su questo repository.
> Leggere prima di modificare codice.

## 1. Cos'è questo progetto

`3ds-zip-extractor` è un'applicazione **homebrew per Nintendo 3DS** scritta in C. Permette di:
- Scaricare archivi da URL HTTP/HTTPS (incluso Google Drive) tramite **libcurl**.
- Estrarli direttamente sulla SD card del 3DS in vari formati (ZIP, TAR.*, 7Z, RAR read-only, ecc.) tramite **libarchive**.
- Operare con un'interfaccia grafica **citro2d/citro3d** (tema dark blue), con queue di download, retry automatico, file browser e notifiche LED.

L'applicazione viene distribuita come `.3dsx` da lanciare dall'**Homebrew Launcher**.

## 2. Toolchain & ambiente di build

- **Toolchain obbligatoria**: [devkitPRO](https://devkitpro.org/) con pacchetto `devkitARM` e portlibs 3DS.
  - Variabili attese: `$DEVKITPRO`, `$DEVKITARM`.
- **Librerie linkate** (tutte da portlibs 3DS, non installare versioni di sistema):
  - `libctru` (SDK 3DS)
  - `citro2d`, `citro3d` (rendering)
  - `libcurl` + `mbedtls` (HTTPS)
  - `libarchive` + `zlib`, `bzip2`, `xz`, `zstd` (estrazione)
- **Build**:
  ```bash
  make            # produce 3ds-zip-extractor.3dsx + .smdh
  make clean
  ```
- **Build via Docker** (utile per CI e per agenti senza devkitPRO locale):
  ```bash
  docker run --rm -v "$PWD":/src -w /src devkitpro/devkitarm:latest make
  ```
- **Build dei test host** (NON 3DS, gira sul PC):
  ```bash
  make -C tests -f Makefile.host test
  make -C tests -f Makefile.host test-asan
  ```

## 3. Layout del repository

```
.
├── source/                 # Codice C target 3DS
│   ├── main.c              # Entry point + state machine principale
│   ├── archive_extractor.c # Driver libarchive (estrazione multi-formato)
│   ├── archive_extractor_detect.c # Magic-byte sniffing (puro, host-testabile)
│   ├── gui.c               # Rendering citro2d (top + bottom screen)
│   ├── config.c            # Parser config.txt (puro, host-testabile)
│   ├── download.c          # libcurl + retry + resume
│   ├── gdrive.c            # Conversione URL Google Drive (puro)
│   ├── integrity.c         # SHA-256 + verify_integrity (mbedtls/host)
│   ├── file_browser.c      # File browser SD card
│   ├── led.c               # Notifiche LED MCU
│   └── queue.c             # Rendering coda di download
├── include/                # Header pubblici corrispondenti a source/
├── tests/                  # Unit test off-target (host build)
│   ├── Makefile.host
│   ├── unity.{c,h}         # Framework Unity vendorato
│   ├── test_*.c            # Suite di test
│   ├── fixtures/           # File di test
│   └── 3ds_stubs.h         # Stub di tipi 3DS per build host
├── .github/workflows/      # CI: build 3DS + host tests
├── config.txt              # Esempio di configurazione utente
├── icon.png                # Icona 48x48 per .smdh
├── Makefile                # Build devkitARM
├── README.md               # Documentazione utente finale
├── CHANGELOG.md
├── CONTRIBUTING.md
├── SECURITY.md
└── CLAUDE.md               # Questo file
```

> ⚠️ I file `*.3dsx` e `*.smdh` **non devono essere committati**: sono artifact di build pubblicati nelle GitHub Releases.

## 4. Architettura runtime

```
                ┌─────────────────────────┐
                │       main.c            │
                │  (state machine + UI)   │
                └────────────┬────────────┘
                             │
        ┌────────┬───────────┼───────────┬────────────┐
        ▼        ▼           ▼           ▼            ▼
   ┌────────┐ ┌──────┐  ┌─────────┐ ┌─────────┐ ┌──────────┐
   │ config │ │ gui  │  │ download│ │ archive │ │ file_brw │
   └────────┘ └──────┘  └────┬────┘ └────┬────┘ └──────────┘
                             │           │
                        ┌────▼────┐ ┌────▼────┐
                        │ libcurl │ │libarchive│
                        └─────────┘ └─────────┘
```

Stato dell'app modellato come macchina a stati gestita nel main loop di `main.c`:
`MENU → (BROWSER | QUEUE_VIEW | RUNNING) → SUMMARY → MENU/EXIT`.

Tutto è **single-thread cooperativo** (richiamato da `aptMainLoop()`). Non introdurre thread.

## 5. Convenzioni di codice

- **Linguaggio**: C11. Niente C++.
- **Stile**: 4 spazi, nessun tab. Brace su stessa riga (`if (x) {`).
- **Naming**: `snake_case` per funzioni e variabili, `UPPER_SNAKE` per macro/define, `PascalCase` per typedef di struct/enum.
- **Include**: prima `<stdlib.h>` standard, poi `<3ds.h>` e libs 3DS, poi `"local.h"`.
- **Allocazioni grandi su heap**: il 3DS ha stack piccolo (~32 KB). Strutture > 4 KB **devono** usare `calloc`/`memalign`. Vedi `DownloadQueue` e `FileBrowser` in `main.c`.
- **Error handling**: tutte le funzioni che possono fallire ritornano `Result` (3DS) o `int` < 0. Mai `exit()` da una funzione: propaga l'errore al `main`.
- **Nessuna stringa hardcoded user-facing fuori da `gui.c`** (preparare per futura i18n).
- **Path SD card**: usa sempre il prefisso `sdmc:/`. Mai path relativi.

## 6. Compatibilità config

Il file `sdmc:/3ds/zip-extractor/config.txt` ha un formato pubblico documentato (vedi README). 
**Qualsiasi modifica deve restare retrocompatibile** con la versione 1.0:
- Linee `#` = commenti.
- `key=value` per settings noti.
- Una URL per riga.
- Settings sconosciuti → ignora con warning, non fallire.

Estensioni accettate (già pianificate):
- `URL sha256:HEX` per integrità.
- `max_urls=`, `download_buffer_kb=`, `connect_timeout_s=`.

## 7. Sicurezza

- **Estrazione**: usare sempre `ARCHIVE_EXTRACT_SECURE_NODOTDOT | ARCHIVE_EXTRACT_SECURE_SYMLINKS | ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS` per prevenire zip-slip.
- **HTTPS**: SSL verification è **disabilitata** (`CURLOPT_SSL_VERIFYPEER=0`) perché il 3DS non ha un certificate store moderno. Documentato come trade-off accettato. Non rimuovere senza fornire un bundle CA.
- **Input non fidato**: tutti gli URL e i nomi file da archivi sono input non fidati. Validare lunghezza prima di `strcpy`/`strcat`. Preferire `snprintf`.
- **Vedi anche**: `SECURITY.md`.

## 8. Limitazioni note (non tentare di "risolvere")

- **LZ4**: non disponibile in portlibs 3DS. Gli stub in `archive_extractor.c` ritornano errore: lasciarli così, ma con log al primo uso.
- **Archivi password-protetti**: non supportati (libarchive 3DS non li gestisce in modo affidabile).
- **Old 3DS + archivi > 200 MB**: limite di RAM, può andare OOM. Documentato.
- **Folder Google Drive (`/folders/`)**: non scaricabili come singolo file.
- **`umask`, `getpwnam`, `getgrnam`**: non implementate in devkitARM, presenti come stub.

## 9. Testing

Vedi `tests/` e Fase 5 nel piano di lavoro. In sintesi:
- I test girano sul **PC host**, non sul 3DS.
- Solo le funzioni **pure** sono testate (parser config, conversione URL, detect formato, sha256).
- Niente rete reale, niente filesystem 3DS.
- CI deve eseguire sia `make` (build 3DS) sia `make -C tests -f Makefile.host test-asan`.

Prima di aggiungere una funzione nuova **non banale**, chiediti: *posso isolarla in un modulo testabile?* Se sì, scrivi prima il test.

## 10. Cosa fare prima di aprire una PR

- [ ] `make` completa senza warning su devkitARM.
- [ ] `make -C tests -f Makefile.host test-asan` passa.
- [ ] Nessun nuovo file binario committato (`.3dsx`, `.smdh`, `.elf`, `.o`).
- [ ] `CHANGELOG.md` aggiornato sotto `[Unreleased]`.
- [ ] Se è cambiato il formato di `config.txt`: README aggiornato + retrocompatibilità verificata con un config v1.0.
- [ ] Commit message in **Conventional Commits** (`feat:`, `fix:`, `refactor:`, `docs:`, `test:`, `ci:`, `chore:`).

## 11. Domande frequenti per agenti

**D: Posso aggiungere una libreria con `apt install` o `brew install`?**
R: No. Solo librerie già fornite da `devkitPRO` portlibs (`pacman -S 3ds-<libname>` nel container devkitpro).

**D: Posso usare `pthread` o `std::thread`?**
R: No. Il 3DS ha le sue API thread (`threadCreate` di libctru), ma per questo progetto si è scelto **single-thread cooperativo**. Non cambiare modello.

**D: Posso refactorare `main.c` in un colpo solo?**
R: Preferibile spezzarlo in commit per modulo. Vedi Fase 1 punto 1 del piano.

**D: Devo testare sul vero hardware 3DS?**
R: Idealmente sì (o emulatore Citra/Lime3DS). La CI verifica solo che compili e che i test host passino.

**D: Posso committare il `.3dsx` di build?**
R: **No.** Va solo nelle Releases come asset.

## 12. Riferimenti utili

- libctru API: https://libctru.devkitpro.org/
- citro2d: https://github.com/devkitPro/citro2d
- libcurl: https://curl.se/libcurl/c/
- libarchive: https://github.com/libarchive/libarchive/wiki
- Homebrew 3DS hub: https://www.3dbrew.org/
- Smealum's homebrew docs: https://smealum.github.io/3ds/
