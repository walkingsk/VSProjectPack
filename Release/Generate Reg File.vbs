Set WshShell = WScript.CreateObject("WScript.Shell")
Set WshFSO = WScript.CreateObject("Scripting.FileSystemObject")
Set RegFile = WshFSO.CreateTextFile("VSProjectPack.reg", True, True)
StrCD = WshShell.CurrentDirectory

RegFile.WriteLine("REGEDIT4")
RegFile.WriteLine("[HKEY_CLASSES_ROOT\Directory\Background\shell\VSProjectPack]")
RegFile.WriteLine("@=""VSProjectPack""")
RegFile.WriteLine("[HKEY_CLASSES_ROOT\Directory\Background\shell\VSProjectPack\Command]")

StrCD = Replace(StrCD, "\", "\\")
' WScript.Echo (StrCD)
RegFile.WriteLine("@=""" + StrCD + "\\VSProjectPack.exe %V""")
RegFile.Close