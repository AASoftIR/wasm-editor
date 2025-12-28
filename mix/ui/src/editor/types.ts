/**
 * Type definitions for mix editor
 */

export type EditorMode = "normal" | "insert" | "visual" | "command";

export interface CursorPosition {
	line: number;
	col: number;
}

export interface Selection {
	start: CursorPosition;
	end: CursorPosition;
}

export interface EditorOptions {
	theme?: "vs-dark" | "vim-zen";
	tabSize?: number;
	insertSpaces?: boolean;
}

export interface KeyBinding {
	key: string;
	mode: EditorMode | "all";
	action: string;
	ctrl?: boolean;
	shift?: boolean;
	alt?: boolean;
}
