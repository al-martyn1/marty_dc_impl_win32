@call clr_distr.bat
@copy /Y /B .\.out\msvc2019\x64\Release\TestDrawContext04_GdiOnly.exe .\.out\msvc2019\x64\Release\TestDrawContext04.exe
@if exist  ..\..\belka_chivy_nut\.out\msvc2019\x64\Release\Belka.Chivy.demo.exe copy /Y /B ..\..\belka_chivy_nut\.out\msvc2019\x64\Release\Belka.Chivy.demo.exe ..\..\belka_chivy_nut\.out\msvc2019\x64\Release\hotpad.exe
@call make_distr_files_impl.bat . TestDrawContext04
@if exist ..\..\belka_chivy_nut call make_distr_files_impl.bat ..\..\belka_chivy_nut Belka.Chivy.demo
@if exist ..\..\belka_chivy_nut call make_distr_files_impl.bat ..\..\belka_chivy_nut hotpad
@call zip_distr.bat squirrel_demos
