extends Node2D

const PLAYER_SCENE := preload("res://scenes/entities/player.tscn")
const PLAYER_TYPE := preload("res://scripts/entities/player.gd")

func _ready() -> void:
	RenderingServer.set_default_clear_color(Color("15171d"))
	var shield_player := _add_player(Vector2(220.0, 290.0), "DEFENSE · 85% TRANSPARENT")
	shield_player.shield = shield_player.max_shield
	shield_player._update_defense_visuals()
	var special_player := _add_player(Vector2(600.0, 290.0), "DEFENSIVE SPECIAL · 20 FACES")
	special_player.energy_mode = PLAYER_TYPE.EnergyMode.DEFENSIVE
	special_player.special_active = true
	special_player._update_defense_visuals()
	var offensive_player := _add_player(Vector2(980.0, 290.0), "OFFENSIVE SPECIAL · DAMAGE ×2")
	offensive_player.energy_mode = PLAYER_TYPE.EnergyMode.OFFENSIVE
	offensive_player.special_active = true
	offensive_player.queue_redraw()
	for _frame in range(12):
		await get_tree().process_frame
	await RenderingServer.frame_post_draw
	var output := "/tmp/capybara-player-defenses.png"
	var error := get_viewport().get_texture().get_image().save_png(output)
	print("CAPTURE_OK: %s (%s)" % [output, error_string(error)])
	get_tree().quit(0 if error == OK else 1)

func _add_player(at: Vector2, label_text: String) -> PLAYER_TYPE:
	var player := PLAYER_SCENE.instantiate() as PLAYER_TYPE
	player.position = at
	player.scale = Vector2.ONE * 1.75
	add_child(player)
	player.set_process(false)
	player.set_visual_bank(1.0, true)
	var label := Label.new()
	label.text = label_text
	label.position = at + Vector2(-180.0, 130.0)
	label.size = Vector2(360.0, 32.0)
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.add_theme_font_size_override("font_size", 15)
	label.add_theme_color_override("font_color", Color("a9dfff"))
	add_child(label)
	return player
