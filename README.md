# Capybara Project — Godot Edition

Shoot ’em up horizontal migrado a Godot 4. El juego conserva las dos campañas del prototipo original, las oleadas musicales, diez clases de enemigo, seis modos de arma, gestión de energía, power-ups, jefes y récords locales.

## Ejecutar

Requiere Godot 4.7 o compatible.

```bash
godot --editor --path .  # abrir el editor
godot --path .           # ejecutar el juego
```

También están disponibles:

```bash
make          # mostrar todos los comandos y su finalidad
make help     # equivalente explícito
make run
make editor
make check
make test
```

`make test` importa el proyecto en headless y ejecuta una prueba de humo de la escena de gameplay, el modelo GLB del jugador, enemigos, proyectiles y power-ups.

## Contenido migrado

- Dos niveles: **Initiation** (553,82 s) y **Escalation** (612,13 s).
- Oleadas por fases, incremento de velocidad y combate de jefe.
- Enemigos Grunt, Tank, Speeder, Zigzag, Bomber, Shield, Swarm, Elite, Ghost y Boss.
- Proyectiles laser, plasma, missile y energy orb.
- Armas Single, Double, Spread, Rapid, Charge y Dual.
- Modos de energía Offensive y Defensive; habilidad especial y regeneración.
- Power-ups de energía, escudo, casco y arma, con drop rate por enemigo.
- Menú, opciones de vídeo/audio, cuatro dificultades y récords por dificultad.
- Pausa, reinicio, fullscreen y visualización de hitboxes.

La nave del jugador usa `assets/models/player_ship.glb` en un `SubViewport` 3D transparente. Los cuatro GLB enemigos se conservan en `assets/models/enemies/` y se prerenderizan a sprites mediante `tools/render_glb_sprites.py`, evitando mantener decenas de viewports 3D durante el combate.

## Controles

| Acción | Control |
|---|---|
| Mover | WASD / flechas |
| Disparar | clic izquierdo / Space |
| Elegir arma | 1–6 |
| Ciclar arma | R durante el juego |
| Cambiar modo de energía | Q |
| Habilidad especial | mantener E |
| Pausa | P |
| Hitboxes | H |
| Fullscreen | F11 |
| Reiniciar tras game over | R |
| Volver al menú | Escape |

## Estructura

```text
project.godot                 Configuración de Godot
scenes/                       Escenas principales y entidades
scripts/core/                 Flujo del juego y configuración de datos
scripts/entities/             Jugador, enemigos, proyectiles y power-ups
scripts/systems/              Oleadas y starfield
scripts/ui/                   HUD y fondo de menú
scripts/autoload/             Ajustes y récords persistentes
assets/models/                GLB fuente
assets/sprites/               Sprites originales y prerenderizados
tools/render_glb_sprites.py   Pipeline Blender → PNG
tests/                        Prueba de humo y capturas visuales
```

El código C/Raylib bajo `src/` e `include/` permanece únicamente como referencia histórica de la migración; ya no forma parte del build ni del runtime.

Consulta [BUILDING.md](BUILDING.md), [docs/GAMEPLAY.md](docs/GAMEPLAY.md) y [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).
