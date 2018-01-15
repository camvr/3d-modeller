# Linux (default)
LDFLAGS = -lGL -lGLU -lglut
CFLAGS=-g -Wall -std=c++11
CC=g++
EXEEXT=
RM=rm

# Windows (cygwin)
ifeq "$(OS)" "Windows_NT"
	EXEEXT=.exe #on windows applications must have .exe extension
	RM=del #rm command for windows powershell
    LDFLAGS =  -lfreeglut -lglu32 -lopengl32
else
	# OS X
	OS := $(shell uname)
	ifeq ($(OS), Darwin)
	    LDFLAGS = -framework Carbon -framework OpenGL -framework GLUT
	endif
endif

# Name of the application
PROGRAM_NAME=3dModeller

# Run target to compile and build, and then launch the executable
run: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)$(EXEEXT)

# Additional source files
$(PROGRAM_NAME): main.o mesh/mesh.o mesh/vertex.o mesh/face.o mesh/edge.o tool/tool.o interface/hud.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) *.o **/*.o $(PROGRAM_NAME)$(EXEEXT)
