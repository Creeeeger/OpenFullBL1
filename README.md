# OpenFullBL1 (Exynos990)

This BL1 stage is a full BL1 implementation intended for EPBL handoff and
related boot-path experimentation.

Load address: `0x02022000`  
Entry point: `0x02022010`

The BL1 footer must be replaced with the one from your own device:

1. Download `sboot.bin`.
2. Split it.
3. Copy data from your original BL1 from offset `9920` to the end.
4. Use that data as your new `bl1-footer`.

This BL1 must be sent via BootROM.

## Upstream Credit

This project builds on work by Umer Uddin (`halal-beef`), who laid the
foundation with OpenMiniBL1:
https://github.com/halal-beef/OpenMiniBL1

## License

This project is distributed under the GNU General Public License v2.0
(`GPL-2.0-only`). See [LICENSE](LICENSE).

## GPLv2 Distribution Notes

If you distribute `bl1.bin` or any modified binaries based on this project, you
must also provide the corresponding source code, including build scripts and
your modifications, under GPLv2 terms.

At minimum when redistributing:

1. Keep copyright and license notices intact.
2. Provide the complete corresponding source for the distributed binary.
3. Include the GPLv2 license text (`LICENSE`).
4. Mark modified files with clear change notices and dates.

## Build

```bash
make
```

Build output:

- `bl1.bin`
