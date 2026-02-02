# 3DS Archive Extractor Documentation

Welcome to the complete documentation for 3DS Archive Extractor!

---

## 📚 Main Guides

### For Users

📖 **[User Guide](USER_GUIDE.md)**  
Detailed instructions for installation, configuration and using the application.

### For Developers

🔧 **[Technical Documentation](TECHNICAL.md)**  
Architecture, API, supported formats and implementation details.

💻 **[Developer Guide](DEVELOPMENT.md)**  
Notes for those who want to contribute or modify the code.

---

## 📁 Documentation Structure

```
docs/
├── README.md          # This file
├── USER_GUIDE.md      # Complete user guide
├── TECHNICAL.md       # Technical documentation
├── DEVELOPMENT.md     # Developer notes
└── archive/           # Historical documentation
    ├── README_v1.md
    └── [other historical files...]
```

---

## 🔗 Quick Links

### Root Documentation
- [Main README](../README.md) - Project overview
- [CONTRIBUTING.md](../CONTRIBUTING.md) - How to contribute
- [SECURITY.md](../SECURITY.md) - Security policy
- [LICENSE](../LICENSE) - MIT License

### Repository
- [GitHub Repository](https://github.com/Marcogn/3ds-zip-extractor)
- [Issues](https://github.com/Marcogn/3ds-zip-extractor/issues)
- [Pull Requests](https://github.com/Marcogn/3ds-zip-extractor/pulls)
- [Releases](https://github.com/Marcogn/3ds-zip-extractor/releases)

---

## 📋 Quick FAQ

### Installation
**Q: Where do I copy the .3dsx file?**  
A: To the `/3ds/` folder on your SD card.

**Q: Do I need Custom Firmware?**  
A: No, Homebrew Launcher is sufficient.

### Usage
**Q: What formats does it support?**  
A: ZIP, TAR, TAR.GZ, TAR.BZ2, TAR.XZ, 7Z, RAR and more. See [README](../README.md#supported-formats).

**Q: Does it work with Google Drive?**  
A: Yes, it automatically converts Google Drive URLs.

### Problems
**Q: Download always fails**  
A: Check WiFi connection and try enabling `auto_retry=true`.

**Q: App crashes with large archives**  
A: Try on New 3DS for more RAM and powerful CPU.

For other questions, see the [User Guide](USER_GUIDE.md).

---

## 🆕 Changelog

### v2.0 (2026-02-02)
- ✨ Multi-format support (11+ formats)
- ✨ Automatic format detection
- ✨ Extraction via libarchive
- ⚡ Improved performance on New 3DS
- 📚 Documentation completely rewritten

### v1.0
- Basic ZIP support
- HTTP/HTTPS downloads
- Console GUI

---

## 🤝 Contributing

Want to contribute? Great! 

1. Read [CONTRIBUTING.md](../CONTRIBUTING.md)
2. Consult [DEVELOPMENT.md](DEVELOPMENT.md) for technical details
3. Open a Pull Request

---

## 📧 Support

- 🐛 **Bug**: Open an [Issue](https://github.com/Marcogn/3ds-zip-extractor/issues)
- 💡 **Feature Request**: Open an [Issue](https://github.com/Marcogn/3ds-zip-extractor/issues)
- 📖 **Questions**: Consult the documentation first

---

**Last revision**: February 2, 2026  
**Docs version**: 2.0
