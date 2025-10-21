@echo off
python -m http.server --bind 127.0.0.1 --directory data 1337
pause