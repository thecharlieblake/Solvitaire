@echo off

if "%~1"=="" (
    docker run -it -v %cd%:/home/Solvitaire --rm thecharlesblake/solvitaire:1.0
) else (
    docker run -it -v %cd%:/home/Solvitaire --rm thecharlesblake/solvitaire:1.0 bash -c "%*"
)
