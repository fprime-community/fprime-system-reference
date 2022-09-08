@echo off
Rem docker-run: run the docker image
set DIRNAME=%~dp0

Rem checks that "docker" exists
WHERE docker >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo [ERROR] 'docker' command not found. Please ensure 'docker' or 'Rancher Desktop' installed.
    exit /B 1
)

Rem checks that the image has been pulled, or pull it
set docker_image=(docker images -q nasafprime/fprime-raspi:devel 2>nul )
IF "%docker_image%" == "" (
    echo [INFO] Pulling Docker Image: nasafprime/fprime-raspi:devel
    call "%DIRNAME%/docker-setup"
)

Rem run the image forwarding arguments
echo [INFO] Running Docker Image: nasafprime/fprime-raspi:devel with args '%*'
docker run --net host -itv "%DIRNAME%..":/project nasafprime/fprime-raspi:devel %*
