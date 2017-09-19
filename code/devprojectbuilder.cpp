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

//==== String Functions ================================================================================
bool32 IsLetter(char Letter)
{
   return ((Letter > 64 && Letter < 91) || (Letter > 96 && Letter < 123));
}

bool32 IsSpecialCharacter(char Character)
{
    return (!IsLetter(Character) && (Character < 48 || Character > 57));
}

void ToUpperCase(char *Str)
{
    if(Str != NULL)
    {
        int i = 0;
        while(Str[i] != 0)
        {
            if(Str[i] >= 97 && Str[i] <= 122)
            {
                Str[i] = Str[i] - 32;
            }
			++i;
        }
    }
}

void ToLowerCase(char *Str)
{
    if(Str != NULL)
    {
        int i = 0;
        while(Str[i] != 0)
        {
            if(Str[i] >= 65 && Str[i] <= 90)
            {
                Str[i] = Str[i] + 32;
            }

			++i;
        }
    }
}

bool32 IsDriveSpecified(char *Path)
{
    bool32 Result = false;

    if(Path != NULL)
    {
        Result = (IsLetter(Path[0]) && Path[1] == 58);
    }

    return Result;
}

void StripSpaces(char *Str, int SizeOfStr)
{
    if(Str != NULL)
    {
        size_t Len = strcspn(Str, "\0");
        char NewStr[256];
        int NewStrIndex = 0;
        for(int i = 0; i < Len; ++i)
        {
            if(Str[i] != 32)
            {
                NewStr[NewStrIndex] = Str[i];
                NewStrIndex++;
            }
        }

        if(NewStrIndex < Len)
        {
            NewStr[NewStrIndex] = 0;
            _snprintf_s(Str, Len, Len, "%s", NewStr);
        }
    }
}

void InsertSubString(char *Str, int SizeOfStr, int StartReplaceIndex, char *SubStr, int SizeOfSubStr)
{
    if(Str != NULL && SubStr != NULL)
    {
        size_t StrLen = strcspn(Str, "\0");
        size_t SubStrLen = strcspn(SubStr, "\0");

		if ((StrLen + SubStrLen) < SizeOfStr)
		{
            char NewStr[256];
            _snprintf_s(NewStr, sizeof(NewStr), StartReplaceIndex, "%s", Str);
			_snprintf_s(NewStr, sizeof(NewStr), "%s%s", NewStr, SubStr);
            
            StartReplaceIndex += 2;
            int NewStrIndex = (int)(StartReplaceIndex + SubStrLen);
            bool32 EndOfStr = false;
            while(!EndOfStr)
            {
                EndOfStr = (Str[StartReplaceIndex] == 0);
                NewStr[NewStrIndex] = Str[StartReplaceIndex];
                ++NewStrIndex;
                ++StartReplaceIndex;
            }

            _snprintf_s(Str, SizeOfStr, SizeOfStr, "%s", NewStr);
		}
    }
}

void AppendString(char *Str, int SizeOfStr, char *AppendStr, int StartIndexOfAppendStr)
{
    if(Str != NULL)
    {
        char NewStr[256];
        _snprintf_s(NewStr, sizeof(NewStr), "%s", Str);
        size_t EndOfStr = strcspn(NewStr, "\n");
        
        if(EndOfStr <= 0)
        {
            EndOfStr = strcspn(NewStr, "\0");
        }

        int i = StartIndexOfAppendStr;
        bool32 Done = false;

        while(!Done)
        {
            NewStr[EndOfStr] = AppendStr[i];

            if(AppendStr[i] == '\0')
            {

                Done = true;
            }

            ++i;
            ++EndOfStr;
        }

        _snprintf_s(Str, SizeOfStr, SizeOfStr, "%s", NewStr);
    }
}

