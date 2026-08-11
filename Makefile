GODOT ?= godot

.PHONY: run editor import check test export-linux export-windows export-macos clean

run:
	$(GODOT) --path .

editor:
	$(GODOT) --editor --path .

import:
	$(GODOT) --headless --editor --path . --quit

check: import
	$(GODOT) --headless --path . --quit-after 2

test: import
	$(GODOT) --headless --path . --scene res://tests/smoke_test.tscn

export-linux: import
	mkdir -p build/linux
	$(GODOT) --headless --path . --export-release "Linux" build/linux/CapybaraProject.x86_64

export-windows: import
	mkdir -p build/windows
	$(GODOT) --headless --path . --export-release "Windows Desktop" build/windows/CapybaraProject.exe

export-macos: import
	mkdir -p build/macos
	$(GODOT) --headless --path . --export-release "macOS" build/macos/CapybaraProject.zip

clean:
	rm -rf build .godot
