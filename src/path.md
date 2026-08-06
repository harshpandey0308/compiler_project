"C:\\MinGW\\bin\\gcc.exe"


"type": "cppbuild",
            "label": "Build Compiler GUI",
            "command": "C:/raylib/w64devkit/bin/gcc.exe",
            "args": [
                "-fdiagnostics-color=always",
                "${workspaceFolder}/src/gui/gui_main.c",
                "-o",
                "${workspaceFolder}/src/gui/compiler_gui.exe",
                "-IC:/raylib/raylib/src",
                "-LC:/raylib/raylib/src",
                "-IC:/raylib/raylib/examples/core",
                "-IC:/Users/USER/Documents/course/compiler_project/src",
                "-lraylib",
                "-lopengl32",
                "-lgdi32",
                "-lwinmm"
            ],
            "options": {
                "cwd": "${workspaceFolder}",
                "env": {
                    "PATH": "C:\\raylib\\w64devkit\\bin;${env:PATH}"
                }
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }