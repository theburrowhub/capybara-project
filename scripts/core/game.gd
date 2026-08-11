class_name CapybaraGame
extends Node2D

signal return_to_menu(score: int, difficulty: int, qualifies: bool)
signal restart_requested(difficulty: int)

const ENEMY_SCENE := preload("res://scenes/entities/enemy.tscn")
const PROJECTILE_SCENE := preload("res://scenes/entities/projectile.tscn")
const POWERUP_SCENE := preload("res://scenes/entities/power_up.tscn")
const EXPLOSION_SCENE := preload("res://scenes/entities/explosion.tscn")

@export var level_index := 0
@export var difficulty := 1

var level_config: Dictionary
var score := 0
var game_time := 0.0
var level_time := 0.0
var speed_level := 1
var paused := false
var game_over := false
var victory := false
var debug_hitboxes := false
var boss_enemy: EnemyShip
var boss_spawn_time := -1.0
var boss_escape := false
var boss_escape_timer := 0.0
var level_transitioning := false
var transition_timer := 0.0

@onready var entities: Node2D = $Entities
@onready var player: PlayerShip = $Entities/Player
@onready var starfield: Starfield = $Starfield
@onready var director: WaveDirector = $WaveDirector
@onready var hud: GameHUD = $HUD
@onready var music: AudioStreamPlayer = $Music

func _ready() -> void:
	level_config = GameConfig.level(level_index)
	director.setup(level_index, difficulty)
	director.spawn_requested.connect(_spawn_enemy)
	director.boss_requested.connect(_spawn_boss)
	director.phase_changed.connect(_on_phase_changed)
	player.projectile_requested.connect(_spawn_projectile)
	player.died.connect(_on_player_died)
	player.revived.connect(func() -> void: hud.show_notice("WEAPON CORE CONSUMED · SHIP REVIVED"))
	hud.setup(self, player, director)
	_load_level_music()
	hud.show_notice("LEVEL %d · %s" % [level_index + 1, str(level_config["name"])], 3.0)

func _process(delta: float) -> void:
	if game_over:
		return
	if paused:
		return
	game_time += delta
	level_time += delta
	speed_level = int(game_time / 20.0) + 1
	starfield.scroll_speed = 2.0 + (speed_level - 1) * 0.5
	if not boss_escape and not level_transitioning:
		director.advance(delta, active_enemy_count())
	_update_boss_escape(delta)
	_update_level_transition(delta)
	if not level_transitioning and level_time >= float(level_config["duration"]) and not boss_escape:
		_begin_level_transition()
	elif not level_transitioning and float(level_config["duration"]) - level_time <= 15.0 and float(level_config["duration"]) - level_time > 14.9:
		hud.show_notice("LEVEL COMPLETE IN 15 SECONDS", 3.0)

func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed and not event.echo:
		match event.physical_keycode:
			KEY_P:
				if not game_over:
					_set_paused(not paused)
			KEY_H:
				debug_hitboxes = not debug_hitboxes
				player.set_meta("debug_hitbox", debug_hitboxes)
				for enemy in get_tree().get_nodes_in_group("enemy"):
					enemy.set_meta("debug_hitbox", debug_hitboxes)
				hud.show_notice("HITBOXES %s" % ("ON" if debug_hitboxes else "OFF"), 1.2)
			KEY_R:
				if game_over:
					restart_requested.emit(difficulty)
			KEY_ESCAPE:
				if paused:
					_set_paused(false)
				_request_menu()
	if event is InputEventKey and event.pressed and event.keycode == KEY_F11:
		GameData.settings["fullscreen"] = not bool(GameData.settings["fullscreen"])
		GameData.apply_settings()
		GameData.save_profile()

func _set_paused(value: bool) -> void:
	paused = value
	entities.process_mode = Node.PROCESS_MODE_DISABLED if paused else Node.PROCESS_MODE_INHERIT
	starfield.process_mode = Node.PROCESS_MODE_DISABLED if paused else Node.PROCESS_MODE_INHERIT
	director.process_mode = Node.PROCESS_MODE_DISABLED if paused else Node.PROCESS_MODE_INHERIT
	music.stream_paused = paused
	hud.show_pause(paused)

func _spawn_enemy(type: String, at: Vector2, pattern: String) -> void:
	if active_enemy_count() >= GameConfig.MAX_ENEMIES:
		return
	var enemy := ENEMY_SCENE.instantiate() as EnemyShip
	enemy.setup(type, pattern, GameConfig.difficulty_scale(difficulty), level_time >= float(level_config["fire_from"]))
	enemy.set_meta("debug_hitbox", debug_hitboxes)
	enemy.position = at
	entities.add_child(enemy)
	enemy.destroyed.connect(_on_enemy_destroyed)
	enemy.projectile_requested.connect(_spawn_projectile)

func _spawn_boss(at: Vector2) -> void:
	if is_instance_valid(boss_enemy):
		return
	_spawn_enemy("boss", at, "boss")
	var bosses := get_tree().get_nodes_in_group("enemy").filter(func(node: Node) -> bool: return node is EnemyShip and node.enemy_type == "boss")
	if not bosses.is_empty():
		boss_enemy = bosses[-1] as EnemyShip
		boss_spawn_time = level_time
		hud.show_notice("WARNING · BOSS SIGNATURE DETECTED", 4.0)

func _spawn_projectile(config: Dictionary) -> void:
	if get_tree().get_nodes_in_group("player_projectile").size() + get_tree().get_nodes_in_group("enemy_projectile").size() >= GameConfig.MAX_PROJECTILES:
		return
	var projectile := PROJECTILE_SCENE.instantiate() as GameProjectile
	projectile.setup(config)
	projectile.position = config.get("position", Vector2.ZERO)
	entities.add_child(projectile)
	projectile.impacted.connect(_spawn_explosion)

