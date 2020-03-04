@echo off

echo CWD is %1

REM Run from root directory!
if not exist "%1build\shaders\" mkdir "%1build\shaders"

echo "Compiling shaders..."
echo "%1shaders/main.vert.glsl -> %1build/shaders/main.vert.spv"
glslc.exe -fshader-stage=vert %1shaders/main.vert.glsl -o %1build/shaders/main.vert.spv
echo "%1shaders/main.frag.glsl -> %1build/shaders/main.frag.spv"
glslc.exe -fshader-stage=frag %1shaders/main.frag.glsl -o %1build/shaders/main.frag.spv

echo "Done."