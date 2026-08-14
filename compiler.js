// Markdown compiler engine (Lexer, Parser, Symbol Table, CodeGen)


// 1. TOKEN DEFINITIONS
const TOKEN_TYPES = {
    HEADER1: 'HEADER1',
    HEADER2: 'HEADER2',
    HEADER3: 'HEADER3',
    BOLD: 'BOLD',
    ITALIC: 'ITALIC',
    UNORDERED_LIST: 'UNORDERED_LIST',
    ORDERED_LIST: 'ORDERED_LIST',
    LINK_START: 'LINK_START',
    LINK_MID: 'LINK_MID',
    LINK_END: 'LINK_END',
    TEXT: 'TEXT',
    NEWLINE: 'NEWLINE',
    BLANK_LINE: 'BLANK_LINE',
    EOF: 'EOF'
};

// 2. LEXICAL ANALYZER (SCANNER)
class Lexer {
    constructor(source) {
        this.src = source;
        this.pos = 0;
        this.line = 1;
        this.col = 1;
        this.isLineStart = true;
    }

    peek() {
        return this.pos < this.src.length ? this.src[this.pos] : '\0';
    }

    advance() {
        const c = this.peek();
        if (c === '\n') {
            this.line++;
            this.col = 1;
        } else {
            this.col++;
        }
        this.pos++;
        return c;
    }

    isAtEnd() {
        return this.pos >= this.src.length;
    }

    defaultLexeme(type) {
        switch (type) {
            case TOKEN_TYPES.HEADER1: return '#';
            case TOKEN_TYPES.HEADER2: return '##';
            case TOKEN_TYPES.HEADER3: return '###';
            case TOKEN_TYPES.BOLD: return '**';
            case TOKEN_TYPES.ITALIC: return '*';
            case TOKEN_TYPES.UNORDERED_LIST: return '-';
            case TOKEN_TYPES.ORDERED_LIST: return '1.';
            case TOKEN_TYPES.LINK_START: return '[';
            case TOKEN_TYPES.LINK_MID: return '](';
            case TOKEN_TYPES.LINK_END: return ')';
            case TOKEN_TYPES.NEWLINE: return '\\n';
            case TOKEN_TYPES.BLANK_LINE: return '\\n\\n';
            case TOKEN_TYPES.EOF: return '<EOF>';
            default: return '';
        }
    }

    tokenize() {
        const tokens = [];
        while (true) {
            const token = this.nextToken();
            tokens.push(token);
            if (token.type === TOKEN_TYPES.EOF) break;
        }
        return tokens;
    }

