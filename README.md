# 🚀 Md2Html Compiler Studio & Formal C Engine

[![Build & Test CI](https://github.com/alijahan365/md-to-html/actions/workflows/ci.yml/badge.svg)](https://github.com/alijahan365/md-to-html/actions/workflows/ci.yml)
[![Live Demo](https://img.shields.io/badge/Live%20Demo-GitHub%20Pages-brightgreen.svg)](https://alijahan365.github.io/md-to-html/)
[![Course](https://img.shields.io/badge/Course-CSE314%20Compiler%20Design%20Lab-blue.svg)](https://github.com/alijahan365/md-to-html)
[![Standard](https://img.shields.io/badge/Language-C99%20%7C%20Flex%20%7C%20Bison-green.svg)](https://github.com/alijahan365/md-to-html)
[![License](https://img.shields.io/badge/License-MIT-orange.svg)](https://github.com/alijahan365/md-to-html/blob/main/LICENSE)

> **CSE314: Compiler Design Lab Project — Department of Computer Science & Engineering**  
> An end-to-end formal 4-phase Markdown-to-HTML compiler implementing character-by-character lexical tokenization with line & column error tracking (`Line X, Col Y`), top-down AST parsing, dynamic symbol table resolution for headings and links, semantic validation, post-order HTML5 code generation, and an interactive 5-view Web Studio.

---

🌐 **Live Web Application Studio**: [https://alijahan365.github.io/md-to-html/](https://alijahan365.github.io/md-to-html/)  
📦 **GitHub Repository**: [https://github.com/alijahan365/md-to-html](https://github.com/alijahan365/md-to-html)

---

## 📑 Table of Contents
- [👥 Academic & Group Details](#-academic--group-details)
- [📌 Project Overview](#-project-overview)
- [🏗️ Compiler Architecture & Dataflow](#%EF%B8%8F-compiler-architecture--dataflow)
- [🔬 4-Phase Pipeline Breakdown](#-4-phase-pipeline-breakdown)
- [🎯 CEP 7-Point Requirement Alignment](#-cep-7-point-requirement-alignment)
- [📊 Contribution & Work Division Matrix](#-contribution--work-division-matrix)
- [🚀 Local Build & Execution Guide](#-local-build--execution-guide)
- [📁 Repository Structure](#-repository-structure)
- [📜 License](#-license)

---

## 👥 Academic & Group Details

| Parameter | Academic Record |
| :--- | :--- |
| **Institution** | Department of Computer Science & Engineering |
| **Course Title** | Compiler Design Lab |
| **Course Code** | **CSE314** |
| **Course Instructor** | **S.K.M. Shadekul Islam** |

### 👨‍💻 Development Team
- **Ali Jahan Riashad** — Student ID: `242-15-846` *(Project Lead / Semantic Engine & Web Studio)*
- **Araft Islam** — Student ID: `242-15-388` *(Contributor / Scanner, Parser & CodeGen Engine)*

---

## 📌 Project Overview

This project implements a formal **Markdown-to-HTML Compiler and Translator** adhering strictly to classic compiler design principles. It parses structural Markdown syntax into valid, semantic HTML5 markup.

### 🌟 Key Technical Features
1. **Handwritten C Compiler Engine (C99 Standard)**: Zero external runtime dependencies, recursive descent parser, AST node tree data structures, and post-order code generation.
2. **Flex & Bison LALR Specifications (`lexer.l` & `parser.y`)**: Formal Context-Free Grammar parser and Lexical scanner compliant with CSE314 lab syllabus.
3. **Precise Source Position Diagnostics**: Exact line and column coordinates (`Line X, Column Y`) for syntax errors and semantic warnings.
4. **Dynamic Symbol Table**: Tracks document Headings (slugified anchor IDs) and Hyperlinks with duplicate anchor collision detection.
5. **Interactive 5-View Web Studio SPA (`index.html`)**: Vercel-inspired dark theme UI supporting live browser preview, token stream table, AST tree inspector, symbol table viewer, formal grammar spec inspector, and real-time execution speed metrics (`⚡ 0.28 ms`).

---

## 🏗️ Compiler Architecture & Dataflow

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

## 🔬 4-Phase Pipeline Breakdown

### Phase 1: Lexical Scanner (`lexer.c` / `lexer.l`)
- Character-by-character scanner emitting 14 token types (`HEADER1..3`, `BOLD`, `ITALIC`, `UNORDERED_LIST`, `ORDERED_LIST`, `LINK_START/MID/END`, `TEXT`, `NEWLINE`, `BLANK_LINE`, `EOF`).
- Maintains exact `line_number` and `column_number` positions for every token.

### Phase 2: Syntax Parser (`parser.c` / `parser.y`)
- Constructs an Abstract Syntax Tree (AST) using a Top-Down Recursive Descent parser (C engine) and formal LALR Context-Free Grammar rules (Bison engine).
- Creates AST nodes for document structural units (`HEADER`, `PARAGRAPH`, `LIST_ITEM`, `BOLD`, `ITALIC`, `LINK`).

### Phase 3: Symbol Table & Semantic Analyzer (`symbol_table.c` & `semantic.c`)
- Maintains a dynamic symbol table for document headings (with auto-slugified anchor IDs) and link URLs.
- Validates semantic integrity: flags empty headings, missing link URLs, and duplicate anchor ID collisions.

### Phase 4: HTML Code Generator (`html_generator.c`)
- Performs a post-order AST traversal to emit clean, standard HTML5 markup (`<h1>..<h3>`, `<p>`, `<ul>`, `<ol>`, `<li>`, `<strong>`, `<em>`, `<a href="..." target="_blank">`).

---

## 🎯 CEP 5-Point Requirement Alignment

| CEP Requirement | Technical Defense & Implementation |
| :--- | :--- |
| **1. Depth of Knowledge** | Applies core compiler concepts: character tokenization, syntax tree parsing, dynamic symbol table management, and semantic verification. |
| **2. Conflicting Requirements** | Balances handwritten recursive descent parsing simplicity against formal Flex/Bison LALR grammar specifications and error diagnostics. |
| **3. Depth of Analysis** | Evaluates Markdown grammar ambiguities (inline formatting vs block list items) and provides precise source position diagnostics. |
| **4. Multiple Components** | Integrates 5 connected compiler modules: Lexer, Parser, Symbol Table, Semantic Analyzer, and Target HTML Code Generator. |
| **5. Unfamiliar Issues** | Handles edge cases: unclosed Markdown delimiters, missing link target URLs, empty headers, and duplicate anchor collisions. |

---

## 📊 Contribution & Work Division Matrix

| Member Name | Student ID | Assigned Modules & Files |
| :--- | :--- | :--- |
| **Ali Jahan Riashad** *(Lead)* | `242-15-846` | • Lexical Scanner & Flex Spec (`lexer.h`, `lexer.c`, `lexer.l`)<br>• HTML Code Generator (`html_generator.h`, `html_generator.c`)<br>• Semantic Analyzer (`semantic.h`, `semantic.c`)<br>• Web Studio Layout (`index.html`, `style.css`)<br>• CI/CD Workflow & Makefile (`.github/workflows/ci.yml`, `Makefile`)<br>• Comprehensive Documentation (`README.md`, `LICENSE`) |
| **Araft Islam** *(Contributor)* | `242-15-388` | • AST Structs & Allocation (`utils.h`, `utils.c`)<br>• Syntax Parser & Bison Spec (`parser.h`, `parser.c`, `parser.y`)<br>• Dynamic Symbol Table (`symbol_table.h`, `symbol_table.c`)<br>• Client Web Compiler Engine (`compiler.js`)<br>• CLI Driver & Sample Test Cases (`main.c`, `sample.md`) |

---

## 🚀 Local Build & Execution Guide

### 1. Interactive Web Studio (Browser)
Open `index.html` directly in any modern browser or visit the live deployment at [https://alijahan365.github.io/md-to-html/](https://alijahan365.github.io/md-to-html/).

---

### 2. Standalone C Compiler (CLI Execution)

#### Build using GCC:
```powershell
gcc -Wall -Wextra -std=c99 -o md2html.exe utils.c lexer.c parser.c symbol_table.c semantic.c html_generator.c main.c
```

#### Compile Markdown file to HTML:
```powershell
.\md2html.exe sample.md output.html
```

---

### 3. Flex & Bison Execution (CSE314 Lab Syllabus)

Build and test using standard lab tools:
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
├── .github/workflows/ci.yml # GitHub Actions Build & Test Workflow
├── lexer.l                  # Flex Scanner Specification (CSE314 Lab)
├── parser.y                 # Bison Parser Specification (CSE314 Lab)
├── main.c                   # Compiler Driver Entry Point
├── lexer.h / .c             # Handwritten Lexical Analyzer (Line & Column tracking)
├── parser.h / .c            # Handwritten Recursive Descent Parser
├── symbol_table.h / .c      # Dynamic Symbol Table Data Structures & Lookup
├── semantic.h / .c          # Semantic Validation & Diagnostic Reporting
├── html_generator.h / .c    # HTML Code Generator (AST Post-Order Traversal)
├── utils.h / .c             # AST Node Definitions & Location Allocation
├── index.html               # 5-View Interactive Web Studio SPA
├── style.css                # Vercel-Style Dark Theme Design System
├── compiler.js              # Client-Side Web Compiler Engine & Router
├── sample.md                # Sample Input Markdown Document
├── Makefile                 # Build Automation Script
├── LICENSE                  # MIT Academic License
└── README.md                # Project Documentation
```

---

## 📜 License
This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details. Created for academic evaluation in **CSE314: Compiler Design Lab**.
