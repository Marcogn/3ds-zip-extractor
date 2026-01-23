# Contributing to 3DS Zip Extractor

Thank you for your interest in contributing to 3DS Zip Extractor!

## How to Contribute

### Reporting Issues

If you find a bug or have a feature request:

1. Check if the issue already exists in the [Issues](https://github.com/Marcogn/3ds-zip-extractor/issues) section
2. If not, create a new issue with:
   - A clear title and description
   - Steps to reproduce (for bugs)
   - Expected vs actual behavior
   - Your 3DS model and CFW version
   - Screenshots if applicable

### Submitting Changes

1. **Fork the Repository**
   ```bash
   git clone https://github.com/YOUR_USERNAME/3ds-zip-extractor.git
   cd 3ds-zip-extractor
   ```

2. **Create a Branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make Your Changes**
   - Follow the existing code style
   - Test your changes on real 3DS hardware if possible
   - Update documentation as needed

4. **Commit Your Changes**
   ```bash
   git add .
   git commit -m "Add: Brief description of changes"
   ```

5. **Push and Create Pull Request**
   ```bash
   git push origin feature/your-feature-name
   ```
   Then open a Pull Request on GitHub

## Development Setup

### Requirements

- devkitARM toolchain
- devkitPro libraries: libctru, libcurl, libarchive, mbedtls, citro3d, citro2d

See [INSTALL.md](INSTALL.md) for detailed setup instructions.

### Building

```bash
make clean
make
```

Output: `3ds-zip-extractor.3dsx`

### Testing

- Test on Citra emulator for basic functionality
- **Always test on real 3DS hardware** before submitting PR
- Test different scenarios:
  - Small files (<10MB)
  - Large files (>100MB)
  - Google Drive URLs
  - Failed downloads (network interruption)
  - Multiple files in queue

## Code Guidelines

### C Code Style

- Use descriptive variable names
- Comment complex logic
- Follow existing formatting (indentation, braces, etc.)
- Keep functions focused and manageable
- Handle errors gracefully

### Documentation

- Update README.md if adding features
- Add examples to EXAMPLES.md
- Update CHANGELOG.md with your changes
- Comment non-obvious code

## Areas for Contribution

We welcome contributions in these areas:

- **Bug Fixes**: Always appreciated!
- **Performance Improvements**: Optimization for large files
- **New Features**: See issues tagged with "enhancement"
- **Documentation**: Translations, clarifications, examples
- **Testing**: Reports from different 3DS models and scenarios
- **UI/UX**: Improvements to the hybrid GUI

## Pull Request Process

1. Ensure your code builds without errors
2. Test on real hardware when possible
3. Update documentation
4. Add your changes to CHANGELOG.md
5. Describe your changes clearly in the PR

## Questions?

Feel free to open an issue for questions or discussions!

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
