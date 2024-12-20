CFLAGS    = -std=c++17 -Wall -g $(shell pkg-config --cflags glfw3 glm)
INCLUDES  = -Iinclude/ -Iinclude/imgui
LIBS   	  = $(shell pkg-config --libs glfw3 glm)
TARGET    = main

SRCDIR    = src
BUILDIR   = build
OBJS      = build/main.o build/glad.o

IMGUI_SRCDIR = $(SRCDIR)/imgui
IMGUI_BUILDIR = $(BUILDIR)/imgui
IMGUI_OBJS   = build/imgui/imgui.o build/imgui/imgui_draw.o build/imgui/imgui_impl_glfw.o build/imgui/imgui_demo.o build/imgui/imgui_impl_opengl3.o build/imgui/imgui_tables.o build/imgui/imgui_widgets.o
CXX = g++ 

all: $(TARGET) 

$(TARGET): $(OBJS) $(IMGUI_OBJS)
	$(CXX) $(CFLAGS) -o main $^ $(LIBS) $(INCLUDES) 

$(BUILDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDIR)
	$(CXX) $(CFLAGS) -c -o $@ $^ $(INCLUDES) 

$(BUILDIR)/glad.o: $(SRCDIR)/glad.c | $(BUILDIR)
	$(CXX) $(CFLAGS) -c -o $@ $^ $(INCLUDES)

$(IMGUI_BUILDIR)/%.o: $(IMGUI_SRCDIR)/%.cpp | $(IMGUI_BUILDIR)
	$(CXX) $(CFLAGS) -c -o $@ $^ $(INCLUDES)

$(BUILDIR):
	mkdir -p $(BUILDIR)

$(IMGUI_BUILDIR):
	mkdir -p $(IMGUI_BUILDIR)

clean:
	rm -rf $(TARGET) $(BUILDIR)

rebuild:
	rm -rf $(OBJS)
	make
