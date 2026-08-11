class_name WaveDirector
extends Node

signal spawn_requested(enemy_type: String, position: Vector2, pattern: String)
signal phase_changed(name: String, index: int)
signal boss_requested(position: Vector2)

var level_config: Dictionary
var difficulty_config: Dictionary
var level_time := 0.0
var spawn_timer := 0.0
var current_phase := -1
var total_spawned := 0
var total_killed := 0
var boss_spawned := false
var running := true

func setup(level_index: int, difficulty: int) -> void:
	level_config = GameConfig.level(level_index)
	difficulty_config = GameConfig.difficulty_scale(difficulty)
	level_time = 0.0
	spawn_timer = 0.05
	current_phase = -1
	total_spawned = 0
	total_killed = 0
	boss_spawned = false
	running = true

func advance(delta: float, active_enemy_count: int) -> void:
	if not running or level_config.is_empty():
		return
	level_time += delta
	var phase_index := _find_phase(level_time)
	if phase_index != current_phase:
		current_phase = phase_index
		spawn_timer = minf(spawn_timer, 0.25)
		phase_changed.emit(phase_name(), current_phase)
	if not boss_spawned and level_time >= float(level_config["boss_time"]):
		boss_spawned = true
		boss_requested.emit(Vector2(GameConfig.WIDTH + 110.0, 265.0))
	if phase_index < 0 or active_enemy_count >= GameConfig.MAX_ENEMIES:
		return
	spawn_timer -= delta
	if spawn_timer > 0.0:
		return
	var phase: Dictionary = level_config["phases"][phase_index]
	var rate_scale := float(difficulty_config["spawn_rate"])
	spawn_timer = float(phase["interval"]) / rate_scale * randf_range(0.82, 1.18)
	var roster: Array = phase["roster"]
	if roster.is_empty():
		return
	var count := int(phase["count"])
	var center_y := randf_range(GameConfig.HUD_TOP + 55.0, GameConfig.HUD_BOTTOM - 55.0)
	var spacing := 42.0
	for index in range(count):
		if active_enemy_count + index >= GameConfig.MAX_ENEMIES:
			break
		var type := str(roster[randi() % roster.size()])
		var y := clampf(center_y + (float(index) - float(count - 1) * 0.5) * spacing, GameConfig.HUD_TOP + 38.0, GameConfig.HUD_BOTTOM - 38.0)
		spawn_requested.emit(type, Vector2(GameConfig.WIDTH + 70.0 + index * 30.0, y), str(phase["pattern"]))
		total_spawned += 1

func register_kill() -> void:
	total_killed += 1

func phase_name() -> String:
	if current_phase < 0 or level_config.is_empty():
		return "Approach"
	return str(level_config["phases"][current_phase]["name"])

func progress() -> float:
	if level_config.is_empty():
		return 0.0
	return clampf(level_time / float(level_config["duration"]), 0.0, 1.0)

func _find_phase(time: float) -> int:
	var phases: Array = level_config.get("phases", [])
	for index in range(phases.size()):
		var phase: Dictionary = phases[index]
		if time >= float(phase["start"]) and time < float(phase["end"]):
			return index
	return -1
