@echo off
rem Sets up (or reuses) a venv in emulator\.venv with the packages from
rem requirements.txt, then runs the Tkinter-based dashboard (tkinter is
rem part of the Python standard library - if a Windows Python install is
rem missing it, reinstall from python.org with the "tcl/tk" component
rem checked). See interactive.bat for the plain REPL alternative - same
rem venv, same requirements.
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
