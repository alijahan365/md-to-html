# 🚀 Markdown-to-HTML Compiler & Web Studio

[![Course](https://img.shields.io/badge/Course-CSE314%20Compiler%20Design%20Lab-blue.svg)](https://github.com/alijahan365/md-to-html)
[![Standard](https://img.shields.io/badge/Language-C99%20%7C%20Flex%20%7C%20Bison-green.svg)](https://github.com/alijahan365/md-to-html)
[![License](https://img.shields.io/badge/License-Academic-orange.svg)](https://github.com/alijahan365/md-to-html)

> **CSE314: Compiler Design Lab Project**  
> A functional 4-phase Markdown-to-HTML compiler implementing lexical analysis with exact line/column error tracking, top-down AST parsing, dynamic symbol table resolution for headings and links, semantic validation, and clean HTML5 code generation.

---

## 👥 Group & Course Details

| Parameter | Details |
| :--- | :--- |
| **Course Title** | Compiler Design Lab |
| **Course Code** | CSE314 |
| **Course Instructor** | **S.K.M. Shadekul Islam** |
| **Department** | Computer Science & Engineering |

### 👨‍💻 Group Members
1. **Ali Jahan Riashad** — ID: `242-15-846`
2. **Araft Islam** — ID: `242-15-388`

---

## 📌 Project Overview

This project implements a formal **Markdown-to-HTML Compiler and Translator** following classic compiler theory architecture. It converts Markdown source documents into valid, semantic HTML5 structure.

### Key Technical Features:
- **Handwritten C Compiler Engine (C99)**: Zero external runtime dependencies, recursive descent parser, AST data structure, and post-order traversal code generation.
- **Flex & Bison Specifications (`lexer.l` & `parser.y`)**: Formal LALR syntax parser and Lexical scanner aligned with the CSE314 lab syllabus.
- **Precise Diagnostic Reporting**: Line and column (`Line X, Column Y`) tracking for syntax errors and semantic warnings.
- **Dynamic Symbol Table**: Manages document Headings (with slugified anchor IDs) and Hyperlinks with duplicate link/anchor detection.
- **Modern Web Studio (`index.html`)**: Interactive dark theme Web UI for live browser preview, token stream, AST visualization, symbol table inspection, and real-time diagnostics.

---

## 🏗️ Compiler Architecture & 4-Phase Pipeline

```
  ┌───────────────────┐        ┌───────────────────┐        ┌───────────────────┐
  │    sample.md      │ ─────► │  Phase 1: Lexer   │ ─────► │  Phase 2: Parser  │
  │  (Markdown Text)  │        │ (Tokens + L#:C#)  │        │(Recursive Descent │
  └───────────────────┘        └───────────────────┘        │   or Bison AST)   │
                                                            └───────────────────┘
                                                                      │
                                                                      ▼
  ┌───────────────────┐        ┌───────────────────┐        ┌───────────────────┐
  │    output.html    │ ◄───── │  Phase 4: CodeGen │ ◄───── │ Phase 3: Semantics│
  │  (HTML5 Document) │        │(HTML5 Generation) │        │  & Symbol Table   │
  └───────────────────┘        └───────────────────┘        └───────────────────┘
```

---

## 🔬 Compiler Phase Breakdown

### Phase 1: Lexical Analyzer (`lexer.c` / `lexer.l`)
- Scans input Markdown character-by-character.
- Emits token types (`HEADER1..3`, `BOLD`, `ITALIC`, `UNORDERED_LIST`, `ORDERED_LIST`, `LINK_START/MID/END`, `TEXT`, `NEWLINE`, `BLANK_LINE`, `EOF`).
- Tracks precise source coordinates (`line_number`, `column_number`).

### Phase 2: Syntax Parser (`parser.c` / `parser.y`)
- Builds an Abstract Syntax Tree (AST) using a Top-Down Recursive Descent parser (C engine) and Context-Free Grammar rules (Bison engine).
- Generates AST nodes for document structural units (`HEADER`, `PARAGRAPH`, `LIST_ITEM`, `BOLD`, `ITALIC`, `LINK`).

### Phase 3: Symbol Table & Semantic Analyzer (`symbol_table.c` & `semantic.c`)
- Maintains a dynamic symbol table for document headings and link URLs with source position mappings.
- Performs semantic checks: flags empty headings, invalid link structures, and duplicate anchor IDs.

### Phase 4: HTML Code Generator (`html_generator.c`)
- Performs post-order traversal on the AST to emit clean, semantic HTML5 tags (`<h1>`, `<h2>`, `<h3>`, `<p>`, `<ul>`, `<ol>`, `<li>`, `<strong>`, `<em>`, `<a href="...">`).

---

## 🎯 CEP (Complex Engineering Problem) Alignment

| Requirement | Project Implementation |
| :--- | :--- |
| **1. Depth of Knowledge** | Implements formal lexical scanning, AST construction, dynamic symbol table management, and semantic verification. |
| **2. Conflicting Requirements** | Balances handwritten recursive descent parsing simplicity against formal Flex/Bison LALR grammar requirements. |
| **3. Depth of Analysis** | Evaluates Markdown grammar ambiguities (e.g. inline formatting vs block list items) and handles line/column diagnostics. |
| **4. Multiple Components** | Integrates 5 connected compiler components: Lexer, Parser, Symbol Table, Semantic Analyzer, and Code Generator. |
| **5. Unfamiliar Issues** | Handles edge cases including malformed Markdown, unclosed tags, empty headers, and duplicate anchor IDs. |

---

## 🚀 Execution & Usage Guide

### 1. Web IDE Studio (Browser Interface)
Double click or open `index.html` in any web browser, or launch via terminal:
```cmd
start index.html
```

---

### 2. Standalone C Compiler (CLI Execution)

#### Build using GCC:
```cmd
gcc -Wall -Wextra -std=c99 -o md2html.exe utils.c lexer.c parser.c symbol_table.c semantic.c html_generator.c main.c
```

#### Compile Markdown file to HTML:
```cmd
.\md2html.exe sample.md output.html
```

---

### 3. Flex & Bison Execution (CSE314 Lab Syllabus)

Compile and run using standard lab tools:

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
├── main.c             # Compiler Driver Entry Point
├── lexer.h / .c       # Handwritten Lexical Analyzer (Line & Column tracking)
├── parser.h / .c      # Handwritten Recursive Descent Parser
├── symbol_table.h/.c  # Symbol Table Data Structures & Lookup Routines
├── semantic.h / .c    # Semantic Validation & Diagnostic Reporting
├── html_generator.h/.c# HTML Code Generator (AST Post-Order Traversal)
├── utils.h / .c       # AST Node Definitions & Utility Functions
├── index.html         # Modern Web Studio Interface
├── style.css          # IDE Dark Theme Layout & Component Styles
├── compiler.js        # Web Studio Client-Side Compiler Engine
├── sample.md          # Sample Input Markdown Document
└── Makefile           # Build Automation Script
```

---

## 📜 License
Created for academic evaluation in **CSE314: Compiler Design Lab**.