    nextToken() {
        const startLine = this.line;
        const startCol = this.col;

        if (this.isAtEnd()) {
            return { type: TOKEN_TYPES.EOF, value: '<EOF>', line: startLine, col: startCol };
        }

        const c = this.peek();

        if (c === '\r') {
            this.advance();
            return this.nextToken();
        }

        if (c === '\n') {
            this.advance();
            if (this.peek() === '\r') this.advance();
            if (this.peek() === '\n') {
                this.advance();
                this.isLineStart = true;
                return { type: TOKEN_TYPES.BLANK_LINE, value: '\\n\\n', line: startLine, col: startCol };
            }
            this.isLineStart = true;
            return { type: TOKEN_TYPES.NEWLINE, value: '\\n', line: startLine, col: startCol };
        }

        if (this.isLineStart) {
            // Headers
            if (c === '#') {
                let hashCount = 0;
                let tempPos = this.pos;
                while (tempPos < this.src.length && this.src[tempPos] === '#') {
                    hashCount++;
                    tempPos++;
                }
                if (tempPos < this.src.length && this.src[tempPos] === ' ') {
                    const totalAdvance = hashCount + 1;
                    for (let i = 0; i < totalAdvance; i++) this.advance();
                    this.isLineStart = false;
                    const type = hashCount === 1 ? TOKEN_TYPES.HEADER1 : (hashCount === 2 ? TOKEN_TYPES.HEADER2 : TOKEN_TYPES.HEADER3);
                    return { type, value: this.defaultLexeme(type), line: startLine, col: startCol };
                }
            }

            // Unordered list
            if ((c === '-' || c === '*') && this.src[this.pos + 1] === ' ') {
                this.advance(); this.advance();
                this.isLineStart = false;
                return { type: TOKEN_TYPES.UNORDERED_LIST, value: '-', line: startLine, col: startCol };
            }

            // Ordered list
            if (/\d/.test(c)) {
                let temp = this.pos;
                while (temp < this.src.length && /\d/.test(this.src[temp])) temp++;
                if (temp < this.src.length && this.src[temp] === '.' && this.src[temp + 1] === ' ') {
                    const count = (temp - this.pos) + 2;
                    for (let i = 0; i < count; i++) this.advance();
                    this.isLineStart = false;
                    return { type: TOKEN_TYPES.ORDERED_LIST, value: '1.', line: startLine, col: startCol };
                }
            }
        }

        this.isLineStart = false;

        // Bold & Italic
        if (c === '*') {
            if (this.src[this.pos + 1] === '*') {
                this.advance(); this.advance();
                return { type: TOKEN_TYPES.BOLD, value: '**', line: startLine, col: startCol };
            }
            this.advance();
            return { type: TOKEN_TYPES.ITALIC, value: '*', line: startLine, col: startCol };
        }

        // Links
        if (c === '[') { this.advance(); return { type: TOKEN_TYPES.LINK_START, value: '[', line: startLine, col: startCol }; }
        if (c === ']' && this.src[this.pos + 1] === '(') { this.advance(); this.advance(); return { type: TOKEN_TYPES.LINK_MID, value: '](', line: startLine, col: startCol }; }
        if (c === ')') { this.advance(); return { type: TOKEN_TYPES.LINK_END, value: ')', line: startLine, col: startCol }; }

        // Text accumulation
        const startPos = this.pos;
        while (!this.isAtEnd()) {
            const next = this.peek();
            if (next === '\n' || next === '\r' || next === '*' || next === '[' || (next === ']' && this.src[this.pos + 1] === '(') || next === ')') {
                break;
            }
            this.advance();
        }

        const len = this.pos - startPos;
        if (len === 0) {
            this.advance();
            return { type: TOKEN_TYPES.TEXT, value: ' ', line: startLine, col: startCol };
        }

        const textVal = this.src.substring(startPos, this.pos);
        return { type: TOKEN_TYPES.TEXT, value: textVal, line: startLine, col: startCol };
    }
}

// 3. RECURSIVE DESCENT PARSER
const NODE_TYPES = {
    DOCUMENT: 'DOCUMENT',
    HEADER: 'HEADER',
    PARAGRAPH: 'PARAGRAPH',
    BOLD: 'BOLD',
    ITALIC: 'ITALIC',
    TEXT: 'TEXT',
    UNORDERED_LIST: 'UNORDERED_LIST',
    ORDERED_LIST: 'ORDERED_LIST',
    LIST_ITEM: 'LIST_ITEM',
    LINK: 'LINK'
};

class Parser {
    constructor(tokens) {
        this.tokens = tokens;
        this.index = 0;
        this.syntaxErrors = [];
    }

    curToken() {
        if (this.index < this.tokens.length) {
            return this.tokens[this.index];
        }
        return { type: TOKEN_TYPES.EOF, value: '<EOF>', line: 1, col: 1 };
    }

    advance() {
        if (this.index < this.tokens.length) {
            this.index++;
        }
    }

    reportError(expected, found) {
        this.syntaxErrors.push({
            type: 'Syntax Error',
            message: `Invalid format - Expected '${expected}', Found '${found.type}' ("${found.value || ''}")`,
            line: found.line,
            col: found.col
        });
    }

