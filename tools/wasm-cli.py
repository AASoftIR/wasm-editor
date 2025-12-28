#!/usr/bin/env python3
"""
🚀 WASM Learning Hub CLI Tool
==============================

A friendly CLI tool to help you set up and navigate the WASM learning repository.

Usage:
    python wasm-cli.py setup          # Set up the complete environment
    python wasm-cli.py check          # Check what's installed
    python wasm-cli.py lesson <num>   # Open a specific lesson
    python wasm-cli.py build          # Build the Rust WASM project
    python wasm-cli.py serve          # Start a local dev server
    python wasm-cli.py new <name>     # Create a new WASM project from template
"""

import os
import sys
import subprocess
import shutil
import json
import webbrowser
from pathlib import Path
from typing import Optional, Tuple
import http.server
import socketserver
import threading

# ═══════════════════════════════════════════════════════════════════════════
# CONFIGURATION
# ═══════════════════════════════════════════════════════════════════════════

SCRIPT_DIR = Path(__file__).parent.resolve()
ROOT_DIR = SCRIPT_DIR.parent
LEARN_DIR = ROOT_DIR / "learn"
MIX_DIR = ROOT_DIR / "mix"

COLORS = {
    "reset": "\033[0m",
    "bold": "\033[1m",
    "red": "\033[91m",
    "green": "\033[92m",
    "yellow": "\033[93m",
    "blue": "\033[94m",
    "purple": "\033[95m",
    "cyan": "\033[96m",
}

LESSONS = {
    1: "01-what-is-wasm",
    2: "02-first-wasm",
    3: "03-js-bridge",
    4: "04-c-wasm",
    5: "05-memory-performance",
}

# ═══════════════════════════════════════════════════════════════════════════
# UTILITIES
# ═══════════════════════════════════════════════════════════════════════════

def color(text: str, c: str) -> str:
    """Wrap text in ANSI color codes."""
    if sys.platform == "win32":
        os.system("")  # Enable ANSI on Windows
    return f"{COLORS.get(c, '')}{text}{COLORS['reset']}"


def print_banner():
    """Print the CLI banner."""
    banner = """
██╗    ██╗ █████╗ ███████╗███╗   ███╗     ██████╗██╗     ██╗
██║    ██║██╔══██╗██╔════╝████╗ ████║    ██╔════╝██║     ██║
██║ █╗ ██║███████║███████╗██╔████╔██║    ██║     ██║     ██║
██║███╗██║██╔══██║╚════██║██║╚██╔╝██║    ██║     ██║     ██║
╚███╔███╔╝██║  ██║███████║██║ ╚═╝ ██║    ╚██████╗███████╗██║
 ╚══╝╚══╝ ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝     ╚═════╝╚══════╝╚═╝
    """
    print(color(banner, "cyan"))
    print(color("  WebAssembly Learning Hub CLI Tool\n", "bold"))


def check_command(cmd: str) -> Tuple[bool, str]:
    """Check if a command is available and get its version."""
    try:
        result = subprocess.run(
            [cmd, "--version"],
            capture_output=True,
            text=True,
            timeout=10
        )
        version = result.stdout.strip().split("\n")[0]
        return True, version
    except (subprocess.TimeoutExpired, FileNotFoundError):
        return False, ""


def run_command(cmd: list, cwd: Optional[Path] = None, show_output: bool = True) -> bool:
    """Run a command and return success status."""
    try:
        if show_output:
            result = subprocess.run(cmd, cwd=cwd)
        else:
            result = subprocess.run(cmd, cwd=cwd, capture_output=True)
        return result.returncode == 0
    except FileNotFoundError:
        return False


# ═══════════════════════════════════════════════════════════════════════════
# COMMANDS
# ═══════════════════════════════════════════════════════════════════════════

