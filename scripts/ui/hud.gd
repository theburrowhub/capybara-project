class_name GameHUD
extends CanvasLayer

var game: Node
var player: PlayerShip
var director: WaveDirector
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

func setup(game_node: Node, player_node: PlayerShip, wave_director: WaveDirector) -> void:
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
	var boss := game.boss_enemy as EnemyShip
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
	top.size = Vector2(GameConfig.WIDTH, GameConfig.HUD_TOP)
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
	progress_bar.size = Vector2(GameConfig.WIDTH, 3.0)
	top.add_child(progress_bar)

	var bottom := ColorRect.new()
	bottom.color = Color(0.012, 0.02, 0.062, 0.98)
	bottom.position = Vector2(0.0, GameConfig.HUD_BOTTOM)
	bottom.size = Vector2(GameConfig.WIDTH, GameConfig.HEIGHT - GameConfig.HUD_BOTTOM)
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
	center_overlay.position = Vector2(0.0, GameConfig.HUD_TOP)
	center_overlay.size = Vector2(GameConfig.WIDTH, GameConfig.HUD_BOTTOM - GameConfig.HUD_TOP)
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
