@echo off
magick "%~1" -define ppm:format=raw "%~dpn1.ppm"
