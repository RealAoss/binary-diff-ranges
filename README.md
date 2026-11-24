# Binary Diff Ranges

Find exactly which byte offsets differ between two files. The C17 utility reads 64 KiB from each file at a time, so it can compare large backups, media exports or disk images without loading them into memory.

## Build and run

```sh
cc -std=c17 -O2 -Wall -Wextra src/bytediff.c -o bytediff
./bytediff original.bin copy.bin
```

On Windows with a Visual Studio developer shell, use `cl /std:c17 /O2 src\bytediff.c /Fe:bytediff.exe`.

## Read the report

Standard output contains one tab-separated range per line. The start is zero-based and inclusive; the end is exclusive. For files containing `abcdef` and `aXYdeZ!`, the ranges are `1–3` and `5–7`. The second range includes the extra trailing byte. Standard error gives the total differing bytes and number of ranges.

Exit codes are 0 for identical files, 1 for differences and 2 for a usage or I/O error. Compare stable files and check the exit code before trusting redirected output. The tool compares the same offsets on both sides, so an insertion may cause differences through the rest of a file.

## Development

See [CONTRIBUTING.md](CONTRIBUTING.md) and [CHANGELOG.md](CHANGELOG.md).

## License

[MIT License](LICENSE).
