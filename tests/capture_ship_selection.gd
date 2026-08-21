extends Node

const MAIN_SCENE := preload("res://scenes/main.tscn")

func _ready() -> void:
	var main := MAIN_SCENE.instantiate()
	get_tree().root.add_child.call_deferred(main)
	for _frame in range(5):
		await get_tree().process_frame
	main._show_ship_selection()
	for _frame in range(6):
		await get_tree().process_frame
	var selector = main.current_panel
	var ship_ids := ["vindicator", "sting", "goliat"]
	var exit_code := 0
	for index in range(ship_ids.size()):
		selector._select_ship(index)
		for _frame in range(12):
			await get_tree().process_frame
		await RenderingServer.frame_post_draw
		var output := "/tmp/capybara-ship-selection-%s.png" % ship_ids[index]
		var error := get_viewport().get_texture().get_image().save_png(output)
		print("CAPTURE_OK: %s (%s)" % [output, error_string(error)])
		if error != OK:
			exit_code = 1
	get_tree().quit(exit_code)
