@echo off

echo CWD is %1

REM Run from root directory!
if not exist "%1build\shaders\" mkdir "%1build\shaders"

echo "Compiling shaders..."
echo "%1shaders/Builtin.UnlitShader.vert.glsl -> %1build/shaders/Builtin.UnlitShader.vert.spv"
glslc.exe -fshader-stage=vert %1shaders/Builtin.UnlitShader.vert.glsl -o %1build/shaders/Builtin.UnlitShader.vert.spv
echo "%1shaders/Builtin.UnlitShader.frag.glsl -> %1build/shaders/Builtin.UnlitShader.frag.spv"
glslc.exe -fshader-stage=frag %1shaders/Builtin.UnlitShader.frag.glsl -o %1build/shaders/Builtin.UnlitShader.frag.spv

echo "Copying assets..."
echo xcopy "%1assets" "%1build\assets" /h /i /c /k /e /r /y
xcopy "%1assets" "%1build\assets" /h /i /c /k /e /r /y

echo "Done."