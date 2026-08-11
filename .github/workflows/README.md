# Godot CI/CD

- `build.yml` instala Godot 4.7.1, importa el proyecto y ejecuta `tests/smoke_test.tscn`.
- `release.yml` instala las plantillas oficiales y exporta Linux x86_64, Windows x86_64 y macOS universal.

Las releases se publican al crear una etiqueta `v*` o mediante `workflow_dispatch`.

