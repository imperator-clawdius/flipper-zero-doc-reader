# Document Reader — Flipper Zero

A small, dependency-free text viewer for the Flipper Zero. Pick any file from
the SD card and read it on-device with smooth scrolling. Built because the
stock firmware has no general way to read `.txt`, `.md`, notes, or logs on the
device itself.

## Features

- Opens **any file** on the SD card and renders it as text (`.txt`, `.md`,
  `.log`, `.csv`, config files, and so on).
- Uses the firmware's built-in scrolling text box — **Up/Down** to scroll.
- **Back** returns to the file browser to pick another file; **Back** again
  from the browser exits the app.
- Reads the first 8 KB of a file (enough for notes and most docs). Larger
  files are shown truncated with a clear on-screen note.
- No firmware changes, no extra services. It is a normal external app
  (`.fap`) that loads from `Apps → Tools`.

## Controls

| Button      | Action                                      |
|-------------|---------------------------------------------|
| Up / Down   | Scroll the document                         |
| Back        | Return to the file browser                  |
| Back (again)| Exit the app from the browser               |

## Building

This app builds with [`ufbt`](https://github.com/flipperdevices/flipperzero-ufbt),
the micro Flipper Build Tool.

```sh
python -m pip install --upgrade ufbt
cd flipper-doc-reader
python -m ufbt            # builds dist/doc_reader.fap
```

The first build downloads the matching SDK automatically.

## Installing

Copy the built `dist/doc_reader.fap` to your Flipper's SD card under
`apps/Tools/`, or let ufbt do it over USB:

```sh
python -m ufbt install    # copy the .fap to the connected Flipper
python -m ufbt launch     # build, upload to a temp dir, and run it now
```

Then on the device: **Apps → Tools → Document Reader**.

## Firmware compatibility

Built against the official Flipper Zero release SDK. It uses only stable public
APIs (`gui`, `dialogs`, `storage`, `text_box`), so it is intended to run on
official firmware as well as **Momentum** and **RogueMaster**.

Custom firmwares occasionally bump the app API. If the `.fap` reports an API
mismatch on a given firmware, rebuild against that firmware's SDK, for example:

```sh
# Momentum / RogueMaster publish their own SDK index; point ufbt at it:
python -m ufbt update --index-url <firmware-sdk-index-url>
python -m ufbt
```

## Roadmap

- Stream large files page-by-page instead of the 8 KB cap.
- Optional smaller/larger font toggle.
- Remember last-read position per file.

## License

MIT — see [LICENSE](LICENSE).
