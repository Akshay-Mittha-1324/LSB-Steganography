# LSB Steganography in C

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A robust, command-line utility written in C that securely hides a secret file (such as a text document) inside a standard uncompressed BMP image using the **Least Significant Bit (LSB)** steganography technique.

## 📖 Table of Contents
- [How It Works](#how-it-works)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Compilation](#compilation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [License](#license)

## 🧠 How It Works
**Steganography** is the practice of concealing a file, message, image, or video within another file. 

This project uses the **Least Significant Bit (LSB)** technique on 24-bit `.bmp` images. 
Every pixel in a 24-bit BMP image is represented by 3 bytes (Red, Green, and Blue). By altering the 0th bit (the *Least Significant Bit*) of each color byte, we can embed our secret data. Because the change is so incredibly minute, the resulting "Stego" image looks completely identical to the original image to the human eye.

* **Rule of Thumb**: To hide 1 byte of secret data (8 bits), the algorithm manipulates 8 bytes of image data.

## ✨ Features
- **Encode**: Seamlessly embed any file inside a 24-bit BMP carrier image.
- **Decode**: Effortlessly extract the hidden file from a Stego BMP image.
- **Data Integrity**: Uses a custom **Magic String** (`#*`) embedded in the image to ensure the decoder only processes valid Stego images.
- **Dynamic Metadata Handling**: Automatically encodes the size and the original extension of the secret file, ensuring flawless reconstruction during the decoding phase.
- **Memory Safe**: Engineered with strict file pointer handling and modular architecture to prevent memory leaks and buffer overflows.

## ⚙️ Prerequisites
To build and run this project, you will need:
- **GCC Compiler** (or any standard C compiler like Clang)
- A 24-bit uncompressed `.bmp` file to use as the carrier image.

## 🛠️ Compilation
Clone the repository and compile the source code using `gcc`:

```bash
# Clone the repository
git clone https://github.com/YourUsername/LSB-Steganography.git
cd LSB-Steganography

# Compile the project
gcc main.c encode.c decode.c -o stego
```

## 🚀 Usage

### Encoding
Hide a secret file inside a BMP image. 

**Syntax:**
```bash
./stego -e <source_image.bmp> <secret_file.txt> [output_stego.bmp]
```
**Example:**
```bash
./stego -e beautiful.bmp secret.txt my_stego.bmp
```
*(Note: If the `[output_stego.bmp]` argument is not provided, the program automatically defaults to creating `stego.bmp`)*

### Decoding
Extract the hidden secret file from a Stego image.

**Syntax:**
```bash
./stego -d <stego_image.bmp> [output_filename]
```
**Example:**
```bash
./stego -d my_stego.bmp my_decoded_secret
```
*(Note: If the `[output_filename]` is not provided, it defaults to `decode_output`. The program will automatically determine and append the correct extension, e.g., `decode_output.txt`)*

## 📂 Project Structure

```text
📁 LSB-Steganography
├── 📄 main.c        # Entry point; validates CLI arguments and routes execution
├── 📄 encode.c      # Implements the bitwise LSB encoding pipeline
├── 📄 encode.h      # Function prototypes and definitions for encoding
├── 📄 decode.c      # Implements the bitwise LSB extraction pipeline
├── 📄 decode.h      # Function prototypes and definitions for decoding
├── 📄 common.h      # Shared utility macros, magic strings, and status enums
├── 📄 LICENSE       # MIT License file
└── 📄 README.md     # Project documentation
```

## 📜 License
This project is open-source and licensed under the [MIT License](LICENSE). 
You are free to use, modify, and distribute this software as you see fit. See the `LICENSE` file for more details.
