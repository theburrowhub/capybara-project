# Route and Screen Map

Godot uses scenes and code-driven panels rather than URL routing.

## Boot route

- Scene: `res://scenes/main.tscn`
- Controller: `scripts/core/main.gd`
- Persistent layout: animated background, product header, dynamic panel host, controls footer.

## Menu panels

- Main menu: `_show_main_menu()` → `_main_menu_panel()`
- Ship selection: `_show_ship_selection()` → `ShipSelectionPanel`; opened by START MISSION.
- Options: `_options_panel()`
- High scores: `_scores_panel()`
- Credits: `_credits_panel()`
- High-score name entry: `_name_panel()`

## Gameplay route

- Scene: `res://scenes/game.tscn`
- Entry: `Main._start_game(ship_id)`, after DEPLOY.
- Session setup: `CapybaraGame.configure_session(difficulty, ship_id)` assigns the player profile before the node enters the tree.
- Exit: `Game.return_to_menu` → `Main._on_game_returned()`.
- Restart: `Game.restart_requested` → `Main._restart_game()`, preserving the selected ship.

## Full boot configuration

```ini
; Capybara Project - Godot 4 migration
config_version=5

[application]

config/name="Capybara Project"
config/description="A music-driven horizontal shoot 'em up"
run/main_scene="res://scenes/main.tscn"
config/icon="res://assets/sprites/player_ship.png"

[autoload]

GameData="*res://scripts/autoload/game_data.gd"

[display]

window/size/viewport_width=1200
window/size/viewport_height=600
window/size/window_width_override=1200
window/size/window_height_override=600
window/stretch/mode="canvas_items"
window/stretch/aspect="keep"

[rendering]

renderer/rendering_method="gl_compatibility"
renderer/rendering_method.mobile="gl_compatibility"
textures/default_filters/use_nearest_mipmap_filter=false
environment/defaults/default_clear_color=Color(0.00784314, 0.0117647, 0.0352941, 1)

[audio]

default_bus_layout="res://default_bus_layout.tres"
```
