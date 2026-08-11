extends Node

func _ready() -> void:
	var menu := (load("res://scenes/main.tscn") as PackedScene).instantiate()
	get_tree().root.add_child.call_deferred(menu)
	for _frame in range(5):
		await get_tree().process_frame
	await RenderingServer.frame_post_draw
	var output := "/tmp/capybara-godot-menu.png"
	var error := get_viewport().get_texture().get_image().save_png(output)
	print("CAPTURE_OK: %s (%s)" % [output, error_string(error)])
	get_tree().quit(0 if error == OK else 1)
