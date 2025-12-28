/**
 * Editor - Main editor class
 *
 * Coordinates the UI layer, manages modes, and communicates with WASM core.
 */

import { EditorMode } from "./types";

export class Editor {
	private container: HTMLElement;
	private editorElement: HTMLElement;
	private lineNumbers: HTMLElement;
	private contentArea: HTMLElement;
	private statusBar: HTMLElement;
	private cursorElement: HTMLElement;

	private mode: EditorMode = "normal";
	private content: string[] = [""];
	private cursorLine: number = 0;
	private cursorCol: number = 0;

	// WASM core (loaded dynamically)
	private wasmCore: any = null;

	constructor(container: HTMLElement) {
		this.container = container;
		this.editorElement = document.createElement("div");
		this.lineNumbers = document.createElement("div");
		this.contentArea = document.createElement("div");
		this.statusBar = document.createElement("div");
		this.cursorElement = document.createElement("div");
	}

	async init(): Promise<void> {
		this.setupDOM();
		this.setupEventListeners();
		await this.loadWasmCore();
		this.render();
	}

	private setupDOM(): void {
		this.editorElement.className = "mix-editor";
		this.editorElement.setAttribute("tabindex", "0");

		// Line numbers
		this.lineNumbers.className = "line-numbers";

		// Content area
		this.contentArea.className = "content-area";

		// Cursor
		this.cursorElement.className = "cursor";
		this.contentArea.appendChild(this.cursorElement);

		// Status bar
		this.statusBar.className = "status-bar";

		// Assemble
		this.editorElement.appendChild(this.lineNumbers);
		this.editorElement.appendChild(this.contentArea);
		this.container.appendChild(this.editorElement);
		this.container.appendChild(this.statusBar);

		// Focus the editor
		this.editorElement.focus();
	}

	private setupEventListeners(): void {
		// Keyboard input
		this.editorElement.addEventListener("keydown", (e) =>
			this.handleKeyDown(e)
		);

		// Focus management
		this.editorElement.addEventListener("focus", () => {
			this.cursorElement.classList.add("focused");
		});
		this.editorElement.addEventListener("blur", () => {
			this.cursorElement.classList.remove("focused");
		});

		// Click to focus
		this.container.addEventListener("click", () => {
			this.editorElement.focus();
		});
	}

	private async loadWasmCore(): Promise<void> {
		try {
			// Try to load WASM core from pkg directory
			const wasmModule = await import("../../core/pkg/mix_core.js");
			await wasmModule.default();
			this.wasmCore = new wasmModule.Editor();
			console.log("WASM core loaded");
		} catch (e) {
			console.log("WASM core not available, using JS fallback");
			this.wasmCore = null;
		}
	}

	private handleKeyDown(e: KeyboardEvent): void {
		const key = e.key;

		// Mode switching
		if (key === "Escape") {
			this.setMode("normal");
			e.preventDefault();
			return;
		}

		if (e.ctrlKey && key === "e") {
			this.setMode(this.mode === "insert" ? "normal" : "insert");
			e.preventDefault();
			return;
		}

		if (this.mode === "normal") {
			this.handleNormalMode(key, e);
		} else if (this.mode === "insert") {
			this.handleInsertMode(key, e);
		}

		this.render();
	}

