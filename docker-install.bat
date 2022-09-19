@echo off
docker login

@REM Downloads the image
docker pull thecharlesblake/solvitaire:1.0

@REM If the cmake-build-[debug/release] folders don't exist, copies pre-generated versions from the docker image into the current directory (so new users don't have to wait for a long build to complete!)
if exist cmake-build-release\ goto end
if exist cmake-build-debug\ goto end

docker run -i -v %cd%:/home/Solvitaire --rm thecharlesblake/solvitaire:1.0 bash -c "cp -r ../.install/cmake-build-* ."

:end