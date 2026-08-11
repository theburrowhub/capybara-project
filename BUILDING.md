# Build y exportación

## Requisitos

- Godot 4.7.x estándar (no hace falta la edición .NET).
- Plantillas de exportación de la misma versión para generar binarios.
- Blender 4+ sólo si se regeneran los sprites desde los GLB enemigos.

No se necesitan CMake, Raylib, SQLite ni un compilador C.

## Desarrollo

```bash
godot --editor --path .
godot --path .
```

La resolución lógica es 1200×600. Godot mantiene el aspect ratio al redimensionar la ventana.

## Validación

```bash
make check
make test
```

Equivalentes directos:

```bash
godot --headless --editor --path . --quit
godot --headless --path . --scene res://tests/smoke_test.tscn
```

## Exportar

Instala las plantillas desde `Editor > Manage Export Templates`, y ejecuta:

```bash
make export-linux
make export-windows
make export-macos
```

Los presets viven en `export_presets.cfg` y generan artefactos bajo `build/`.

## Regenerar sprites enemigos

Los GLB fuente están versionados. Para volver a producir los PNG transparentes:

```bash
/Applications/Blender.app/Contents/MacOS/Blender \
  --background \
  --python tools/render_glb_sprites.py -- \
  --input assets/models/enemies \
  --output assets/sprites/enemies_3d \
  --size 384
```

En Linux o Windows, sustituye la ruta al ejecutable de Blender.

## CI/CD

`.github/workflows/build.yml` importa y prueba el proyecto en cada cambio a `main-godot`. `.github/workflows/release.yml` exporta Linux, Windows y macOS desde una etiqueta `v*` o desde ejecución manual.

