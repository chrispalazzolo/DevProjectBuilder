#if !defined(DEVPROJECTBUILDER_H)

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
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
    char RootPath[256];
    char ProjectPath[256];
    char CodePath[256];
    char MiscPath[256];

    char SubStRootPath[256];
}project_paths;

typedef struct PROJECT_DETAILS
{
    project_paths Paths;
    char ProjectName[256];
    char ProjectFolderName[256];
    char ProjectFileNameLower[256];
    char ProjectFileNameUpper[256];
    char SubStDriveLetter[2];
    char IDECommand[256];
    char CompilerPath[256];
    char CompilerFlags[256];
    char LinkerFlags[256];
}project_details;

typedef struct DEFAULT_INPUTS
{
    char SubStDriveLetter[2];
    char RootPath[256];
    char Compiler[256];
    char CompilerFlags[256];
    char LinkerFlags[256];
    char EditorCMD[256];
}default_inputs;

typedef struct CMD_SHORTCUT_INFO
{
    char DesktopPath[256];
    char *ObjPath;
    char SaveFile[256];
    char Arguments[256];
    char Description[256];
}cmd_shortcut_info;

bool32 IsLetter(char Letter);
bool32 IsSpecialCharacter(char Character);
void ToUpperCase(char *Str);
void ToLowerCase(char *Str);
bool32 IsDriveSpecified(char *Path);
void StripSpaces(char *Str, int SizeOfStr);
void InsertSubString(char *Str, int SizeOfStr, int StartReplaceIndex, char *SubStr, int SizeOfSubStr);
void AppendString(char *Str, int SizeOfStr, char *AppendStr, int StartIndexOfAppendStr);
void StrSpecifierSub(char *Str, int SizeOfStr, project_details *Details);
void GetShortCutInfo(cmd_shortcut_info *Info, project_details *Details);
HRESULT CreateLauncherCMDShortcut(project_details *Details);
bool32 CreateHeaderFile(project_details *Details);
bool32 CreateCPPFile(project_details *Details);
void CreateProjectFiles(project_details *Details);
bool32 CreateStartupFile(project_details *Details);
bool32 CreateBuildFile(project_details *Details);
bool32 SaveDefaults(default_inputs *Defaults, char *FileName);
bool32 GetDefaultsFromFile(default_inputs *Defaults, char *FileName);
void GetDesktopPath(char *Path);
bool32 RemoveDir(char *Path);
bool32 CreateProjectDirectories(project_paths *Paths);
void SetProjectFileNames(project_details *Details);
void SetPaths(project_details *Details);
void PrintPaths(project_paths *Paths);
void PrintProjectDetails(project_details *Details);
void PrintDefaultDetails(default_inputs *Defaults);
void DisplayHelp(void);
void AskQuestion(char *Question, char *Detail, int SizeOfDetail);
void GetProjectName(char *ProjectName, int SizeOfProjectName);
void GetRootPath(char *Detail, int SizeOfDetail, char *Default, int SizeOfDefault);
void GetSubStrDriveLetter(char *SubSt, char *Default);
void GetCompilerPath(char *Detail, int SizeOfDetail, char *Default);
void GetIDECMLCommand(char *Detail, int SizeofDetail, char *Default);
void GetCompilerFlags(char *Detail, int SizeOfDetail, char *Default);
void GetLinkerFlags(char *Detail, int SizeOfDetail, char *Default);
void AskStartupQuestions(project_details *Details, default_inputs *Defaults);
void AskBuildQuestions(project_details *Details, default_inputs *Defaults);
void AskDefaultQuestions(default_inputs *Defaults);
int GetProjectConfirmation(project_details *Details);
int GetDefaultConfirmation(default_inputs *Defaults);
void CreateProject(project_details *ProjectDetails, default_inputs *Defaults);
bool32 CreateDefaults(default_inputs *Defaults, char *DefaultsFileName);
bool32 DeleteDefaults(default_inputs *Defaults, char *DefaultFileName);
void DefaultsMenu(default_inputs *Defaults, bool32 *HasDefaults, char *DefaultsFileName);
void MainMenu(project_details *Details, default_inputs *Defaults, bool32 *IsDefaultsSet, char *DefaultsFileName);

#ifdef __cplusplus
}
#endif

#define DEVPROJECTBUILDER_H
#endif