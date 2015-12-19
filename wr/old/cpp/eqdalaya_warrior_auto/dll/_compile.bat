g++ --version
g++ dll.cpp -Ic:\mingw\include -Id:\code\vk_keys\include -Id:\code\memory\include -Id:\code\everquest\eqdalaya\include -Lc:\mingw\lib -lkernel32 -luser32 -lgdi32 -lpsapi -Wl,-subsystem,windows -w -O0 -s -shared -o release/eqdalaya_warrior_auto.dll
copy release\eqdalaya_warrior_auto.dll c:\eqdalaya\
copy release\eqdalaya_warrior_auto.ini c:\eqdalaya\
pause