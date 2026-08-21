# Shared UI Components

## Framework note

This is a Godot 4 project. UI is assembled with native `Control` nodes from GDScript rather than a web component library. The reusable runtime UI components are listed with their complete source below.

## GameplayHUD

- File: `scripts/ui/hud.gd`
- Description: Shared in-game status display, bars, score/time, boss status, pause overlay, and notices.

```gdscript
class_name GameHUD
extends CanvasLayer

const CONFIG := preload("res://scripts/core/game_config.gd")
const ENEMY_TYPE := preload("res://scripts/entities/enemy.gd")
const PLAYER_TYPE := preload("res://scripts/entities/player.gd")
const WAVE_DIRECTOR_TYPE := preload("res://scripts/systems/wave_director.gd")

var game: Node
var player: PLAYER_TYPE
var director: WAVE_DIRECTOR_TYPE
var top_label: Label
var time_label: Label
var enemy_label: Label
var score_label: Label
var status_label: Label
var weapon_label: Label
var progress_bar: ProgressBar
var hull_bar: ProgressBar
var shield_bar: ProgressBar
var energy_bar: ProgressBar
var boss_bar: ProgressBar
var boss_label: Label
var center_overlay: ColorRect
var overlay_title: Label
var overlay_subtitle: Label
var notice_label: Label
var notice_timer := 0.0

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_ALWAYS
	_build_hud()

func setup(game_node: Node, player_node: PLAYER_TYPE, wave_director: WAVE_DIRECTOR_TYPE) -> void:
	game = game_node
	player = player_node
	director = wave_director

func _process(delta: float) -> void:
	if not is_instance_valid(game) or not is_instance_valid(player) or not is_instance_valid(director):
		return
	top_label.text = "LEVEL %d · %s  |  %s" % [int(game.level_index) + 1, str(game.level_config["name"]), director.phase_name().to_upper()]
	time_label.text = _format_time(float(game.level_time))
	enemy_label.text = "HOSTILES %02d" % game.active_enemy_count()
	score_label.text = "SCORE  %09d" % int(game.score)
	status_label.text = "HULL %3d%%   SHIELD %3d%%   ENERGY %3d%%   MODE %s" % [
		int(player.health / player.max_health * 100.0), int(player.shield / maxf(player.max_shield, 1.0) * 100.0),
		int(player.energy), player.energy_mode_name()
	]
	weapon_label.text = "WEAPON %s  ·  POWER +%d  ·  [1–6] SELECT  [Q] MODE  [E] SPECIAL" % [player.weapon_name().to_upper(), player.weapon_powerups]
	progress_bar.value = director.progress() * 100.0
	hull_bar.value = player.health / player.max_health * 100.0
	shield_bar.value = player.shield / maxf(player.max_shield, 1.0) * 100.0
	energy_bar.value = player.energy
	var boss := game.boss_enemy as ENEMY_TYPE
	if is_instance_valid(boss):
		boss_bar.visible = true
		boss_label.visible = true
		boss_bar.value = clampf(boss.health / boss.max_health * 100.0, 0.0, 100.0)
	else:
		boss_bar.visible = false
		boss_label.visible = false
	if notice_timer > 0.0:
		notice_timer -= delta
		if notice_timer <= 0.0:
			notice_label.hide()

func show_pause(paused: bool) -> void:
	if paused:
		show_overlay("PAUSED", "[P] RESUME   ·   [ESC] MAIN MENU")
	else:
		hide_overlay()

func show_overlay(title: String, subtitle: String) -> void:
	center_overlay.show()
	overlay_title.text = title
	overlay_subtitle.text = subtitle

func hide_overlay() -> void:
	center_overlay.hide()

func show_notice(text: String, duration := 2.5) -> void:
	notice_label.text = text
	notice_label.show()
	notice_timer = duration

func _build_hud() -> void:
	var top := ColorRect.new()
	top.color = Color(0.015, 0.025, 0.075, 0.96)
	top.position = Vector2.ZERO
	top.size = Vector2(CONFIG.WIDTH, CONFIG.HUD_TOP)
	add_child(top)
	top_label = _label(16, Color("9ee9ff"), HORIZONTAL_ALIGNMENT_LEFT)
	top_label.position = Vector2(16.0, 4.0)
	top_label.size = Vector2(760.0, 24.0)
	top.add_child(top_label)
	time_label = _label(16, Color.WHITE, HORIZONTAL_ALIGNMENT_CENTER)
	time_label.position = Vector2(805.0, 4.0)
	time_label.size = Vector2(150.0, 24.0)
	top.add_child(time_label)
	enemy_label = _label(16, Color("ff708a"), HORIZONTAL_ALIGNMENT_RIGHT)
	enemy_label.position = Vector2(980.0, 4.0)
	enemy_label.size = Vector2(200.0, 24.0)
	top.add_child(enemy_label)
	progress_bar = ProgressBar.new()
	progress_bar.show_percentage = false
	progress_bar.position = Vector2(0.0, 27.0)
	progress_bar.size = Vector2(CONFIG.WIDTH, 3.0)
	top.add_child(progress_bar)

	var bottom := ColorRect.new()
	bottom.color = Color(0.012, 0.02, 0.062, 0.98)
	bottom.position = Vector2(0.0, CONFIG.HUD_BOTTOM)
	bottom.size = Vector2(CONFIG.WIDTH, CONFIG.HEIGHT - CONFIG.HUD_BOTTOM)
	add_child(bottom)
	score_label = _label(23, Color("ffe45e"), HORIZONTAL_ALIGNMENT_RIGHT)
	score_label.position = Vector2(900.0, 10.0)
	score_label.size = Vector2(280.0, 32.0)
	bottom.add_child(score_label)
	status_label = _label(16, Color("b5c7ed"), HORIZONTAL_ALIGNMENT_LEFT)
	status_label.position = Vector2(18.0, 10.0)
	status_label.size = Vector2(850.0, 24.0)
	bottom.add_child(status_label)
	weapon_label = _label(14, Color("86a5d9"), HORIZONTAL_ALIGNMENT_LEFT)
	weapon_label.position = Vector2(18.0, 69.0)
	weapon_label.size = Vector2(950.0, 24.0)
	bottom.add_child(weapon_label)
	hull_bar = _bar(Color("ff526d"), Vector2(18.0, 41.0), Vector2(245.0, 13.0))
	shield_bar = _bar(Color("48d9ff"), Vector2(275.0, 41.0), Vector2(245.0, 13.0))
	energy_bar = _bar(Color("ffd84d"), Vector2(532.0, 41.0), Vector2(245.0, 13.0))
	bottom.add_child(hull_bar)
	bottom.add_child(shield_bar)
	bottom.add_child(energy_bar)

	boss_label = _label(15, Color("ff6a78"), HORIZONTAL_ALIGNMENT_CENTER)
	boss_label.text = "BOSS CORE"
	boss_label.position = Vector2(385.0, 43.0)
	boss_label.size = Vector2(430.0, 22.0)
	add_child(boss_label)
	boss_bar = _bar(Color("ed2447"), Vector2(360.0, 65.0), Vector2(480.0, 12.0))
	add_child(boss_bar)
	boss_label.hide()
	boss_bar.hide()

	notice_label = _label(28, Color("d7f5ff"), HORIZONTAL_ALIGNMENT_CENTER)
	notice_label.position = Vector2(150.0, 88.0)
	notice_label.size = Vector2(900.0, 42.0)
	notice_label.add_theme_color_override("font_shadow_color", Color.BLACK)
	notice_label.add_theme_constant_override("shadow_offset_x", 2)
	notice_label.add_theme_constant_override("shadow_offset_y", 2)
	add_child(notice_label)
	notice_label.hide()

	center_overlay = ColorRect.new()
	center_overlay.color = Color(0.0, 0.0, 0.03, 0.82)
	center_overlay.position = Vector2(0.0, CONFIG.HUD_TOP)
	center_overlay.size = Vector2(CONFIG.WIDTH, CONFIG.HUD_BOTTOM - CONFIG.HUD_TOP)
	add_child(center_overlay)
	overlay_title = _label(52, Color("e8f9ff"), HORIZONTAL_ALIGNMENT_CENTER)
	overlay_title.position = Vector2(120.0, 145.0)
	overlay_title.size = Vector2(960.0, 75.0)
	center_overlay.add_child(overlay_title)
	overlay_subtitle = _label(20, Color("8db4dc"), HORIZONTAL_ALIGNMENT_CENTER)
	overlay_subtitle.position = Vector2(120.0, 230.0)
	overlay_subtitle.size = Vector2(960.0, 60.0)
	overlay_subtitle.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	center_overlay.add_child(overlay_subtitle)
	center_overlay.hide()

func _label(font_size: int, color: Color, alignment: HorizontalAlignment) -> Label:
	var label := Label.new()
	label.add_theme_font_size_override("font_size", font_size)
	label.add_theme_color_override("font_color", color)
	label.horizontal_alignment = alignment
	return label

func _bar(color: Color, at: Vector2, dimensions: Vector2) -> ProgressBar:
	var bar := ProgressBar.new()
	bar.position = at
	bar.size = dimensions
	bar.show_percentage = false
	bar.value = 100.0
	var fill := StyleBoxFlat.new()
	fill.bg_color = color
	fill.corner_radius_top_left = 3
	fill.corner_radius_top_right = 3
	fill.corner_radius_bottom_left = 3
	fill.corner_radius_bottom_right = 3
	bar.add_theme_stylebox_override("fill", fill)
	var background := StyleBoxFlat.new()
	background.bg_color = Color(0.12, 0.16, 0.25, 0.9)
	background.corner_radius_top_left = 3
	background.corner_radius_top_right = 3
	background.corner_radius_bottom_left = 3
	background.corner_radius_bottom_right = 3
	bar.add_theme_stylebox_override("background", background)
	return bar

func _format_time(seconds: float) -> String:
	return "%02d:%02d" % [int(seconds) / 60, int(seconds) % 60]
```

