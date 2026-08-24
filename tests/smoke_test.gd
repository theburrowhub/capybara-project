extends Node

const CONFIG := preload("res://scripts/core/game_config.gd")
const ENEMY_TYPE := preload("res://scripts/entities/enemy.gd")
const GAME_TYPE := preload("res://scripts/core/game.gd")
const PLAYER_TYPE := preload("res://scripts/entities/player.gd")
const PROJECTILE_TYPE := preload("res://scripts/entities/projectile.gd")

var failures: Array[String] = []

func _ready() -> void:
	call_deferred("_run")

func _check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error("SMOKE: " + message)

func _capture_mode(ship_id: String, mode: int, trigger_count: int) -> Dictionary:
	var test_player := PLAYER_TYPE.new() as PlayerShip
	test_player.configure_ship(ship_id)
	test_player._apply_ship_profile()
	test_player.weapon_mode = mode
	var shots: Array[Dictionary] = []
	var volley_sizes: Array[int] = []
	var cooldowns: Array[float] = []
	test_player.projectile_requested.connect(func(config: Dictionary) -> void:
		shots.append(config.duplicate(true))
	)
	for _trigger in range(trigger_count):
		var previous_size := shots.size()
		test_player._fire_current_mode()
		volley_sizes.append(shots.size() - previous_size)
		cooldowns.append(test_player.fire_timer)
	test_player.free()
	return {"shots": shots, "volley_sizes": volley_sizes, "cooldowns": cooldowns}

func _capture_charge(ship_id: String, level: float) -> Dictionary:
	var test_player := PLAYER_TYPE.new() as PlayerShip
	test_player.configure_ship(ship_id)
	test_player._apply_ship_profile()
	var shots: Array[Dictionary] = []
	test_player.projectile_requested.connect(func(config: Dictionary) -> void:
		shots.append(config.duplicate(true))
	)
	test_player.charge_level = level
	test_player._fire_charge()
	var charge_rate := test_player._charge_rate()
	test_player.free()
	return {"shots": shots, "charge_rate": charge_rate}

func _positions_match(pattern: Dictionary, expected: Array[Vector2]) -> bool:
	var shots: Array = pattern["shots"]
	if shots.size() != expected.size():
		return false
	for index in range(expected.size()):
		var position: Vector2 = shots[index].get("position", Vector2.ZERO)
		if not position.is_equal_approx(expected[index]):
			return false
	return true

func _directions_match(pattern: Dictionary, expected_angles: Array[float]) -> bool:
	var shots: Array = pattern["shots"]
	if shots.size() != expected_angles.size():
		return false
	for index in range(expected_angles.size()):
		var direction: Vector2 = shots[index].get("direction", Vector2.ZERO)
		if absf(angle_difference(direction.angle(), expected_angles[index])) > 0.0001:
			return false
	return true

func _damage_matches(pattern: Dictionary, expected_damage: float) -> bool:
	for shot: Dictionary in pattern["shots"]:
		if not is_equal_approx(float(shot.get("damage", 0.0)), expected_damage):
			return false
	return true

func _cooldowns_match(pattern: Dictionary, expected_cooldown: float) -> bool:
	for cooldown: float in pattern["cooldowns"]:
		if not is_equal_approx(cooldown, expected_cooldown):
			return false
	return true

func _check_mode_pattern(
	label: String,
	pattern: Dictionary,
	expected_sizes: Array[int],
	expected_cooldown: float,
	expected_positions: Array[Vector2],
	expected_angles: Array[float],
	expected_damage: float,
) -> void:
	_check(pattern["volley_sizes"] == expected_sizes, "%s uses the expected shells per trigger" % label)
	_check(_cooldowns_match(pattern, expected_cooldown), "%s uses the expected cooldown" % label)
	_check(_positions_match(pattern, expected_positions), "%s uses the expected firing ports" % label)
	_check(_directions_match(pattern, expected_angles), "%s uses the expected directions" % label)
	_check(_damage_matches(pattern, expected_damage), "%s uses the expected damage per shell" % label)

func _check_charge_pattern(
	label: String,
	pattern: Dictionary,
	expected_count: int,
	expected_spread: float,
	expected_damage: float,
	expected_rate: float,
) -> void:
	var shots: Array = pattern["shots"]
	_check(shots.size() == expected_count, "%s uses the expected shell count" % label)
	if shots.size() == expected_count and not shots.is_empty():
		var first_direction: Vector2 = shots[0].get("direction", Vector2.ZERO)
		var last_direction: Vector2 = shots[-1].get("direction", Vector2.ZERO)
		_check(absf(angle_difference(first_direction.angle(), -expected_spread * 0.5)) <= 0.0001, "%s starts at the expected upper angle" % label)
		_check(absf(angle_difference(last_direction.angle(), expected_spread * 0.5)) <= 0.0001, "%s ends at the expected lower angle" % label)
	_check(_damage_matches(pattern, expected_damage), "%s uses the expected damage per shell" % label)
	_check(is_equal_approx(float(pattern["charge_rate"]), expected_rate), "%s uses the expected charge rate" % label)

