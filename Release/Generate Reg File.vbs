Set WshShell = WScript.CreateObject("WScript.Shell")
Set WshFSO = WScript.CreateObject("Scripting.FileSystemObject")
Set RegFile = WshFSO.CreateTextFile("VSProjectPack.reg", True, True)
StrCD = WshShell.CurrentDirectory

RegFile.WriteLine("REGEDIT4")
RegFile.WriteLine("[HKEY_CLASSES_ROOT\Directory\Background\shell\VSProjectPack]")
RegFile.WriteLine("@=""VSProjectPack""")
RegFile.WriteLine("[HKEY_CLASSES_ROOT\Directory\Background\shell\VSProjectPack\Command]")
StrCD = Replace(StrCD, "\", "\\")
RegFile.WriteLine("@=""" + StrCD + "\\VSProjectPack.exe %V""")


RegFile.WriteLine("[HKEY_CLASSES_ROOT\Directory\Background\shell\VSProjectPack Clean]")
RegFile.WriteLine("@=""VSProjectPack Clean""")
RegFile.WriteLine("[HKEY_CLASSES_ROOT\Directory\Background\shell\VSProjectPack Clean\Command]")
RegFile.WriteLine("@=""" + StrCD + "\\VSProjectPack.exe %V CLEAN""")

RegFile.Close