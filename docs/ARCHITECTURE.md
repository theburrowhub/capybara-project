# Arquitectura Godot

## Escenas

```text
main.tscn
└── Main (Control, navegación y pantallas)
    └── game.tscn (instanciada al iniciar misión)
        ├── Starfield
        ├── Entities
        │   ├── Player (Area2D + SubViewport 3D)
        │   ├── Enemy[] (Area2D)
        │   ├── Projectile[] (Area2D)
        │   ├── PowerUp[] (Area2D)
        │   └── Explosion[] (Node2D)
        ├── WaveDirector
        ├── Music
        └── HUD (CanvasLayer)
```

## Responsabilidades

- `main.gd`: menús, opciones, récords, entrada de nombre y ciclo de vida de la partida.
- `game.gd`: score, niveles, spawns, jefe, pausa, game over y transición.
- `game_config.gd`: datos inmutables de enemigos, proyectiles, dificultades y fases.
- `player.gd`: input, movimiento, armas, escudo/energía y render del GLB.
- `enemy.gd`: stats, patrones, fuego y sprites prerenderizados de GLB.
- `projectile.gd`: homing, wave motion, colisión y daño.
- `wave_director.gd`: selección de fase, cadencia, formaciones y boss trigger.
- `game_data.gd`: autoload para ajustes y récords en JSON.

## Flujo de señales

Las entidades no instancian otras entidades. Jugador y enemigos emiten `projectile_requested`; `Game` valida el límite global y crea el proyectil. Un enemigo emite `destroyed`; `Game` suma puntos, registra la baja, crea la explosión y evalúa el drop. Este diseño mantiene el acoplamiento bajo y permite probar entidades de forma aislada.

## Colisiones

| Capa | Contenido | Máscara principal |
|---:|---|---|
| 1 | jugador | enemigos, proyectiles enemigos, power-ups |
| 2 | enemigos | jugador, proyectiles del jugador |
| 4 | proyectiles del jugador | enemigos |
| 8 | proyectiles enemigos | jugador |
| 16 | power-ups | jugador |

## Assets 3D en gameplay 2D

El jugador mantiene un modelo 3D vivo porque sólo existe un viewport. Para los enemigos, el pipeline Blender genera PNG RGBA cenitales desde cuatro GLB. Así se conservan volumen y textura sin multiplicar el coste de render 3D por cada enemigo activo.
