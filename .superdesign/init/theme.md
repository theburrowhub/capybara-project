# Theme and Design Tokens

## Part 1 — Compact token summary

### Canvas and geometry

- Reference viewport: 1200 × 600, aspect kept.
- Persistent header: x 54, y 36.
- Standard panel host: y 145, height 400.
- Ship-selection host: y 126, height 418; selection panel 1108 × 418.
- Persistent footer: y 560.
- Ship selector uses a chamfered technical frame, 24 px blueprint grid, 1–2 px cyan borders, three equal roster tabs, split ship/radar body, and 38 px action controls.

### Color palette

- Void: `#020613`; clear color: `#020309`.
- Panel: rgba(6, 14, 31, 0.97).
- Border/navigation cyan: `#45D6FF`; bright cyan: `#56D7FF`.
- Main text: `#EDFAFF`; muted text: `#7692B7`; controls: `#86A5D0`.
- Energy yellow: `#FFE45E`; success green: `#62E77F`.
- Vindicator: `#FF445E`; Sting: `#35A7FF`; Goliat: `#5DE071`.
- Radar grids use cyan at 16–22% alpha; data fills use the active ship color at 31% alpha.

### Typography

- Godot fallback sans-serif.
- Product title 44 px; ship name 29 px; section title 22 px; buttons 10–16 px; telemetry 8–12 px.
- Uppercase labels, compact technical copy, and tabular numeric formatting.

### Motion

- Menu stars drift left and pulse.
- Ship selector smoothly interpolates radar geometry between profiles.
- The selected GLB rotates continuously around world Y with an elevated camera, so the ship top remains correctly oriented.
- Buttons and frame accents switch to the selected ship color.

### Responsive behavior

- Fixed logical 1200 × 600 canvas scaled with `canvas_items` while keeping aspect ratio.

## Part 2 — Raw source dumps

### `scripts/ui/menu_background.gd`

```gdscript
extends Node2D

const CONFIG := preload("res://scripts/core/game_config.gd")

var stars: Array[Dictionary] = []
var elapsed := 0.0

func _ready() -> void:
	var generator := RandomNumberGenerator.new()
	generator.seed = 0x5EEDCA9
	for index in range(90):
		stars.append({
			"position": Vector2(generator.randf_range(0.0, CONFIG.WIDTH), generator.randf_range(0.0, CONFIG.HEIGHT)),
			"size": generator.randf_range(0.7, 2.2), "phase": generator.randf_range(0.0, TAU),
		})

func _process(delta: float) -> void:
	elapsed += delta
	for star in stars:
		var position: Vector2 = star["position"]
		position.x -= float(star["size"]) * 4.0 * delta
		if position.x < -4.0:
			position.x = CONFIG.WIDTH + 4.0
		star["position"] = position
	queue_redraw()

func _draw() -> void:
	draw_rect(Rect2(0.0, 0.0, CONFIG.WIDTH, CONFIG.HEIGHT), Color("020613"))
	for band in range(8):
		var band_color := Color(0.02, 0.08 + band * 0.006, 0.16 + band * 0.012, 0.14)
		draw_circle(Vector2(940.0, 280.0), 520.0 - band * 52.0, band_color)
	for star in stars:
		var alpha := 0.42 + sin(elapsed * 1.8 + float(star["phase"])) * 0.18
		draw_circle(star["position"], float(star["size"]), Color(0.65, 0.86, 1.0, alpha))
	# Navigation motif: a stylized flight vector behind the menu card.
	var path := PackedVector2Array([Vector2(70, 470), Vector2(250, 420), Vector2(410, 455), Vector2(630, 360), Vector2(850, 405), Vector2(1130, 260)])
	draw_polyline(path, Color(0.13, 0.65, 0.9, 0.16), 2.0, true)
	for point in path:
		draw_circle(point, 5.0, Color(0.25, 0.85, 1.0, 0.24))
```

### `scripts/ui/ship_selection_panel.gd`