## MenuBackground

- File: `scripts/ui/menu_background.gd`
- Description: Animated space background used behind every menu panel.

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

## ShipSelectionPanel

- File: `scripts/ui/ship_selection_panel.gd`
- Description: Complete arcade ship-selection stage, input handling, tabs, stat pips, and launch/cancel signals.

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

## ShipShowcasePreview

- File: `scripts/ui/ship_showcase_preview.gd`
- Description: Transparent live 3D GLB viewer with an elevated camera and continuous Y-axis showcase rotation.

```gdscript
class_name ShipShowcasePreview
extends TextureRect

const CONFIG := preload("res://scripts/core/game_config.gd")

var current_ship_id := "vindicator"
var rotation_speed := 0.42
var viewport: SubViewport
var model_pivot: Node3D
var active_model: Node3D
var accent_light: OmniLight3D

func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	expand_mode = TextureRect.EXPAND_IGNORE_SIZE
	stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
	_build_viewport()
	show_ship(current_ship_id)
	set_process(true)

func _process(delta: float) -> void:
	if is_instance_valid(model_pivot):
		model_pivot.rotation.y = wrapf(model_pivot.rotation.y + rotation_speed * delta, 0.0, TAU)

func show_ship(ship_id: String) -> void:
	current_ship_id = ship_id if CONFIG.PLAYER_SHIPS.has(ship_id) else "vindicator"
	if not is_node_ready() or not is_instance_valid(model_pivot):
		return
	if is_instance_valid(active_model):
		active_model.free()
		active_model = null
	var profile := CONFIG.player_ship(current_ship_id)
	var resource := load(str(profile["model"]))
	if resource == null:
		return
	active_model = resource.instantiate() as Node3D
	if active_model == null:
		return
	active_model.name = "ShowcaseModel"
	active_model.scale = Vector3.ONE * float(profile["model_scale"])
	model_pivot.add_child(active_model)
	if is_instance_valid(accent_light):
		accent_light.light_color = profile["accent"]

func _build_viewport() -> void:
	viewport = SubViewport.new()
	viewport.name = "ShipShowcaseViewport"
	viewport.size = Vector2i(720, 360)
	viewport.transparent_bg = true
	viewport.render_target_update_mode = SubViewport.UPDATE_ALWAYS
	viewport.world_3d = World3D.new()
	add_child(viewport)
	texture = viewport.get_texture()

	model_pivot = Node3D.new()
	model_pivot.name = "RotatingShip"
	model_pivot.rotation.y = -0.35
	viewport.add_child(model_pivot)

	var key := DirectionalLight3D.new()
	key.name = "KeyLight"
	key.rotation_degrees = Vector3(-52.0, -32.0, -12.0)
	key.light_energy = 2.6
	viewport.add_child(key)

	accent_light = OmniLight3D.new()
	accent_light.name = "AccentLight"
	accent_light.position = Vector3(-9.0, 9.0, 9.0)
	accent_light.omni_range = 42.0
	accent_light.light_energy = 5.0
	accent_light.light_color = Color("ff445e")
	viewport.add_child(accent_light)

	var rim := OmniLight3D.new()
	rim.name = "RimLight"
	rim.position = Vector3(10.0, 5.0, -9.0)
	rim.omni_range = 40.0
	rim.light_energy = 4.2
	rim.light_color = Color("70cfff")
	viewport.add_child(rim)

	var camera := Camera3D.new()
	camera.name = "ShowcaseCamera"
	camera.projection = Camera3D.PROJECTION_ORTHOGONAL
	camera.size = 23.5
	camera.position = Vector3(0.0, 12.5, 25.0)
	viewport.add_child(camera)
	camera.look_at(Vector3(0.0, 0.5, 0.0), Vector3.UP)
	camera.current = true

```

