echo Preparing IME folder for WinPE. 
setlocal 

set WinPEWinDir=C:\Users\michael\Downloads\peboot\windows
set IMEBinSrc=C:\Users\michael\Downloads\pesrc\windows

md %WinPEWinDir%\IME md %WinPEWinDir%\IME\IMESC5 
md %WinPEWinDir%\IME\IMESC5\DICTS 
md %WinPEWinDir%\IME\IMESC5\HELP 
md %WinPEWinDir%\System32\IME 
md %WinPEWinDir%\System32\IME\IMESC5 
md %WinPEWinDir%\System32\IME\shared 

rem SYSTEM32 folder (Cicero) 
%windir%\system32\xcopy %IMEBinSrc%\System32\CTFMON.EXE %WinPEWinDir%\System32\CTFMON.EXE /y /d 
%windir%\system32\xcopy %IMEBinSrc%\System32\MSUTB.DLL %WinPEWinDir%\System32\MSUTB.DLL /y /d 
%windir%\system32\xcopy %IMEBinSrc%\System32\MsCtfMonitor.DLL %WinPEWinDir%\System32\MsCtfMonitor.DLL /y /d 
%windir%\system32\xcopy %IMEBinSrc%\System32\PINTLGNT.IME %WinPEWinDir%\System32 /y /d 
%windir%\system32\xcopy %IMEBinSrc%\IME\IMESC5 %WinPEWinDir%\IME\IMESC5 /s /y /d 

rem IMESC5 folder 
%windir%\system32\xcopy %IMEBinSrc%\System32\IME\IMESC5 %WinPEWinDir%\System32\IME\IMESC5 /s /y /d 

rem shared folder 
%windir%\system32\xcopy %IMEBinSrc%\System32\IME\shared %WinPEWinDir%\System32\IME\shared /s /y /d 

rem Creating some batch files... 
@echo @echo off    > %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REGSVR32 /S %%WinDir%%\System32\MSUTB.DLL   >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REGSVR32 /S %%WinDir%%\System32\MsCtfMonitor.DLL    >> %WinPEWinDir%\System32\IME\IMEReg.CMD 

@for %%i in (IMEAPIS.DLL imecfm.dll IMEPADSM.DLL IMETIP.DLL imever.dll IMJKAPI.DLL MSCAND20.DLL) do @echo REGSVR32 /S %%WinDir%%\System32\IME\shared\%%i >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@for %%i in (ImSCCfg.DLL ImSCCore.dll IMSCTIP.dll imscui.DLL PMIGRATE.dll) do @echo REGSVR32 /S %%WinDir%%\System32\IME\IMESC5\%%i >> %WinPEWinDir%\System32\IME\IMEReg.CMD 

@rem @echo REG ADD "HKLM\Software\Microsoft\CTF" /v "StartOnNoTaskEng" /t REG_DWORD /d 1 ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 

@echo REG ADD HKCU\Software\Microsoft\CTF\TIP\{81D4E9C9-1D3B-41BC-9E6C-4B40BF79E35E} ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REG ADD HKCU\Software\Microsoft\CTF\TIP\{81D4E9C9-1D3B-41BC-9E6C-4B40BF79E35E}\LanguageProfile ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REG ADD HKCU\Software\Microsoft\CTF\TIP\{81D4E9C9-1D3B-41BC-9E6C-4B40BF79E35E}\LanguageProfile\0x00000804 ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REG ADD HKCU\Software\Microsoft\CTF\TIP\{81D4E9C9-1D3B-41BC-9E6C-4B40BF79E35E}\LanguageProfile\0x00000804\{F3BA9077-6C7E-11D4-97FA-0080C882687E}   ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REG ADD HKCU\Software\Microsoft\CTF\TIP\{81D4E9C9-1D3B-41BC-9E6C-4B40BF79E35E}\LanguageProfile\0x00000804\{F3BA9077-6C7E-11D4-97FA-0080C882687E} /v "Enable" /t REG_DWORD /d 1 ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder     ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder\AssemblyItem    ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder\AssemblyItem\0x00000804     ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder\AssemblyItem\0x00000804\{34745C63-B2F0-4784-8B67-5E12C8701A31} ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder\AssemblyItem\0x00000804\{34745C63-B2F0-4784-8B67-5E12C8701A31}\00000000 ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder\AssemblyItem\0x00000804\{34745C63-B2F0-4784-8B67-5E12C8701A31}\00000000 /v "CLSID" /d "{81D4E9C9-1D3B-41BC-9E6C-4B40BF79E35E}" ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder\AssemblyItem\0x00000804\{34745C63-B2F0-4784-8B67-5E12C8701A31}\00000000 /v "Profile" /d "{F3BA9077-6C7E-11D4-97FA-0080C882687E}" ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder\AssemblyItem\0x00000804\{34745C63-B2F0-4784-8B67-5E12C8701A31}\00000000 /v "KeyboardLayout" /t REG_DWORD /d 0 ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\SortOrder\Language /v "00000000" /d "00000804"     ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@rem @echo REG ADD HKCU\Software\Microsoft\CTF\HiddenDummyLayouts /v "00000804" /d "00000804" ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REG ADD HKCU\Software\Microsoft\CTF\Assemblies\0x00000804\{34745C63-B2F0-4784-8B67-5E12C8701A31} /v "Default" /d "{00000000-0000-0000-0000-000000000000}" /f ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REG ADD HKCU\Software\Microsoft\CTF\Assemblies\0x00000804\{34745C63-B2F0-4784-8B67-5E12C8701A31} /v "Profile" /d "{00000000-0000-0000-0000-000000000000}" /f ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 
@echo REG ADD HKCU\Software\Microsoft\CTF\Assemblies\0x00000804\{34745C63-B2F0-4784-8B67-5E12C8701A31} /v "KeyboardLayout" /t REG_DWORD /d "00000804" /f ^> nul >> %WinPEWinDir%\System32\IME\IMEReg.CMD 

@echo start CTFMON.EXE >> %WinPEWinDir%\System32\IME\IMEReg.CMD 

@if not exist %WinPEWinDir%\System32\startnet.org copy %WinPEWinDir%\System32\startnet.cmd %WinPEWinDir%\System32\startnet.org 

@echo @echo off   > %WinPEWinDir%\System32\STARTNET.CMD 
@echo echo Preparing WinPE ...    >> %WinPEWinDir%\System32\STARTNET.CMD 
@type %WinPEWinDir%\System32\STARTNET.org >> %WinPEWinDir%\System32\STARTNET.CMD 
@echo @echo.       >> %WinPEWinDir%\System32\STARTNET.CMD 
@echo Call %%WinDir%%\System32\IME\IMEReg.CMD    >> %WinPEWinDir%\System32\STARTNET.CMD 

endlocal