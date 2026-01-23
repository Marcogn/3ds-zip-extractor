# Security Policy

## Supported Versions

Currently, only the latest version is supported with security updates.

| Version | Supported          |
| ------- | ------------------ |
| 1.4.x   | :white_check_mark: |
| < 1.4   | :x:                |

## Security Considerations

### SSL/TLS

The application disables SSL certificate verification for HTTPS connections due to limitations with certificate stores on the 3DS platform. This is a known trade-off:

- **Risk**: Connections may be vulnerable to man-in-the-middle attacks
- **Mitigation**: 
  - Use trusted networks when possible
  - Verify file integrity after download (checksums)
  - Only download from trusted sources

This is documented in the README and is a common practice for 3DS homebrew applications.

### Downloaded Content

This application downloads and extracts files from user-specified URLs:

- **User Responsibility**: Ensure URLs are from trusted sources
- **Risk**: Malicious archives could contain harmful content
- **Mitigation**: Only use URLs from sources you trust

### Configuration File

The application reads from `sdmc:/3ds/zip-extractor/config.txt`:

- Stored in plain text
- User-editable
- No execution of code from config
- Only reads URLs and settings

## Reporting a Vulnerability

If you discover a security vulnerability:

1. **Do NOT open a public issue**
2. Contact the maintainer privately:
   - Open a Security Advisory on GitHub (preferred)
   - Or email the maintainer (see GitHub profile)
3. Include:
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if any)

### Response Timeline

- **Initial Response**: Within 48 hours
- **Status Update**: Within 1 week
- **Fix Released**: Depends on severity
  - Critical: As soon as possible
  - High: Within 2 weeks
  - Medium: Within 1 month
  - Low: Next release cycle

## Security Best Practices for Users

1. **Use Trusted Networks**: Avoid public WiFi for downloads
2. **Verify Sources**: Only download from reputable sites
3. **Keep Updated**: Use the latest version of the app
4. **Check Files**: Verify downloaded files before use
5. **Backup**: Always backup your SD card before use

## Acknowledgments

We appreciate responsible disclosure of security issues and will acknowledge contributors (with permission) in release notes.
