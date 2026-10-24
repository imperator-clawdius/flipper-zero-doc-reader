# HANDOFF — flipper-zero-doc-reader

**One-liner:** A tiny, dependency-free text/document viewer app (`.fap`) for the Flipper Zero that reads SD-card files with on-device scrolling.
**Status:** live — ~95% to shippable (v1.0 released; roadmap items are nice-to-haves)
**Monetization:** Free MIT community app; not directly monetizable — distribution (not revenue) via the Flipper App Catalog.

## What it is
A single-file C external app that uses only stable Flipper public APIs (gui/dialogs/storage/text_box) to let a user pick any SD-card file and read it in a scrolling text box. Reads the first 8 KB (larger files are truncated with a note). Good README/docs already exist — this handoff is intentionally brief and defers to `README.md` for full build/install/compatibility detail.

## Who pays & why
No one pays — it is a free utility for Flipper Zero owners who want to read `.txt`/`.md`/notes/logs on-device (stock firmware has no general text reader). Value is adoption and reputation, not revenue.

## Stack
C against the Flipper Zero firmware SDK (`furi`, `gui`, `dialogs`, `storage`, `text_box`); built/installed with `ufbt` (micro Flipper Build Tool). Manifest: `application.fam`. Targets official firmware plus Momentum/RogueMaster.

## Architecture
User launches app (Apps → Tools) → DialogsApp file browser picks a file from `/ext` → `doc_reader.c` loads up to 8 KB via the storage API into a `text_box` view → Up/Down scroll, Back returns to browser/exits. Key files: `doc_reader.c` (the entire app), `application.fam` (manifest), `doc_reader_10px.png` (icon), `README.md` (full build/install/compat docs).

## What works today
- Opens any SD-card file and renders it as scrolling text; Back navigation to browser and exit.
- 8 KB read cap with a clear on-screen truncation note; fixed 4 KB app stack.
- Builds with `ufbt` (auto-downloads the matching SDK); installs/launches over USB.
- Intended-compatible with official, Momentum, and RogueMaster firmware (stable APIs only).

## Gaps to ship (ranked)
1. Stream large files page-by-page instead of the 8 KB cap (roadmap).
2. Optional smaller/larger font toggle (roadmap).
3. Remember last-read position per file (roadmap).

## Run it
```bash
python -m pip install --upgrade ufbt
python -m ufbt            # builds dist/doc_reader.fap
python -m ufbt install    # copy the .fap to a connected Flipper
python -m ufbt launch     # build, upload to a temp dir, and run it now
# then on device: Apps → Tools → Document Reader
```

## Risks / unknowns
- Custom firmwares occasionally bump the app API; a mismatch requires rebuilding against that firmware's SDK (see `README.md`).
- The 8 KB cap and text-box formatting limit very large or binary files.
- No standalone revenue model; wider distribution depends on Flipper App Catalog acceptance.

## Fastest path to revenue
Not a revenue product. Ship to the Flipper App Catalog for reach and use it as a portfolio/credibility piece; the roadmap items (large-file streaming, font toggle, resume position) strengthen that case.
