@echo off
rem Sets up (or reuses) a venv in emulator\.venv with the packages from
rem requirements.txt, then runs the Textual-based TUI. See
rem interactive.bat for the plain REPL alternative - same venv, same
rem requirements.
setlocal
cd /d "%~dp0"

if not exist ".venv\Scripts\python.exe" (
    echo Creating virtual environment in %~dp0.venv ...
    python -m venv .venv
    if errorlevel 1 (
        echo Failed to create the virtual environment - is Python installed and on PATH?
        exit /b 1
    )
)

".venv\Scripts\python.exe" -m pip install -q --disable-pip-version-check -r requirements.txt
if errorlevel 1 (
    echo pip install failed - see the output above.
    exit /b 1
)

".venv\Scripts\python.exe" tui.py %*
