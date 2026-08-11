extends Node

func _ready() -> void:
	var game := (load("res://scenes/game.tscn") as PackedScene).instantiate() as CapybaraGame
	get_tree().root.add_child.call_deferred(game)
	await get_tree().process_frame
	game.director.running = false
	for index in range(GameConfig.ENEMY_ORDER.size()):
		var column := index % 2
		var row := index / 2
		game._spawn_enemy(GameConfig.ENEMY_ORDER[index], Vector2(760.0 + column * 240.0, 90.0 + row * 82.0), "hover")
	game._spawn_projectile({"kind": "player_bullet", "position": Vector2(245.0, 265.0), "direction": Vector2.RIGHT, "damage": 1.0, "from_player": true})
	for _frame in range(8):
		await get_tree().process_frame
	await RenderingServer.frame_post_draw
	var output := "/tmp/capybara-godot-game.png"
	var error := get_viewport().get_texture().get_image().save_png(output)
	print("CAPTURE_OK: %s (%s)" % [output, error_string(error)])
	get_tree().quit(0 if error == OK else 1)
