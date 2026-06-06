# img-forensics

[![Language-C](https://img.shields.io/badge/Language-C-00599C)](main.c) [![Build-Make](https://img.shields.io/badge/Build-Make-6f42c1)](Makefile) [![Status-Manual](https://img.shields.io/badge/Status-Manual--run-7a7a7a)](README.md)

A small C program for image forensics that detects copy-move (cloned)
regions and draws lines between similar areas. This is a didactic project
focused on basic block-comparison detection techniques.

## Table of Contents

- [What This Project Does](#what-this-project-does)
- [Why This Project Is Useful](#why-this-project-is-useful)
- [Algorithm Overview](#algorithm-overview)
- [Getting Started](#getting-started)
- [Usage Examples](#usage-examples)
- [Project Structure](#project-structure)
- [Where To Get Help](#where-to-get-help)
- [Maintainers and Contributions](#maintainers-and-contributions)

## What This Project Does

The program scans an input image in small blocks, selects textured blocks (high variance), and compares them with other blocks. When two blocks match within a threshold and are sufficiently distant, a red line is drawn between their centers. The annotated image is saved as `saida.jpg`.

- `main.c` — main detection algorithm and image I/O.
- `include/stb_image.h` and `include/stb_image_write.h` — lightweight image loaders/writers (STB single-file headers).

## Why This Project Is Useful

- Demonstrates a simple, interpretable copy-move detection technique using block matching.
- Provides a compact, runnable C example to study basic image-processing concepts and performance trade-offs.
- Easy to extend: change the comparison metric, add multiscale scanning, or improve visualizations.

## Algorithm Overview

- Load the image as RGB pixels using `stb_image`.
- Partition the image into overlapping blocks (`blockSize`) with stride `step`.
- Compute variance per block and skip low-variance blocks.
- For remaining blocks, accumulate per-pixel absolute color differences; if the accumulated difference is below `threshold * blockSize * blockSize`, the blocks are considered matching (subject to `minDist`).
- Draw lines between matched block centers using a Bresenham implementation and write `saida.jpg`.

Detection parameters (`blockSize`, `step`, `threshold`, `minVariance`, `minDist`) are defined in `main.c`.

## Getting Started

### Prerequisites

- `gcc` (or equivalent) and `make` on Linux/macOS. On Windows use MinGW or WSL.

### Build

```bash
make
```

This builds the `forensics` executable using the repository `Makefile`.

### Run

```bash
./forensics input/cars.jpg
```

The program prints image information and writes `saida.jpg` with detected matches.

## Usage Examples

- Run on an example image:

```bash
./forensics input/futebol.jpg
```

- Tune detection by editing parameters in `main.c` and rebuilding.

## Project Structure

```text
.
├── main.c
├── Makefile
├── CMakeLists.txt
└── include/
  ├── stb_image.h
  └── stb_image_write.h
```

## Where To Get Help

- Inspect `main.c` for algorithm details and parameters.

## Maintainers

Maintainer:

- [@kydoa](https://github.com/kydoa)
