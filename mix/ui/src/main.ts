/**
 * mix Editor - Main Entry Point
 *
 * A minimal, dual-mode code editor powered by WebAssembly
 */

import { Editor } from "./editor/Editor";
import "./styles/main.css";

// Initialize the editor when DOM is ready
document.addEventListener("DOMContentLoaded", async () => {
	const container = document.getElementById("editor-container");
	if (!container) {
		console.error("Editor container not found");
		return;
	}

	try {
		// Create and initialize the editor
		const editor = new Editor(container);
		await editor.init();

		// Make editor available for debugging
		(window as any).mixEditor = editor;

		console.log("✅ mix editor initialized");
	} catch (error) {
		console.error("Failed to initialize editor:", error);
	}
});
