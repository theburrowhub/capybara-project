# Page and Dependency Trees

## Main Menu and Ship Selection

Entry: `scenes/main.tscn`

Dependencies:
- `scripts/core/main.gd`
  - `scripts/ui/ship_selection_panel.gd`
    - `scripts/core/game_config.gd`
    - `scripts/ui/ship_showcase_preview.gd`
      - `assets/models/player_ship.glb`
      - `assets/models/player_ship_sting.glb`
      - `assets/models/player_ship_goliat.glb`
    - `scripts/ui/ship_radar_chart.gd`
  - `scenes/game.tscn`
    - `scripts/core/game.gd`
    - `scenes/entities/player.tscn`
      - `scripts/entities/player.gd`
  - Autoload: `scripts/autoload/game_data.gd`
- `scripts/ui/menu_background.gd`
- Boot/theme configuration: `project.godot`

Rendered branch:
- `Main._ready()` builds the persistent interface and shows the main menu.
- START MISSION calls `_show_ship_selection()`.
- `ShipSelectionPanel` owns selection, keyboard/mouse navigation, live preview, radar, and DEPLOY/BACK signals.
- DEPLOY calls `Main._start_game(ship_id)`; the selected profile reaches the player before `PlayerShip._ready()`.

## Gameplay

Entry: `scenes/game.tscn`

Dependencies:
- `scripts/core/game.gd`
  - `scripts/core/game_config.gd`
  - `scripts/systems/starfield.gd`
  - `scripts/systems/wave_director.gd`
  - `scripts/ui/hud.gd`
- `scenes/entities/player.tscn`
  - `scripts/entities/player.gd`
  - all three player GLBs listed above
