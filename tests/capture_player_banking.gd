extends Node2D

const PLAYER_SCENE := preload("res://scenes/entities/player.tscn")
const PLAYER_TYPE := preload("res://scripts/entities/player.gd")

func _ready() -> void:
	RenderingServer.set_default_clear_color(Color("15171d"))
	for index in range(3):
		var player := PLAYER_SCENE.instantiate() as PLAYER_TYPE
		player.position = Vector2(260.0 + index * 340.0, 300.0)
		player.scale = Vector2.ONE * 1.65
		add_child(player)
		player.shield = 0.0
		player.set_process(false)
		player.set_visual_bank([0.0, 1.0, -1.0][index], true)
		_add_label(["NEUTRAL", "DOWN · TOP", "UP · UNDERSIDE"][index], Vector2(155.0 + index * 340.0, 390.0))
	for _frame in range(12):
		await get_tree().process_frame
	await RenderingServer.frame_post_draw
	var output := "/tmp/capybara-player-banking.png"
	var error := get_viewport().get_texture().get_image().save_png(output)
	print("CAPTURE_OK: %s (%s)" % [output, error_string(error)])
	get_tree().quit(0 if error == OK else 1)

func _add_label(text: String, at: Vector2) -> void:
	var label := Label.new()
	label.text = text
	label.position = at
	label.size = Vector2(210.0, 30.0)
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	label.add_theme_font_size_override("font_size", 18)
	label.add_theme_color_override("font_color", Color("a9dfff"))
	add_child(label)
