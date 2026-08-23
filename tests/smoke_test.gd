extends Node

const CONFIG := preload("res://scripts/core/game_config.gd")
const ENEMY_TYPE := preload("res://scripts/entities/enemy.gd")
const GAME_TYPE := preload("res://scripts/core/game.gd")
const PLAYER_TYPE := preload("res://scripts/entities/player.gd")

var failures: Array[String] = []

func _ready() -> void:
	call_deferred("_run")

func _check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error("SMOKE: " + message)

func _capture_spread_angles(ship_id: String, volley_count: int, volley_sizes: Array[int]) -> Array[float]:
	var test_player := PLAYER_TYPE.new() as PlayerShip
	test_player.configure_ship(ship_id)
	test_player._apply_ship_profile()
	test_player.weapon_mode = PLAYER_TYPE.WeaponMode.SPREAD
	var angles: Array[float] = []
	test_player.projectile_requested.connect(func(config: Dictionary) -> void:
		var direction: Vector2 = config.get("direction", Vector2.ZERO)
		angles.append(direction.angle())
	)
	for _volley in range(volley_count):
		var previous_size := angles.size()
		test_player._fire_current_mode()
		volley_sizes.append(angles.size() - previous_size)
	test_player.free()
	return angles

func _angles_match(actual: Array[float], expected: Array[float]) -> bool:
	if actual.size() != expected.size():
		return false
	for index in range(expected.size()):
		if not is_equal_approx(actual[index], expected[index]):
			return false
	return true

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
	var vindicator_volley_sizes: Array[int] = []
	var sting_volley_sizes: Array[int] = []
	var goliat_volley_sizes: Array[int] = []
	var vindicator_spread := _capture_spread_angles("vindicator", 1, vindicator_volley_sizes)
	var sting_spread := _capture_spread_angles("sting", 4, sting_volley_sizes)
	var goliat_spread := _capture_spread_angles("goliat", 1, goliat_volley_sizes)
	_check(vindicator_volley_sizes == [3], "Vindicator still fires all three spread shells per trigger")
	_check(_angles_match(vindicator_spread, [-0.20, 0.0, 0.20]), "Vindicator keeps its simultaneous three-way spread")
	_check(sting_volley_sizes == [1, 1, 1, 1], "Sting fires only one spread shell per trigger")
	_check(_angles_match(sting_spread, [-0.20, 0.0, 0.20, -0.20]), "Sting cycles one spread shell through upper, center and lower directions")
	_check(goliat_volley_sizes == [6], "Goliat fires all six spread shells per trigger")
	_check(_angles_match(goliat_spread, [-0.55, -0.33, -0.11, 0.11, 0.33, 0.55]), "Goliat fires six simultaneous spread shells across a wider cone")
	_check(game.player.has_node("Ship3DViewport"), "GLB player model is rendered through a SubViewport")
	_check(FileAccess.file_exists("res://assets/models/player_ship.glb"), "player GLB source is bundled")
	var ship_viewport := game.player.get_node("Ship3DViewport")
	var shield_sphere := ship_viewport.find_child("ShieldSphere", true, false) as MeshInstance3D
	var defensive_special := ship_viewport.find_child("DefensiveSpecialIcosahedron", true, false) as MeshInstance3D
	_check(is_instance_valid(shield_sphere), "normal defense uses a 3D sphere")
	_check(is_instance_valid(defensive_special), "defensive special uses a 3D polyhedron")
	_check(int(defensive_special.get_meta("face_count", 0)) == 20, "defensive special polyhedron has exactly twenty faces")
	_check(defensive_special.mesh.surface_get_array_len(0) == 60, "defensive special contains twenty triangular faces")
	_check(is_equal_approx((shield_sphere.material_override as StandardMaterial3D).albedo_color.a, 0.15), "shield sphere is eighty-five percent transparent")
	_check(is_equal_approx((defensive_special.material_override as StandardMaterial3D).albedo_color.a, 0.15), "defensive special is eighty-five percent transparent")
	_check(shield_sphere.visible and not defensive_special.visible, "normal shield geometry is exclusive")
	game.player.energy_mode = PLAYER_TYPE.EnergyMode.DEFENSIVE
	game.player.special_active = true
	game.player._update_defense_visuals()
	_check(not shield_sphere.visible and defensive_special.visible, "defensive special replaces the shield sphere")
	var protected_shield := game.player.shield
	var protected_health := game.player.health
	game.player.take_damage(20.0)
	_check(is_equal_approx(game.player.shield, protected_shield), "defensive special absorbs impacts without consuming shield")
	_check(is_equal_approx(game.player.health, protected_health), "defensive special prevents impact damage to the hull")
	game.player.special_active = false
	game.player._update_defense_visuals()
	_check(is_equal_approx(game.player.bank, -PI * 0.5), "player rests in side-profile orientation")
	game.player.set_visual_bank(1.0, true)
	_check(is_zero_approx(game.player.bank), "moving down exposes the top of the player ship")
	game.player.set_visual_bank(-1.0, true)
	_check(is_equal_approx(game.player.bank, -PI), "moving up exposes the underside of the player ship")
	game.player.set_visual_bank(0.0, true)
	for index in range(CONFIG.ENEMY_ORDER.size()):
		game._spawn_enemy(CONFIG.ENEMY_ORDER[index], Vector2(760.0 + index * 22.0, 80.0 + index * 35.0), "straight")
	await get_tree().process_frame
	var enemies := get_tree().get_nodes_in_group("enemy")
	_check(enemies.size() == CONFIG.ENEMY_ORDER.size(), "all ten enemy types spawn")
	var model_sprites: Dictionary = {}
	for enemy_node in enemies:
		var enemy := enemy_node as ENEMY_TYPE
		enemy.set_visual_bank(0.0, true)
		if enemy.sprite.texture:
			model_sprites[enemy.sprite.texture.resource_path] = true
		_check(enemy._bank_textures.size() == 9, "%s loads all nine banked sprite frames" % enemy.enemy_type)
		_check(enemy.bank_frame == 4, "%s starts in mirrored side-profile orientation" % enemy.enemy_type)
		_check(not enemy.sprite.flip_h, "%s consistently faces left" % enemy.enemy_type)
		var sprite_image := enemy.sprite.texture.get_image()
		var visible_width := float(sprite_image.get_used_rect().size.x) * absf(enemy.sprite.scale.x)
		_check(visible_width > enemy.radius * 2.8 and visible_width < enemy.radius * 3.2, "%s uses its visible pixels for proportional scaling" % enemy.enemy_type)
	_check(model_sprites.size() == 4, "four selected enemy GLBs provide visible neutral runtime sprites")
	if not enemies.is_empty():
		var banking_enemy := enemies[0] as ENEMY_TYPE
		banking_enemy.set_visual_bank(1.0, true)
		_check(banking_enemy.bank_frame == 8, "enemy moving down exposes the top of its mirrored ship")
		banking_enemy.set_visual_bank(-1.0, true)
		_check(banking_enemy.bank_frame == 0, "enemy moving up exposes the underside of its mirrored ship")
		banking_enemy.set_visual_bank(0.0, true)
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