	private handleNormalMode(key: string, e: KeyboardEvent): void {
		switch (key) {
			// Mode switch
			case "i":
				this.setMode("insert");
				e.preventDefault();
				break;
			case "a":
				this.cursorCol = Math.min(this.cursorCol + 1, this.currentLine.length);
				this.setMode("insert");
				e.preventDefault();
				break;
			case "A":
				this.cursorCol = this.currentLine.length;
				this.setMode("insert");
				e.preventDefault();
				break;
			case "I":
				this.cursorCol = 0;
				this.setMode("insert");
				e.preventDefault();
				break;
			case "o":
				this.content.splice(this.cursorLine + 1, 0, "");
				this.cursorLine++;
				this.cursorCol = 0;
				this.setMode("insert");
				e.preventDefault();
				break;
			case "O":
				this.content.splice(this.cursorLine, 0, "");
				this.cursorCol = 0;
				this.setMode("insert");
				e.preventDefault();
				break;

			// Movement
			case "h":
			case "ArrowLeft":
				this.cursorCol = Math.max(0, this.cursorCol - 1);
				e.preventDefault();
				break;
			case "l":
			case "ArrowRight":
				this.cursorCol = Math.min(
					this.currentLine.length - 1,
					this.cursorCol + 1
				);
				e.preventDefault();
				break;
			case "j":
			case "ArrowDown":
				this.cursorLine = Math.min(
					this.content.length - 1,
					this.cursorLine + 1
				);
				this.clampCursor();
				e.preventDefault();
				break;
			case "k":
			case "ArrowUp":
				this.cursorLine = Math.max(0, this.cursorLine - 1);
				this.clampCursor();
				e.preventDefault();
				break;

			// Line movement
			case "0":
				this.cursorCol = 0;
				e.preventDefault();
				break;
			case "$":
				this.cursorCol = Math.max(0, this.currentLine.length - 1);
				e.preventDefault();
				break;
			case "g":
				// Would need to track 'gg' sequence
				this.cursorLine = 0;
				this.clampCursor();
				e.preventDefault();
				break;
			case "G":
				this.cursorLine = this.content.length - 1;
				this.clampCursor();
				e.preventDefault();
				break;

			// Word movement
			case "w":
				this.moveWordForward();
				e.preventDefault();
				break;
			case "b":
				this.moveWordBackward();
				e.preventDefault();
				break;
			case "e":
				this.moveWordEnd();
				e.preventDefault();
				break;

			// Delete
			case "x":
				if (this.currentLine.length > 0) {
					const line = this.currentLine;
					this.content[this.cursorLine] =
						line.slice(0, this.cursorCol) + line.slice(this.cursorCol + 1);
					this.clampCursor();
				}
				e.preventDefault();
				break;
		}
	}

	private handleInsertMode(key: string, e: KeyboardEvent): void {
		if (key.length === 1 && !e.ctrlKey && !e.metaKey) {
			// Regular character input
			this.insertChar(key);
			e.preventDefault();
		} else if (key === "Backspace") {
			this.deleteCharBefore();
			e.preventDefault();
		} else if (key === "Enter") {
			this.insertNewline();
			e.preventDefault();
		} else if (key === "Tab") {
			this.insertChar("    "); // 4 spaces
			e.preventDefault();
		} else if (key === "ArrowLeft") {
			this.cursorCol = Math.max(0, this.cursorCol - 1);
			e.preventDefault();
		} else if (key === "ArrowRight") {
			this.cursorCol = Math.min(this.currentLine.length, this.cursorCol + 1);
			e.preventDefault();
		} else if (key === "ArrowUp") {
			this.cursorLine = Math.max(0, this.cursorLine - 1);
			this.clampCursor();
			e.preventDefault();
		} else if (key === "ArrowDown") {
			this.cursorLine = Math.min(this.content.length - 1, this.cursorLine + 1);
			this.clampCursor();
			e.preventDefault();
		}
	}

	private insertChar(ch: string): void {
		const line = this.currentLine;
		this.content[this.cursorLine] =
			line.slice(0, this.cursorCol) + ch + line.slice(this.cursorCol);
		this.cursorCol += ch.length;
	}

	private insertNewline(): void {
		const line = this.currentLine;
		const before = line.slice(0, this.cursorCol);
		const after = line.slice(this.cursorCol);
		this.content[this.cursorLine] = before;
		this.content.splice(this.cursorLine + 1, 0, after);
		this.cursorLine++;
		this.cursorCol = 0;
	}

	private deleteCharBefore(): void {
		if (this.cursorCol > 0) {
			const line = this.currentLine;
			this.content[this.cursorLine] =
				line.slice(0, this.cursorCol - 1) + line.slice(this.cursorCol);
			this.cursorCol--;
		} else if (this.cursorLine > 0) {
			const prevLine = this.content[this.cursorLine - 1];
			const currentLine = this.currentLine;
			this.content[this.cursorLine - 1] = prevLine + currentLine;
			this.content.splice(this.cursorLine, 1);
			this.cursorLine--;
			this.cursorCol = prevLine.length;
		}
	}

