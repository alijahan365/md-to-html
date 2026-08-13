# 🚀 Markdown-to-HTML Compiler & Web Studio

> **CSE314: Compiler Design Lab Project**  
> A complete 4-Phase Markdown-to-HTML Compiler featuring line and column error tracking, a dynamic Symbol Table for headings and links, Flex & Bison specification support, and an interactive Web IDE Studio.

---

## 👥 Group & Course Information

| Parameter | Details |
| :--- | :--- |
| **Course Title** | Compiler Design Lab |
| **Course Code** | CSE314 |
| **Course Instructor** | **S.K.M. Shadekul Islam** |
| **Institution** | Department of Computer Science & Engineering |

### 👨‍💻 Group Members
1. **Ali Jahan Riashad** — ID: `242-15-846`
2. **Araft Islam** — ID: `242-15-388`

---

## 📌 Project Overview

This project implements a functional **Markdown-to-HTML Compiler and Translator** designed according to formal compiler theory principles. It translates Markdown text into standard, semantic HTML5 markup.

### 🌟 Key Highlights:
- **Handwritten Compiler Engine (C99)**: Zero external dependency, recursive descent parser, post-order AST traversal code generator.
- **Flex & Bison Specifications (`lexer.l` & `parser.y`)**: Includes formal LALR parser and Lexical scanner matching the CSE314 lab syllabus.
- **Precise Error Reporting**: Emits exact `Line X, Column Y` positions for syntax errors and semantic warnings.
- **Dynamic Symbol Table**: Tracks document Headings (with slugified anchor IDs) and Hyperlinks with duplicate anchor detection.
- **Vercel / VS Code Style Web Studio (`index.html`)**: Interactive dark theme Web UI displaying Tokens, AST Tree, Symbol Table, Diagnostic Logs, and Live Browser Preview.

---

## 🏗️ Compiler Architecture & 4-Phase Pipeline

```
  ┌─────────────────┐       ┌─────────────────┐       ┌──────────────────┐
  │   sample.md     │ ────► │  Phase 1: Lexer │ ────► │ Phase 2: Parser  │
  │ (Markdown Text) │       │(Line & Col Tokens)│     │(Recursive Descent│
  └─────────────────┘       └─────────────────┘       │   or Bison AST)  │
                                                      └──────────────────┘
                                                               │
                                                               ▼
  ┌─────────────────┐       ┌─────────────────┐       ┌──────────────────┐
  │   output.html   │ ◄──── │ Phase 4: CodeGen│ ◄──── │Phase 3: Semantics│
  │ (Rendered Page) │       │ (HTML5 Traversal│       │  & Symbol Table  │
  └─────────────────┘       └─────────────────┘       └──────────────────┘
```

---

## 🔬 Compiler Phase Breakdown

### Phase 1: Lexical Scanner (`lexer.c` / `lexer.l`)
- Scans input Markdown character-by-character.
- Emits 14 token types (`HEADER1..3`, `BOLD`, `ITALIC`, `UNORDERED_LIST`, `ORDERED_LIST`, `LINK_START/MID/END`, `TEXT`, `NEWLINE`, `BLANK_LINE`, `EOF`).
- Tracks exact line numbers (`line_number`) and column positions (`column_number`).

### Phase 2: Syntax Parser (`parser.c` / `parser.y`)
- Constructs an Abstract Syntax Tree (AST) using a Top-Down Recursive Descent approach (C engine) and Context-Free Grammar rules (Bison engine).
- Generates AST nodes for document blocks (`HEADER`, `PARAGRAPH`, `LIST_ITEM`, `BOLD`, `ITALIC`, `LINK`).

### Phase 3: Symbol Table & Semantic Analyzer (`symbol_table.c` & `semantic.c`)
- Extracts document Headings and Link URLs into a dynamic Symbol Table with `L{line}:C{col}` source coordinates.
- Validates semantic integrity: checks for empty headings, missing link URLs, and flags duplicate heading anchor IDs.

### Phase 4: HTML Code Generator (`html_generator.c`)
- Traverses AST nodes post-order and outputs clean HTML5 tags (`<h1>`, `<h2>`, `<h3>`, `<p>`, `<ul>`, `<ol>`, `<li>`, `<strong>`, `<em>`, `<a href="...">`).

---

## 🚀 How to Run the Project

### 1. Web IDE Studio (Live Browser Demo)
Simply open `index.html` in any web browser or use VS Code Live Server:
```cmd
start index.html
```
*Features interactive tabbed views for Live Preview, Raw HTML Code, Token Table, AST Tree, Symbol Table, Diagnostics, and Pipeline Architecture Diagram.*

---

### 2. Standalone C Compiler Execution (CLI)

#### Build using GCC:
```cmd
gcc -Wall -Wextra -std=c99 -o md2html.exe utils.c lexer.c parser.c symbol_table.c semantic.c html_generator.c main.c
```
*(Or use `mingw32-make` if installed)*

#### Run Compiler on Markdown File:
```cmd
.\md2html.exe sample.md output.html
```

---

### 3. Flex & Bison Execution (CSE314 Lab Syllabus)

Run the standard lab commands in VS Code terminal:

```powershell
bison -d parser.y
flex lexer.l
gcc parser.tab.c lex.yy.c -o flex_bison_md2html.exe
.\flex_bison_md2html.exe sample.md
```

---

## 📁 Repository Structure

```
md-to-html/
├── lexer.l            # Flex Scanner Specification (CSE314 Lab)
├── parser.y           # Bison Parser Specification (CSE314 Lab)
├── main.c             # C CLI Compiler Driver Entry Point
├── lexer.h / .c       # Handwritten Lexical Analyzer (Line & Col tracking)
├── parser.h / .c      # Handwritten Recursive Descent Parser
├── symbol_table.h/.c  # Symbol Table Data Structures & Extraction
├── semantic.h / .c    # Semantic Validation & Error Reporting
├── html_generator.h/.c# HTML Code Generator (AST Traversal)
├── utils.h / .c       # AST Node Data Structures & Memory Utilities
├── index.html         # Web IDE Studio Layout & Visualizer
├── style.css          # Modern Dark Theme IDE Styling
├── compiler.js        # Web Compiler Engine (Mirror JS Implementation)
├── sample.md          # Sample Input Markdown Document
└── Makefile           # Compilation & Build Automation
```

---

## 📜 License
This project is created for academic coursework in **CSE314: Compiler Design Lab**.
