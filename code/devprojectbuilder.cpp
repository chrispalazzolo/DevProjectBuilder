/* 
*  File:    DevProjectBuilder.cpp
*  Author:  Chris Palazzolo
*  Date:    8/3/2017
*
*  Description:
*  
*  This program takes input from a user and builds a dev environment
*  for a new project.  It produces a build.bat, a startup.bat, directories
*  and CMD shortcut pointing to the startup.bat.
*/

#include "devprojectbuilder.h"

HWND hProjectNameEdit, hProjRootPathEdit, hProjRootPathBtn, hSubstComboBox, hCompilerEdit, hCompilerPathBtn, hCompilerArgsEdit, hCompilerFlagsEdit, hCompilerLinksEdit, hLinkFilesEdit, hDebuggerEdit, hDebuggerPathBtn, hDebuggerArgsEdit, hEditorEdit, hEditorPathBtn, hEditorArgsEdit, hShortCutCkBox, hCreateBtn, hResetBtn;

//==== String Functions ================================================================================
int StrLen(char *str)
{
    int ct = 0;
    
    while(str[ct] != '\0')
    {
        ct++;
    }
    
    return ct;
}

bool32 IsLetter(char letter)
{
    return ((letter > 64 && letter < 91) || (letter > 96 && letter < 123));
}

bool32 IsSpecialCharacter(char character)
{
    return (!IsLetter(character) && (character < 48 || character > 57));
}

void ToUpperCase(char *str)
{
    if(str != NULL)
    {
        int i = 0;
        
        while(str[i] != 0)
        {
            if(str[i] >= 97 && str[i] <= 122)
            {
                str[i] = str[i] - 32;
            }
            
			++i;
        }
    }
}

void ToLowerCase(char *str)
{
    if(str != NULL)
    {
        int i = 0;
        
        while(str[i] != 0)
        {
            if(str[i] >= 65 && str[i] <= 90)
            {
                str[i] = str[i] + 32;
            }
            
			++i;
        }
    }
}

bool32 IsDriveSpecified(char *path)
{
    bool32 result = false;
    
    if(path != NULL)
    {
        result = (IsLetter(path[0]) && path[1] == 58);
    }
    
    return result;
}

bool32 ContainsSpace(char *str)
{
    if(str != NULL)
    {
        int i = 0;
        
        while(str[i] != '\0')
        {
            if(str[i] == ' ')
            {
                return 1;
            }
            
            i++;
        }
    }
    
    return 0;
}

void StripSpaces(char *str, int sizeOfStr)
{
    if(str != NULL)
    {
        size_t Len = strcspn(str, "\0");
        char newStr[256];
        int newStrIndex = 0;
        for(int i = 0; i < Len; ++i)
        {
            if(str[i] != 32)
            {
                newStr[newStrIndex] = str[i];
                newStrIndex++;
            }
        }
        
        if(newStrIndex < Len)
        {
            newStr[newStrIndex] = 0;
            _snprintf_s(str, Len, Len, "%s", newStr);
        }
    }
}

void InsertSubString(char *str, int sizeOfStr, int startReplaceIndex, char *subStr, int sizeOfSubStr)
{
    if(str != NULL && subStr != NULL)
    {
        size_t strLen = strcspn(str, "\0");
        size_t subStrLen = strcspn(subStr, "\0");
        
		if ((strLen + subStrLen) < sizeOfStr)
		{
            char newStr[256];
            _snprintf_s(newStr, sizeof(newStr), startReplaceIndex, "%s", str);
			_snprintf_s(newStr, sizeof(newStr), "%s%s", newStr, subStr);
            
            startReplaceIndex += 2;
            int newStrIndex = (int)(startReplaceIndex + subStrLen);
            
            bool32 endOfStr = 0;
            
            while(!endOfStr)
            {
                endOfStr = (str[startReplaceIndex] == 0);
                newStr[newStrIndex] = str[startReplaceIndex];
                ++newStrIndex;
                ++startReplaceIndex;
            }
            
            _snprintf_s(str, sizeOfStr, sizeOfStr, "%s", newStr);
		}
    }
}

void AppendString(char *str, int sizeOfStr, char *appendStr, int startIndexOfAppendStr)
{
    if(str != NULL)
    {
        char newStr[256];
        _snprintf_s(newStr, sizeof(newStr), "%s", str);
        size_t endOfStr = strcspn(newStr, "\n");
        
        if(endOfStr <= 0)
        {
            endOfStr = strcspn(newStr, "\0");
        }
        
        int i = startIndexOfAppendStr;
        bool32 done = 0;
        
        while(!done)
        {
            newStr[endOfStr] = appendStr[i];
            
            if(appendStr[i] == '\0')
            {
                
                done = 1;
            }
            
            ++i;
            ++endOfStr;
        }
        
        _snprintf_s(str, sizeOfStr, sizeOfStr, "%s", newStr);
    }
}