    parse() {
        const doc = { type: NODE_TYPES.DOCUMENT, children: [], line: 1, col: 1 };
        while (this.curToken().type !== TOKEN_TYPES.EOF) {
            if (this.curToken().type === TOKEN_TYPES.BLANK_LINE || this.curToken().type === TOKEN_TYPES.NEWLINE) {
                this.advance();
                continue;
            }
            const startIndex = this.index;
            const block = this.parseBlock();
            if (block) doc.children.push(block);

            if (this.index === startIndex) {
                this.reportError('Valid Markdown Block', this.curToken());
                this.advance();
            }
        }
        return doc;
    }

    parseBlock() {
        const tok = this.curToken();

        if (tok.type === TOKEN_TYPES.HEADER1 || tok.type === TOKEN_TYPES.HEADER2 || tok.type === TOKEN_TYPES.HEADER3) {
            const level = tok.type === TOKEN_TYPES.HEADER1 ? 1 : (tok.type === TOKEN_TYPES.HEADER2 ? 2 : 3);
            const node = { type: NODE_TYPES.HEADER, level, children: [], line: tok.line, col: tok.col };
            this.advance();
            while (this.curToken().type !== TOKEN_TYPES.NEWLINE && this.curToken().type !== TOKEN_TYPES.BLANK_LINE && this.curToken().type !== TOKEN_TYPES.EOF) {
                node.children.push(this.parseInline());
            }
            if (this.curToken().type === TOKEN_TYPES.NEWLINE || this.curToken().type === TOKEN_TYPES.BLANK_LINE) this.advance();
            return node;
        }

        if (tok.type === TOKEN_TYPES.UNORDERED_LIST || tok.type === TOKEN_TYPES.ORDERED_LIST) {
            const listType = tok.type === TOKEN_TYPES.UNORDERED_LIST ? NODE_TYPES.UNORDERED_LIST : NODE_TYPES.ORDERED_LIST;
            const listNode = { type: listType, children: [], line: tok.line, col: tok.col };

            while (this.curToken().type === TOKEN_TYPES.UNORDERED_LIST || this.curToken().type === TOKEN_TYPES.ORDERED_LIST) {
                const itemNode = { type: NODE_TYPES.LIST_ITEM, children: [], line: this.curToken().line, col: this.curToken().col };
                this.advance();
                while (this.curToken().type !== TOKEN_TYPES.NEWLINE && this.curToken().type !== TOKEN_TYPES.BLANK_LINE && this.curToken().type !== TOKEN_TYPES.EOF) {
                    itemNode.children.push(this.parseInline());
                }
                if (this.curToken().type === TOKEN_TYPES.NEWLINE || this.curToken().type === TOKEN_TYPES.BLANK_LINE) this.advance();
                listNode.children.push(itemNode);
            }
            return listNode;
        }

        const node = { type: NODE_TYPES.PARAGRAPH, children: [], line: tok.line, col: tok.col };
        while (this.curToken().type !== TOKEN_TYPES.NEWLINE && this.curToken().type !== TOKEN_TYPES.BLANK_LINE && this.curToken().type !== TOKEN_TYPES.EOF) {
            node.children.push(this.parseInline());
        }
        if (this.curToken().type === TOKEN_TYPES.NEWLINE || this.curToken().type === TOKEN_TYPES.BLANK_LINE) this.advance();
        return node;
    }

