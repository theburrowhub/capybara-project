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
var _deployment_started := false
var _stylebox_cache: Dictionary = {}
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
	assert(not CONFIG.PLAYER_SHIP_ORDER.is_empty(), "Ship selection requires at least one configured ship")
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
	ship_index_label = _label("01/%02d" % CONFIG.PLAYER_SHIP_ORDER.size(), 21, accent, true)
	ship_index_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
	_place(ship_index_label, Vector2(1000.0, 5.0), Vector2(88.0, 30.0))

	var ship_count := CONFIG.PLAYER_SHIP_ORDER.size()
	var tab_width := (PANEL_SIZE.x - 40.0) / float(ship_count)
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
	var ship_count := CONFIG.PLAYER_SHIP_ORDER.size()
	selected_index = posmod(index, ship_count)
	var ship_id: String = CONFIG.PLAYER_SHIP_ORDER[selected_index]
	var profile := CONFIG.player_ship(ship_id)
	accent = profile["accent"]
	ship_name_label.text = str(profile["name"])
	ship_name_label.add_theme_color_override("font_color", accent)
	ship_role_label.text = str(profile["role"])
	ship_code_label.text = "%s-%02d" % [str(profile["name"]).substr(0, 2), selected_index + 1]
	ship_code_label.add_theme_color_override("font_color", accent)
	ship_index_label.text = "%02d/%02d" % [selected_index + 1, ship_count]
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
	if _deployment_started:
		return
	_deployment_started = true
	set_process_unhandled_input(false)
	deploy_button.disabled = true
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
	var prefix := "tab:%s:%s" % [str(profile["id"]), "active" if active else "inactive"]
	button.add_theme_stylebox_override("normal", _cached_box(prefix + ":normal", background, border, 1))
	button.add_theme_stylebox_override("hover", _cached_box(prefix + ":hover", background.lightened(0.06), ship_accent, 1))
	button.add_theme_stylebox_override("pressed", _cached_box(prefix + ":pressed", background.darkened(0.08), ship_accent, 2))
	button.add_theme_stylebox_override("focus", _cached_box(prefix + ":focus", background, Color("edfaff"), 2))
	button.add_theme_color_override("font_color", Color("edfaff") if active else Color("7692b7"))
	button.add_theme_color_override("font_hover_color", Color("edfaff"))
	button.add_theme_color_override("font_focus_color", Color("edfaff"))
	var index := CONFIG.PLAYER_SHIP_ORDER.find(str(profile["id"]))
	var cursor := "▶  " if active else "    "
	button.text = "%s%s\n     %02d · %s" % [cursor, str(profile["name"]), index + 1, str(profile["role"])]

func _update_deploy_style() -> void:
	var prefix := "deploy:%s" % selected_ship_id()
	deploy_button.add_theme_stylebox_override("normal", _cached_box(prefix + ":normal", accent, accent, 1))
	deploy_button.add_theme_stylebox_override("hover", _cached_box(prefix + ":hover", accent.lightened(0.12), Color("edfaff"), 2))
	deploy_button.add_theme_stylebox_override("pressed", _cached_box(prefix + ":pressed", accent.darkened(0.16), accent, 2))
	deploy_button.add_theme_stylebox_override("focus", _cached_box(prefix + ":focus", accent, Color("edfaff"), 3))
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

func _cached_box(cache_key: String, background: Color, border: Color, width: int) -> StyleBoxFlat:
	if not _stylebox_cache.has(cache_key):
		_stylebox_cache[cache_key] = _box(background, border, width)
	return _stylebox_cache[cache_key] as StyleBoxFlat

func _place(control: Control, at: Vector2, dimensions: Vector2) -> void:
	control.position = at
	control.size = dimensions
	control.custom_minimum_size = dimensions
	add_child(control)