void StrSpecifierSub(char *str, int sizeOfStr, project_details *details)
{
    if(str != NULL)
    {
        bool32 endOfStr = 0;
        int i = 0;
        
        while(!endOfStr)
        {
            if(str[i] == 123 && (((i+2) < sizeOfStr) && str[i+2] == 125)) // if {*}
            {
                char identifier = str[i+1];
                
                switch(identifier)
                {
                    case 'r': //root path
                    {
                        InsertSubString(str, sizeOfStr, i, details->Paths.Root, sizeof(details->Paths.Root));
                        
                        break;
                    }
                    case 'p': // project path
                    {
                        InsertSubString(str, sizeOfStr, i, details->Paths.Project, sizeof(details->Paths.Project));
                        
                        break;
                    }
                    case 'f': // path to cpp file
                    {
                        char newStr[256];
                        _snprintf_s(newStr, sizeof(newStr), "%s%s.cpp", details->Paths.Code, details->ProjectFileNameLower);
                        InsertSubString(str, sizeOfStr, i, newStr, sizeof(newStr));
                        
                        break;
                    }
                    case 'h': // path to header file
                    {
                        char newStr[256];
                        _snprintf_s(newStr, sizeof(newStr), "%s%s.h", details->Paths.Code, details->ProjectFileNameLower);
                        InsertSubString(str, sizeOfStr, i, newStr, sizeof(newStr));
                        
                        break;
                    }
                }
            }
            
            ++i;
            
            endOfStr = (str[i] == '\0' || i >= sizeOfStr);
        }
    }
}
// === End of String Functions ==================================================================

// === File Functions ===========================================================================
void GetShortCutInfo(cmd_shortcut_info *Info, project_details *Details)
{
    Info->ObjPath = "C:\\Windows\\System32\\cmd.exe";
    _snprintf_s(Info->Description, sizeof(Info->Description), "CMD and Editor Launcher for project %s", Details->ProjectName);
    _snprintf_s(Info->Arguments, sizeof(Info->Arguments), "/k %sstartup.bat", Details->Paths.Misc);
    GetDesktopPath(Info->DesktopPath);
    _snprintf_s(Info->SaveFile, sizeof(Info->SaveFile), "%s\\%s.lnk", Info->DesktopPath, Details->ProjectFolderName);
}

HRESULT CreateLauncherCMDShortcut(project_details *Details)
{ 
    HRESULT Result; 
    void *PVReserved = NULL;
    
    Result = CoInitialize(PVReserved);
    
    if(SUCCEEDED(Result))
    {
        IShellLink* ShellLink;
        
        Result = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&ShellLink);
        
        if (SUCCEEDED(Result))
        {    
            cmd_shortcut_info Info = {};
            GetShortCutInfo(&Info, Details);
            
            ShellLink->SetPath(Info.ObjPath);
            ShellLink->SetArguments(Info.Arguments);
            ShellLink->SetDescription(Info.Description);
            
            IPersistFile* PersistFile;
            
            Result = ShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&PersistFile);
            
            if (SUCCEEDED(Result))
            {
                WCHAR UnicodeStr[MAX_PATH];
                
                // Ensure that the string is Unicode.
                MultiByteToWideChar(CP_ACP, 0, Info.SaveFile, -1, UnicodeStr, MAX_PATH);
                
                Result = PersistFile->Save(UnicodeStr, TRUE);
				
                PersistFile->Release();
            }
            
            ShellLink->Release();
        }
    }
    
    return Result;
}

bool32 CreateHeaderFile(project_details *Details)
{
    char HeaderFileNameBuffer[256];
    _snprintf_s(HeaderFileNameBuffer, sizeof(HeaderFileNameBuffer), "%s%s.h", Details->Paths.Code, Details->ProjectFileNameLower);
    FILE *HeaderFile;
    
    errno_t Error = fopen_s(&HeaderFile, HeaderFileNameBuffer, "w");
    
    if(!Error)
    {
        fprintf(HeaderFile, "%s%s%s", "/*\n*  File: ", Details->ProjectFileNameLower, ".h\n*  Author: \n*  Date: \n*\n*  Description:\n* \n*/\n\n");
        fprintf(HeaderFile, "%s%s%s", "#if !defined(", Details->ProjectFileNameUpper, "_H)\n\n");
        fprintf(HeaderFile, "%s", "#ifdef __cplusplus\n");
        fprintf(HeaderFile, "%s", "extern \"C\" {\n");
        fprintf(HeaderFile, "%s", "#endif\n\n");
        fprintf(HeaderFile, "%s", "// Add code here...\n\n");
        fprintf(HeaderFile, "%s", "#ifdef __cplusplus\n");
        fprintf(HeaderFile, "%s", "}\n");
        fprintf(HeaderFile, "%s", "#endif\n\n");
        fprintf(HeaderFile, "%s%s%s", "#define ", Details->ProjectFileNameUpper, "_H\n");
        fprintf(HeaderFile, "%s", "#endif");
        fclose(HeaderFile);
    }
    
    return !Error;
}

bool32 CreateCPPFile(project_details *Details)
{
    char CPPFileNameBuffer[256];
    _snprintf_s(CPPFileNameBuffer, sizeof(CPPFileNameBuffer), "%s%s.cpp", Details->Paths.Code, Details->ProjectFileNameLower);
    FILE *CPPFile;
    
    errno_t Error = fopen_s(&CPPFile, CPPFileNameBuffer, "w");
    
    if(!Error)
    {
        fprintf(CPPFile, "%s%s%s", "/*\n*  File: ", Details->ProjectFileNameLower, ".cpp\n*  Author: \n*  Date: \n*\n*  Description:\n* \n*/\n\n");
        fprintf(CPPFile, "%s%s%s", "#include \"", Details->ProjectFileNameLower, ".h\"\n\n");
        fprintf(CPPFile, "%s", "// Add code here...\n\n");
        fclose(CPPFile);
    }
    
    return !Error;
}

bool32 CreateProjectFiles(project_details *Details)
{
    if(!CreateHeaderFile(Details))
    {
        return 0;
    }
    
    if(!CreateCPPFile(Details))
    {
        return 0;
    }
    
    return 1;
}