```gdscript
class_name ShipSelectionPanel
extends Control

signal deploy_requested(ship_id: String)
signal cancel_requested

const CONFIG := preload("res://scripts/core/game_config.gd")
const PREVIEW_TYPE := preload("res://scripts/ui/ship_showcase_preview.gd")
const RADAR_TYPE := preload("res://scripts/ui/ship_radar_chart.gd")
const PANEL_SIZE := Vector2(1108.0, 418.0)
const STAT_LABELS := ["SPEED", "HULL", "SHIELD", "FIREPOWER", "RECOVERY"]

var selected_index := 0
var accent := Color("ff445e")
var tab_buttons: Array[Button] = []
var stat_values: Array[Label] = []
var stat_pips: Array = []
var ship_name_label: Label
var ship_role_label: Label
var ship_code_label: Label
var ship_index_label: Label
var description_label: Label
var status_label: Label
var deploy_button: Button
var preview
var radar

func _ready() -> void:
	custom_minimum_size = PANEL_SIZE
	size = PANEL_SIZE
	mouse_filter = Control.MOUSE_FILTER_STOP
	set_meta("ship_selection", true)
	set_process_unhandled_input(true)
	_build_interface()
	_select_ship(0)
	call_deferred("_focus_selected_tab")
	queue_redraw()

func selected_ship_id() -> String:
	return CONFIG.PLAYER_SHIP_ORDER[selected_index]

func _build_interface() -> void:
	var channel := _label("LOADOUT CHANNEL  //  PILOT FRAME", 10, Color("50688d"))
	_place(channel, Vector2(20.0, 10.0), Vector2(240.0, 24.0))
	ship_code_label = _label("VN-01", 12, accent)
	_place(ship_code_label, Vector2(270.0, 9.0), Vector2(90.0, 24.0))
	status_label = _label("●  SHIP LINK READY", 9, Color("62e77f"))
	status_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
	_place(status_label, Vector2(735.0, 10.0), Vector2(260.0, 24.0))
	ship_index_label = _label("01/03", 21, accent, true)
	ship_index_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
	_place(ship_index_label, Vector2(1000.0, 5.0), Vector2(88.0, 30.0))

	var tab_width := 356.0
	for index in range(CONFIG.PLAYER_SHIP_ORDER.size()):
		var ship_id: String = CONFIG.PLAYER_SHIP_ORDER[index]
		var profile := CONFIG.player_ship(ship_id)
		var button := Button.new()
		button.alignment = HORIZONTAL_ALIGNMENT_LEFT
		button.focus_mode = Control.FOCUS_ALL
		button.add_theme_font_size_override("font_size", 12)
		button.set_meta("ship_id", ship_id)
		button.pressed.connect(_select_ship.bind(index))
		_place(button, Vector2(20.0 + tab_width * index, 43.0), Vector2(tab_width - 1.0, 53.0))
		tab_buttons.append(button)
		_apply_tab_style(button, profile, false)

	preview = PREVIEW_TYPE.new()
	_place(preview, Vector2(31.0, 101.0), Vector2(548.0, 252.0))

	var acquisition := _label("VISUAL ACQUISITION  //  TARGET LOCK", 9, Color("62e77f"))
	_place(acquisition, Vector2(39.0, 106.0), Vector2(260.0, 18.0))

	var previous := _navigation_button("‹")
	previous.tooltip_text = "Previous ship"
	previous.pressed.connect(func() -> void: _select_ship(selected_index - 1))
	_place(previous, Vector2(32.0, 184.0), Vector2(36.0, 70.0))
	var next := _navigation_button("›")
	next.tooltip_text = "Next ship"
	next.pressed.connect(func() -> void: _select_ship(selected_index + 1))
	_place(next, Vector2(542.0, 184.0), Vector2(36.0, 70.0))

	ship_name_label = _label("VINDICATOR", 29, accent, true)
	_place(ship_name_label, Vector2(39.0, 303.0), Vector2(390.0, 38.0))
	ship_role_label = _label("BALANCED STRIKER", 10, Color("a9bddb"))
	_place(ship_role_label, Vector2(42.0, 336.0), Vector2(300.0, 18.0))
	var rotation_copy := _label("ROT // AUTO\nTOP // VERIFIED", 8, Color("50688d"))
	rotation_copy.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
	_place(rotation_copy, Vector2(447.0, 318.0), Vector2(122.0, 34.0))

	radar = RADAR_TYPE.new()
	_place(radar, Vector2(596.0, 104.0), Vector2(300.0, 238.0))
	var radar_title := _label("PERFORMANCE ENVELOPE", 9, Color("50688d"))
	_place(radar_title, Vector2(606.0, 106.0), Vector2(210.0, 18.0))

	for index in range(STAT_LABELS.size()):
		var row_y := 116.0 + float(index) * 42.0
		var stat_name := _label(STAT_LABELS[index], 9, Color("86a5d0"))
		_place(stat_name, Vector2(909.0, row_y), Vector2(112.0, 17.0))
		var stat_value := _label("3/5", 9, accent)
		stat_value.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
		_place(stat_value, Vector2(1034.0, row_y), Vector2(48.0, 17.0))
		stat_values.append(stat_value)
		var pip_row := HBoxContainer.new()
		pip_row.add_theme_constant_override("separation", 4)
		_place(pip_row, Vector2(909.0, row_y + 18.0), Vector2(174.0, 9.0))
		var pips: Array[ColorRect] = []
		for _pip in range(5):
			var pip := ColorRect.new()
			pip.custom_minimum_size = Vector2(29.0, 7.0)
			pip.mouse_filter = Control.MOUSE_FILTER_IGNORE
			pip_row.add_child(pip)
			pips.append(pip)
		stat_pips.append(pips)

	description_label = _label("Balanced armor, firepower and recovery. No critical weakness.", 9, Color("a9bddb"))
	description_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	description_label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	_place(description_label, Vector2(612.0, 315.0), Vector2(284.0, 37.0))

	var back_button := _action_button("ESC  BACK", false)
	back_button.pressed.connect(func() -> void: cancel_requested.emit())
	_place(back_button, Vector2(20.0, 369.0), Vector2(160.0, 38.0))
	var navigation_copy := _label("◀  A / D OR ARROWS  ·  CHANGE SHIP  ▶", 9, Color("50688d"))
	navigation_copy.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	navigation_copy.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	_place(navigation_copy, Vector2(194.0, 369.0), Vector2(385.0, 38.0))
	var ready_copy := _label("CONFIRM FRAME & START MISSION\nALL SYSTEMS NOMINAL", 8, Color("62e77f"))
	ready_copy.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
	ready_copy.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	_place(ready_copy, Vector2(595.0, 369.0), Vector2(260.0, 38.0))
	deploy_button = _action_button("▶  DEPLOY VINDICATOR", true)
	deploy_button.pressed.connect(_deploy_current)
	_place(deploy_button, Vector2(866.0, 369.0), Vector2(222.0, 38.0))

func _select_ship(index: int) -> void:
	selected_index = posmod(index, CONFIG.PLAYER_SHIP_ORDER.size())
	var ship_id: String = CONFIG.PLAYER_SHIP_ORDER[selected_index]
	var profile := CONFIG.player_ship(ship_id)
	accent = profile["accent"]
	ship_name_label.text = str(profile["name"])
	ship_name_label.add_theme_color_override("font_color", accent)
	ship_role_label.text = str(profile["role"])
	ship_code_label.text = "%s-%02d" % [str(profile["name"]).substr(0, 2), selected_index + 1]
	ship_code_label.add_theme_color_override("font_color", accent)
	ship_index_label.text = "%02d/03" % (selected_index + 1)
	ship_index_label.add_theme_color_override("font_color", accent)
	description_label.text = str(profile["description"])
	status_label.text = "●  SHIP LINK READY"
	deploy_button.text = "▶  DEPLOY %s" % str(profile["name"])
	preview.show_ship(ship_id)
	radar.set_profile(profile["radar"], accent, ship_id != "vindicator")

	for tab_index in range(tab_buttons.size()):
		var tab_profile := CONFIG.player_ship(CONFIG.PLAYER_SHIP_ORDER[tab_index])
		_apply_tab_style(tab_buttons[tab_index], tab_profile, tab_index == selected_index)

	var values: Array = profile["radar"]
	for stat_index in range(values.size()):
		var value := int(values[stat_index])
		stat_values[stat_index].text = "%d/5" % value
		stat_values[stat_index].add_theme_color_override("font_color", accent)
		var pips: Array = stat_pips[stat_index]
		for pip_index in range(pips.size()):
			(pips[pip_index] as ColorRect).color = accent if pip_index < value else Color("10243e")

	_update_deploy_style()
	queue_redraw()

func _deploy_current() -> void:
	status_label.text = "●  DEPLOYMENT CONFIRMED"
	deploy_requested.emit(selected_ship_id())

func _focus_selected_tab() -> void:
	if selected_index < tab_buttons.size():
		tab_buttons[selected_index].grab_focus()

func _unhandled_input(event: InputEvent) -> void:
	if not visible or not event is InputEventKey:
		return
	var key_event := event as InputEventKey
	if not key_event.pressed or key_event.echo:
		return
	var code := key_event.physical_keycode if key_event.physical_keycode != 0 else key_event.keycode
	var handled := true
	match code:
		KEY_LEFT, KEY_A:
			_select_ship(selected_index - 1)
			_focus_selected_tab()
		KEY_RIGHT, KEY_D:
			_select_ship(selected_index + 1)
			_focus_selected_tab()
		KEY_ENTER, KEY_KP_ENTER:
			_deploy_current()
		KEY_ESCAPE:
			cancel_requested.emit()
		_:
			handled = false
	if handled:
		get_viewport().set_input_as_handled()

func _draw() -> void:
	var frame := PackedVector2Array([
		Vector2(0.0, 14.0), Vector2(14.0, 0.0), Vector2(size.x - 28.0, 0.0),
		Vector2(size.x, 28.0), Vector2(size.x, size.y - 14.0),
		Vector2(size.x - 14.0, size.y), Vector2(28.0, size.y),
		Vector2(0.0, size.y - 28.0),
	])
	draw_colored_polygon(frame, Color(0.025, 0.055, 0.12, 0.97))
	var outline := frame.duplicate()
	outline.append(frame[0])
	draw_polyline(outline, Color(0.13, 0.55, 0.78, 0.78), 1.4, true)

	for x in range(24, int(size.x), 24):
		draw_line(Vector2(x, 1.0), Vector2(x, size.y - 1.0), Color(0.27, 0.84, 1.0, 0.025), 1.0)
	for y in range(24, int(size.y), 24):
		draw_line(Vector2(1.0, y), Vector2(size.x - 1.0, y), Color(0.27, 0.84, 1.0, 0.025), 1.0)
	for separator_y in [41.0, 97.0, 359.0]:
		draw_line(Vector2(20.0, separator_y), Vector2(size.x - 20.0, separator_y), Color(0.27, 0.84, 1.0, 0.20), 1.0)
	draw_line(Vector2(589.0, 97.0), Vector2(589.0, 359.0), Color(0.27, 0.84, 1.0, 0.18), 1.0)

	var reticle_center := Vector2(305.0, 223.0)
	for radius in [102.0, 73.0, 43.0]:
		draw_arc(reticle_center, radius, 0.0, TAU, 72, Color(0.27, 0.84, 1.0, 0.14), 1.0, true)
	draw_line(reticle_center - Vector2(132.0, 0.0), reticle_center + Vector2(132.0, 0.0), Color(0.27, 0.84, 1.0, 0.12), 1.0)
	draw_line(reticle_center - Vector2(0.0, 118.0), reticle_center + Vector2(0.0, 118.0), Color(0.27, 0.84, 1.0, 0.12), 1.0)
	draw_arc(reticle_center, 109.0, -0.28, 0.28, 14, accent, 2.0, true)
	draw_arc(reticle_center, 109.0, PI - 0.28, PI + 0.28, 14, accent, 2.0, true)
	draw_rect(Rect2(14.0, 0.0, 40.0, 4.0), accent)
	draw_rect(Rect2(size.x - 62.0, size.y - 4.0, 42.0, 4.0), accent)

func _apply_tab_style(button: Button, profile: Dictionary, active: bool) -> void:
	var ship_accent: Color = profile["accent"]
	var background := Color(0.018, 0.047, 0.105, 0.92)
	if active:
		background = Color(ship_accent.r * 0.12, ship_accent.g * 0.12, ship_accent.b * 0.12, 0.96)
	var border := ship_accent if active else Color(0.16, 0.45, 0.65, 0.32)
	button.add_theme_stylebox_override("normal", _box(background, border, 1))
	button.add_theme_stylebox_override("hover", _box(background.lightened(0.06), ship_accent, 1))
	button.add_theme_stylebox_override("pressed", _box(background.darkened(0.08), ship_accent, 2))
	button.add_theme_stylebox_override("focus", _box(background, Color("edfaff"), 2))
	button.add_theme_color_override("font_color", Color("edfaff") if active else Color("7692b7"))
	button.add_theme_color_override("font_hover_color", Color("edfaff"))
	button.add_theme_color_override("font_focus_color", Color("edfaff"))
	var index := CONFIG.PLAYER_SHIP_ORDER.find(str(profile["id"]))
	var cursor := "▶  " if active else "    "
	button.text = "%s%s\n     %02d · %s" % [cursor, str(profile["name"]), index + 1, str(profile["role"])]

func _update_deploy_style() -> void:
	deploy_button.add_theme_stylebox_override("normal", _box(accent, accent, 1))
	deploy_button.add_theme_stylebox_override("hover", _box(accent.lightened(0.12), Color("edfaff"), 2))
	deploy_button.add_theme_stylebox_override("pressed", _box(accent.darkened(0.16), accent, 2))
	deploy_button.add_theme_stylebox_override("focus", _box(accent, Color("edfaff"), 3))
	deploy_button.add_theme_color_override("font_color", Color("020613"))
	deploy_button.add_theme_color_override("font_hover_color", Color("020613"))
	deploy_button.add_theme_color_override("font_focus_color", Color("020613"))

func _navigation_button(text: String) -> Button:
	var button := Button.new()
	button.text = text
	button.focus_mode = Control.FOCUS_NONE
	button.add_theme_font_size_override("font_size", 21)
	button.add_theme_stylebox_override("normal", _box(Color(0.008, 0.024, 0.055, 0.76), Color(0.27, 0.84, 1.0, 0.28), 1))
	button.add_theme_stylebox_override("hover", _box(Color(0.03, 0.10, 0.18, 0.92), Color("45d6ff"), 1))
	button.add_theme_color_override("font_color", Color("86a5d0"))
	button.add_theme_color_override("font_hover_color", Color("edfaff"))
	return button

func _action_button(text: String, primary: bool) -> Button:
	var button := Button.new()
	button.text = text
	button.add_theme_font_size_override("font_size", 10)
	if primary:
		button.add_theme_color_override("font_color", Color("020613"))
	else:
		button.add_theme_stylebox_override("normal", _box(Color(0.018, 0.047, 0.105, 0.92), Color(0.27, 0.84, 1.0, 0.42), 1))
		button.add_theme_stylebox_override("hover", _box(Color(0.03, 0.10, 0.18, 0.94), Color("45d6ff"), 1))
		button.add_theme_stylebox_override("focus", _box(Color(0.03, 0.10, 0.18, 0.94), Color("edfaff"), 2))
		button.add_theme_color_override("font_color", Color("9bb7d8"))
		button.add_theme_color_override("font_hover_color", Color("edfaff"))
		button.add_theme_color_override("font_focus_color", Color("edfaff"))
	return button

func _label(text: String, font_size: int, color: Color, display_font := false) -> Label:
	var label := Label.new()
	label.text = text
	label.add_theme_font_size_override("font_size", font_size)
	label.add_theme_color_override("font_color", color)
	if display_font:
		label.add_theme_font_override("font", ThemeDB.fallback_font)
	return label

func _box(background: Color, border: Color, width: int) -> StyleBoxFlat:
	var style := StyleBoxFlat.new()
	style.bg_color = background
	style.border_color = border
	style.set_border_width_all(width)
	style.corner_radius_top_left = 2
	style.corner_radius_top_right = 2
	style.corner_radius_bottom_left = 2
	style.corner_radius_bottom_right = 2
	return style

func _place(control: Control, at: Vector2, dimensions: Vector2) -> void:
	control.position = at
	control.size = dimensions
	control.custom_minimum_size = dimensions
	add_child(control)

```

