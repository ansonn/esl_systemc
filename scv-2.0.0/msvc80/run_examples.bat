@echo off
echo ------------------------------------------------------------ > examples.log
for /F %%X in ('dir ..\examples\*.exe /s /b') do (
  echo Running %%X
  echo example: %%X >> examples.log
  echo. >> examples.log
  %%X >> examples.log 2>&1
  echo ------------------------------------------------------------ >> examples.log
)