bool32 CreateStartBatFile(project_details *Details)
{
    char FileNameAndPath[MAX_PATH];
    _snprintf_s(FileNameAndPath, sizeof(FileNameAndPath), "%sstartup.bat", Details->Paths.Misc);
    
    FILE *File;
    errno_t Error = fopen_s(&File, FileNameAndPath, "w");
    
    if(Error)
    {
        return 0;
    }
    
    fprintf(File, "%s", "@echo off\n");
    fprintf(File, "%s %s%s %s%s", "subst", &Details->SubStDriveLetter, ":", Details->Paths.Root, "\n");
    
    if(Details->CompilerArgs[0] != '\0')
    {
        fprintf(File, "%s %s %s%s", "call", Details->Paths.Compiler, Details->CompilerArgs, "\n");
    }
    else
    {
        fprintf(File, "%s %s%s", "call", Details->Paths.Compiler, "\n");
    }
    
    fprintf(File, "%s%s%s", "set PATH=%PATH%;", Details->Paths.SubStRoot, "\n" );
    
    if(Details->Paths.Debugger[0] != '\0')
    {
        fprintf(File, "%s%s%s", "set PATH=%PATH%;", Details->Paths.Debugger, ";%PATH%\n");
    }
    
    
    fprintf(File, "%s %s%s", "cd /D", Details->Paths.SubStRoot, "\n");
    fprintf(File, "%s", "start /b cmd.exe\n");
    
    if(Details->Paths.Debugger[0] != '\0')
    {
        if(Details->DebuggerArgs[0] != '\0')
        {
            fprintf(File, "%s %s %s %s", "start /b", Details->DebuggerFile, Details->DebuggerArgs, "\n");
        }
        else
        {
            fprintf(File, "%s %s %s", "start /b", Details->DebuggerFile, "\n");
        }
    }
    
    if(Details->Paths.Editor[0] != '\0')
    {
        if(Details->EditorArgs[0] != '\0')
        {
            fprintf(File, "%s %s", Details->Paths.Editor, Details->EditorArgs);
        }
        else
        {
            fprintf(File, "%s", Details->Paths.Editor);
        }
    }
    
    fclose(File);
    
    return 1;
}

bool32 CreateBuildBatFile(project_details *Details)
{
    char FileNameAndPath[MAX_PATH];
    _snprintf_s(FileNameAndPath, sizeof(FileNameAndPath), "%sbuild.bat", Details->Paths.Code);
    
    FILE *File;
    errno_t Error = fopen_s(&File, FileNameAndPath, "w");
	
    if(!Error)
    {
        fprintf(File, "%s", "@echo off\n\n");
        
        if(Details->CompilerFlags[0] != '\0')
        {
            fprintf(File, "%s%s%s", "set CommonCompilerFlags=", Details->CompilerFlags, "\n");
        }
        
        if(Details->CompilerLinkerFlags[0] != '\0')
        {
            fprintf(File, "%s%s%s", "set CommonLinkerFlags=", Details->CompilerLinkerFlags, "\n");
        }
        
        if(Details->LinkFiles[0] != '\0')
        {
            fprintf(File, "%s%s%s", "set LinkerFiles=", Details->LinkFiles , "\n\n");
        }
        
        fprintf(File, "%s", "IF NOT EXIST ..\\build mkdir ..\\build\n");
        fprintf(File, "%s", "pushd ..\\build\n\n");
        fprintf(File, "%s", "REM 32bit build\n");
        fprintf(File, "%s%s%s%s%s", "REM cl %CommonCompilerFlags% \\", Details->ProjectFileNameLower, "\\code\\", Details->ProjectFileNameLower, ".cpp /link -subsystem:windows,5.10 %CommonLinkerFlags%\n\n");
        
        fprintf(File, "%s", "REM 64bit build\n");
        fprintf(File, "%s", "del *.pdb > NUL 2> NUL\n");
        fprintf(File, "%s", "REM If you want to compile a DLL\n");
        fprintf(File, "%s%s%s", "REM cl %CommonCompilerFlags% \\", Details->ProjectFileNameLower, "\\code\\DLLNAMEHERE.cpp -FmNAMEHERE.map -LD /link -incremental:no -PDB:NAMEHERE.pdb -EXPORT:FUNCTIONEXPORTHERE\n");
        fprintf(File, "%s%s%s%s%s%s%s", "cl %CommonCompilerFlags% \\", Details->ProjectFileNameLower, "\\code\\", Details->ProjectFileNameLower, ".cpp /Z7 -Fm", Details->ProjectFileNameLower, ".map /link %CommonLinkerFlags% %LinkerFiles%\n");
        fprintf(File, "%s", "popd");
        fclose(File);
    }
    
    return !Error;
}

bool32 CreateBatFiles(project_details *Details)
{
    if(!CreateStartBatFile(Details))
    {
        return 0;
    }
    
    if(!CreateBuildBatFile(Details))
    {
        return 0;
    }
    
    return 1;
}

bool32 SaveData(project_details *Data, char *FileName)
{
    if(Data == NULL || FileName == NULL)
    {
        return 0;
    }
    
    FILE *File;
    errno_t Error = fopen_s(&File, FileName, "wb"); // not sure how I want to handle the errno
    
    if(Error)
    {
        return 0;
    }
    
    fwrite(Data, sizeof(project_details), 1, File);
    fclose(File);
    
    return 1;
}

