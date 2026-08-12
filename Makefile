GODOT ?= godot

.DEFAULT_GOAL := help
.PHONY: help run editor import check test export-linux export-windows export-macos clean

help: ## Muestra dinámicamente los comandos disponibles y su finalidad
	@awk 'BEGIN {FS = ":.*## "; printf "Capybara Project · comandos disponibles\n\n"} /^[a-zA-Z0-9_-]+:.*## / {printf "  %-18s %s\n", $$1, $$2}' $(MAKEFILE_LIST)

run: import ## Importa recursos y ejecuta el juego
	$(GODOT) --path .

editor: ## Abre el proyecto en el editor de Godot
	$(GODOT) --editor --path .

import: ## Importa recursos y regenera la caché de clases de Godot
	$(GODOT) --headless --editor --path . --quit

check: import ## Comprueba que el proyecto carga y arranca sin errores
	$(GODOT) --headless --path . --quit-after 2

test: import ## Ejecuta la prueba funcional de gameplay
	$(GODOT) --headless --path . --scene res://tests/smoke_test.tscn

export-linux: import ## Genera la versión de lanzamiento para Linux
	mkdir -p build/linux
	$(GODOT) --headless --path . --export-release "Linux" build/linux/CapybaraProject.x86_64

export-windows: import ## Genera la versión de lanzamiento para Windows
	mkdir -p build/windows
	$(GODOT) --headless --path . --export-release "Windows Desktop" build/windows/CapybaraProject.exe

export-macos: import ## Genera la versión de lanzamiento para macOS
	mkdir -p build/macos
	$(GODOT) --headless --path . --export-release "macOS" build/macos/CapybaraProject.zip

clean: ## Elimina builds y cachés locales generados por Godot
	rm -rf -- build .godot