	private moveWordForward(): void {
		const line = this.currentLine;
		let col = this.cursorCol;

		// Skip current word
		while (col < line.length && /\w/.test(line[col])) col++;
		// Skip whitespace
		while (col < line.length && /\s/.test(line[col])) col++;

		if (col >= line.length && this.cursorLine < this.content.length - 1) {
			this.cursorLine++;
			this.cursorCol = 0;
		} else {
			this.cursorCol = col;
		}
	}

	private moveWordBackward(): void {
		const line = this.currentLine;
		let col = this.cursorCol;

		if (col === 0 && this.cursorLine > 0) {
			this.cursorLine--;
			this.cursorCol = this.currentLine.length;
			return;
		}

		col--;
		// Skip whitespace
		while (col > 0 && /\s/.test(line[col])) col--;
		// Skip to start of word
		while (col > 0 && /\w/.test(line[col - 1])) col--;

		this.cursorCol = Math.max(0, col);
	}

	private moveWordEnd(): void {
		const line = this.currentLine;
		let col = this.cursorCol + 1;

		// Skip whitespace
		while (col < line.length && /\s/.test(line[col])) col++;
		// Move to end of word
		while (col < line.length - 1 && /\w/.test(line[col + 1])) col++;

		this.cursorCol = Math.min(line.length - 1, col);
	}

	private get currentLine(): string {
		return this.content[this.cursorLine] || "";
	}

	private clampCursor(): void {
		const maxCol =
			this.mode === "insert"
				? this.currentLine.length
				: Math.max(0, this.currentLine.length - 1);
		this.cursorCol = Math.min(this.cursorCol, maxCol);
		this.cursorCol = Math.max(0, this.cursorCol);
	}

	private setMode(mode: EditorMode): void {
		this.mode = mode;
		this.editorElement.setAttribute("data-mode", mode);
		if (mode === "normal") {
			this.clampCursor();
		}
	}

	private render(): void {
		this.renderLineNumbers();
		this.renderContent();
		this.renderCursor();
		this.renderStatusBar();
	}

	private renderLineNumbers(): void {
		this.lineNumbers.innerHTML = this.content
			.map((_, i) => `<div class="line-number">${i + 1}</div>`)
			.join("");
	}

	private renderContent(): void {
		const lines = this.content
			.map((line, i) => {
				const escapedLine = this.escapeHtml(line) || " ";
				return `<div class="line" data-line="${i}">${escapedLine}</div>`;
			})
			.join("");

		// Preserve cursor element
		const cursor = this.cursorElement;
		this.contentArea.innerHTML = lines;
		this.contentArea.appendChild(cursor);
	}

	private renderCursor(): void {
		const lineHeight = 22; // Match CSS
		const charWidth = 9.6; // Approximate for monospace

		this.cursorElement.style.top = `${this.cursorLine * lineHeight}px`;
		this.cursorElement.style.left = `${this.cursorCol * charWidth}px`;

		// Different cursor style for different modes
		if (this.mode === "insert") {
			this.cursorElement.classList.add("insert-mode");
			this.cursorElement.classList.remove("normal-mode");
		} else {
			this.cursorElement.classList.add("normal-mode");
			this.cursorElement.classList.remove("insert-mode");
		}
	}

	private renderStatusBar(): void {
		const modeDisplay = this.mode.toUpperCase();
		const position = `${this.cursorLine + 1}:${this.cursorCol + 1}`;

		this.statusBar.innerHTML = `
            <span class="mode mode-${this.mode}">${modeDisplay}</span>
            <span class="spacer"></span>
            <span class="position">${position}</span>
        `;
	}

	private escapeHtml(text: string): string {
		return text
			.replace(/&/g, "&amp;")
			.replace(/</g, "&lt;")
			.replace(/>/g, "&gt;")
			.replace(/ /g, "&nbsp;");
	}

	// Public API
	public loadContent(content: string): void {
		this.content = content.split("\n");
		if (this.content.length === 0) {
			this.content = [""];
		}
		this.cursorLine = 0;
		this.cursorCol = 0;
		this.render();
	}

	public getContent(): string {
		return this.content.join("\n");
	}
}