func _check_projectile_style(label: String, pattern: Dictionary, expected_speed: float, expected_radius: float, expected_color: Color) -> void:
	var shots: Array = pattern["shots"]
	if shots.is_empty():
		_check(false, "%s provides a projectile config" % label)
		return
	var projectile := PROJECTILE_TYPE.new() as GameProjectile
	projectile.setup(shots[0])
	_check(is_equal_approx(projectile.speed, expected_speed), "%s uses the expected projectile speed" % label)
	_check(is_equal_approx(projectile.radius, expected_radius), "%s uses the expected projectile size" % label)
	_check(projectile.projectile_color.is_equal_approx(expected_color), "%s uses the expected projectile color" % label)
	projectile.free()

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
	var vindicator_single := _capture_mode("vindicator", PLAYER_TYPE.WeaponMode.SINGLE, 1)
	_check_mode_pattern("Vindicator Single", vindicator_single, [1], 0.12, [Vector2(30.0, 0.0)], [0.0], 1.0)
	_check_mode_pattern("Vindicator Double", _capture_mode("vindicator", PLAYER_TYPE.WeaponMode.DOUBLE, 1), [2], 0.12, [Vector2(30.0, -10.0), Vector2(30.0, 10.0)], [0.0, 0.0], 1.0)
	_check_mode_pattern("Vindicator Spread", _capture_mode("vindicator", PLAYER_TYPE.WeaponMode.SPREAD, 1), [3], 0.12, [Vector2(30.0, 0.0), Vector2(30.0, 0.0), Vector2(30.0, 0.0)], [-0.20, 0.0, 0.20], 1.0)
	_check_mode_pattern("Vindicator Rapid", _capture_mode("vindicator", PLAYER_TYPE.WeaponMode.RAPID, 1), [1], 0.06, [Vector2(30.0, 0.0)], [0.0], 1.0)
	_check_mode_pattern("Vindicator Dual", _capture_mode("vindicator", PLAYER_TYPE.WeaponMode.DUAL, 1), [2], 0.12, [Vector2(30.0, 0.0), Vector2(-30.0, 0.0)], [0.0, PI], 1.0)
	_check_charge_pattern("Vindicator minimum Charge", _capture_charge("vindicator", 0.20), 3, 0.51, 1.40, 1.50)
	_check_charge_pattern("Vindicator full Charge", _capture_charge("vindicator", 1.0), 11, 1.15, 3.0, 1.50)

	var sting_single := _capture_mode("sting", PLAYER_TYPE.WeaponMode.SINGLE, 1)
	_check_mode_pattern("Sting Single", sting_single, [1], 0.08, [Vector2(30.0, 0.0)], [0.0], 0.70)
	_check_mode_pattern("Sting Double", _capture_mode("sting", PLAYER_TYPE.WeaponMode.DOUBLE, 3), [1, 1, 1], 0.06, [Vector2(30.0, -10.0), Vector2(30.0, 10.0), Vector2(30.0, -10.0)], [0.0, 0.0, 0.0], 0.70)
	_check_mode_pattern("Sting Spread", _capture_mode("sting", PLAYER_TYPE.WeaponMode.SPREAD, 4), [1, 1, 1, 1], 0.08, [Vector2(30.0, 0.0), Vector2(30.0, 0.0), Vector2(30.0, 0.0), Vector2(30.0, 0.0)], [-0.20, 0.0, 0.20, -0.20], 0.70)
	_check_mode_pattern("Sting Rapid", _capture_mode("sting", PLAYER_TYPE.WeaponMode.RAPID, 3), [1, 1, 1], 0.04, [Vector2(30.0, -6.0), Vector2(30.0, 6.0), Vector2(30.0, -6.0)], [0.0, 0.0, 0.0], 0.70)
	_check_mode_pattern("Sting Dual", _capture_mode("sting", PLAYER_TYPE.WeaponMode.DUAL, 3), [1, 1, 1], 0.06, [Vector2(30.0, 0.0), Vector2(-30.0, 0.0), Vector2(30.0, 0.0)], [0.0, PI, 0.0], 0.70)
	_check_charge_pattern("Sting minimum Charge", _capture_charge("sting", 0.20), 2, 0.12, 0.98, 2.50)
	_check_charge_pattern("Sting full Charge", _capture_charge("sting", 1.0), 6, 0.35, 2.10, 2.50)

	var goliat_single := _capture_mode("goliat", PLAYER_TYPE.WeaponMode.SINGLE, 1)
	_check_mode_pattern("Goliat Single", goliat_single, [1], 0.18, [Vector2(30.0, 0.0)], [0.0], 2.25)
	_check_mode_pattern("Goliat Double", _capture_mode("goliat", PLAYER_TYPE.WeaponMode.DOUBLE, 1), [2], 0.18, [Vector2(30.0, -18.0), Vector2(30.0, 18.0)], [-0.03, 0.03], 2.25)
	_check_mode_pattern("Goliat Spread", _capture_mode("goliat", PLAYER_TYPE.WeaponMode.SPREAD, 1), [6], 0.20, [Vector2(30.0, 0.0), Vector2(30.0, 0.0), Vector2(30.0, 0.0), Vector2(30.0, 0.0), Vector2(30.0, 0.0), Vector2(30.0, 0.0)], [-0.55, -0.33, -0.11, 0.11, 0.33, 0.55], 2.25)
	_check_mode_pattern("Goliat Rapid", _capture_mode("goliat", PLAYER_TYPE.WeaponMode.RAPID, 1), [2], 0.10, [Vector2(30.0, -8.0), Vector2(30.0, 8.0)], [0.0, 0.0], 2.25)
	_check_mode_pattern("Goliat Dual", _capture_mode("goliat", PLAYER_TYPE.WeaponMode.DUAL, 1), [4], 0.24, [Vector2(30.0, -15.0), Vector2(30.0, 15.0), Vector2(-30.0, -15.0), Vector2(-30.0, 15.0)], [0.0, 0.0, PI, PI], 2.25)
	_check_charge_pattern("Goliat minimum Charge", _capture_charge("goliat", 0.20), 4, 0.70, 3.15, 0.75)
	_check_charge_pattern("Goliat full Charge", _capture_charge("goliat", 1.0), 15, 1.50, 6.75, 0.75)

	_check_projectile_style("Vindicator", vindicator_single, 600.0, 4.0, Color("ffe45e"))
	_check_projectile_style("Sting", sting_single, 750.0, 3.0, Color("35a7ff"))
	_check_projectile_style("Goliat", goliat_single, 480.0, 6.4, Color("5de071"))
	var vindicator_shots: Array = vindicator_single["shots"]
	var vindicator_config: Dictionary = vindicator_shots[0]
	_check(not vindicator_config.has("speed_scale") and not vindicator_config.has("radius_scale") and not vindicator_config.has("color"), "Vindicator emits no projectile overrides")
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
	var destruction_start_y := game.player.position.y
	game.player.shield = 0.0
	game.player.weapon_powerups = 0
	game.player.take_damage(game.player.health)
	_check(game.player_destruction_active, "lethal hull damage starts the player destruction sequence")
	_check(not game.game_over, "game over waits for the player destruction sequence")
	_check(game.player.visible and not game.player.active, "destroyed player remains visible but inactive during descent")
	var initial_destruction_effects := game.entities.get_children().filter(func(node: Node) -> bool: return node is ExplosionEffect).size()
	_check(initial_destruction_effects >= 1, "player destruction starts with a small explosion")
	game._update_player_destruction(GAME_TYPE.PLAYER_DESTRUCTION_EXPLOSION_INTERVAL)
	_check(game.player.position.y > destruction_start_y, "destroyed player descends toward the lower edge")
	var repeated_destruction_effects := game.entities.get_children().filter(func(node: Node) -> bool: return node is ExplosionEffect).size()
	_check(repeated_destruction_effects > initial_destruction_effects, "small explosions continue during the descent")
	game._update_player_destruction(GAME_TYPE.PLAYER_DESTRUCTION_DESCENT_DURATION)
	_check(game.player_destruction_reached_edge, "player destruction reaches the lower edge before game over")
	_check(is_equal_approx(game.player.position.y, GAME_TYPE.PLAYER_DESTRUCTION_EDGE_Y), "destroyed player stops at the lower gameplay edge")
	_check(not game.player.visible and not game.game_over, "player disappears in the final explosion before game over")
	var final_explosion := game.entities.get_child(-1) as ExplosionEffect
	_check(is_instance_valid(final_explosion) and is_equal_approx(final_explosion.size, GAME_TYPE.PLAYER_DESTRUCTION_FINAL_SIZE), "player disappears in a large final explosion")
	game._update_player_destruction(GAME_TYPE.PLAYER_DESTRUCTION_FINAL_DELAY)
	_check(game.game_over, "game over appears after the final explosion has played")
	var exit_code := 0 if failures.is_empty() else 1
	if exit_code == 0:
		print("SMOKE_OK: Godot gameplay scene, combat systems and staged player destruction")
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
