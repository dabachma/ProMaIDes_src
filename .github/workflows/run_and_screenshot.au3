#include <ScreenCapture.au3>

Local $dir = "build\Release\"
Local $search = FileFindFirstFile($dir & "Pro*.exe")

Local $file = FileFindNextFile($search)

Run($dir & $file)
Sleep(10000) ;

;WinClose(WinGetHandle("[ACTIVE]"))
Sleep(500) ;

;WinSetState(WinGetHandle("[ACTIVE]"), "", @SW_MAXIMIZE)
Sleep(500) ;

_ScreenCapture_Capture("screenshot2.png")
Sleep(500) ;

;WinClose(WinGetHandle("[ACTIVE]"))
;WinClose(WinGetHandle("[ACTIVE]"))

exit