bool32 LoadData(project_details *Data, char *FileName)
{
    FILE *File;
    errno_t Error = fopen_s(&File, FileName, "rb");
    
    if(Error)
    {
        return 0;
    }
    
    fread(Data, sizeof(project_details), 1, File);
    fclose(File);
    
    return 1;
}
// === End Files Functions =====================================================================

// === Util Functions ==========================================================================
void GetDesktopPath(char *Path)
{
#ifdef _WIN32
    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, Path);
#else
    SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, Path);
#endif
}

int CreateDir(char *Path)
{
    int result = 0; //0 - error, 1 - success, EEXIST = 17 - exists
    
    int MkDirResult = _mkdir(Path);
    
    if(MkDirResult == 0 || errno == EEXIST)
    {
        if(MkDirResult == -1 && errno == EEXIST)
        {
            result = EEXIST;
        }
        else
        {
            result = 1;
        }
    }
    
    return result;
}

bool32 RemoveDir(char *Path)
{
    return _rmdir(Path);
}

bool32 CreateProjectDirectories(project_paths *Paths)
{
    if(!CreateDir(Paths->Root))
    {
        return 0;
    }
    
    if(!CreateDir(Paths->Project))
    {
        RemoveDir(Paths->Root);
        return 0;
    }
    
    if(!CreateDir(Paths->Code))
    {
        RemoveDir(Paths->Root);
        RemoveDir(Paths->Project);
        return 0;
    }
    
    if(!CreateDir(Paths->Misc))
    {
        RemoveDir(Paths->Root);
        RemoveDir(Paths->Project);
        RemoveDir(Paths->Code);
        return 0;
    }
    
    return 1;
}

void SetProjectFileNames(project_details *Details)
{
    char Name[sizeof(Details->ProjectName)];
    _snprintf_s(Name, sizeof(Name), "%s", Details->ProjectName);
    
    StripSpaces(Name, sizeof(Name));
    
    _snprintf_s(Details->ProjectFolderName, sizeof(Details->ProjectFolderName), 256, "%s", Name);
    
    _snprintf_s(Details->ProjectFileNameUpper, sizeof(Details->ProjectFileNameUpper), 256, "%s", Name);
    ToUpperCase(Details->ProjectFileNameUpper);
    
    _snprintf_s(Details->ProjectFileNameLower, sizeof(Details->ProjectFileNameLower), 256, "%s", Name);
    ToLowerCase(Details->ProjectFileNameLower);
}

void SetPaths(project_details *Details)
{
    _snprintf_s(Details->Paths.Project, sizeof(Details->Paths.Project), "%s\\%s\\", Details->Paths.Root, Details->ProjectFolderName);
    _snprintf_s(Details->Paths.Code, sizeof(Details->Paths.Code), "%scode\\", Details->Paths.Project);
    _snprintf_s(Details->Paths.Misc, sizeof(Details->Paths.Misc), "%smisc\\", Details->Paths.Project);
    _snprintf_s(Details->Paths.SubStRoot, sizeof(Details->Paths.SubStRoot), "%s:\\%s", Details->SubStDriveLetter, Details->ProjectFolderName);
}

void GetFileName(char *fileName, int sizeOfFileName, char *filePath)
{
    char newStr[256];
    newStr[0] = '\0';
    
    if(filePath != NULL)
    {
        int pathLen = StrLen(filePath);
        int i = pathLen;
        
        while(i > -1)
        {
            if(filePath[i] == '\\')
            {
                break;
            }
            
            i--;
        }
        
        i++; //move off the \\ on to first char of file name.
        int newI = 0;
        
        while(i <= pathLen)
        {
            newStr[newI] = filePath[i];
            
            i++;
            newI++;
        }
        
    }
    
    _snprintf_s(fileName, sizeOfFileName, sizeOfFileName, "%s", newStr);
}

// === End Util Functions ================================================================================

// === Main Project Functions ==========================================================================================
void ResetForm(HWND window)
{
    SetWindowText(hProjectNameEdit, "");
    //hSubstComboBox
    SetWindowText(hProjRootPathEdit, "");
    SetWindowText(hCompilerEdit, "");
    SetWindowText(hCompilerArgsEdit, "");
    SetWindowText(hCompilerFlagsEdit, "");
    SetWindowText(hCompilerLinksEdit, "");
    SetWindowText(hLinkFilesEdit, "");
    SetWindowText(hDebuggerEdit, "");
    SetWindowText(hDebuggerArgsEdit, "");
    SetWindowText(hEditorEdit, "");
    SetWindowText(hEditorArgsEdit, "");
    CheckDlgButton(window, APP_CKBOX_SHORTCUT, BST_UNCHECKED);
}

void FillForm(project_details *details)
{
    if(details != NULL)
    {
        SetWindowText(hProjectNameEdit, details->ProjectName);
        //hSubstComboBox
        SetWindowText(hProjRootPathEdit, details->Paths.Root);
        SetWindowText(hCompilerEdit, details->Paths.Compiler);
        SetWindowText(hCompilerArgsEdit, details->CompilerArgs);
        SetWindowText(hCompilerFlagsEdit, details->CompilerFlags);
        SetWindowText(hCompilerLinksEdit, details->CompilerLinkerFlags);
        SetWindowText(hLinkFilesEdit, details->LinkFiles);
        SetWindowText(hDebuggerEdit, details->Paths.Debugger);
        SetWindowText(hDebuggerArgsEdit, details->DebuggerArgs);
        SetWindowText(hEditorEdit, details->Paths.Editor);
        SetWindowText(hEditorArgsEdit, details->EditorArgs);
        //CheckDlgButton(window, APP_CKBOX_SHORTCUT, BST_UNCHECKED);
    }
}

