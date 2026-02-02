# Documentazione 3DS Archive Extractor

Benvenuto nella documentazione completa di 3DS Archive Extractor!

---

## 📚 Guide Principali

### Per Utenti

📖 **[Guida Utente](USER_GUIDE.md)**  
Istruzioni dettagliate per installazione, configurazione e utilizzo dell'applicazione.

### Per Sviluppatori

🔧 **[Documentazione Tecnica](TECHNICAL.md)**  
Architettura, API, formati supportati e dettagli implementazione.

💻 **[Guida Sviluppatori](DEVELOPMENT.md)**  
Note per chi vuole contribuire o modificare il codice.

---

## 📁 Struttura Documentazione

```
docs/
├── README.md          # Questo file
├── USER_GUIDE.md      # Guida utente completa
├── TECHNICAL.md       # Documentazione tecnica
├── DEVELOPMENT.md     # Note sviluppatori
└── archive/           # Documentazione storica
    ├── README_v1.md
    └── [altri file storici...]
```

---

## 🔗 Link Rapidi

### Documentazione Root
- [README principale](../README.md) - Panoramica progetto
- [CONTRIBUTING.md](../CONTRIBUTING.md) - Come contribuire
- [SECURITY.md](../SECURITY.md) - Policy sicurezza
- [LICENSE](../LICENSE) - Licenza MIT

### Repository
- [GitHub Repository](https://github.com/Marcogn/3ds-zip-extractor)
- [Issues](https://github.com/Marcogn/3ds-zip-extractor/issues)
- [Pull Requests](https://github.com/Marcogn/3ds-zip-extractor/pulls)
- [Releases](https://github.com/Marcogn/3ds-zip-extractor/releases)

---

## 📋 FAQ Rapide

### Installazione
**D: Dove copio il file .3dsx?**  
R: Nella cartella `/3ds/` della tua scheda SD.

**D: Serve Custom Firmware?**  
R: No, basta l'Homebrew Launcher.

### Utilizzo
**D: Quali formati supporta?**  
R: ZIP, TAR, TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR e altri. Vedi [README](../README.md#formati-supportati).

**D: Funziona con Google Drive?**  
R: Sì, converte automaticamente gli URL di Google Drive.

### Problemi
**D: Download fallisce sempre**  
R: Verifica connessione WiFi e prova ad abilitare `auto_retry=true`.

**D: App crash con archivi grandi**  
R: Prova su New 3DS per più RAM e CPU potente.

Per altre domande, vedi la [Guida Utente](USER_GUIDE.md).

---

## 🆕 Changelog

### v2.0 (2026-02-02)
- ✨ Supporto multi-formato (11+ formati)
- ✨ Rilevamento automatico formato
- ✨ Estrazione via libarchive
- ⚡ Performance migliorate su New 3DS
- 📚 Documentazione completamente riscritta

### v1.0
- Supporto base ZIP
- Download HTTP/HTTPS
- GUI console

---

## 🤝 Contribuire

Vuoi contribuire? Fantastico! 

1. Leggi [CONTRIBUTING.md](../CONTRIBUTING.md)
2. Consulta [DEVELOPMENT.md](DEVELOPMENT.md) per dettagli tecnici
3. Apri una Pull Request

---

## 📧 Supporto

- 🐛 **Bug**: Apri una [Issue](https://github.com/Marcogn/3ds-zip-extractor/issues)
- 💡 **Feature Request**: Apri una [Issue](https://github.com/Marcogn/3ds-zip-extractor/issues)
- 📖 **Domande**: Consulta prima la documentazione

---

**Ultima revisione**: 2 Febbraio 2026  
**Versione docs**: 2.0