void StrSpecifierSub(char *Str, int SizeOfStr, project_details *Details)
{
    if(Str != NULL)
    {
        bool32 EndOfStr = false;
        int i = 0;

        while(!EndOfStr)
        {
            if(Str[i] == 123 && (((i+2) < SizeOfStr) && Str[i+2] == 125)) // if {*}
            {
                char Identifier = Str[i+1];
                switch(Identifier)
                {
                    case 'r': //root path
                    {
                        InsertSubString(Str, SizeOfStr, i, Details->Paths.RootPath, sizeof(Details->Paths.RootPath));
                    }break;
                    case 'p': // project path
                    {
                        InsertSubString(Str, SizeOfStr, i, Details->Paths.ProjectPath, sizeof(Details->Paths.ProjectPath));
                    }break;
                    case 'f': // path to cpp file
                    {
                        char NewStr[256];
                        _snprintf_s(NewStr, sizeof(NewStr), "%s%s.cpp", Details->Paths.CodePath, Details->ProjectFileNameLower);
                        InsertSubString(Str, SizeOfStr, i, NewStr, sizeof(NewStr));
                    }break;
                    case 'h': // path to header file
                    {
                        char NewStr[256];
                        _snprintf_s(NewStr, sizeof(NewStr), "%s%s.h", Details->Paths.CodePath, Details->ProjectFileNameLower);
                        InsertSubString(Str, SizeOfStr, i, NewStr, sizeof(NewStr));
                    }break;
                }
            }

            ++i;

            EndOfStr = (Str[i] == '\0' || i >= SizeOfStr);
        }
    }
}
// === End of String Functions ==================================================================

