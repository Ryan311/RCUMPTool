set BuildReleaseDIR=".\Release"
set RCUReleaseDIR=".\RCURelease"
if not exist %RCUReleaseDIR% md %RCUReleaseDIR%

XCOPY %BuildReleaseDIR%\*.dll %RCUReleaseDIR% /e /y
XCOPY %BuildReleaseDIR%\*.exe %RCUReleaseDIR% /e /y
XCOPY %BuildReleaseDIR%\*.ini %RCUReleaseDIR% /e /y
XCOPY %BuildReleaseDIR%\*.bmp %RCUReleaseDIR% /e /y
XCOPY %BuildReleaseDIR%\*.png %RCUReleaseDIR% /e /y