    parseInline() {
        const tok = this.curToken();

        if (tok.type === TOKEN_TYPES.TEXT) {
            const node = { type: NODE_TYPES.TEXT, text: tok.value, line: tok.line, col: tok.col };
            this.advance();
            return node;
        }

        if (tok.type === TOKEN_TYPES.BOLD) {
            const node = { type: NODE_TYPES.BOLD, children: [], line: tok.line, col: tok.col };
            this.advance();
            while (this.curToken().type !== TOKEN_TYPES.BOLD && this.curToken().type !== TOKEN_TYPES.EOF && this.curToken().type !== TOKEN_TYPES.NEWLINE && this.curToken().type !== TOKEN_TYPES.BLANK_LINE) {
                node.children.push(this.parseInline());
            }
            if (this.curToken().type === TOKEN_TYPES.BOLD) {
                this.advance();
            } else {
                this.reportError('** (Closing BOLD delimiter)', this.curToken());
            }
            return node;
        }

        if (tok.type === TOKEN_TYPES.ITALIC) {
            const node = { type: NODE_TYPES.ITALIC, children: [], line: tok.line, col: tok.col };
            this.advance();
            while (this.curToken().type !== TOKEN_TYPES.ITALIC && this.curToken().type !== TOKEN_TYPES.EOF && this.curToken().type !== TOKEN_TYPES.NEWLINE && this.curToken().type !== TOKEN_TYPES.BLANK_LINE) {
                node.children.push(this.parseInline());
            }
            if (this.curToken().type === TOKEN_TYPES.ITALIC) {
                this.advance();
            } else {
                this.reportError('* (Closing ITALIC delimiter)', this.curToken());
            }
            return node;
        }

        if (tok.type === TOKEN_TYPES.LINK_START) {
            const node = { type: NODE_TYPES.LINK, children: [], url: null, line: tok.line, col: tok.col };
            this.advance();
            while (this.curToken().type !== TOKEN_TYPES.LINK_MID && this.curToken().type !== TOKEN_TYPES.EOF && this.curToken().type !== TOKEN_TYPES.NEWLINE && this.curToken().type !== TOKEN_TYPES.BLANK_LINE) {
                node.children.push(this.parseInline());
            }
            if (this.curToken().type === TOKEN_TYPES.LINK_MID) {
                this.advance();
                if (this.curToken().type === TOKEN_TYPES.TEXT) {
                    node.url = this.curToken().value;
                    this.advance();
                }
                if (this.curToken().type === TOKEN_TYPES.LINK_END) {
                    this.advance();
                } else {
                    this.reportError(') (Closing Link URL delimiter)', this.curToken());
                }
            } else {
                this.reportError(']( (Link Mid delimiter)', this.curToken());
            }
            return node;
        }

        if (tok.type === TOKEN_TYPES.NEWLINE) {
            const node = { type: NODE_TYPES.TEXT, text: ' ', line: tok.line, col: tok.col };
            this.advance();
            return node;
        }

        const node = { type: NODE_TYPES.TEXT, text: tok.value || '', line: tok.line, col: tok.col };
        this.advance();
        return node;
    }
}

// 4. SYMBOL TABLE & SEMANTIC ANALYZER
class SymbolTableAnalyzer {
    constructor() {
        this.symbols = [];
        this.semanticErrors = [];
    }

    extractText(node) {
        if (!node) return '';
        let text = node.text || '';
        if (node.children) {
            for (const child of node.children) {
                text += this.extractText(child);
            }
        }
        return text;
    }

    sanitizeSlug(text) {
        return text.toLowerCase().replace(/[^a-z0-9\s-]/g, '').trim().replace(/\s+/g, '-');
    }

    populateAndAnalyze(ast) {
        this.symbols = [];
        this.semanticErrors = [];
        this.traverseAST(ast);
        this.checkDuplicateHeadings();
    }

