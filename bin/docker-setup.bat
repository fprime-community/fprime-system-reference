@echo off
WHERE docker >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo "[ERROR] 'docker' command not found. Please ensure 'docker' or 'Rancher Desktop' installed."
    exit /B 1
)
docker pull nasafprime/fprime-raspi:devel
