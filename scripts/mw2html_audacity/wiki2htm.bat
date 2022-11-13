python2 mw2html.py https://manual.audacityteam.org/man/ ..\..\help\temp -s
rmdir /S /Q ..\..\help\manual
mkdir ..\..\help\manual
xcopy ..\..\help\temp\manual.audacityteam.org ..\..\help\manual\ /E /C /Y /Q
rmdir /S /Q ..\..\help\temp

