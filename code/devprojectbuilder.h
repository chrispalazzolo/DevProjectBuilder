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
    char RootPath[MAX_PATH];
    char ProjectPath[MAX_PATH];
    char CodePath[MAX_PATH];
    char MiscPath[MAX_PATH];

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

#ifdef __cplusplus
}
#endif

#define DEVPROJECTBUILDER_H
#endif