def cmd_check():
    """Check the development environment."""
    print(color("\n🔍 Checking Development Environment\n", "bold"))
    print("=" * 50)
    
    checks = [
        ("Python", "python", "Required for this CLI"),
        ("Emscripten (emcc)", "emcc", "C/C++ to WASM compiler"),
        ("Node.js", "node", "For npm packages and dev server"),
        ("wabt (wat2wasm)", "wat2wasm", "WAT to WASM compiler (optional)"),
    ]
    
    all_good = True
    for name, cmd, description in checks:
        found, version = check_command(cmd)
        if found:
            print(f"  {color('✓', 'green')} {name}: {color(version, 'cyan')}")
        else:
            if cmd in ["emcc", "python"]:
                print(f"  {color('✗', 'red')} {name}: {color('Not found', 'yellow')} - {description}")
                all_good = False
            else:
                print(f"  {color('○', 'yellow')} {name}: {color('Not found (optional)', 'yellow')} - {description}")
    
    print("=" * 50)
    
    if all_good:
        print(color("\n✅ All required tools installed! You're ready to go.\n", "green"))
    else:
        print(color("\n⚠️  Some required tools are missing. Run 'python wasm-cli.py setup' to install.\n", "yellow"))
    
    return all_good


def cmd_setup():
    """Set up the development environment."""
    print(color("\n🚀 Setting Up WASM Development Environment\n", "bold"))
    
    steps = []
    
    # Check Emscripten
    emcc_found, _ = check_command("emcc")
    if not emcc_found:
        import sys
        if sys.platform == "win32":
            steps.append(("Install Emscripten", [
                "git clone https://github.com/emscripten-core/emsdk.git",
                "cd emsdk",
                "emsdk install latest",
                "emsdk activate latest",
                "emsdk_env.bat  (run in each new terminal)"
            ]))
        elif sys.platform == "darwin":
            steps.append(("Install Emscripten", [
                "brew install emscripten",
                "Or: git clone https://github.com/emscripten-core/emsdk.git && cd emsdk && ./emsdk install latest && ./emsdk activate latest"
            ]))
        else:
            steps.append(("Install Emscripten", [
                "git clone https://github.com/emscripten-core/emsdk.git",
                "cd emsdk && ./emsdk install latest && ./emsdk activate latest",
                "source ./emsdk_env.sh"
            ]))
    else:
        print(f"  {color('✓', 'green')} Emscripten already installed")
    
    # Check Node.js
    node_found, _ = check_command("node")
    if not node_found:
        steps.append(("Install Node.js", ["Visit https://nodejs.org/ and download LTS version"]))
    else:
        print(f"  {color('✓', 'green')} Node.js already installed")
    
    # Print remaining steps
    if steps:
        print(color("\n📝 Manual Steps Required:\n", "yellow"))
        for i, (name, instructions) in enumerate(steps, 1):
            print(f"  {i}. {color(name, 'bold')}")
            for instruction in instructions:
                print(f"     {instruction}")
            print()
    else:
        print(color("\n✅ Setup complete! Start learning with:", "green"))
        print(color("   python wasm-cli.py lesson 1\n", "cyan"))


def cmd_lesson(lesson_num: int):
    """Open a specific lesson."""
    if lesson_num not in LESSONS:
        print(color(f"\n❌ Invalid lesson number: {lesson_num}", "red"))
        print(f"   Available lessons: 1-{len(LESSONS)}\n")
        return
    
    lesson_dir = LEARN_DIR / LESSONS[lesson_num]
    demo_file = lesson_dir / "demo.html"
    readme_file = lesson_dir / "README.md"
    
    print(color(f"\n📖 Opening Lesson {lesson_num}: {LESSONS[lesson_num]}\n", "bold"))
    
    if readme_file.exists():
        print(f"  README: {readme_file}")
    
    if demo_file.exists():
        print(f"  Demo:   {demo_file}")
        print(color("\n  Opening demo in browser...", "cyan"))
        webbrowser.open(f"file://{demo_file}")
    else:
        print(color("  No demo.html found for this lesson", "yellow"))


