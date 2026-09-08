# stb_image dependency record

- Upstream: <https://github.com/nothings/stb>
- File: `stb_image.h`
- Upstream commit: `2c980bb59875b0d32144a71867fbdebb2f77cd20`
- SHA-256: `594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3`
- Reported version: 2.30
- License: public domain or MIT; the full alternative license text is included in `stb_image.h`

MiniStudio vendors this single header so builds do not depend on network access or a platform package manager. Only the image-decoding implementation may include it; rendering resource headers must not expose stb types.

To update it, select and record a new upstream commit, replace the header from that exact commit, update the checksum above, and rerun Debug and Sanitizer validation with valid and invalid image inputs.
