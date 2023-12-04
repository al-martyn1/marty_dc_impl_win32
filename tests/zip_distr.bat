@set ZIPNAME=%1
@cd .distr
@rem zip %ZIPNAME% -r
@rem zip umba-tools_windows_x86_release_1.0.zip -r umba-tools_windows_x86_release_1.0.zip umba-tools
zip %ZIPNAME%.zip -r %ZIPNAME%.zip .
@cd ..
