# Guida Rapida / Quick Start Guide

## 🇮🇹 Italiano

### Requisiti

**Per compilare su PC:**
- Windows, Linux o macOS
- devkitPro installato (vedi sotto)
- Circa 1GB di spazio su disco

**Per usare su 3DS:**
- New Nintendo 3DS con Custom Firmware (CFW)
- Homebrew Launcher installato
- Scheda SD con spazio libero

### Passo 1: Installare devkitPro (su PC)

#### Su Windows:
1. Scarica l'installer da: https://github.com/devkitPro/installer/releases
2. Esegui l'installer e scegli "3DS Development"
3. Aspetta il completamento dell'installazione

#### Su Linux:
```bash
# Debian/Ubuntu
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
sudo ./install-devkitpro-pacman

# Installa i tool per 3DS
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-libarchive 3ds-mbedtls
```

#### Su macOS:
```bash
# Installa Homebrew se non l'hai già
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Installa devkitPro
brew tap devkitpro/tools
brew install devkitpro-pacman

# Installa i tool per 3DS
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-libarchive 3ds-mbedtls
```

### Passo 2: Compilare l'applicazione (su PC)

```bash
# Clona il repository
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor

# Compila
make

# Se tutto va bene, verrà creato: 3ds-zip-extractor.3dsx
```

### Passo 3: Preparare la scheda SD

1. **Copia l'applicazione sulla SD:**
   - Copia `3ds-zip-extractor.3dsx` in `/3ds/` sulla tua scheda SD

2. **Crea il file di configurazione:**
   - Crea la cartella `/3ds/zip-extractor/` sulla scheda SD
   - Copia `urls.txt` in `/3ds/zip-extractor/` 
   - OPPURE crea un nuovo file vuoto `urls.txt` in quella cartella

3. **Aggiungi i tuoi URL:**
   - Apri `/3ds/zip-extractor/urls.txt` con un editor di testo
   - Aggiungi un URL per riga, esempio:
   ```
   # I miei download
   https://example.com/file1.zip
   https://example.com/file2.tar.gz
   https://drive.google.com/file/d/1ABC_ID_FILE_XYZ/view
   ```

### Passo 4: Usare sul 3DS

1. Inserisci la scheda SD nel tuo 3DS
2. Avvia Homebrew Launcher
3. Trova e avvia "Zip Extractor"
4. Premi **X** per vedere gli URL configurati
5. Premi **A** per iniziare il download e l'estrazione
6. Premi **B** per annullare (durante download/estrazione)
7. Premi **START** per uscire

### Note Importanti

- ⚠️ **Devi compilare su PC**, non puoi compilare direttamente sul 3DS
- ✅ Il file compilato (.3dsx) funziona su qualsiasi 3DS con CFW
- 📁 I file estratti vanno in `/extracted/` sulla scheda SD
- 🌐 Assicurati che il 3DS sia connesso a Internet prima di usare l'app
- 💾 Controlla di avere spazio sufficiente sulla SD per i download

---

## 🇬🇧 English

### Requirements

**To compile on PC:**
- Windows, Linux or macOS
- devkitPro installed (see below)
- About 1GB disk space

**To use on 3DS:**
- New Nintendo 3DS with Custom Firmware (CFW)
- Homebrew Launcher installed
- SD card with free space

### Step 1: Install devkitPro (on PC)

#### On Windows:
1. Download installer from: https://github.com/devkitPro/installer/releases
2. Run installer and choose "3DS Development"
3. Wait for installation to complete

#### On Linux:
```bash
# Debian/Ubuntu
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
sudo ./install-devkitpro-pacman

# Install 3DS development tools
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-libarchive 3ds-mbedtls
```

#### On macOS:
```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install devkitPro
brew tap devkitpro/tools
brew install devkitpro-pacman

# Install 3DS development tools
sudo dkp-pacman -S 3ds-dev 3ds-curl 3ds-libarchive 3ds-mbedtls
```

### Step 2: Compile the application (on PC)

```bash
# Clone the repository
git clone https://github.com/Marcogn/3ds-zip-extractor.git
cd 3ds-zip-extractor

# Build
make

# If successful, you'll get: 3ds-zip-extractor.3dsx
```

### Step 3: Prepare the SD card

1. **Copy the application to SD:**
   - Copy `3ds-zip-extractor.3dsx` to `/3ds/` on your SD card

2. **Create the configuration file:**
   - Create folder `/3ds/zip-extractor/` on SD card
   - Copy `urls.txt` to `/3ds/zip-extractor/`
   - OR create a new empty `urls.txt` file in that folder

3. **Add your URLs:**
   - Open `/3ds/zip-extractor/urls.txt` with a text editor
   - Add one URL per line, example:
   ```
   # My downloads
   https://example.com/file1.zip
   https://example.com/file2.tar.gz
   https://drive.google.com/file/d/1ABC_FILE_ID_XYZ/view
   ```

### Step 4: Use on 3DS

1. Insert SD card into your 3DS
2. Launch Homebrew Launcher
3. Find and launch "Zip Extractor"
4. Press **X** to view configured URLs
5. Press **A** to start downloading and extracting
6. Press **B** to cancel (during download/extraction)
7. Press **START** to exit

### Important Notes

- ⚠️ **You must compile on PC**, you cannot compile directly on 3DS
- ✅ The compiled file (.3dsx) works on any 3DS with CFW
- 📁 Extracted files go to `/extracted/` on SD card
- 🌐 Make sure your 3DS is connected to Internet before using
- 💾 Check you have enough space on SD card for downloads

---

## Risoluzione Problemi / Troubleshooting

### Errore di compilazione / Build error: "DEVKITARM not set"
```bash
# Linux/macOS
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=$DEVKITPRO/devkitARM
export PATH=$DEVKITARM/bin:$PATH

# Windows (Git Bash / MSYS2)
export DEVKITPRO=/c/devkitPro
export DEVKITARM=$DEVKITPRO/devkitARM
```

### "No config file found!" sul 3DS / on 3DS
- Verifica che il file sia in `/3ds/zip-extractor/urls.txt` sulla SD
- Check the file is at `/3ds/zip-extractor/urls.txt` on SD card

### "socInit failed" sul 3DS / on 3DS
- Assicurati di essere connesso al WiFi
- Make sure you're connected to WiFi
- Riavvia il 3DS e riprova
- Restart 3DS and try again

## Link Utili / Useful Links

- devkitPro: https://devkitpro.org
- 3DS Homebrew Guide: https://3ds.hacks.guide
- GitHub Issues: https://github.com/Marcogn/3ds-zip-extractor/issues
