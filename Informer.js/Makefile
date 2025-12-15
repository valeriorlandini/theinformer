# Makefile for compiling informer_bindings.cpp using emscripten (emcc)

# Define variables for common parts of the command
CC = emcc
CXXFLAGS = -I../Library --std=c++20 -O3 --no-entry
LDFLAGS = --bind
SOURCES = informer_js_wrapper.cpp
TARGET_JS = informer.js

# Emscripten-specific settings
EMSCRIPTEN_SETTINGS = \
	-s WASM=1 \
	-s MODULARIZE=1 \
	-s EXPORT_NAME="InformerModule" \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s NO_EXIT_RUNTIME=1 \
	-s ENVIRONMENT='web,webview,worker,node'

# --- Targets ---

.PHONY: all clean

# Default target: builds the informer.js and informer.wasm files
all: $(TARGET_JS)

# Rule to compile the source file into the JavaScript and WebAssembly output
$(TARGET_JS): $(SOURCES)
	@echo "Compiling $< to $@"
	$(CC) $(SOURCES) $(LDFLAGS) $(CXXFLAGS) $(EMSCRIPTEN_SETTINGS) -o $@

# Clean target: removes the generated output files
clean:
	@echo "Cleaning up build files..."
	rm -f $(TARGET_JS) $(TARGET_JS:.js=.wasm)