    traverseAST(node) {
        if (!node) return;

        if (node.type === NODE_TYPES.HEADER) {
            const text = this.extractText(node).trim();
            if (text.length === 0) {
                this.semanticErrors.push({
                    type: 'Semantic Warning',
                    message: 'Empty header text.',
                    line: node.line,
                    col: node.col
                });
            } else {
                const slug = this.sanitizeSlug(text);
                this.symbols.push({
                    category: 'HEADING',
                    identifier: text,
                    target: slug,
                    line: node.line,
                    col: node.col
                });
            }
        } else if (node.type === NODE_TYPES.BOLD && (!node.children || node.children.length === 0)) {
            this.semanticErrors.push({
                type: 'Semantic Warning',
                message: 'Empty bold block (**).',
                line: node.line,
                col: node.col
            });
        } else if (node.type === NODE_TYPES.ITALIC && (!node.children || node.children.length === 0)) {
            this.semanticErrors.push({
                type: 'Semantic Warning',
                message: 'Empty italic block (*).',
                line: node.line,
                col: node.col
            });
        } else if (node.type === NODE_TYPES.LINK) {
            const text = this.extractText(node).trim();
            if (text.length === 0) {
                this.semanticErrors.push({
                    type: 'Semantic Warning',
                    message: 'Link is missing display text.',
                    line: node.line,
                    col: node.col
                });
            }
            if (!node.url || node.url.trim().length === 0) {
                this.semanticErrors.push({
                    type: 'Semantic Error',
                    message: 'Link is missing target URL.',
                    line: node.line,
                    col: node.col
                });
            }
            this.symbols.push({
                category: 'LINK',
                identifier: text || '[No Display Text]',
                target: node.url || 'N/A',
                line: node.line,
                col: node.col
            });
        }

        if (node.children) {
            for (const child of node.children) {
                this.traverseAST(child);
            }
        }
    }

    checkDuplicateHeadings() {
        const headings = this.symbols.filter(s => s.category === 'HEADING');
        for (let i = 0; i < headings.length; i++) {
            for (let j = i + 1; j < headings.length; j++) {
                if (headings[i].target === headings[j].target) {
                    this.semanticErrors.push({
                        type: 'Semantic Warning',
                        message: `Duplicate heading anchor ID '#${headings[j].target}' generated (First declared at Line ${headings[i].line}, Col ${headings[i].col}).`,
                        line: headings[j].line,
                        col: headings[j].col
                    });
                }
            }
        }
    }
}

// 5. TARGET CODE GENERATOR
class CodeGenerator {
    generate(ast) {
        let body = '';
        if (ast && ast.children) {
            for (const child of ast.children) {
                body += this.generateNode(child);
            }
        }
        return body;
    }

    generateNode(node) {
        if (!node) return '';

        switch (node.type) {
            case NODE_TYPES.HEADER:
                const tag = `h${node.level || 1}`;
                return `<${tag}>${this.generateChildren(node)}</${tag}>\n`;
            case NODE_TYPES.PARAGRAPH:
                return `<p>${this.generateChildren(node)}</p>\n`;
            case NODE_TYPES.BOLD:
                return `<strong>${this.generateChildren(node)}</strong>`;
            case NODE_TYPES.ITALIC:
                return `<em>${this.generateChildren(node)}</em>`;
            case NODE_TYPES.TEXT:
                return this.escapeHTML(node.text || '');
            case NODE_TYPES.UNORDERED_LIST:
                return `<ul>\n${this.generateChildren(node)}</ul>\n`;
            case NODE_TYPES.ORDERED_LIST:
                return `<ol>\n${this.generateChildren(node)}</ol>\n`;
            case NODE_TYPES.LIST_ITEM:
                return `  <li>${this.generateChildren(node)}</li>\n`;
            case NODE_TYPES.LINK:
                let url = node.url;
                if (!url) {
                    const rawText = this.generateChildren(node);
                    const match = rawText.match(/https?:\/\/[^\s\)\>]+/);
                    if (match) url = match[0];
                }
                const href = url ? this.escapeHTML(url) : '#';
                return `<a href="${href}" target="_blank" rel="noopener noreferrer">${this.generateChildren(node)}</a>`;
            default:
                return this.generateChildren(node);
        }
    }

    generateChildren(node) {
        let res = '';
        if (node.children) {
            for (const child of node.children) {
                res += this.generateNode(child);
            }
        }
        return res;
    }

    escapeHTML(str) {
        if (!str) return '';
        return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
    }
}