bool32 GetFormValues(project_details *details, int dataForFlag)
{
    details->ProjectName[0] = '\0';
    GetWindowText(hProjectNameEdit, details->ProjectName, sizeof(details->ProjectName));
    
    if(dataForFlag == CREATE_PROJECT && details->ProjectName[0] == '\0')
    {
        MessageBox(NULL, "Please provide a name for the project!", "Create Project Error", NULL);
        return 0;
    }
    
    if(dataForFlag == CREATE_PROJECT && ContainsSpace(details->ProjectName))
    {
        MessageBox(NULL, "No spaces are allow in project name!", "Create Project Error", NULL);
        return 0;
    }
    
    SetProjectFileNames(details);
    
    details->Paths.Root[0] = '\0';
    GetWindowText(hProjRootPathEdit, details->Paths.Root, sizeof(details->Paths.Root));
    
    if(dataForFlag == CREATE_PROJECT && details->Paths.Root[0] == '\0')
    {
        MessageBox(NULL, "Please provide a root path of the project!", "Create Project Error", NULL);
        return 0;
    }
    
    GetWindowText(hSubstComboBox, details->SubStDriveLetter, sizeof(details->SubStDriveLetter));
    
    details->Paths.Compiler[0] = '\0';
    GetWindowText(hCompilerEdit, details->Paths.Compiler, sizeof(details->Paths.Compiler));
    
    if(dataForFlag == CREATE_PROJECT && details->Paths.Compiler[0] == '\0')
    {
        MessageBox(NULL, "Please provide the application path to a compiler!", "Create Project Error", NULL);
        return 0;
    }
    
    details->CompilerArgs[0] = '\0';
    GetWindowText(hCompilerArgsEdit, details->CompilerArgs, sizeof(details->CompilerArgs));
    
    details->CompilerFlags[0] = '\0';
    GetWindowText(hCompilerFlagsEdit, details->CompilerFlags, sizeof(details->CompilerFlags));
    
    details->CompilerLinkerFlags[0] = '\0';
    GetWindowText(hCompilerLinksEdit, details->CompilerLinkerFlags, sizeof(details->CompilerLinkerFlags));
    
    details->LinkFiles[0] = '\0';
    GetWindowText(hLinkFilesEdit, details->LinkFiles, sizeof(details->LinkFiles));
    
    details->Paths.Debugger[0] = '\0';
    details->DebuggerFile[0] = '\0';
    details->DebuggerArgs[0] = '\0';
    
    GetWindowText(hDebuggerEdit, details->Paths.Debugger, sizeof(details->Paths.Debugger));
    
    if(details->Paths.Debugger[0] != '\0')
    {
        GetFileName(details->DebuggerFile, sizeof(details->DebuggerFile), details->Paths.Debugger);
        GetWindowText(hDebuggerArgsEdit, details->DebuggerArgs, sizeof(details->DebuggerArgs));
    }
    
    details->Paths.Editor[0] = '\0';
    details->EditorArgs[0] = '\0';
    GetWindowText(hEditorEdit, details->Paths.Editor, sizeof(details->Paths.Editor));
    
    if(details->Paths.Editor[0] != '\0')
    {
        GetWindowText(hEditorArgsEdit, details->EditorArgs, sizeof(details->EditorArgs));
    }
    
    SetPaths(details);
    
    return 1;
}

bool32 SaveProject(HWND window)
{
    project_details projectDetails = {};
    char saveFile[256] = "";
    
    if(!GetFormValues(&projectDetails, SAVE_PROJECT))
    {
        return 0;
    }
    
    if(!ShowSaveFileDialog(window, saveFile, 256, SAVE_PROJECT))
    {
        return 0;
    }
    
    if(!SaveData(&projectDetails, saveFile))
    {
        //MessageBox
        return 0;
    }
    
    return 1;
}

bool32 LoadProject(HWND window)
{
    project_details projectDetails = {};
    
    char loadFile[256] = "";
    
    if(!ShowOpenFileDialog(window, loadFile, 256, LOAD_PROJECT))
    {
        return 0;
    }
    
    if(!LoadData(&projectDetails, loadFile))
    {
        //MessageBox
        return 0;
    }
    
    ResetForm(window);
    FillForm(&projectDetails);
    
    return 1;
}

bool32 SaveDefaults(HWND window)
{
    project_details defaults = {};
    char saveFile[256] = "";
    
    if(!GetFormValues(&defaults, SAVE_DEFAULTS))
    {
        return 0;
    }
    
    if(!ShowSaveFileDialog(window, saveFile, 256, SAVE_DEFAULTS))
    {
        return 0;
    }
    
    if(!SaveData(&defaults, saveFile))
    {
        //MessageBox
        return 0;
    }
    
    return 1;
}

bool32 LoadDefaults(HWND window)
{
    project_details defaults = {};
    
    char loadFile[256] = "";
    
    if(!ShowOpenFileDialog(window, loadFile, 256, LOAD_DEFAULTS))
    {
        return 0;
    }
    
    if(!LoadData(&defaults, loadFile))
    {
        //MessageBox
        return 0;
    }
    
    ResetForm(window);
    FillForm(&defaults);
    
    return 1;
}

