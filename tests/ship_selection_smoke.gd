extends Node

const CONFIG := preload("res://scripts/core/game_config.gd")
const MAIN_SCENE := preload("res://scenes/main.tscn")
const SELECTOR_TYPE := preload("res://scripts/ui/ship_selection_panel.gd")

var failures: Array[String] = []

func _ready() -> void:
	call_deferred("_run")

func _check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error("SHIP_SELECTION: " + message)

func _run() -> void:
	_check(CONFIG.PLAYER_SHIP_ORDER == ["vindicator", "sting", "goliat"], "ship order matches the selection screen")
	_check(CONFIG.player_ship("vindicator")["radar"] == [3, 3, 3, 3, 3], "Vindicator is balanced")
	_check(CONFIG.player_ship("sting")["radar"] == [5, 2, 2, 2, 5], "Sting trades endurance and firepower for speed and recovery")
	_check(CONFIG.player_ship("goliat")["radar"] == [1, 5, 5, 5, 1], "Goliat trades speed and recovery for overwhelming durability and firepower")
	_check(FileAccess.file_exists(str(CONFIG.player_ship("sting")["model"])), "Sting GLB is bundled")
	_check(FileAccess.file_exists(str(CONFIG.player_ship("goliat")["model"])), "Goliat GLB is bundled")

	var selector := SELECTOR_TYPE.new() as ShipSelectionPanel
	get_tree().root.add_child(selector)
	await get_tree().process_frame
	await get_tree().process_frame
	_check(selector.selected_ship_id() == "vindicator", "selector defaults to Vindicator")
	var starting_rotation: float = selector.preview.model_pivot.rotation.y
	for _frame in range(4):
		await get_tree().process_frame
	_check(not is_equal_approx(selector.preview.model_pivot.rotation.y, starting_rotation), "showcase ship rotates continuously")
	selector._select_ship(1)
	_check(selector.selected_ship_id() == "sting", "selector changes to Sting")
	_check(selector.preview.current_ship_id == "sting", "showcase swaps to Sting GLB")
	_check(selector.radar.target_stats == [5.0, 2.0, 2.0, 2.0, 5.0], "radar updates to Sting stats")
	selector.queue_free()
	await get_tree().process_frame

	var main := MAIN_SCENE.instantiate()
	get_tree().root.add_child(main)
	await get_tree().process_frame
	await get_tree().process_frame
	main._show_ship_selection()
	await get_tree().process_frame
	var live_selector := main.current_panel as ShipSelectionPanel
	_check(is_instance_valid(live_selector), "START MISSION opens the selector")
	if is_instance_valid(live_selector):
		live_selector._select_ship(2)
		live_selector._deploy_current()
	await get_tree().process_frame
	await get_tree().process_frame
	_check(is_instance_valid(main.game_instance), "deploy starts gameplay")
	if is_instance_valid(main.game_instance):
		var player = main.game_instance.player
		_check(main.active_ship_id == "goliat", "main session keeps the selected ship")
		_check(player.ship_id == "goliat", "selected profile reaches the player before ready")
		_check(is_equal_approx(player.move_speed, 180.0), "Goliat movement is substantially slower")
		_check(is_equal_approx(player.max_health, 180.0), "Goliat hull is substantially stronger")
		_check(is_equal_approx(player.base_max_shield, 60.0), "Goliat shield is substantially stronger")
		_check(is_equal_approx(player.damage_multiplier, 2.25), "Goliat firepower is overwhelming")
		_check(is_equal_approx(player.energy_regen_rate, 0.5) and is_equal_approx(player.energy_regen_delay, 10.0), "Goliat bars recover extremely slowly")
		_check(is_equal_approx(player.model_scale, 0.608025), "Goliat model uses the normalized visual scale")
		main.game_instance.director.running = false
		main.game_instance.music.stop()
		main.game_instance.music.stream = null
		main._restart_game(1)
		for _frame in range(3):
			await get_tree().process_frame
		_check(is_instance_valid(main.game_instance) and main.game_instance.player.ship_id == "goliat", "restart keeps the selected ship without reopening the selector")
		if is_instance_valid(main.game_instance):
			main.game_instance.director.running = false
			main.game_instance.music.stop()
			main.game_instance.music.stream = null

	var exit_code := 0 if failures.is_empty() else 1
	if exit_code == 0:
		print("SHIP_SELECTION_OK: selector, rotating GLBs, radar and selected gameplay profile")
	else:
		print("SHIP_SELECTION_FAILED: %s" % ", ".join(failures))
	main.queue_free()
	await get_tree().process_frame
	get_tree().quit(exit_code)