def cmd_build():
    """Build the C WASM project."""
    c_project = LEARN_DIR / "04-c-wasm"
    
    if not (c_project / "src" / "main.c").exists():
        print(color("\n❌ C project not found at expected location", "red"))
        return
    
    print(color("\n🔨 Building C → WASM...\n", "bold"))
    
    # Check emcc
    emcc_found, _ = check_command("emcc")
    if not emcc_found:
        print(color("❌ Emscripten (emcc) not found. Run 'python wasm-cli.py setup' for installation instructions.", "red"))
        return
    
    # Build
    import sys
    build_script = "build.bat" if sys.platform == "win32" else "./build.sh"
    if run_command([build_script], cwd=c_project):
        print(color("\n✅ Build successful!", "green"))
        print(f"   Output: {c_project / 'www'}")
        print(color("\n   Start server with: python wasm-cli.py serve", "cyan"))
    else:
        print(color("\n❌ Build failed. Check the errors above.", "red"))


def cmd_serve(port: int = 8080):
    """Start a local development server."""
    print(color(f"\n🌐 Starting development server on port {port}...\n", "bold"))
    
    os.chdir(ROOT_DIR)
    
    handler = http.server.SimpleHTTPRequestHandler
    
    with socketserver.TCPServer(("", port), handler) as httpd:
        url = f"http://localhost:{port}"
        print(f"  Server running at: {color(url, 'cyan')}")
        print(f"  Press Ctrl+C to stop\n")
        
        # Open browser
        webbrowser.open(url)
        
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print(color("\n\n👋 Server stopped", "yellow"))


def cmd_new(name: str):
    """Create a new WASM project from template."""
    project_dir = ROOT_DIR / "projects" / name
    
    if project_dir.exists():
        print(color(f"\n❌ Project '{name}' already exists!", "red"))
        return
    
    print(color(f"\n📦 Creating new C/WASM project: {name}\n", "bold"))
    
    # Create directory structure
    (project_dir / "src").mkdir(parents=True)
    (project_dir / "www").mkdir(parents=True)
    
    # Create main.c
    main_c = f'''// {name} - WASM module compiled from C
#include <emscripten.h>
#include <string.h>
#include <stdlib.h>

EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {{
    return a + b;
}}

EMSCRIPTEN_KEEPALIVE
int multiply(int a, int b) {{
    return a * b;
}}

EMSCRIPTEN_KEEPALIVE
int factorial(int n) {{
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}}

EMSCRIPTEN_KEEPALIVE
char* greet(const char* name) {{
    static char buffer[256];
    snprintf(buffer, sizeof(buffer), "Hello, %s!", name);
    return buffer;
}}

int main() {{
    return 0;
}}
'''
    (project_dir / "src" / "main.c").write_text(main_c)
    
    # Create index.html
    index_html = f'''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{name}</title>
    <style>
        body {{
            font-family: system-ui, sans-serif;
            max-width: 800px;
            margin: 2rem auto;
            padding: 1rem;
            background: #1e1e1e;
            color: #d4d4d4;
        }}
        h1 {{ color: #00d4ff; }}
        .result {{
            background: #0a0a15;
            padding: 1rem;
            border-radius: 8px;
            font-family: monospace;
            margin: 1rem 0;
        }}
        button {{
            background: linear-gradient(90deg, #00d4ff, #7c3aed);
            border: none;
            padding: 0.8rem 1.5rem;
            border-radius: 8px;
            color: white;
            cursor: pointer;
            margin: 0.5rem;
        }}
    </style>
</head>
<body>
    <h1>🚀 {name}</h1>
    <p>Your C/WASM module is ready!</p>
    
    <div class="result" id="output">Loading WASM...</div>
    
    <button onclick="testAdd()">Test Add(5, 3)</button>
    <button onclick="testFactorial()">Test Factorial(6)</button>
    <button onclick="testGreet()">Test Greet</button>
    
    <script>
        var Module = {{
            onRuntimeInitialized: function() {{
                document.getElementById('output').textContent = '✅ WASM loaded!';
            }}
        }};
        
        function testAdd() {{
            const result = Module._add(5, 3);
            document.getElementById('output').textContent = `add(5, 3) = ${{result}}`;
        }}
        
        function testFactorial() {{
            const result = Module._factorial(6);
            document.getElementById('output').textContent = `factorial(6) = ${{result}}`;
        }}
        
        function testGreet() {{
            const greet = Module.cwrap('greet', 'string', ['string']);
            document.getElementById('output').textContent = greet('WASM Developer');
        }}
    </script>
    <script src="main.js"></script>
</body>
</html>
'''
    (project_dir / "www" / "index.html").write_text(index_html)
    
    # Create build script
    import sys
    if sys.platform == "win32":
        build_script = f'''@echo off
echo Building {name}...
emcc src/main.c -o www/main.js ^
    -s WASM=1 ^
    -s EXPORTED_FUNCTIONS="['_add', '_multiply', '_factorial', '_greet', '_main']" ^
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" ^
    -O2
echo Done! Open www/index.html in your browser.
'''
        (project_dir / "build.bat").write_text(build_script)
    else:
        build_script = f'''#!/bin/bash
echo "Building {name}..."
emcc src/main.c -o www/main.js \\
    -s WASM=1 \\
    -s EXPORTED_FUNCTIONS="['_add', '_multiply', '_factorial', '_greet', '_main']" \\
    -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \\
    -O2
echo "Done! Open www/index.html in your browser."
'''
        build_path = project_dir / "build.sh"
        build_path.write_text(build_script)
        build_path.chmod(0o755)
    
    print(f"  {color('✓', 'green')} Created {project_dir}")
    print(f"\\n  Next steps:")
    print(f"    cd {project_dir}")
    if os.name == 'nt':
        print(f"    build.bat")
    else:
        print(f"    ./build.sh")
    print(f"    # Then open www/index.html")