bool32 CreateProject(HWND window)
{
    project_details details = {};
    
    if(!GetFormValues(&details, CREATE_PROJECT))
    {
        return 0;
    }
    
    if(!CreateProjectDirectories(&details.Paths))
    {
        MessageBox(NULL, "Error: Could not create project directories!", "Create Project Error", NULL);
        return 0;
    }
    
    if(!CreateBatFiles(&details))
    {
        MessageBox(NULL, "Error: Could not create BAT files!", "Create Project Error", NULL);
        return 0;
    }
    
    if(!CreateProjectFiles(&details))
    {
        MessageBox(NULL, "Error: Could not create project files!", "Create Project Error", NULL);
        return 0;
    }
    
    if(IsDlgButtonChecked(window, APP_CKBOX_SHORTCUT))
    {
        if(!SUCCEEDED(CreateLauncherCMDShortcut(&details)))
        {
            return 0;
        }
    }
    
    return 1;
}

int CALLBACK BrowseCallbackProc(HWND window, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    switch (uMsg)
    {
        case BFFM_INITIALIZED:
        // Set initial directory
        // SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)"C:\\");
        break;
    }
    
    return 0;
}

bool32 ShowSaveFileDialog(HWND window, char* filePath, DWORD maxPath, int saveReasonFlag)
{
    OPENFILENAME ofn;
    char fileName[256] = "";
    
    ZeroMemory(&ofn, sizeof(ofn));
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = window;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = sizeof(fileName);
    
    if(saveReasonFlag == SAVE_PROJECT)
    {
        ofn.lpstrFilter = "DPB Project Files (*.dpb)\0*.dpb"; //.dpb created to store project data.
        ofn.lpstrDefExt = "dpb";
    }
    else
    {
        ofn.lpstrFilter = "DPB Defaults Files (*.dft)\0*.dft"; //.dft created to store defaults.
        ofn.lpstrDefExt = "dft";
    }
    
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    
    if (GetSaveFileName(&ofn))
    {
        strcpy_s(filePath, maxPath, fileName);
        return 1;
    }
    
    return 0;
}

bool32 ShowOpenFileDialog(HWND window, char* filePath, DWORD maxPath, int openReasonFlag)
{
    OPENFILENAME ofn;
    char szFile[256] = "";
    
    // Initialize OPENFILENAME structure
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = window;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    
    // File type filters
    switch(openReasonFlag)
    {
        case APP_PATH: //Filter for .exe or .bat files
        {
            ofn.lpstrFilter = "Files (*.exe;*.bat)\0*.exe;*.bat\0";
            break;
        }
        
        case LOAD_PROJECT: //Filter for .dpb files
        {
            ofn.lpstrFilter = "Project Files (*.dpb)\0*.dpb\0";
            break;
        }
        
        case LOAD_DEFAULTS: //Filter for .dft files
        {
            ofn.lpstrFilter = "Defaults Files (*.dft)\0*.dft\0";
            break;
        }
    }
    
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select File";
    
    // Flags
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    
    // Show the dialog
    if(GetOpenFileName(&ofn))
    {
        strcpy_s(filePath, maxPath, szFile);
        return 1;
    }
    
    return 0;
}

bool32 ShowFolderDialog(HWND window, char* folderPath, DWORD maxPath)
{
    BROWSEINFO bi;
    char displayName[MAX_PATH];
    LPITEMIDLIST pidl;
    
    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = window;
    bi.pszDisplayName = displayName;
    bi.lpszTitle = "Select Folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
    bi.lpfn = BrowseCallbackProc;
    
    pidl = SHBrowseForFolder(&bi);
    
    if(pidl)
    {
        if(SHGetPathFromIDList(pidl, folderPath))
        {
            // Free the PIDL
            CoTaskMemFree(pidl);
            return 1;
        }
        
        CoTaskMemFree(pidl);
    }
    
    return 0;
}

void CreateMenuBar(HWND window)
{
    HMENU hProjectMenu = CreateMenu();
    AppendMenu(hProjectMenu, MF_STRING, APP_MENU_FILE_SAVE_PROJECT, "&Save");
    AppendMenu(hProjectMenu, MF_STRING, APP_MENU_FILE_LOAD_PROJECT, "&Load");
    
    HMENU hDefaultsMenu = CreateMenu();
    AppendMenu(hDefaultsMenu, MF_STRING, APP_MENU_FILE_SAVE_DEFAULTS, "&Save");
    AppendMenu(hDefaultsMenu, MF_STRING, APP_MENU_FILE_LOAD_DEFAULTS, "&Load");
    
    HMENU hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hProjectMenu, "&Project");
    AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hDefaultsMenu, "&Defaults");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0 , 0);
    AppendMenu(hFileMenu, MF_STRING, APP_MENU_FILE_EXIT, "&Exit");
    
    HMENU hHelpMenu = CreateMenu();
    AppendMenu(hHelpMenu, MF_STRING, APP_MENU_HELP_VIEW, "&View Help");
    AppendMenu(hHelpMenu, MF_STRING, APP_MENU_HELP_ABOUT, "&About");
    
    HMENU hMenu = CreateMenu();
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "&File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, "&Help");
    
    SetMenu(window, hMenu);
}

void CreateProjectNameLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Project Name:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hProjectNameEdit = CreateWindow("EDIT", "",
                                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                    APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                    window, (HMENU)APP_EDIT_PROJECT_NAME, NULL, NULL);
}

void CreateProjectRootPathLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Project Path:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hProjRootPathEdit = CreateWindow("EDIT", "",
                                     WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,
                                     APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                     window, (HMENU)APP_EDIT_PROJECT_PATH, NULL, NULL);
    
    hProjRootPathBtn = CreateWindow("BUTTON", "...",
                                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                    APP_FORM_DIALOG_BTN_X, yPos, APP_FORM_DIALOG_BTN_WIDTH, APP_FORM_LINE_HEIGHT,
                                    window, (HMENU)APP_BTN_PROJECT_PATH, NULL, NULL);
}

void CreateSubstLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "SUBST:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hSubstComboBox = CreateWindow(
                                  "COMBOBOX",
                                  "",
                                  WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
                                  APP_FORM_SEC_ITEM_X, yPos, 50, 200,
                                  window,
                                  (HMENU)APP_COMBOBOX_SUBST_LETTER,
                                  GetModuleHandle(NULL),
                                  NULL
                                  );
    
    //Fill with unused drive letters.
    DWORD drives = GetLogicalDrives();
    
    // Check each drive letter from A to Z
    for (int i = 0; i < 26; i++)
    {
        int driveLetter = 'A' + i;
        
        if (driveLetter == 'A' || driveLetter == 'B')
        {
            continue; // A and B are reserved for Floppy drives (The sounds of the A floppy still echos in mind)
        }
        
        // Unused drive letters will have a 0 bit)
        if (!(drives & (1 << i)))
        {
            char driveString[4];
            sprintf_s(driveString, "%c:", driveLetter);
            
            // Add to combo box
            SendMessage(hSubstComboBox, CB_ADDSTRING, 0, (LPARAM)driveString);
        }
    }
    
    // Select first item if any
    if (SendMessage(hSubstComboBox, CB_GETCOUNT, 0, 0) > 0)
    {
        SendMessage(hSubstComboBox, CB_SETCURSEL, 0, 0);
    }
}

void CreateCompilerLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Compiler:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hCompilerEdit = CreateWindow("EDIT", "",
                                 WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,
                                 APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                 window, (HMENU)APP_EDIT_COMPILER, NULL, NULL);
    
    hCompilerPathBtn = CreateWindow("BUTTON", "...",
                                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                    APP_FORM_DIALOG_BTN_X, yPos, APP_FORM_DIALOG_BTN_WIDTH, APP_FORM_LINE_HEIGHT,
                                    window, (HMENU)APP_BTN_COMPILER_PATH, NULL, NULL);
}

void CreateCompilerArgsLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Compiler Start Args:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hCompilerArgsEdit = CreateWindow("EDIT", "",
                                     WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                     APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                     window, (HMENU)APP_EDIT_COMPILER_ARGS, NULL, NULL);
}

void CreateCompilerFlagsLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Compiler Flags:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hCompilerFlagsEdit = CreateWindow("EDIT", "",
                                      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                      APP_FORM_SEC_ITEM_X,yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                      window, (HMENU)APP_EDIT_COMPILER_FLAGS, NULL, NULL);
}

void CreateCompilerLinksLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Compiler Linker Flags:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hCompilerLinksEdit = CreateWindow("EDIT", "",
                                      WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                      APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                      window, (HMENU)APP_EDIT_COMPILER_LINKS, NULL, NULL);
}

void CreateLinkFilesLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Link Files:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hLinkFilesEdit = CreateWindow("EDIT", "",
                                  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                  APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                  window, (HMENU)APP_EDIT_LINK_FILES, NULL, NULL);
}

void CreateDebuggerLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Debugger:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hDebuggerEdit = CreateWindow("EDIT", "",
                                 WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,
                                 APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                 window, (HMENU)APP_EDIT_DEBUGGER, NULL, NULL);
    
    hDebuggerPathBtn = CreateWindow("BUTTON", "...",
                                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                    APP_FORM_DIALOG_BTN_X, yPos, APP_FORM_DIALOG_BTN_WIDTH, APP_FORM_LINE_HEIGHT,
                                    window, (HMENU)APP_BTN_DEBUGGER_PATH, NULL, NULL);
}

void CreateDebuggerArgsLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Debugger Args:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hDebuggerArgsEdit = CreateWindow("EDIT", "",
                                     WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                     APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                     window, (HMENU)APP_EDIT_COMPILER_ARGS, NULL, NULL);
}

void CreateEditorLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Editor/IDE:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hEditorEdit = CreateWindow("EDIT", "",
                               WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,
                               APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                               window, (HMENU)APP_EDIT_EDITOR, NULL, NULL);
    
    hEditorPathBtn = CreateWindow("BUTTON", "...",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                  APP_FORM_DIALOG_BTN_X, yPos, APP_FORM_DIALOG_BTN_WIDTH, APP_FORM_LINE_HEIGHT,
                                  window, (HMENU)APP_BTN_EDITOR_PATH, NULL, NULL);
}

void CreateEditorArgsLine(HWND window, int yPos)
{
    CreateWindow("STATIC", "Editor Args:", WS_VISIBLE | WS_CHILD | SS_RIGHT,
                 APP_FORM_X, yPos, APP_FORM_LABEL_WIDTH, APP_FORM_LINE_HEIGHT,
                 window, NULL, NULL, NULL);
    
    hEditorArgsEdit = CreateWindow("EDIT", "",
                                   WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                   APP_FORM_SEC_ITEM_X, yPos, APP_FORM_EDIT_WIDTH, APP_FORM_LINE_HEIGHT,
                                   window, (HMENU)APP_EDIT_COMPILER_ARGS, NULL, NULL);
}

void CreateShortCutLine(HWND window, int yPos)
{
    hShortCutCkBox = CreateWindow("BUTTON", "Create Desktop Shortcut",
                                  WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
                                  APP_FORM_SEC_ITEM_X, yPos, 185, 35,        
                                  window, (HMENU)APP_CKBOX_SHORTCUT, NULL, NULL);
}

