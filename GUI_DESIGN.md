# Hybrid GUI - Visual Design

## 🎨 Come Funziona / How It Works

L'applicazione usa un sistema ibrido:
The application uses a hybrid system:

- **Schermo Superiore (Top Screen)**: Console testuale per informazioni dettagliate
- **Schermo Inferiore (Bottom Screen)**: Barre di progresso grafiche

## Layout Schermo Superiore / Top Screen Layout

```
┌─────────────────────────────────────────┐
│  Zip Extractor for 3DS                  │
│  ================================        │
│                                         │
│  Loaded 3 URL(s) from config            │
│                                         │
│  Config file:                           │
│    sdmc:/3ds/zip-extractor/config.txt  │
│                                         │
│  Extract path:                          │
│    sdmc:/extracted/                     │
│  Auto-retry: ON (max 3)                 │
│                                         │
│  Press A to start downloads             │
│  Press X to view queue                  │
│  Press SELECT to browse path            │
│  Press START to exit                    │
└─────────────────────────────────────────┘
```

## Layout Schermo Inferiore - Download / Bottom Screen - Download

```
┌─────────────────────────────────────────┐
│                                         │
│                                         │
│                                         │
│                                         │
│                                         │
│                                         │
│         ╔═══════════════════════╗       │
│         ║███████████░░░░░░░░░░░║       │
│         ╚═══════════════════════╝       │
│              55% Complete               │
│           128 MB / 256 MB               │
│                                         │
└─────────────────────────────────────────┘
```

## Layout Schermo Inferiore - Extraction / Bottom Screen - Extraction

```
┌─────────────────────────────────────────┐
│                                         │
│                                         │
│                                         │
│                                         │
│                                         │
│                                         │
│         ╔═══════════════════════╗       │
│         ║░░███░░░░░░░░░░░░░░░░░║       │
│         ╚═══════════════════════╝       │
│           Extracting Files...           │
│          42 files extracted             │
│                                         │
└─────────────────────────────────────────┘
```

## Colori / Colors

La barra di progresso usa colori diversi:
The progress bar uses different colors:

- **Giallo/Ambra** (`#FFC107`): Download in corso / Download in progress
- **Verde** (`#4CAF50`): Estrazione in corso / Extraction in progress
- **Grigio** (`#9E9E9E`): Parte non completata / Uncompleted part
- **Bianco** (`#FFFFFF`): Bordo / Border

## Caratteristiche Tecniche / Technical Features

### Hybrid Approach
- **Top Screen**: Console libctru (testo)
- **Bottom Screen**: citro2d (grafica 2D)
- Rendering separato per ogni schermo
- No conflitti tra console e grafica

### Performance
- Rendering solo quando necessario
- Frame sync per evitare flickering
- Basso impatto sulla CPU
- Non rallenta download/estrazione

### Fallback
Se citro2d non si inizializza:
If citro2d fails to initialize:
- Fallback automatico a console pura
- Messaggio di avviso mostrato
- Tutte le funzionalità rimangono disponibili

## Esempio di Utilizzo / Usage Example

### 1. Avvio / Startup
```
Top Screen: Mostra menu principale
Bottom Screen: Vuoto (nero)
```

### 2. Durante Download / During Download
```
Top Screen: 
  "Downloading file 2 of 3"
  "URL: https://example.com/file.zip"
  "Progress: 128.5 MB / 256.0 MB"
  "Percentage: 50.2%"
  "Press B to cancel"

Bottom Screen:
  [Barra di progresso gialla al 50%]
```

### 3. Durante Estrazione / During Extraction
```
Top Screen:
  "Extracting archive 2 of 3"
  "Files extracted: 42"
  "Current file: data/textures/player.png"
  "Press B to cancel"

Bottom Screen:
  [Barra di progresso verde animata]
```

### 4. Completamento / Completion
```
Top Screen:
  "All downloads complete!"
  "Summary:"
  "  Successful: 3"
  "  Failed: 0"
  "  Total files extracted: 156"
  "Press START to exit"

Bottom Screen:
  [Barra piena verde al 100%]
```

## Vantaggi del Sistema Ibrido / Hybrid System Benefits

✅ **Pro:**
- Facile da implementare
- Mantiene leggibilità del testo
- Feedback visuale chiaro
- Compatibile con console esistente
- Fallback automatico
- Performance eccellenti

✅ **Rispetto a Console Pura:**
- Più attraente visivamente
- Feedback immediato sul progresso
- Utilizzo di entrambi gli schermi
- Sensazione più moderna

✅ **Rispetto a GUI Completa:**
- Più semplice da mantenere
- Più stabile
- Sviluppo più veloce
- Testo sempre leggibile
- Debug più facile

## Note per Sviluppatori / Developer Notes

### Struttura del Codice / Code Structure
```c
// GUI inizializzato all'avvio
GUI g_gui = {0};
bool g_use_gui = false;

// In main()
g_use_gui = gui_init(&g_gui);

// Durante download
if (g_use_gui) {
    gui_begin_frame(&g_gui);
    C2D_SceneBegin(g_gui.bottom_screen);
    gui_draw_progress_bar(...);
    gui_end_frame(&g_gui);
}

// Cleanup
if (g_use_gui) {
    gui_cleanup(&g_gui);
}
```

### Estensioni Future / Future Extensions

Possibili miglioramenti:
Possible improvements:
- Icone per tipi di file
- Touch screen per interazione
- Grafici di velocità download
- Animazioni più complesse
- Temi colore personalizzabili
