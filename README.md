DevProjectBuilder
=======

A Windows console application to create and setup development environment for a new project.

Overview
--------
The application takes in a few details about the project; from those details creates the directories, development file stubs, startup.bat, build.bat and a CMD shortcut which runs the startup.bat file.

The user has the ability to create/set defaults for those commonly used environment details.

Installation
------------
Pull down the project and build. If building from the build.bat file some tweaks will need to be made to the startup.bat and build.bat to match your environment.  Run the startup.bat file first as it runs the subst command to associate the project root path to the V drive which the build.bat file uses.

Usage:
-------
Run the generated exe file and follow the prompts to create a project.

When creating a project without defaults set just follow the prompts and fill in the values for your project.

When creating a project with defaults set...
 - If a default is set for a question, the prompt will show the default.
 - Pressing enter will use the default.
 - A value entered will be used instead of the default value.
 - Using the {a} specifier followed by text will append the value to the end of the default value.  This applies to only few questions.

String replacement specifiers, if any of the specifiers listed below are placed in some of the inputs they will be replaced with the string value they represent.  These are good for when you set your default for the CMD of your editor (Example: code -n {p} and the project path is "C:\Dev\MyProject" will translate to code -n C:\Dev\MyProject).

{r} - Replaced with the root path provided by the user or set as default.

{p} - Replaced with the project path.

{f} - Replaced with the path to the generated project cpp file.

{h} - Replaced with the path to the generated project header file.

{a} - Append. Specifies that the following input gets appened to the default value. Example: Enter root path (Default: C:\Dev\): {a}MyProject "enter", the root path will be C:\Dev\MyProject

Parameters
----------
There are currently no options to pass in parameters to the program in the command line.

Notes
-----
This is a tool I currently use to setup my C/C++ projects and it favors this language.

Yes, I know about string.h but what is the fun in that?...

Would like TODOs
----------------
- The ability to save multiple defaults files.
- The ability to pass args in the command line to quickly create projects
- Make it more generalized development and not so C/C++ focused.
- User specified file templates for generated project files.
- Multi-Platform (MacOS and Linux)

License
-------
MIT