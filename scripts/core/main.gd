extends Control

const GAME_SCENE := preload("res://scenes/game.tscn")
const CONFIG := preload("res://scripts/core/game_config.gd")
const GAME_TYPE := preload("res://scripts/core/game.gd")
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
	start.pressed.connect(_start_game)
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

func _start_game() -> void:
	if is_instance_valid(game_instance):
		game_instance.queue_free()
	menu_layer.hide()
	game_instance = GAME_SCENE.instantiate() as GAME_TYPE
	game_instance.difficulty = int(GameData.settings["difficulty"])
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
			if current_panel and current_panel.get_meta("main", false):
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
