extends Node

const CONFIG := preload("res://scripts/core/game_config.gd")
const ENEMY_TYPE := preload("res://scripts/entities/enemy.gd")
const GAME_TYPE := preload("res://scripts/core/game.gd")

var failures: Array[String] = []

func _ready() -> void:
	call_deferred("_run")

func _check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error("SMOKE: " + message)

func _run() -> void:
	var game_scene := load("res://scenes/game.tscn") as PackedScene
	_check(game_scene != null, "game scene loads")
	if game_scene == null:
		get_tree().quit(1)
		return
	var game := game_scene.instantiate() as GAME_TYPE
	get_tree().root.add_child(game)
	game.director.running = false
	await get_tree().process_frame
	await get_tree().process_frame
	_check(is_instance_valid(game.player), "player exists")
	_check(game.player.has_node("Ship3DViewport"), "GLB player model is rendered through a SubViewport")
	_check(FileAccess.file_exists("res://assets/models/player_ship.glb"), "player GLB source is bundled")
	for index in range(CONFIG.ENEMY_ORDER.size()):
		game._spawn_enemy(CONFIG.ENEMY_ORDER[index], Vector2(760.0 + index * 22.0, 80.0 + index * 35.0), "straight")
	await get_tree().process_frame
	var enemies := get_tree().get_nodes_in_group("enemy")
	_check(enemies.size() == CONFIG.ENEMY_ORDER.size(), "all ten enemy types spawn")
	var model_sprites: Dictionary = {}
	for enemy_node in enemies:
		var enemy := enemy_node as ENEMY_TYPE
		if enemy.sprite.texture:
			model_sprites[enemy.sprite.texture.resource_path] = true
	_check(model_sprites.size() == 4, "four selected enemy GLBs provide the runtime sprites")
	if not enemies.is_empty():
		(enemies[0] as ENEMY_TYPE).take_damage(9999.0)
	await get_tree().process_frame
	_check(game.score > 0, "enemy destruction awards score")
	game._spawn_projectile({"kind": "player_bullet", "position": Vector2(250.0, 240.0), "direction": Vector2.RIGHT, "damage": 1.0, "from_player": true})
	await get_tree().process_frame
	_check(get_tree().get_nodes_in_group("player_projectile").size() == 1, "projectile spawning works")
	game._try_drop_powerup("boss", Vector2(500.0, 250.0))
	await get_tree().process_frame
	_check(get_tree().get_nodes_in_group("powerup").size() >= 1, "power-up spawning works")
	var exit_code := 0 if failures.is_empty() else 1
	if exit_code == 0:
		print("SMOKE_OK: Godot gameplay scene, GLB ship, enemies, projectiles and power-ups")
	else:
		print("SMOKE_FAILED: %s" % ", ".join(failures))
	game.music.stop()
	game.music.stream = null
	game.music.free()
	game.queue_free()
	await get_tree().process_frame
	await get_tree().process_frame
	await get_tree().create_timer(0.5).timeout
	get_tree().quit(exit_code)
