NAME := wiiu-hbas

MINIZIP_O   :=  zip.o ioapi.o unzip.o


INCLUDES := -Isrc -Ilibs/libgui-sdl/include/gui-sdl -Ilibs/get/src -Ilibs/get/src/libs/rapidjson/include -Ilibs/get/src/libs/minizip/ -Ilibs/get/src/libs/tinyxml/ -I/opt/homebrew/include
SOURCES :=  src libs/libgui-sdl/source libs/get/src
LIBS := -lz -lcurl -lSDL2_ttf -lSDL2_gfx -lSDL2_image -lSDL2_mixer -lmpg123 -lSDL2 -lSDL2main -lfreetype -L/opt/homebrew/lib

CPPFILES := $(shell find $(SOURCES) -name '*.cpp')

make all:
	gcc -c libs/get/src/libs/minizip/*.c
	$(CXX) $(INCLUDES) -o $(NAME).exe $(CPPFILES) $(MINIZIP_O) $(LIBS) -DPC -g -std=c++17

.PHONY: clean
clean:
	rm -f $(NAME).exe