void CreateControlsLine(HWND window, int yPos)
{
    hCreateBtn = CreateWindow("BUTTON", "Create",
                              WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                              APP_FORM_CTRL_BTN_X, yPos, APP_FORM_CTRL_BTN_WIDTH, APP_FORM_LINE_HEIGHT,
                              window, (HMENU)APP_BTN_CREATE, NULL, NULL);
    
    hResetBtn = CreateWindow("BUTTON", "Reset",
                             WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                             APP_FORM_CTRL_BTN_X + APP_FORM_CTRL_BTN_WIDTH + 10, yPos, APP_FORM_CTRL_BTN_WIDTH, APP_FORM_LINE_HEIGHT,
                             window, (HMENU)APP_BTN_RESET, NULL, NULL);
}

void CreateMainForm(HWND window)
{
    int yPosStep = APP_FORM_LINE_HEIGHT + APP_FORM_LINE_GAP;
    int lineYPos = APP_FORM_LINE_START_Y;
    
    CreateProjectNameLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateProjectRootPathLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateSubstLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateCompilerLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateCompilerArgsLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateCompilerFlagsLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateCompilerLinksLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateLinkFilesLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateDebuggerLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateDebuggerArgsLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateEditorLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateEditorArgsLine(window, lineYPos);
    
    lineYPos += yPosStep;
    CreateShortCutLine(window, lineYPos);
    
    lineYPos += yPosStep + 20;
    CreateControlsLine(window, lineYPos);
}

void CreateWindowForm(HWND window)
{
    CreateMenuBar(window);
    CreateMainForm(window);
}

LRESULT CALLBACK WindowProc(HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            CreateWindowForm(window);
            
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(window, &ps);
            EndPaint(window, &ps);
            
            break;
        }
        
        case WM_COMMAND:
        {
            int lWord = LOWORD(wParam);
            int hWord = HIWORD(wParam);
            
            char filePath[MAX_PATH * 10];
            
            switch (lWord)
            {
                case APP_MENU_FILE_SAVE_PROJECT:
                {
                    if(SaveProject(window))
                    {
                        //do something
                    }
                    
                    break;
                }
                
                case APP_MENU_FILE_LOAD_PROJECT:
                {
                    if(LoadProject(window))
                    {
                        //do something
                    }
                    
                    break;
                }
                
                case APP_MENU_FILE_SAVE_DEFAULTS:
                {
                    if(SaveDefaults(window))
                    {
                        //do something
                    }
                    
                    break;
                }
                
                case APP_MENU_FILE_LOAD_DEFAULTS:
                {
                    if(LoadDefaults(window))
                    {
                        //do something
                    }
                    
                    break;
                }
                case APP_MENU_FILE_EXIT:
                {
                    PostQuitMessage(0);
                    break;
                }
                
                case APP_BTN_PROJECT_PATH:
                {
                    if (hWord == BN_CLICKED)
                    {
                        if(ShowFolderDialog(window, filePath, sizeof(filePath)))
                        {
                            SetWindowText(hProjRootPathEdit, filePath);
                        }
                    }
                    
                    break;
                }
                
                case APP_BTN_COMPILER_PATH:
                {
                    if(hWord == BN_CLICKED)
                    {
                        if(ShowOpenFileDialog(window, filePath, sizeof(filePath), APP_PATH))
                        {
                            SetWindowText(hCompilerEdit, filePath);
                        }
                    }
                    
                    break;
                }
                
                case APP_BTN_DEBUGGER_PATH:
                {
                    if(hWord == BN_CLICKED)
                    {
                        if(ShowOpenFileDialog(window, filePath, sizeof(filePath), APP_PATH))
                        {
                            SetWindowText(hDebuggerEdit, filePath);
                        }
                    }
                    
                    break;
                }
                
                case APP_BTN_EDITOR_PATH:
                {
                    if(hWord == BN_CLICKED)
                    {
                        if(ShowOpenFileDialog(window, filePath, sizeof(filePath), APP_PATH))
                        {
                            SetWindowText(hEditorEdit, filePath);
                        }
                    }
                    
                    break;
                }
                
                case APP_CKBOX_SHORTCUT:
                {
                    if (IsDlgButtonChecked(window, APP_CKBOX_SHORTCUT))
                    {
                        CheckDlgButton(window, APP_CKBOX_SHORTCUT, BST_UNCHECKED);
                    }
                    else
                    {
                        CheckDlgButton(window, APP_CKBOX_SHORTCUT, BST_CHECKED);
                    }
                    
                    break;
                }
                
                case APP_BTN_CREATE:
                {
                    if(hWord == BN_CLICKED)
                    {
                        if(CreateProject(window))
                        {
                            MessageBox(NULL, "Project Created!", "Success", NULL);
                        }
                        else
                        {
                            MessageBox(NULL, "Failed To Create Project!", "Error", NULL);
                        }
                    }
                    
                    break;
                }
                
                case APP_BTN_RESET:
                {
                    if(hWord == BN_CLICKED)
                    {
                        ResetForm(window);
                    }
                    
                    break;
                }
            }
            
            break;
        }
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        
        default:
        {
            return DefWindowProc(window, uMsg, wParam, lParam);
        }
    }
    
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char* className = "DevProjectBuilder";
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);
    
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    
    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Create main window
    HWND hwnd = CreateWindow(
                             className,
                             "Dev Project Builder",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             780, 530,
                             NULL, NULL, hInstance, NULL
                             );
    
    if (!hwnd)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}
// === End Main Project Functions ==========================================================================