def cmd_help():
    """Show help information."""
    print_banner()
    print(color("Commands:\n", "bold"))
    commands = [
        ("setup", "Set up the WASM development environment"),
        ("check", "Check what tools are installed"),
        ("lesson <num>", "Open a specific lesson (1-5)"),
        ("build", "Build the C WASM project with Emscripten"),
        ("serve [port]", "Start a local dev server (default: 8080)"),
        ("new <name>", "Create a new WASM project from template"),
        ("help", "Show this help message"),
    ]
    
    for cmd, desc in commands:
        print(f"  {color(cmd, 'cyan'):20} {desc}")
    
    print(color("\nExamples:\n", "bold"))
    print("  python wasm-cli.py setup")
    print("  python wasm-cli.py lesson 1")
    print("  python wasm-cli.py serve 3000")
    print("  python wasm-cli.py new my-wasm-app")
    print()


# ═══════════════════════════════════════════════════════════════════════════
# MAIN
# ═══════════════════════════════════════════════════════════════════════════

def main():
    if len(sys.argv) < 2:
        cmd_help()
        return
    
    command = sys.argv[1].lower()
    
    if command == "setup":
        cmd_setup()
    elif command == "check":
        cmd_check()
    elif command == "lesson":
        if len(sys.argv) < 3:
            print(color("Usage: python wasm-cli.py lesson <number>", "yellow"))
            return
        try:
            lesson_num = int(sys.argv[2])
            cmd_lesson(lesson_num)
        except ValueError:
            print(color("Lesson number must be an integer", "red"))
    elif command == "build":
        cmd_build()
    elif command == "serve":
        port = int(sys.argv[2]) if len(sys.argv) > 2 else 8080
        cmd_serve(port)
    elif command == "new":
        if len(sys.argv) < 3:
            print(color("Usage: python wasm-cli.py new <project-name>", "yellow"))
            return
        cmd_new(sys.argv[2])
    elif command in ["help", "-h", "--help"]:
        cmd_help()
    else:
        print(color(f"Unknown command: {command}", "red"))
        cmd_help()


if __name__ == "__main__":
    main()