// 6. UI CONTROLLER & PRESET DEMOS
const PRESETS = {
    valid: `# Compiler Design Project
## Markdown to HTML Compiler

### Key Features
This compiler parses **Markdown syntax** into clean HTML.

- Lexical Scanner
- Recursive Parser
- Symbol Table

1. Input Tokenization
2. AST Construction
3. Semantic Validation

For project details, visit [Google Search](https://google.com).`,

    syntax_err: `# Syntax Error Demonstration

This paragraph has **unclosed bold text delimiter.

Here is an unclosed link: [Google Search](https://google.com)`,

    semantic_err: `# 

# Duplicate Title

# Duplicate Title

This line has an empty link with no URL: [Broken Link]()`,

    complex: `# Complex Compiler Project
## Advanced Markdown Parsing Demonstration

This document demonstrates **bold**, *italic*, and **nested *bold-italic*** formatting in our pure parser.

### Core Modules:
- **Lexer**: Scans tokens character by character.
- **Parser**: Builds hierarchical Abstract Syntax Tree.
- **Symbol Table**: Tracks anchor IDs and links.

1. High Execution Speed
2. Accurate Error Locations (Line & Column)
3. Zero External Dependencies

Learn more at [Google Search](https://google.com).`
};

document.addEventListener('DOMContentLoaded', () => {
    const markdownInput = document.getElementById('markdown-input');
    const lineNumbers = document.getElementById('line-numbers');
    const presetSelect = document.getElementById('preset-select');
    const btnCompile = document.getElementById('btn-compile');
    const btnClear = document.getElementById('btn-clear');
    const btnCopyHTML = document.getElementById('btn-copy-html');
    const btnDownloadHTML = document.getElementById('btn-download-html');
    const btnToggleTheme = document.getElementById('btn-toggle-theme');

    const browserMockup = document.querySelector('.browser-mockup');
    const statusBadge = document.getElementById('status-badge');
    const statusText = document.getElementById('status-text');

    const statLines = document.getElementById('stat-lines');
    const statChars = document.getElementById('stat-chars');

    const badgeTokens = document.getElementById('badge-tokens');
    const badgeSymbols = document.getElementById('badge-symbols');
    const badgeErrors = document.getElementById('badge-errors');

    const htmlPreviewBody = document.getElementById('html-preview-body');
    const htmlCodeOutput = document.getElementById('html-code-output');
    const tokensTbody = document.getElementById('tokens-tbody');
    const symbolsTbody = document.getElementById('symbols-tbody');
    const astContainer = document.getElementById('ast-container');
    const errorsContainer = document.getElementById('errors-container');

    // Mobile Pane Switcher
    const btnShowEditor = document.getElementById('btn-show-editor');
    const btnShowVisualizer = document.getElementById('btn-show-visualizer');
    const panelEditor = document.getElementById('panel-editor');
    const panelVisualizer = document.getElementById('panel-visualizer');

    if (btnShowEditor && btnShowVisualizer && panelEditor && panelVisualizer) {
        btnShowEditor.addEventListener('click', () => {
            btnShowEditor.classList.add('active');
            btnShowVisualizer.classList.remove('active');
            panelEditor.classList.add('active-mobile');
            panelVisualizer.classList.remove('active-mobile');
        });

        btnShowVisualizer.addEventListener('click', () => {
            btnShowVisualizer.classList.add('active');
            btnShowEditor.classList.remove('active');
            panelVisualizer.classList.add('active-mobile');
            panelEditor.classList.remove('active-mobile');
        });
    }

    // Theme toggle
    let isDarkTheme = false;
    btnToggleTheme.addEventListener('click', () => {
        isDarkTheme = !isDarkTheme;
        if (isDarkTheme) {
            browserMockup.classList.add('dark-theme');
            btnToggleTheme.innerHTML = '<i class="fa-solid fa-sun"></i>';
        } else {
            browserMockup.classList.remove('dark-theme');
            btnToggleTheme.innerHTML = '<i class="fa-solid fa-moon"></i>';
        }
    });

    // Load initial preset
    markdownInput.value = PRESETS.valid;
    updateEditor();
    compileSource();

    // Event Listeners
    markdownInput.addEventListener('input', () => {
        updateEditor();
        compileSource();
    });

    markdownInput.addEventListener('scroll', () => {
        lineNumbers.scrollTop = markdownInput.scrollTop;
    });

    presetSelect.addEventListener('change', (e) => {
        const key = e.target.value;
        if (PRESETS[key]) {
            markdownInput.value = PRESETS[key];
            updateEditor();
            compileSource();
        }
    });

    btnCompile.addEventListener('click', compileSource);

    btnClear.addEventListener('click', () => {
        markdownInput.value = '';
        updateEditor();
        compileSource();
    });

    btnCopyHTML.addEventListener('click', () => {
        const code = htmlCodeOutput.textContent;
        navigator.clipboard.writeText(code).then(() => {
            btnCopyHTML.innerHTML = '<i class="fa-solid fa-check"></i> Copied!';
            setTimeout(() => {
                btnCopyHTML.innerHTML = '<i class="fa-solid fa-copy"></i> Copy Code';
            }, 2000);
        });
    });

    btnDownloadHTML.addEventListener('click', () => {
        const code = htmlCodeOutput.textContent;
        const blob = new Blob([code], { type: 'text/html' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'compiled_output.html';
        a.click();
        URL.revokeObjectURL(url);
    });

    // Tab Navigation
    document.querySelectorAll('.nav-tab').forEach(btn => {
        btn.addEventListener('click', () => {
            document.querySelectorAll('.nav-tab').forEach(b => b.classList.remove('active'));
            document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));

            btn.classList.add('active');
            const tabId = btn.getAttribute('data-tab');
            document.getElementById(tabId).classList.add('active');
        });
    });

    function updateEditor() {
        const text = markdownInput.value;
        const lines = text === '' ? 1 : text.split('\n').length;
        
        let lineNumsHTML = '';
        for (let i = 1; i <= lines; i++) {
            lineNumsHTML += `${i}<br>`;
        }
        lineNumbers.innerHTML = lineNumsHTML;

        statLines.textContent = `${lines} lines`;
        statChars.textContent = `${text.length} chars`;
    }

    function compileSource() {
        const source = markdownInput.value;

        // 1. Lexical Scanner
        const lexer = new Lexer(source);
        const tokens = lexer.tokenize();

        // 2. Recursive Descent Parser
        const parser = new Parser(tokens);
        const ast = parser.parse();

        // 3. Symbol Table & Semantic Analysis
        const symAnalyzer = new SymbolTableAnalyzer();
        symAnalyzer.populateAndAnalyze(ast);

        // 4. Target Code Generator
        const codeGen = new CodeGenerator();
        const generatedBody = codeGen.generate(ast);

        const fullHTMLDoc = `<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Compiler Output</title>
<style>
body { font-family: sans-serif; padding: 20px; line-height: 1.6; }
h1 { border-bottom: 2px solid #eee; padding-bottom: 8px; }
h2 { border-bottom: 1px solid #eee; padding-bottom: 6px; }
a { color: #2563eb; }
</style>
</head>
<body>
${generatedBody}</body>
</html>`;

        // Render Views
        renderTokens(tokens);
        renderAST(ast);
        renderSymbols(symAnalyzer.symbols);
        
        const allErrors = [...parser.syntaxErrors, ...symAnalyzer.semanticErrors];
        renderErrors(allErrors);
        renderPreview(generatedBody);
        htmlCodeOutput.textContent = fullHTMLDoc;

        // Update Stats Badges
        badgeTokens.textContent = tokens.length;
        badgeSymbols.textContent = symAnalyzer.symbols.length;
        badgeErrors.textContent = allErrors.length;

        if (allErrors.length > 0) {
            const hasSyntaxErr = parser.syntaxErrors.length > 0;
            statusBadge.className = hasSyntaxErr ? 'status-indicator status-error' : 'status-indicator status-warning';
            statusText.textContent = hasSyntaxErr ? `${allErrors.length} Syntax Error(s)` : `${allErrors.length} Semantic Warning(s)`;
        } else {
            statusBadge.className = 'status-indicator';
            statusText.textContent = 'Compilation Passed';
        }
    }

    function renderTokens(tokens) {
        if (tokens.length === 0) {
            tokensTbody.innerHTML = '<tr><td colspan="4" class="empty-cell">No tokens scanned.</td></tr>';
            return;
        }

        let html = '';
        tokens.forEach((t, idx) => {
            html += `<tr>
                <td>${idx + 1}</td>
                <td><span class="loc-chip">L${t.line}:C${t.col}</span></td>
                <td><span class="token-chip token-${t.type}">${t.type}</span></td>
                <td><code>${escapeHTML(t.value)}</code></td>
            </tr>`;
        });
        tokensTbody.innerHTML = html;
    }

    function renderSymbols(symbols) {
        if (symbols.length === 0) {
            symbolsTbody.innerHTML = '<tr><td colspan="5" class="empty-cell">Symbol table is empty.</td></tr>';
            return;
        }

        let html = '';
        symbols.forEach((s, idx) => {
            html += `<tr>
                <td>${idx + 1}</td>
                <td><span class="token-chip token-${s.category === 'HEADING' ? 'HEADER1' : 'LINK_START'}">${s.category}</span></td>
                <td>${escapeHTML(s.identifier)}</td>
                <td><code>${escapeHTML(s.target)}</code></td>
                <td><span class="loc-chip">L${s.line}:C${s.col}</span></td>
            </tr>`;
        });
        symbolsTbody.innerHTML = html;
    }

    function renderAST(ast) {
        astContainer.innerHTML = formatASTNode(ast);
    }

    function formatASTNode(node) {
        if (!node) return '';

        let res = `<div class="tree-node">`;
        res += `<span class="tree-node-title">├── [${node.type}]</span>`;

        if (node.level) res += ` <span class="tree-node-url">(Level ${node.level})</span>`;
        if (node.text) res += ` : <span class="tree-node-text">"${escapeHTML(node.text)}"</span>`;
        if (node.url) res += ` <span class="tree-node-url">[URL: ${escapeHTML(node.url)}]</span>`;

        if (node.children && node.children.length > 0) {
            for (const child of node.children) {
                res += formatASTNode(child);
            }
        }

        res += `</div>`;
        return res;
    }

    function renderErrors(errors) {
        if (errors.length === 0) {
            errorsContainer.innerHTML = `
            <div class="clean-status-card">
                <i class="fa-solid fa-circle-check"></i>
                <div>
                    <h4>Compilation Passed cleanly</h4>
                    <p>0 Syntax Errors, 0 Semantic Warnings detected across all phases.</p>
                </div>
            </div>`;
            return;
        }

        let html = '';
        errors.forEach(e => {
            const isWarning = e.type.includes('Warning');
            const cardClass = isWarning ? 'diag-card warning-card' : 'diag-card';
            html += `<div class="${cardClass}">
                <div class="diag-title-bar">
                    <span>${e.type}</span>
                    <span class="loc-chip">Line ${e.line}, Column ${e.col}</span>
                </div>
                <div class="diag-text">${escapeHTML(e.message)}</div>
            </div>`;
        });
        errorsContainer.innerHTML = html;
    }

    function renderPreview(htmlBody) {
        if (!htmlBody || htmlBody.trim() === '') {
            htmlPreviewBody.innerHTML = '<p class="empty-state-text">No output generated.</p>';
            return;
        }
        htmlPreviewBody.innerHTML = htmlBody;
    }

    function escapeHTML(str) {
        if (!str) return '';
        return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
    }
});