## ShipRadarChart

- File: `scripts/ui/ship_radar_chart.gd`
- Description: Animated five-axis area/radar comparison chart with balanced reference outline.

```gdscript
class_name ShipRadarChart
extends Control

const LABELS := ["SPEED", "HULL", "SHIELD", "FIREPOWER", "RECOVERY"]
const BALANCED_STATS := [3.0, 3.0, 3.0, 3.0, 3.0]

var target_stats: Array[float] = [3.0, 3.0, 3.0, 3.0, 3.0]
var display_stats: Array[float] = [3.0, 3.0, 3.0, 3.0, 3.0]
var accent := Color("ff445e")
var show_balanced_reference := false

func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	set_process(true)
	queue_redraw()

func set_profile(values: Array, color: Color, compare_balanced: bool) -> void:
	for index in range(5):
		target_stats[index] = float(values[index])
	accent = color
	show_balanced_reference = compare_balanced
	queue_redraw()

func _process(delta: float) -> void:
	var changed := false
	for index in range(display_stats.size()):
		var next_value := lerpf(display_stats[index], target_stats[index], minf(1.0, delta * 9.0))
		if absf(next_value - target_stats[index]) < 0.01:
			next_value = target_stats[index]
		if not is_equal_approx(next_value, display_stats[index]):
			display_stats[index] = next_value
			changed = true
	if changed:
		queue_redraw()

func _draw() -> void:
	var center := Vector2(size.x * 0.50, size.y * 0.49)
	var radius := minf(size.x * 0.30, size.y * 0.34)
	var grid_color := Color(0.34, 0.84, 1.0, 0.22)
	var axis_color := Color(0.34, 0.84, 1.0, 0.16)

	for ring in range(5, 0, -1):
		var ring_values := [float(ring), float(ring), float(ring), float(ring), float(ring)]
		var ring_points := _points_for(ring_values, center, radius)
		if ring == 5:
			draw_colored_polygon(ring_points, Color(0.15, 0.55, 0.78, 0.025))
		draw_polyline(_closed(ring_points), grid_color, 1.0, true)

	for index in range(5):
		var angle := -PI * 0.5 + TAU * float(index) / 5.0
		draw_line(center, center + Vector2.from_angle(angle) * radius, axis_color, 1.0, true)

	if show_balanced_reference:
		var reference := _points_for(BALANCED_STATS, center, radius)
		for index in range(reference.size()):
			_draw_dashed_segment(reference[index], reference[(index + 1) % reference.size()])

	var value_points := _points_for(display_stats, center, radius)
	draw_colored_polygon(value_points, Color(accent.r, accent.g, accent.b, 0.31))
	draw_polyline(_closed(value_points), accent, 2.5, true)
	for point in value_points:
		draw_circle(point, 3.2, Color("edfaff"))
		draw_arc(point, 4.4, 0.0, TAU, 12, accent, 1.4, true)

	var font := ThemeDB.fallback_font
	var font_size := 10
	for index in range(5):
		var angle := -PI * 0.5 + TAU * float(index) / 5.0
		var label_center := center + Vector2.from_angle(angle) * (radius + 19.0)
		var text_size := font.get_string_size(LABELS[index], HORIZONTAL_ALIGNMENT_LEFT, -1.0, font_size)
		var baseline := label_center + Vector2(-text_size.x * 0.5, text_size.y * 0.34)
		draw_string(font, baseline, LABELS[index], HORIZONTAL_ALIGNMENT_LEFT, -1.0, font_size, Color("86a5d0"))

func _points_for(values: Array, center: Vector2, radius: float) -> PackedVector2Array:
	var points := PackedVector2Array()
	for index in range(5):
		var angle := -PI * 0.5 + TAU * float(index) / 5.0
		points.append(center + Vector2.from_angle(angle) * radius * float(values[index]) / 5.0)
	return points

func _closed(points: PackedVector2Array) -> PackedVector2Array:
	var result := points.duplicate()
	if not result.is_empty():
		result.append(result[0])
	return result

func _draw_dashed_segment(start: Vector2, finish: Vector2) -> void:
	var color := Color(0.93, 0.98, 1.0, 0.42)
	var length := start.distance_to(finish)
	if length <= 0.0:
		return
	var direction := start.direction_to(finish)
	var offset := 0.0
	while offset < length:
		var dash_end := minf(offset + 4.0, length)
		draw_line(start + direction * offset, start + direction * dash_end, color, 1.3, true)
		offset += 8.0
```
