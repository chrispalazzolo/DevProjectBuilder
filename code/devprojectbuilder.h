#if !defined(DEVPROJECTBUILDER_H)

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef UNICODE
#define UNICODE
#endif
    
#ifdef _UNICODE
#define _UNICODE
#endif
    
#define STRICT
    
#define APP_FORM_X                20
#define APP_FORM_LINE_HEIGHT      20
#define APP_FORM_LINE_START_Y     20
#define APP_FORM_LINE_GAP         10
#define APP_FORM_LABEL_WIDTH      150
#define APP_FORM_SEC_ITEM_X       180
#define APP_FORM_EDIT_WIDTH       500
#define APP_FORM_DIALOG_BTN_X     690
#define APP_FORM_DIALOG_BTN_WIDTH 30
#define APP_FORM_CTRL_BTN_X       530
#define APP_FORM_CTRL_BTN_WIDTH   70
    
    //Menus
#define APP_MENU_FILE_SAVE_PROJECT  1000
#define APP_MENU_FILE_LOAD_PROJECT  1001
#define APP_MENU_FILE_SAVE_DEFAULTS 1002
#define APP_MENU_FILE_LOAD_DEFAULTS 1003
#define APP_MENU_FILE_EXIT          1004
#define APP_MENU_HELP_VIEW          1100
#define APP_MENU_HELP_ABOUT         1101
    
#define APP_EDIT_PROJECT_NAME       2000
#define APP_EDIT_PROJECT_PATH       2001
#define APP_BTN_PROJECT_PATH        2002
#define APP_COMBOBOX_SUBST_LETTER   2100
#define APP_EDIT_COMPILER           2200
#define APP_BTN_COMPILER_PATH       2201
#define APP_EDIT_COMPILER_ARGS      2202
#define APP_EDIT_COMPILER_FLAGS     2203
#define APP_EDIT_COMPILER_LINKS     2204
#define APP_EDIT_LINK_FILES         2300
#define APP_EDIT_DEBUGGER           2400
#define APP_BTN_DEBUGGER_PATH       2401
#define APP_EDIT_EDITOR             2500
#define APP_BTN_EDITOR_PATH         2501
#define APP_EDIT_EDITOR_ARGS        2503
#define APP_BTN_CREATE              2601
#define APP_BTN_RESET               2602
#define APP_CKBOX_SHORTCUT          2701
    
#define CREATE_PROJECT 0
#define SAVE_PROJECT   1
#define SAVE_DEFAULTS  2
#define LOAD_PROJECT   3
#define LOAD_DEFAULTS  4
#define APP_PATH       5
    
#include <windows.h>
#include <wingdi.h>
#include <stdio.h>
#include <stdint.h>
    
    //#include <stdafx.h>
#include <winnls.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <direct.h>
#include <errno.h>
    
    typedef int32_t int32;
    typedef int32 bool32;
    
    typedef struct PROJECT_PATHS
    {
        char Root[256];
        char Project[256];
        char Code[256];
        char Misc[256];
        char SubStRoot[256];
        char Compiler[256];
        char Debugger[256];
        char Editor[256];
    }project_paths;
    
    typedef struct PROJECT_DETAILS
    {
        project_paths Paths;
        char ProjectName[256];
        char ProjectFolderName[256];
        char ProjectFileNameLower[256];
        char ProjectFileNameUpper[256];
        int SubStDriveLetterIndex;
        char SubStDriveLetter[2];
        char CompilerArgs[256];
        char CompilerFlags[256];
        char CompilerLinkerFlags[256];
        char LinkFiles[256];
        char DebuggerFile[256];
        char DebuggerArgs[256];
        char EditorArgs[256];
        int CreateShortCut;
    }project_details;
    
    typedef struct CMD_SHORTCUT_INFO
    {
        char DesktopPath[256];
        char *ObjPath;
        char SaveFile[256];
        char Arguments[256];
        char Description[256];
    }cmd_shortcut_info;
    
    int StrLen(char *str);
    bool32 IsLetter(char letter);
    bool32 IsSpecialCharacter(char character);
    void ToUpperCase(char *str);
    void ToLowerCase(char *str);
    bool32 IsDriveSpecified(char *path);
    bool32 ContainsSpace(char *str);
    void StripSpaces(char *str, int sizeOfStr);
    void InsertSubString(char *str, int sizeOfStr, int startReplaceIndex, char *subStr, int sizeOfSubStr);
    void AppendString(char *str, int sizeOfStr, char *appendStr, int startIndexOfAppendStr);
    void StrSpecifierSub(char *str, int sizeOfStr, project_details *details);
    void GetShortCutInfo(cmd_shortcut_info *Info, project_details *Details);
    HRESULT CreateLauncherCMDShortcut(project_details *Details);
    bool32 CreateHeaderFile(project_details *Details);
    bool32 CreateCPPFile(project_details *Details);
    bool32 CreateProjectFiles(project_details *Details);
    bool32 CreateStartBatFile(project_details *Details);
    bool32 CreateBuildBatFile(project_details *Details);
    bool32 CreateBatFiles(project_details *Details);
    bool32 SaveData(project_details *Data, char *FileName);
    bool32 LoadData(project_details *Data, char *FileName);
    void GetDesktopPath(char *Path);
    int CreateDir(char *Path);
    bool32 RemoveDir(char *Path);
    bool32 CreateProjectDirectories(project_paths *Paths);
    void SetProjectFileNames(project_details *Details);
    void SetPaths(project_details *Details);
    void GetFileName(char *fileName, int sizeOfFileName, char *filePath);
    void ResetForm(HWND window);
    void FillForm(project_details *details);
    bool32 GetFormValues(project_details *details, int dataForFlag);
    bool32 Save(HWND window, int saveWhat);
    bool32 Load(HWND window, int loadWhat);
    bool32 CreateProject(HWND window);
    int CALLBACK BrowseCallbackProc(HWND window, UINT uMsg, LPARAM lParam, LPARAM lpData);
    bool32 ShowSaveFileDialog(HWND window, char* saveName, char* filePath, DWORD maxPath, int saveReasonFlag);
    bool32 ShowOpenFileDialog(HWND window, char* filePath, DWORD maxPath, int openReasonFlag);
    bool32 ShowFolderDialog(HWND window, char* folderPath, DWORD maxPath);
    void CreateMenuBar(HWND window);
    void CreateProjectNameLine(HWND window, int yPos);
    void CreateProjectRootPathLine(HWND window, int yPos);
    void CreateSubstLine(HWND window, int yPos);
    void CreateCompilerLine(HWND window, int yPos);
    void CreateCompilerArgsLine(HWND window, int yPos);
    void CreateCompilerFlagsLine(HWND window, int yPos);
    void CreateCompilerLinksLine(HWND window, int yPos);
    void CreateLinkFilesLine(HWND window, int yPos);
    void CreateDebuggerLine(HWND window, int yPos);
    void CreateDebuggerArgsLine(HWND window, int yPos);
    void CreateEditorLine(HWND window, int yPos);
    void CreateEditorArgsLine(HWND window, int yPos);
    void CreateShortCutLine(HWND window, int yPos);
    void CreateControlsLine(HWND window, int yPos);
    void CreateMainForm(HWND window);
    void CreateWindowForm(HWND window);
    
#ifdef __cplusplus
}
#endif

#define DEVPROJECTBUILDER_H
#endif