// === File Functions ===========================================================================
void GetShortCutInfo(cmd_shortcut_info *Info, project_details *Details)
{
    Info->ObjPath = "C:\\Windows\\System32\\cmd.exe";
    _snprintf_s(Info->Description, sizeof(Info->Description), "CMD and Editor Launcher for project %s", Details->ProjectName);
    _snprintf_s(Info->Arguments, sizeof(Info->Arguments), "/k %sstartup.bat", Details->Paths.MiscPath);
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
    _snprintf_s(HeaderFileNameBuffer, sizeof(HeaderFileNameBuffer), "%s%s.h", Details->Paths.CodePath, Details->ProjectFileNameLower);
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
    _snprintf_s(CPPFileNameBuffer, sizeof(CPPFileNameBuffer), "%s%s.cpp", Details->Paths.CodePath, Details->ProjectFileNameLower);
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

void CreateProjectFiles(project_details *Details)
{
    printf_s(" Creating Header File (%s.h)... ", Details->ProjectFileNameLower);
    if(CreateHeaderFile(Details))
    {
        printf_s("Success!\n");
    }
    else
    {
        printf_s("Failed!\n");
    }
    
    printf_s(" Creating .cpp File (%s.cpp)... ", Details->ProjectFileNameLower);
    if(CreateCPPFile(Details))
    {
        printf_s("Success!\n");
    }
    else
    {
        printf_s("Failed!\n");
    }
}

bool32 CreateStartupFile(project_details *Details)
{
    char FileNameAndPath[MAX_PATH];
    _snprintf_s(FileNameAndPath, sizeof(FileNameAndPath), "%sstartup.bat", Details->Paths.MiscPath);

    FILE *File;
    errno_t Error = fopen_s(&File, FileNameAndPath, "w");

    if(!Error)
    {
        fprintf(File, "%s", "@echo off\n");
        fprintf(File, "%s %s%s %s%s", "subst", &Details->SubStDriveLetter, ":", Details->Paths.RootPath, "\n");
        fprintf(File, "%s %s%s", "call", Details->CompilerPath, "\n");
        fprintf(File, "%s%s%s", "set path=", Details->Paths.SubStRootPath, ";%path%\n");
        fprintf(File, "%s", Details->IDECommand);
        fclose(File);
    }

    return !Error;
}

bool32 CreateBuildFile(project_details *Details)
{
    char FileNameAndPath[MAX_PATH];
    _snprintf_s(FileNameAndPath, sizeof(FileNameAndPath), "%sbuild.bat", Details->Paths.CodePath);

    FILE *File;
    errno_t Error = fopen_s(&File, FileNameAndPath, "w");
	
    if(!Error)
    {
        fprintf(File, "%s", "@echo off\n\n");
        fprintf(File, "%s%s%s", "set RootPath=", Details->Paths.ProjectPath, "\n");
        fprintf(File, "%s%s%s", "set CommonCompilerFlags=", Details->CompilerFlags, "\n");
        fprintf(File, "%s%s%s", "set CommonLinkerFlags=", Details->LinkerFlags, "\n\n");
        fprintf(File, "%s", "IF NOT EXIST %RootPath%\\build mkdir %RootPath%\\build\n");
        fprintf(File, "%s", "pushd %RootPath%\\build\n");
        fprintf(File, "%s", "REM 32bit build\n");
        fprintf(File, "%s%s%s", "REM cl %CommonCompilerFlags% %RootPath%\\code\\", Details->ProjectFileNameLower, ".cpp /link -subsystem:windows,5.10 %CommonLinkerFlags%\n\n");
        fprintf(File, "%s", "REM 64bit build\n");
        fprintf(File, "%s", "del *.pdb > NUL 2> NUL\n");
        fprintf(File, "%s", "REM If you want to compile a DLL\n");
        fprintf(File, "%s", "REM cl %CommonCompilerFlags% %RootPath%\\code\\DLLNAMEHERE.cpp -FmNAMEHERE.map -LD /link -incremental:no -PDB:NAMEHERE.pdb -EXPORT:FUNCTIONEXPORTHERE\n");
        fprintf(File, "%s%s%s%s%s", "cl %CommonCompilerFlags% %RootPath%\\code\\", Details->ProjectFileNameLower, ".cpp /Z7 -Fm", Details->ProjectFileNameLower, ".map /link %CommonLinkerFlags%\n");
        fprintf(File, "%s", "popd");
        fclose(File);
    }

    return !Error;
}

// NOTE: Passing file name just in case I want to set up multiple defaults
bool32 SaveDefaults(default_inputs *Defaults, char *FileName)
{
    bool32 Result = false;

    if(Defaults != NULL && FileName != NULL)
    {
        FILE *File;
        errno_t Error = fopen_s(&File, FileName, "wb");

        if(!Error)
        {
            fwrite(Defaults, sizeof(default_inputs), 1, File);
            fclose(File);
            Result = true;
        }
    }

    return Result;
}

bool32 GetDefaultsFromFile(default_inputs *Defaults, char *FileName)
{
    bool32 Result = false;

    FILE *File;
    errno_t Error = fopen_s(&File, FileName, "rb");
    if(!Error)
    {
        fread(Defaults, sizeof(default_inputs), 1, File);
        fclose(File);
        Result = true;
    }

    return Result;
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
    int Result = 0; //0 - error, 1 - success, EEXIST = 17 - exists
    
    int MkDirResult = _mkdir(Path);

    if(MkDirResult == 0 || errno == EEXIST)
    {
        if(MkDirResult == -1 && errno == EEXIST)
        {
            Result = EEXIST;
        }
        else
        {
            Result = 1;
        }
    }

    return Result;
}

bool32 RemoveDir(char *Path)
{
    bool32 Result = false;

    if(_rmdir(Path))
    {
        printf_s(" Path %s has been deleted...\n", Path);
        Result = true;
    }
    else
    {
        printf_s(" Error deleting path %s...\n", Path);
    }

    return Result;
}

bool32 CreateProjectDirectories(project_paths *Paths)
{
    //NOTE: I have a feeling this function can be done better
    bool32 Result = false;

    bool32 RootCreated = false;
    bool32 ProjectCreated = false;
    bool32 CodeCreated = false;

    int CreateResult;

    printf_s(" Creating directory: %s... ", Paths->RootPath);
    CreateResult = CreateDir(Paths->RootPath);
    if(CreateResult)
    {
        if(CreateResult != EEXIST)
        {
            printf_s("Success!\n");
            RootCreated = true;
        }
        else
        {
            printf_s("Already Exists!\n");
        }
        
        printf_s(" Creating directory: %s... ", Paths->ProjectPath);
        CreateResult = CreateDir(Paths->ProjectPath);
        if(CreateResult)
        {
            if(CreateResult != EEXIST)
            {
                printf_s("Success!\n");
                ProjectCreated = true;
            }
            else
            {
                printf_s("Already Exists!\n");
            }

            printf_s(" Creating directory: %s... ", Paths->CodePath);
            CreateResult = CreateDir(Paths->CodePath);
            if(CreateResult)
            {
                if(CreateResult != EEXIST)
                {
                    printf_s("Success!\n");
                    CodeCreated = true;
                }
                else
                {
                    printf_s("Already Exists!\n");
                }

                printf_s(" Creating directory: %s... ", Paths->MiscPath);
                CreateResult = CreateDir(Paths->MiscPath);
                printf_s((CreateResult != EEXIST)?"Success!\n" : "Already Exists!\n");
                Result = true;
            }
            else
            {
                printf_s("Failed!\n");
            }
        }
        else
        {
            printf_s("Failed!\n");
        }
    }
    else
    {
        printf_s("Failed!\n");
    }


    if(!Result)
    {
        printf_s(" !!!Error creating directories, removing any directories created...\n");
        
        if(RootCreated)
        {
            RemoveDir(Paths->RootPath);
        }
    
        if(ProjectCreated)
        {
            RemoveDir(Paths->ProjectPath);
        }
    
        if(CodeCreated)
        {
            RemoveDir(Paths->CodePath);
        }
    }

    return Result;
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
    _snprintf_s(Details->Paths.ProjectPath, sizeof(Details->Paths.ProjectPath), "%s%s\\", Details->Paths.RootPath, Details->ProjectFolderName);
    _snprintf_s(Details->Paths.CodePath, sizeof(Details->Paths.CodePath), "%scode\\", Details->Paths.ProjectPath);
    _snprintf_s(Details->Paths.MiscPath, sizeof(Details->Paths.MiscPath), "%smisc\\", Details->Paths.ProjectPath);
    _snprintf_s(Details->Paths.SubStRootPath, sizeof(Details->Paths.SubStRootPath), "%s:\\%s", Details->SubStDriveLetter, Details->ProjectFolderName);
}

void PrintPaths(project_paths *Paths)
{
    printf_s("Project Paths...\n");
    printf_s("%s\n%s\n%s\n%s\n\n", Paths->RootPath, Paths->ProjectPath, Paths->CodePath, Paths->MiscPath);
}

void PrintProjectDetails(project_details *Details)
{
    printf_s("\n>>> Project Details <<<\n");
    printf_s("Project Name: %s\n", Details->ProjectName);
    printf_s("Root Path: %s\n", Details->Paths.RootPath);
	printf_s("Project Path: %s\n", Details->Paths.ProjectPath);
    printf_s("Compiler: %s\n", Details->CompilerPath);
    printf_s("Compiler Flags: %s\n", Details->CompilerFlags);
    printf_s("Linker Flags: %s\n", Details->LinkerFlags);
    printf_s("Editor command: %s\n", Details->IDECommand);
    printf_s("Subst Drive Letter: %s\n", Details->SubStDriveLetter);
    printf_s("Subst Mapping: %s will be mapped to %s\n\n", Details->Paths.ProjectPath, Details->Paths.SubStRootPath);
    printf_s("The following directories will be created based on your inputs:\n");
    PrintPaths(&Details->Paths);
}

void PrintDefaultDetails(default_inputs *Defaults)
{
    printf_s("\n>>> Default Details <<<\n");
    printf_s("Root Path: %s\n", Defaults->RootPath);
    printf_s("Compiler Path: %s\n", Defaults->Compiler);
    printf_s("Compiler Flags: %s\n", Defaults->CompilerFlags);
    printf_s("Linker Flags: %s\n", Defaults->LinkerFlags);
    printf_s("Editor Command Line: %s\n", Defaults->EditorCMD);
}

void DisplayHelp(void)
{
    printf_s("\n>>> Help\n");
    printf_s("This part still needs to be completed!!!\n");
}
// === End Util Functions ================================================================================

// === Question and Input Function =======================================================================
void AskQuestion(char *Question, char *Detail, int SizeOfDetail)
{
    bool32 Answered = false;
    char *Answer;

    do
    {
        printf_s(Question);
        Answer = fgets(Detail, SizeOfDetail, stdin);
        if(Answer != NULL && Answer[0] != '\n')
        {
            Answered = true;
        }
    }
    while(!Answered);
}

void AskQuestion(char *QuestionToAsk, int SizeOfQuestion, char *Detail, int SizeOfDetail, char *DefaultAnswer)
{
    bool32 HasDefault = (DefaultAnswer != NULL && DefaultAnswer[0] != '\0');
    bool32 Answered = false;
    char Question[256];
    char *FormatStr = (HasDefault) ? "%s (Default: %s): " : "%s: ";

    _snprintf_s(Question, sizeof(Question), FormatStr, QuestionToAsk, DefaultAnswer);

    do
    {
        printf_s(Question);
        Detail = fgets(Detail, SizeOfDetail, stdin);

        if(Detail != NULL && Detail[0] != '\n')
        {
            Answered = true;
        }
        else if(HasDefault)
        {
            _snprintf_s(Detail, SizeOfDetail, SizeOfDetail, "%s", DefaultAnswer);
            Answered = true;
        }

    }
    while(!Answered);
}

void GetProjectName(char *ProjectName, int SizeOfProjectName)
{
    bool32 Answered = false;
    size_t NewlineIndex = 0;
    
    do
    {
        AskQuestion("Enter project name: ", ProjectName, SizeOfProjectName);
        NewlineIndex = strcspn(ProjectName, "\n");
        
        if(NewlineIndex > 1)
        {
            if(!IsSpecialCharacter(ProjectName[0]))
            {
                Answered = true;
            }
        }

    }while(!Answered);

    ProjectName[NewlineIndex] = 0;
} 

void GetRootPath(char *Detail, int SizeOfDetail, char *Default, int SizeOfDefault)
{
    bool32 Answered = false;
    size_t NewlineIndex = NULL;

    char Question[256] = "Enter root path";

    while(!Answered)
    {
        AskQuestion(Question, sizeof(Question), Detail, SizeOfDetail, Default);
		NewlineIndex = strcspn(Detail, "\n");

        if(NewlineIndex >= 2) // at least has a drive letter and ':'...  'C:' should be acceptable
        {
            if(IsDriveSpecified(Detail)) // does the path start with <a-z>|<A-Z>:
            {
                if(Detail[NewlineIndex - 1] != 92 && Detail[NewlineIndex - 1] != 47)
                {
                    Detail[NewlineIndex] = 92; // back slash
					++NewlineIndex;
					if (NewlineIndex < SizeOfDetail)
					{
						Detail[NewlineIndex] = 0;
					}
                }
                else
                {
                    Detail[NewlineIndex] = 0;
                }

                Answered = true;
            }
            else if(Default != NULL && Default[0] != '\0' && strcspn(Detail, "{a}") == 0)
            {
				Detail[NewlineIndex] = 92;
				++NewlineIndex;
				if (NewlineIndex < SizeOfDetail)
				{
					Detail[NewlineIndex] = 0;
                }
                char NewStr[256];
                _snprintf_s(NewStr, sizeof(NewStr), "%s", Default);
                AppendString(NewStr, sizeof(NewStr), Detail, 3);
                _snprintf_s(Detail, SizeOfDetail, SizeOfDetail, "%s", NewStr);
                Answered = true;
            }
        }
    }
}

void GetSubStrDriveLetter(char *SubSt, char *Default)
{
    bool32 Answered = false;
    size_t NewLineIndex = NULL;
    
    char Question[256] = "Enter subst drive letter";
    char Answer[256];

    while(!Answered)
    {
        AskQuestion(Question, sizeof(Question), Answer, sizeof(Answer), Default);
        NewLineIndex = strcspn(Answer, "\n");

        StripSpaces(Answer, sizeof(Answer));

        if(NewLineIndex != 0 && (Answer[1] == '\n' || Answer[1] == ' ' || Answer[1] == ':' || Answer[1] == '\0'))
        {
            if(IsLetter(Answer[0]))
            {
                SubSt[0] = Answer[0];
                SubSt[1] = 0;
                Answered = true;
            }
        }
    }
}

void GetCompilerPath(char *Detail, int SizeOfDetail, char *Default)
{
    bool32 Answered = false;
    size_t NewlineIndex = NULL;

    char Question[256] = "Enter path to compiler";

    while(!Answered)
    {
        AskQuestion(Question, sizeof(Question), Detail, SizeOfDetail, Default);
        NewlineIndex = strcspn(Detail, "\n");
        
        if(NewlineIndex >= 2)
        {
            if(IsDriveSpecified(Detail))
            {
                Answered = true;
            } 
        }
    }

    Detail[strcspn(Detail, "\n")] = 0;

}

void GetIDECMLCommand(char *Detail, int SizeofDetail, char *Default)
{
    char Question[256] = "Enter editor's command line command";
    AskQuestion(Question, sizeof(Question), Detail, SizeofDetail, Default);
    Detail[strcspn(Detail, "\n")] = 0;
}

void GetCompilerFlags(char *Detail, int SizeOfDetail, char *Default)
{
    char Question[256] = "Enter Compiler Flags";
    AskQuestion(Question, sizeof(Question), Detail, SizeOfDetail, Default);
    Detail[strcspn(Detail, "\n")] = 0;
    if(Default != NULL && Default[0] != '\0' && strcspn(Detail, "{a}") == 0)
    {
        char NewStr[256];
        _snprintf_s(NewStr, sizeof(NewStr), "%s", Default);
        AppendString(NewStr, sizeof(NewStr), Detail, 3);
        _snprintf_s(Detail, SizeOfDetail, SizeOfDetail, "%s", NewStr);
    }
}

void GetLinkerFlags(char *Detail, int SizeOfDetail, char *Default)
{
    char Question[256] = "Enter Linker Flags";
    AskQuestion(Question, sizeof(Question), Detail, SizeOfDetail, Default);
    Detail[strcspn(Detail, "\n")] = 0;
    if(Default != NULL && Default[0] != '\0' && strcspn(Detail, "{a}") == 0)
    {
        char NewStr[256];
        _snprintf_s(NewStr, sizeof(NewStr), "%s", Default);
        AppendString(NewStr, sizeof(NewStr), Detail, 3);
        _snprintf_s(Detail, SizeOfDetail, SizeOfDetail, "%s", NewStr);
    }
}

void AskStartupQuestions(project_details *Details, default_inputs *Defaults)
{
    GetProjectName(Details->ProjectName, sizeof(Details->ProjectName));
    
    SetProjectFileNames(Details);
    
    GetRootPath(Details->Paths.RootPath, sizeof(Details->Paths.RootPath), Defaults->RootPath, sizeof(Defaults->RootPath));
    GetSubStrDriveLetter(Details->SubStDriveLetter, Defaults->SubStDriveLetter);
    
    SetPaths(Details);

    GetCompilerPath(Details->CompilerPath, sizeof(Details->CompilerPath), Defaults->Compiler);
    GetIDECMLCommand(Details->IDECommand, sizeof(Details->IDECommand), Defaults->EditorCMD);
    StrSpecifierSub(Details->IDECommand, sizeof(Details->IDECommand), Details);
}

void AskBuildQuestions(project_details *Details, default_inputs *Defaults)
{
    GetCompilerFlags(Details->CompilerFlags, sizeof(Details->CompilerFlags), Defaults->CompilerFlags);
    GetLinkerFlags(Details->LinkerFlags, sizeof(Details->LinkerFlags), Defaults->LinkerFlags);
}

void AskDefaultQuestions(default_inputs *Defaults)
{
    GetRootPath(Defaults->RootPath, sizeof(Defaults->RootPath), NULL, 0);
    GetSubStrDriveLetter(Defaults->SubStDriveLetter, NULL);
    GetCompilerPath(Defaults->Compiler, sizeof(Defaults->Compiler), NULL);
    GetIDECMLCommand(Defaults->EditorCMD, sizeof(Defaults->EditorCMD), NULL);
    GetCompilerFlags(Defaults->CompilerFlags, sizeof(Defaults->CompilerFlags), NULL);
    GetLinkerFlags(Defaults->LinkerFlags, sizeof(Defaults->LinkerFlags), NULL);
}
// === End Questions and Inputs Functions ==============================================================================

// === Main Project Functions ==========================================================================================
int GetProjectConfirmation(project_details *Details)
{
    int Result = 0;
    bool32 Answered = false;
    char Input[256];

    PrintProjectDetails(Details);

    while(!Answered)
    {
        printf_s(">>> Build Project?\n  1 -Yes\n  2 -No, Redo\n  0 -No, Return to Main Menu\n\n> ");
		if(fgets(Input, sizeof(Input), stdin))
		{
            size_t InputCount = strcspn(Input, "\n");

            if(InputCount == 1)
            {
                char Answer = Input[0];
                Result = Answer - 48;
                if(Result >= 0 && Result <= 2)
                {
                    Answered = true;
                }
            }
		}
    }

    return Result;
}

int GetDefaultConfirmation(default_inputs *Defaults)
{
    int Result = 0;
    bool32 Answered = false;
    char Input[256];

    PrintDefaultDetails(Defaults);

    while(!Answered)
    {
        printf_s("\n>>> Save Defaults?\n  1 -Yes\n  2 -No, Redo\n  0 -No, Return to Defaults Menu\n\n> ");
		if(fgets(Input, sizeof(Input), stdin))
		{
            size_t InputCount = strcspn(Input, "\n");

            if(InputCount == 1)
            {
                char Answer = Input[0];
                Result = Answer - 48;
                if(Result >= 0 && Result <= 2)
                {
                    Answered = true;
                }
            }
		}
    }

    return Result;
}

void CreateProject(project_details *ProjectDetails, default_inputs *Defaults)
{
    bool32 Redo = true;
    int ProjectOption = 0;

    printf_s("\n>>> Create A Project <<<\n");
    
    do
    {
        AskStartupQuestions(ProjectDetails, Defaults);
        AskBuildQuestions(ProjectDetails, Defaults);

        ProjectOption = GetProjectConfirmation(ProjectDetails);
        if(ProjectOption != 2)
        {
            Redo = false;
        }
    }while(Redo);

    if(ProjectOption == 1)
    {
        printf_s("\n>>> Creating Project...\n");
        printf_s("Creating Directories...\n");
        CreateProjectDirectories(&ProjectDetails->Paths);
    
        printf_s("Creating Project Files...\n");
        CreateProjectFiles(ProjectDetails);

        printf_s("Creating .Bat Files...\n");
        printf_s(" Creating Startup Bat File... ");
        if(CreateStartupFile(ProjectDetails))
        {
            printf_s("Success!\n");
        }
        else
        {
            printf_s("Failed!\n");
        }

        printf_s(" Creating Build Bat File... ");
        if(CreateBuildFile(ProjectDetails))
        {
            printf_s("Success!\n");
        }
        else
        {
            printf_s("Failed!\n");
        }
        
        printf_s("Creating CMD Shortcut To Desktop... ");
    
        if(SUCCEEDED(CreateLauncherCMDShortcut(ProjectDetails)))
        {
            printf_s("Success!\n");
        }
        else
        {
            printf_s("Failed!\n");
        }

        printf_s("\nCompleted Creating Project!\n\n");
    }
}

bool32 CreateDefaults(default_inputs *Defaults, char *DefaultsFileName)
{
    bool32 Result = false;
    bool32 Redo = true;
    int DefaultOption = 0;
    
    printf_s("\n>>> Create Defaults <<<\n");

    do
    {
        AskDefaultQuestions(Defaults);

        //NOTE: Options: 1 - Save, 2 - Redo, 0 = Quit
        DefaultOption = GetDefaultConfirmation(Defaults);
        if(DefaultOption != 2)
        {
            Redo = false;
        }
    }while(Redo);

    if(DefaultOption == 1)
    {
        printf_s("\n>>> Saving Defaults... ");
        if(SaveDefaults(Defaults, DefaultsFileName))
        {
            printf_s("Success!\n");
            Result = true;
        }
        else
        {
            printf_s("Failed!");
        }
    }

    return Result;
}

bool32 DeleteDefaults(default_inputs *Defaults, char *DefaultFileName)
{
    *Defaults = {};
    return !(remove(DefaultFileName));
}

void DefaultsMenu(default_inputs *Defaults, bool32 *HasDefaults, char *DefaultsFileName)
{
	char OptionBuffer[256] = { 0 };
	bool32 Running = true;
	char Input = 0;

    do
    {
        printf_s("\n>>> Defaults Options:\n");
        if(*HasDefaults)
        {
            printf_s("  1 : Change\n");
            printf_s("  2 : View\n");
            printf_s("  3 : Delete\n");
        }
        else
        {
            printf_s("  1 : Create\n");
        }
        printf_s("  0 : Main Menu\n\n");

        bool32 Answered = false;

        do
        {
            printf_s("> ");
            if(fgets(OptionBuffer, sizeof(OptionBuffer), stdin))
            {
                StripSpaces(OptionBuffer, sizeof(OptionBuffer));
                size_t BufferLength = strcspn(OptionBuffer, "\n");

                if(BufferLength == 1)
                {
                    Input = OptionBuffer[0];

                    switch(Input)
                    {
                        case '1':
                        {
                            if(CreateDefaults(Defaults, DefaultsFileName))
                            {
                                *HasDefaults = true;
                                Answered = true;
                            }
                        }break;
                        case '2':
                        {
                            if(*HasDefaults)
                            {
                                PrintDefaultDetails(Defaults);
                                Answered = true;
                            }
                        }break;
                        case '3':
                        {
                            if (*HasDefaults && DeleteDefaults(Defaults, DefaultsFileName))
                            {
                                *HasDefaults = false;
                                Answered = true;
                            }
                        }break;
                        case '0':
                        {
                            Running = false;
                            Answered = true;
                        }break;
                    }
                }
            }
        }while(!Answered);
    }while(Running);
}

void MainMenu(project_details *Details, default_inputs *Defaults, bool32 *IsDefaultsSet, char *DefaultsFileName)
{
    char OptionBuffer[256] = {0};
    bool32 Running = true;
    char Input = 0;

    do
    {
        printf_s("========= Main Menu =========\n\n");
        printf_s(">>> Options:\n");
        printf_s("  1 : Create Project\n");
        printf_s("  2 : Defaults\n");
        printf_s("  3 : Help\n");
        printf_s("  0 : Quit\n\n");
        
        bool32 Answered = false;

        do
        {
            printf_s("> ");
            if(fgets(OptionBuffer, sizeof(OptionBuffer), stdin))
            {
                StripSpaces(OptionBuffer, sizeof(OptionBuffer));
                size_t BufferLength = strcspn(OptionBuffer, "\n");

                if(BufferLength == 1)
                {
                    Input = OptionBuffer[0];
                    switch(Input)
                    {
                        case '1': // Create Project
                        {
                            CreateProject(Details, Defaults);
                            Answered = true;
                        }break;
                        case '2': // Defaults Menu
                        {
                            DefaultsMenu(Defaults, IsDefaultsSet, DefaultsFileName);
                            Answered = true;
                        }break;
                        case '3': // Help
                        {
                            DisplayHelp();
                            Answered = true;
                        }break;
                        case '0': // Quit
                        {
                            Running = false;
                            Answered = true;
                        }
                    }
                }
            }
        }while(!Answered);
    }while(Running);
}

int main(void)
{
    //TODO: Finish Help display.
    project_details ProjectDetails = {};
    default_inputs Defaults = {};
    char *DefaultsFileName = "devprojectbulider.dft";
    bool32 HasDefaults = GetDefaultsFromFile(&Defaults, DefaultsFileName);

    printf_s(":::: :: Development Project Builder :: ::::\n\n");

    MainMenu(&ProjectDetails, &Defaults, &HasDefaults, DefaultsFileName);
    
    return 0;
}
// === End Main Project Functions ==========================================================================