func _on_enemy_destroyed(type: String, points: int, at: Vector2) -> void:
	score += points
	director.register_kill()
	_spawn_explosion(at, Color("ff6b5d") if type != "boss" else Color("ff244d"), 70.0 if type == "boss" else 25.0)
	_try_drop_powerup(type, at)
	if type == "boss":
		boss_enemy = null
		score += 2500
		hud.show_notice("BOSS CORE DESTROYED · +2500", 4.0)

func _try_drop_powerup(type: String, at: Vector2) -> void:
	var rates: Array = GameConfig.DROP_RATES.get(type, [0.0, 0.0, 0.0, 0.0])
	var roll := randf() * 100.0
	var cumulative := 0.0
	var kinds := ["energy", "shield", "hull", "weapon"]
	for index in range(rates.size()):
		cumulative += float(rates[index])
		if roll <= cumulative:
			var powerup := POWERUP_SCENE.instantiate() as PowerUp
			powerup.setup(kinds[index])
			powerup.position = at
			entities.add_child(powerup)
			powerup.collected.connect(_on_powerup_collected)
			return

func _on_powerup_collected(_kind: String) -> void:
	score += 50

func _spawn_explosion(at: Vector2, color: Color, size: float) -> void:
	var explosion := EXPLOSION_SCENE.instantiate() as ExplosionEffect
	explosion.position = at
	explosion.setup(color, size)
	entities.add_child(explosion)

func _on_player_died() -> void:
	if boss_escape:
		return
	_spawn_explosion(player.global_position, Color("64d9ff"), 70.0)
	_end_game(false, "SHIP DESTROYED", "[R] RESTART   ·   [ESC] MAIN MENU")

func _update_boss_escape(delta: float) -> void:
	if not boss_escape and is_instance_valid(boss_enemy) and boss_spawn_time >= 0.0:
		var limit := 70.0 if level_index == 1 else 90.0
		if level_time - boss_spawn_time >= limit:
			boss_escape = true
			boss_escape_timer = 0.0
			director.running = false
			boss_enemy.begin_escape()
			hud.show_notice("DOOMSDAY SIGNAL · BOSS ESCAPING", 4.0)
	if not boss_escape:
		return
	boss_escape_timer += delta
	if boss_escape_timer >= 0.5 and not get_meta("escape_clear", false):
		set_meta("escape_clear", true)
		for node in get_tree().get_nodes_in_group("enemy_projectile") + get_tree().get_nodes_in_group("player_projectile"):
			if is_instance_valid(node):
				_spawn_explosion(node.global_position, Color("ffb84d"), 12.0)
				node.queue_free()
		for enemy in get_tree().get_nodes_in_group("enemy"):
			if enemy != boss_enemy and is_instance_valid(enemy):
				_spawn_explosion(enemy.global_position, Color("ff596d"), 24.0)
				enemy.queue_free()
	if boss_escape_timer >= 1.5 and player.active:
		player.weapon_powerups = 0
		player.take_damage(99999.0)
	if boss_escape_timer >= 3.5:
		_end_game(false, "DEFEAT", "THE BOSS ESCAPED AND OBLITERATED THE FLEET\n[R] RESTART   ·   [ESC] MAIN MENU")

func _begin_level_transition() -> void:
	if level_transitioning:
		return
	level_transitioning = true
	transition_timer = 0.0
	director.running = false
	if level_index + 1 < GameConfig.LEVELS.size():
		hud.show_overlay("LEVEL COMPLETE", "%s SECURED · PREPARING NEXT SECTOR" % str(level_config["name"]).to_upper())
	else:
		victory = true
		hud.show_overlay("VICTORY", "ALL SECTORS SECURED\nFINAL SCORE %09d" % score)

func _update_level_transition(delta: float) -> void:
	if not level_transitioning:
		return
	transition_timer += delta
	if transition_timer < 4.0:
		return
	if victory:
		_end_game(true, "VICTORY", "ALL SECTORS SECURED · FINAL SCORE %09d\n[ESC] MAIN MENU" % score)
		return
	level_index += 1
	level_config = GameConfig.level(level_index)
	level_time = 0.0
	level_transitioning = false
	transition_timer = 0.0
	boss_enemy = null
	boss_spawn_time = -1.0
	boss_escape = false
	remove_meta("escape_clear")
	for node in get_tree().get_nodes_in_group("enemy") + get_tree().get_nodes_in_group("enemy_projectile"):
		if is_instance_valid(node):
			node.queue_free()
	director.setup(level_index, difficulty)
	_load_level_music()
	hud.hide_overlay()
	hud.show_notice("LEVEL %d · %s" % [level_index + 1, str(level_config["name"])], 3.0)

func _end_game(won: bool, title: String, subtitle: String) -> void:
	if game_over:
		return
	game_over = true
	victory = won
	director.running = false
	entities.process_mode = Node.PROCESS_MODE_DISABLED
	starfield.process_mode = Node.PROCESS_MODE_DISABLED
	music.stop()
	hud.show_overlay(title, subtitle)

func _request_menu() -> void:
	var qualifies := GameData.is_high_score(score, difficulty)
	return_to_menu.emit(score, difficulty, qualifies)

func _on_phase_changed(name: String, _index: int) -> void:
	if level_time > 1.0:
		hud.show_notice(name.to_upper(), 1.8)

func _load_level_music() -> void:
	music.stop()
	var path := str(level_config["music"])
	if ResourceLoader.exists(path):
		music.stream = load(path)
		music.play()

func active_enemy_count() -> int:
	return get_tree().get_nodes_in_group("enemy").size()
