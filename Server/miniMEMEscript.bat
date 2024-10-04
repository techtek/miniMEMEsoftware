@echo off
setlocal enabledelayedexpansion

:loop
REM Define the folder path (change this to your target folder)
set "folder=c:\Users\User\Downloads"

REM Change to the target folder
cd /d "%folder%"

REM Count the number of .jpg files
set "fileCount=0"
for %%F in (*.jpg) do set /a fileCount+=1

REM If there are exactly 5 files, rename them to image1.jpg to image5.jpg
if %fileCount% equ 5 (
    echo Exactly 5 files present. Renaming files...
    goto renameFiles
)



REM If there are more than 5 files, delete the oldest
if %fileCount% gtr 5 (
    echo Deleting the oldest files...
    for /f "skip=5 delims=" %%F in ('dir /b /o-d *.jpg') do (
        del "%%F"
    )
)

:renameFiles


REM Now rename the remaining files
set "fileNum=1"
for /f "delims=" %%F in ('dir /b /o-d *.jpg') do (
    if !fileNum! leq 5 (
        ren "%%F" "image!fileNum!.jpg"
        set /a fileNum+=1
    )
)


REM Stage and commit the files
git add .
git commit -m "Auto-sync: Updated images"

REM Push the changes to GitHub
git push origin main
git push --force

echo GitHub sync completed.




:wait
echo Operation completed. Waiting for 15 seconds... Press SPACE to skip wait.
timeout /t 15

REM Go back to the start
goto loop
