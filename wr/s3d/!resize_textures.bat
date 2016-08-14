for /R %%f in (*.dds) do magick %%f -resize 64^> %%f
pause