; ------------------------------------------------------------
; Установщик для Lab2_TiMP (демо-версия с защитой от переустановки)
; ------------------------------------------------------------

!define APP_NAME "Lab2_TiMP"
!define COMPANY_NAME "Manafi"
!define EXE_NAME "Lab2_TiMP.exe"
!define SHORT_NAME "Laba2"

; Используем modern UI
!include "MUI2.nsh"

; Общие настройки
Name "${APP_NAME}"
OutFile "setup-${SHORT_NAME}.exe"
InstallDir "$PROGRAMFILES64\${SHORT_NAME}"
RequestExecutionLevel admin  ; Требуем права администратора

; Интерфейс
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Russian"  ; или "English"

; Иконка (раскомментируйте, если есть файл icon.ico)
; !define MUI_ICON "icon.ico"
; !define MUI_UNICON "icon.ico"

Section "Основная программа" SecMain
    SetOutPath "$INSTDIR"
    
    ; Копируем исполняемый файл
    File "${EXE_NAME}"
    
    ; Создаём деинсталлятор (копия текущего установщика, но с меткой удаления)
    WriteUninstaller "$INSTDIR\uninstall.exe"
    
    ; Ярлык на рабочем столе
    CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${EXE_NAME}"
    
    ; Ярлык в меню «Пуск»
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\Запуск.lnk" "$INSTDIR\${EXE_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\Удалить.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

; Регистрация в "Установка и удаление программ"
Section -Post
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORT_NAME}" "DisplayName" "${APP_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORT_NAME}" "Publisher" "${COMPANY_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORT_NAME}" "DisplayVersion" "1.0"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORT_NAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORT_NAME}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORT_NAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORT_NAME}" "NoRepair" 1
SectionEnd

; ------------------------------------------------------------
; Деинсталлятор
; ------------------------------------------------------------
Section "Uninstall"
    ; Удаляем файлы программы
    Delete "$INSTDIR\${EXE_NAME}"
    Delete "$INSTDIR\list.txt"
    Delete "$INSTDIR\uninstall.exe"
    
    ; Удаляем папку (если пуста)
    RMDir "$INSTDIR"
    
    ; Удаляем ярлыки
    Delete "$DESKTOP\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Запуск.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Удалить.lnk"
    RMDir "$SMPROGRAMS\${APP_NAME}"
    
    ; Удаляем запись из "Установка и удаление программ"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${SHORT_NAME}"
    
    ; ❗❗ ВАЖНО: НЕ ТРОГАЕМ HKCU\Software\YourCompany\YourApp
    ; Именно там хранится счётчик запусков и времени!
    ; Это обеспечивает защиту от простой переустановки.
SectionEnd