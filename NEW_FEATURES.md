# Guida alle Nuove Funzionalità / New Features Guide

## 🇮🇹 Italiano

### File Browser per Scegliere la Destinazione

**Come usarlo:**
1. Avvia l'applicazione
2. Premi **SELECT** per aprire il file browser
3. Usa il **D-Pad** (↑↓) per navigare tra le cartelle
4. Premi **A** per entrare in una cartella
5. Premi **Y** per usare la cartella corrente come destinazione
6. Premi **B** per annullare e tornare indietro

**Caratteristiche:**
- Visualizza cartelle e file dell'SD card
- Le cartelle sono mostrate per prime
- Il simbolo `/` indica una cartella
- Il simbolo `>` indica la selezione corrente
- `..` per tornare alla cartella superiore

**Esempio:**
```
File Browser - Select Extract Path
================================
Current: sdmc:/games/
================================

  > ../
    3ds/
    extracted/
    Nintendo 3DS/
    games/
```

### Supporto File Google Drive Grandi (>100MB)

**Problema precedente:**
I file Google Drive oltre 100MB mostravano una pagina di scansione virus che impediva il download diretto.

**Soluzione implementata:**
- Aggiunto parametro `confirm=t` agli URL di Google Drive
- Gestione automatica della conferma virus scan
- Funziona per la maggior parte dei file grandi

**Come usare:**
Nessuna azione richiesta! L'app gestisce automaticamente i file grandi di Google Drive.

**Nota:** 
Alcuni file molto grandi potrebbero ancora richiedere il download manuale dal browser. In questi casi:
1. Apri l'URL nel browser del PC
2. Completa la verifica del virus scan
3. Ottieni il link di download diretto
4. Usa quel link nel config.txt

### Controlli Aggiornati

**Menu Principale:**
- **A** - Avvia download
- **X** - Visualizza coda download
- **SELECT** - Apri file browser
- **START** - Esci

**File Browser:**
- **D-Pad ↑↓** - Naviga
- **A** - Entra nella cartella
- **Y** - Usa cartella corrente
- **B** - Annulla/Indietro

**Coda Download:**
- **L/R** - Cambia pagina
- **Y** - Salta file falliti
- **A** - Continua/Riprova
- **B** - Indietro

---

## 🇬🇧 English

### File Browser for Destination Selection

**How to use:**
1. Launch the application
2. Press **SELECT** to open the file browser
3. Use **D-Pad** (↑↓) to navigate folders
4. Press **A** to enter a folder
5. Press **Y** to use current folder as destination
6. Press **B** to cancel and go back

**Features:**
- Browse SD card folders and files
- Folders are shown first
- `/` symbol indicates a folder
- `>` symbol shows current selection
- `..` to go to parent directory

**Example:**
```
File Browser - Select Extract Path
================================
Current: sdmc:/games/
================================

  > ../
    3ds/
    extracted/
    Nintendo 3DS/
    games/
```

### Large Google Drive Files Support (>100MB)

**Previous problem:**
Google Drive files over 100MB showed a virus scan page that prevented direct download.

**Implemented solution:**
- Added `confirm=t` parameter to Google Drive URLs
- Automatic handling of virus scan confirmation
- Works for most large files

**How to use:**
No action required! The app automatically handles large Google Drive files.

**Note:** 
Some very large files might still require manual download from browser. In these cases:
1. Open the URL in PC browser
2. Complete the virus scan verification
3. Get the direct download link
4. Use that link in config.txt

### Updated Controls

**Main Menu:**
- **A** - Start downloads
- **X** - View download queue
- **SELECT** - Open file browser
- **START** - Exit

**File Browser:**
- **D-Pad ↑↓** - Navigate
- **A** - Enter folder
- **Y** - Use current folder
- **B** - Cancel/Back

**Download Queue:**
- **L/R** - Change page
- **Y** - Skip failed items
- **A** - Continue/Retry
- **B** - Back

---

## 🎨 GUI Integration - Prossimi Passi / Next Steps

### Cosa Serve per una GUI / What's Needed for GUI

Per integrare una GUI grafica completa, serviranno:
To integrate a full graphical GUI, we'll need:

1. **citro3d** - Libreria 3D per rendering / 3D rendering library
2. **citro2d** - Libreria 2D per UI / 2D library for UI
3. **Sprite e grafiche** - Icone, pulsanti, sfondi / Icons, buttons, backgrounds
4. **Touch screen support** - Interazione touch / Touch interaction

### Opzioni GUI / GUI Options

**Opzione 1: GUI Completa con citro2d** (Complessa / Complex)
- Rendering grafico completo
- Touch screen
- Animazioni e transizioni
- Richiede più tempo di sviluppo

**Opzione 2: GUI Ibrida** (Consigliata / Recommended)
- Usa console per testo
- Aggiungi overlay grafici per barre progresso
- Touch per selezione file
- Più facile da implementare

**Opzione 3: Console Migliorata** (Attuale / Current)
- UI testuale migliorata (quello che abbiamo ora)
- Veloce e funzionale
- Nessuna grafica aggiuntiva

### Esempio GUI Proposta / Proposed GUI Example

```
┌─────────────────────────────────────┐
│  3DS Zip Extractor       [WiFi] [⚙]│
├─────────────────────────────────────┤
│                                     │
│  📋 Download Queue (3 items)        │
│                                     │
│  ✓ game-data.zip      [Completed]  │
│  ► updates.tar.gz     [50%]        │
│    ████████░░░░░░░░  128/256 MB   │
│  ⏸ dlc-pack.7z        [Pending]    │
│                                     │
│  📁 Extract to: /games/             │
│  [Browse...] [Start] [Settings]    │
│                                     │
└─────────────────────────────────────┘
```

### Vantaggi GUI / GUI Advantages

✅ **Pro:**
- Più user-friendly
- Progress bar visive
- Touch screen support
- Aspetto più professionale
- Icone per tipi di file

❌ **Contro:**
- Più complesso da sviluppare
- Richiede risorse grafiche
- Più difficile da debuggare
- Potrebbe essere più lento

### Implementazione GUI - Roadmap

**Fase 1:** Setup citro2d
- Inizializzare rendering 2D
- Creare sistema di font
- Test rendering base

**Fase 2:** UI Components
- Bottoni cliccabili
- Liste scrollabili
- Progress bars
- Dialog boxes

**Fase 3:** File Browser Grafico
- Lista file con icone
- Scroll con touch
- Breadcrumb navigation
- Anteprima cartelle

**Fase 4:** Download UI
- Progress bars animate
- Statistiche in tempo reale
- Notifiche
- Gestione errori visuale

**Fase 5:** Polish
- Tema/colori
- Animazioni
- Icone personalizzate
- Settings grafici

### Vuoi la GUI? / Want GUI?

Se vuoi che implementi la GUI, posso procedere passo dopo passo.
Dimmi quale opzione preferisci!

If you want me to implement the GUI, I can proceed step by step.
Tell me which option you prefer!

1. **GUI Completa** - Tutto grafico (richiede tempo)
2. **GUI Ibrida** - Misto console + grafica (bilanciato)
3. **Continuare con Console** - Migliorare UI testuale (veloce)
