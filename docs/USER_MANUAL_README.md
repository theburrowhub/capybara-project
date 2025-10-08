# User Manual - Compilation Guide

This directory contains a comprehensive LaTeX-based user manual for the Capybara Shoot'em Up game.

## Files

- **USER_MANUAL.tex** - Main LaTeX source file
- **USER_MANUAL.pdf** - Compiled PDF (generated)

## Prerequisites

### macOS

```bash
# Install MacTeX (includes all necessary LaTeX packages)
brew install --cask mactex

# Or install BasicTeX (smaller, may need additional packages)
brew install --cask basictex

# If using BasicTeX, install required packages:
sudo tlmgr update --self
sudo tlmgr install geometry tikz tcolorbox enumitem tabularx booktabs fancyhdr hyperref fontawesome5 pgf
```

### Linux (Ubuntu/Debian)

```bash
# Install TeX Live with required packages
sudo apt install texlive-latex-extra texlive-fonts-extra texlive-pictures

# Install fontawesome5 (may need)
sudo apt install texlive-fonts-recommended
```

### Linux (Fedora)

```bash
sudo dnf install texlive-scheme-full
```

### Windows

Download and install MiKTeX from: https://miktex.org/download

## Compiling the PDF

### Method 1: Using pdflatex directly

```bash
cd docs
pdflatex USER_MANUAL.tex
```

You may need to run it twice to resolve all references:

```bash
pdflatex USER_MANUAL.tex
pdflatex USER_MANUAL.tex
```

### Method 2: Using latexmk (recommended)

```bash
cd docs
latexmk -pdf USER_MANUAL.tex
```

### Method 3: Using the project Makefile

From the project root:

```bash
make manual
```

This will automatically compile the manual and place it in the docs directory.

## Output

The compilation will generate:
- **USER_MANUAL.pdf** - The final user manual (this is what you want!)
- USER_MANUAL.aux - Auxiliary file (can be deleted)
- USER_MANUAL.log - Compilation log (useful for debugging)
- USER_MANUAL.out - Outline file (can be deleted)
- USER_MANUAL.toc - Table of contents file (can be deleted)

## Cleaning Up

To remove generated files:

```bash
# Remove all auxiliary files
rm USER_MANUAL.aux USER_MANUAL.log USER_MANUAL.out USER_MANUAL.toc

# Or use latexmk
latexmk -c USER_MANUAL.tex

# To remove PDF as well
latexmk -C USER_MANUAL.tex
```

From project root with Makefile:

```bash
make clean-manual
```

## Troubleshooting

### Missing Packages

If you get errors about missing packages:

```bash
# macOS
sudo tlmgr install <package-name>

# Linux (TeX Live)
sudo apt install texlive-<package-category>
```

### fontawesome5 Issues

If fontawesome5 is not found:

```bash
# macOS
sudo tlmgr install fontawesome5

# Linux
sudo apt install texlive-fonts-extra
```

### Compilation Errors

1. **Check the log file**: `USER_MANUAL.log`
2. **Ensure all packages are installed**
3. **Try compiling twice**: Some references need two passes
4. **Update TeX distribution**: `sudo tlmgr update --all` (macOS)

## Customization

The manual uses several customizable elements:

### Colors

Edit the color definitions in the preamble:
```latex
\definecolor{primarycolor}{RGB}{41,128,185}
\definecolor{secondarycolor}{RGB}{52,152,219}
\definecolor{warningcolor}{RGB}{231,76,60}
```

### Version Number

Update in the title section:
```latex
\author{Version 0.9}
```

### Date

The date is automatically set to compilation date with `\today`. To use a fixed date:
```latex
\date{January 2025}
```

## Features

The manual includes:

- ✅ Professional layout with custom headers/footers
- ✅ Color-coded sections and information boxes
- ✅ TikZ graphics for visual clarity
- ✅ Tables for quick reference
- ✅ Comprehensive controls guide
- ✅ Strategy tips and advanced tactics
- ✅ Troubleshooting section
- ✅ Quick reference card
- ✅ Hyperlinked table of contents

## Distribution

To distribute the manual:

1. Compile the PDF: `pdflatex USER_MANUAL.tex`
2. Verify it looks correct
3. Distribute only the **USER_MANUAL.pdf** file
4. Users don't need LaTeX to read the PDF!

## Printing

The manual is designed for:
- **Page Size**: A4 (can change to US Letter if needed)
- **Orientation**: Portrait
- **Color**: Full color (can be printed in grayscale)
- **Duplex**: Suitable for double-sided printing

To change to US Letter format, edit the documentclass line:
```latex
\documentclass[11pt,letterpaper]{article}
```

## Contributing

To improve the manual:

1. Edit `USER_MANUAL.tex`
2. Compile and verify changes
3. Submit updates
4. Keep content user-focused (not developer-focused)

## Notes

- The manual is written for **players**, not developers
- It focuses on gameplay, not code
- Graphics are created with TikZ (vector graphics, always crisp)
- All content is based on the actual game implementation
- Version numbers should match the game version

---

**Need help?** Check the main project documentation or open an issue.