### `scripts/core/main.gd`

```gdscript
extends Control

const GAME_SCENE := preload("res://scenes/game.tscn")
const CONFIG := preload("res://scripts/core/game_config.gd")
const GAME_TYPE := preload("res://scripts/core/game.gd")
const SHIP_SELECTION_TYPE := preload("res://scripts/ui/ship_selection_panel.gd")
const RESOLUTIONS := [Vector2i(960, 480), Vector2i(1200, 600), Vector2i(1440, 720), Vector2i(1920, 960)]

var menu_layer: Control
var panel_host: CenterContainer
var current_panel: Control
var game_instance: GAME_TYPE
var options_music: HSlider
var options_sfx: HSlider
var options_resolution: OptionButton
var options_fullscreen: CheckButton
var options_vsync: CheckButton
var options_difficulty: OptionButton
var scores_difficulty: OptionButton
var scores_list: VBoxContainer
var name_input: LineEdit
var pending_score := 0
var pending_difficulty := 1
var active_ship_id := "vindicator"

func _ready() -> void:
	set_process_unhandled_input(true)
	_build_interface()
	_show_main_menu()

func _build_interface() -> void:
	menu_layer = Control.new()
	menu_layer.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	add_child(menu_layer)
	var header := VBoxContainer.new()
	header.position = Vector2(54.0, 36.0)
	header.size = Vector2(760.0, 112.0)
	menu_layer.add_child(header)
	var eyebrow := _label("GODOT FLIGHT SYSTEM · BUILD 0.3", 14, Color("56d7ff"))
	header.add_child(eyebrow)
	var title := _label("CAPYBARA PROJECT", 44, Color("edfaff"))
	header.add_child(title)
	var subtitle := _label("MUSIC-DRIVEN HORIZONTAL SHOOT ’EM UP", 15, Color("7692b7"))
	header.add_child(subtitle)
	panel_host = CenterContainer.new()
	panel_host.position = Vector2(0.0, 145.0)
	panel_host.size = Vector2(CONFIG.WIDTH, 400.0)
	menu_layer.add_child(panel_host)
	var footer := _label("WASD / ARROWS  NAVIGATE     ·     ENTER  SELECT     ·     F11  FULLSCREEN", 13, Color("50688d"))
	footer.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	footer.position = Vector2(100.0, 560.0)
	footer.size = Vector2(1000.0, 24.0)
	menu_layer.add_child(footer)

func _show_main_menu() -> void:
	var panel := _main_menu_panel()
	panel.set_meta("main", true)
	_show_panel(panel)

func _main_menu_panel() -> Control:
	var card := _card(Vector2(480.0, 350.0))
	var content := _vbox(card, 32, 28)
	content.add_child(_section_title("MISSION CONTROL"))
	content.add_child(_separator())
	var start := _menu_button("START MISSION", Color("45d6ff"))
	start.pressed.connect(_show_ship_selection)
	content.add_child(start)
	var options := _menu_button("OPTIONS")
	options.pressed.connect(func() -> void: _show_panel(_options_panel()))
	content.add_child(options)
	var scores := _menu_button("HIGH SCORES")
	scores.pressed.connect(func() -> void: _show_panel(_scores_panel()))
	content.add_child(scores)
	var credits := _menu_button("CREDITS")
	credits.pressed.connect(func() -> void: _show_panel(_credits_panel()))
	content.add_child(credits)
	var quit := _menu_button("QUIT")
	quit.pressed.connect(func() -> void: get_tree().quit())
	content.add_child(quit)
	return card

func _show_ship_selection() -> void:
	active_ship_id = "vindicator"
	var selector := SHIP_SELECTION_TYPE.new() as ShipSelectionPanel
	selector.set_meta("ship_selection", true)
	selector.deploy_requested.connect(_start_game)
	selector.cancel_requested.connect(_show_main_menu)
	_show_panel(selector)

func _options_panel() -> Control:
	var card := _card(Vector2(720.0, 390.0))
	var content := _vbox(card, 32, 22)
	content.add_child(_section_title("SYSTEM OPTIONS"))
	content.add_child(_separator())
	options_music = _slider_row(content, "MUSIC", float(GameData.settings["music_volume"]))
	options_sfx = _slider_row(content, "SOUND FX", float(GameData.settings["sound_volume"]))
	options_resolution = OptionButton.new()
	for resolution in RESOLUTIONS:
		options_resolution.add_item("%d × %d" % [resolution.x, resolution.y])
	var current_resolution: Vector2i = GameData.settings["resolution"]
	options_resolution.selected = maxi(0, RESOLUTIONS.find(current_resolution))
	content.add_child(_control_row("RESOLUTION", options_resolution))
	options_fullscreen = CheckButton.new()
	options_fullscreen.text = "ENABLED"
	options_fullscreen.button_pressed = bool(GameData.settings["fullscreen"])
	content.add_child(_control_row("FULLSCREEN", options_fullscreen))
	options_vsync = CheckButton.new()
	options_vsync.text = "ENABLED"
	options_vsync.button_pressed = bool(GameData.settings["vsync"])
	content.add_child(_control_row("V-SYNC", options_vsync))
	options_difficulty = OptionButton.new()
	for difficulty_name in GameData.DIFFICULTY_NAMES:
		options_difficulty.add_item(difficulty_name)
	options_difficulty.selected = int(GameData.settings["difficulty"])
	content.add_child(_control_row("DIFFICULTY", options_difficulty))
	var actions := HBoxContainer.new()
	actions.alignment = BoxContainer.ALIGNMENT_END
	var back := _menu_button("CANCEL")
	back.pressed.connect(_show_main_menu)
	actions.add_child(back)
	var apply := _menu_button("APPLY", Color("45d6ff"))
	apply.pressed.connect(_apply_options)
	actions.add_child(apply)
	content.add_child(actions)
	return card

func _scores_panel() -> Control:
	var card := _card(Vector2(650.0, 390.0))
	var content := _vbox(card, 32, 20)
	content.add_child(_section_title("HIGH SCORES"))
	scores_difficulty = OptionButton.new()
	for difficulty_name in GameData.DIFFICULTY_NAMES:
		scores_difficulty.add_item(difficulty_name)
	scores_difficulty.selected = int(GameData.settings["difficulty"])
	scores_difficulty.item_selected.connect(func(_index: int) -> void: _refresh_scores())
	content.add_child(_control_row("LEADERBOARD", scores_difficulty))
	scores_list = VBoxContainer.new()
	scores_list.custom_minimum_size = Vector2(560.0, 230.0)
	content.add_child(scores_list)
	var back := _menu_button("BACK")
	back.pressed.connect(_show_main_menu)
	content.add_child(back)
	call_deferred("_refresh_scores")
	return card

func _credits_panel() -> Control:
	var card := _card(Vector2(650.0, 350.0))
	var content := _vbox(card, 36, 28)
	content.add_child(_section_title("CREDITS"))
	content.add_child(_separator())
	var copy := _label(
		"CAPYBARA PROJECT\n\nOriginal game design and C/Raylib prototype by the Capybara Project team.\nGodot 4 migration with a native scene architecture.\n\nPlayer ship rendered from the supplied 3D GLB model.\nEnemy and projectile art migrated from the original asset set.",
		17, Color("a9bddb"))
	copy.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	copy.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	copy.custom_minimum_size = Vector2(560.0, 210.0)
	content.add_child(copy)
	var back := _menu_button("BACK")
	back.pressed.connect(_show_main_menu)
	content.add_child(back)
	return card

func _name_panel() -> Control:
	var card := _card(Vector2(560.0, 300.0))
	var content := _vbox(card, 40, 32)
	content.add_child(_section_title("NEW HIGH SCORE"))
	var score_copy := _label("%09d  ·  %s" % [pending_score, GameData.DIFFICULTY_NAMES[pending_difficulty]], 25, Color("ffe45e"))
	score_copy.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	content.add_child(score_copy)
	name_input = LineEdit.new()
	name_input.placeholder_text = "PILOT NAME"
	name_input.max_length = 31
	name_input.custom_minimum_size = Vector2(460.0, 48.0)
	name_input.text_submitted.connect(func(_text: String) -> void: _save_score())
	content.add_child(name_input)
	var save := _menu_button("SAVE SCORE", Color("45d6ff"))
	save.pressed.connect(_save_score)
	content.add_child(save)
	name_input.call_deferred("grab_focus")
	return card

func _start_game(ship_id := "") -> void:
	if not ship_id.is_empty() and CONFIG.PLAYER_SHIPS.has(ship_id):
		active_ship_id = ship_id
	if is_instance_valid(game_instance):
		game_instance.queue_free()
	menu_layer.hide()
	game_instance = GAME_SCENE.instantiate() as GAME_TYPE
	game_instance.configure_session(int(GameData.settings["difficulty"]), active_ship_id)
	add_child(game_instance)
	move_child(game_instance, 1)
	game_instance.return_to_menu.connect(_on_game_returned)
	game_instance.restart_requested.connect(_restart_game)

func _restart_game(_difficulty: int) -> void:
	if is_instance_valid(game_instance):
		game_instance.queue_free()
	game_instance = null
	call_deferred("_start_game")

func _on_game_returned(score: int, difficulty: int, qualifies: bool) -> void:
	if is_instance_valid(game_instance):
		game_instance.queue_free()
	game_instance = null
	menu_layer.show()
	if qualifies and score > 0:
		pending_score = score
		pending_difficulty = difficulty
		_show_panel(_name_panel())
	else:
		_show_main_menu()

func _save_score() -> void:
	GameData.add_high_score(name_input.text if is_instance_valid(name_input) else "Player", pending_score, pending_difficulty)
	_show_panel(_scores_panel())
	if is_instance_valid(scores_difficulty):
		scores_difficulty.selected = pending_difficulty
		_refresh_scores()

func _apply_options() -> void:
	GameData.settings["music_volume"] = options_music.value
	GameData.settings["sound_volume"] = options_sfx.value
	GameData.settings["resolution"] = RESOLUTIONS[options_resolution.selected]
	GameData.settings["fullscreen"] = options_fullscreen.button_pressed
	GameData.settings["vsync"] = options_vsync.button_pressed
	GameData.settings["difficulty"] = options_difficulty.selected
	GameData.apply_settings()
	GameData.save_profile()
	_show_main_menu()

func _refresh_scores() -> void:
	if not is_instance_valid(scores_list) or not is_instance_valid(scores_difficulty):
		return
	for child in scores_list.get_children():
		child.queue_free()
	var entries := GameData.scores_for(scores_difficulty.selected)
	for rank in range(entries.size()):
		var entry: Dictionary = entries[rank]
		var row := HBoxContainer.new()
		var rank_label := _label("%02d" % (rank + 1), 16, Color("56d7ff"))
		rank_label.custom_minimum_size.x = 48.0
		row.add_child(rank_label)
		var pilot := _label(str(entry["name"]), 16, Color("c8daf3"))
		pilot.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		row.add_child(pilot)
		var points := _label("%09d" % int(entry["score"]), 16, Color("ffe45e"))
		points.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
		points.custom_minimum_size.x = 150.0
		row.add_child(points)
		scores_list.add_child(row)

func _show_panel(panel: Control) -> void:
	if is_instance_valid(current_panel):
		current_panel.queue_free()
	current_panel = panel
	if panel.get_meta("ship_selection", false):
		panel_host.position = Vector2(0.0, 126.0)
		panel_host.size = Vector2(CONFIG.WIDTH, 418.0)
	else:
		panel_host.position = Vector2(0.0, 145.0)
		panel_host.size = Vector2(CONFIG.WIDTH, 400.0)
	panel_host.add_child(current_panel)
	var first_button := current_panel.find_children("*", "Button", true, false)
	if not first_button.is_empty():
		(first_button[0] as Button).call_deferred("grab_focus")

func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed and not event.echo:
		if event.keycode == KEY_F11:
			GameData.settings["fullscreen"] = not bool(GameData.settings["fullscreen"])
			GameData.apply_settings()
			GameData.save_profile()
		elif event.keycode == KEY_ESCAPE and not is_instance_valid(game_instance):
			if current_panel and current_panel.get_meta("ship_selection", false):
				return
			elif current_panel and current_panel.get_meta("main", false):
				get_tree().quit()
			else:
				_show_main_menu()

func _card(dimensions: Vector2) -> PanelContainer:
	var card := PanelContainer.new()
	card.custom_minimum_size = dimensions
	var style := StyleBoxFlat.new()
	style.bg_color = Color(0.025, 0.055, 0.12, 0.94)
	style.border_color = Color(0.13, 0.55, 0.78, 0.58)
	style.set_border_width_all(1)
	style.corner_radius_top_left = 10
	style.corner_radius_top_right = 10
	style.corner_radius_bottom_left = 10
	style.corner_radius_bottom_right = 10
	style.shadow_color = Color(0, 0, 0, 0.55)
	style.shadow_size = 18
	card.add_theme_stylebox_override("panel", style)
	return card

func _vbox(parent: Control, horizontal_margin: int, vertical_margin: int) -> VBoxContainer:
	var margin := MarginContainer.new()
	margin.add_theme_constant_override("margin_left", horizontal_margin)
	margin.add_theme_constant_override("margin_right", horizontal_margin)
	margin.add_theme_constant_override("margin_top", vertical_margin)
	margin.add_theme_constant_override("margin_bottom", vertical_margin)
	parent.add_child(margin)
	var box := VBoxContainer.new()
	box.add_theme_constant_override("separation", 10)
	margin.add_child(box)
	return box

func _section_title(text: String) -> Label:
	var label := _label(text, 22, Color("e8f9ff"))
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.custom_minimum_size.y = 34.0
	return label

func _label(text: String, font_size: int, color: Color) -> Label:
	var label := Label.new()
	label.text = text
	label.add_theme_font_size_override("font_size", font_size)
	label.add_theme_color_override("font_color", color)
	return label

func _menu_button(text: String, accent := Color("88a7cf")) -> Button:
	var button := Button.new()
	button.text = text
	button.custom_minimum_size = Vector2(0.0, 42.0)
	button.add_theme_font_size_override("font_size", 16)
	button.add_theme_color_override("font_color", accent)
	button.add_theme_color_override("font_hover_color", Color.WHITE)
	button.add_theme_color_override("font_focus_color", Color.WHITE)
	return button

func _separator() -> HSeparator:
	var separator := HSeparator.new()
	separator.modulate = Color(0.2, 0.75, 1.0, 0.35)
	return separator

func _slider_row(parent: VBoxContainer, title: String, value: float) -> HSlider:
	var slider := HSlider.new()
	slider.min_value = 0.0
	slider.max_value = 1.0
	slider.step = 0.05
	slider.value = value
	slider.custom_minimum_size = Vector2(330.0, 36.0)
	parent.add_child(_control_row(title, slider))
	return slider

func _control_row(title: String, control: Control) -> HBoxContainer:
	var row := HBoxContainer.new()
	var label := _label(title, 15, Color("86a5d0"))
	label.custom_minimum_size = Vector2(190.0, 36.0)
	label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	row.add_child(label)
	control.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	row.add_child(control)
	return row
```

### `project.godot`

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
