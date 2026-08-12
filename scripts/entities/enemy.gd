class_name EnemyShip
extends Area2D

const CONFIG := preload("res://scripts/core/game_config.gd")

signal destroyed(enemy_type: String, points: int, at: Vector2)
signal projectile_requested(config: Dictionary)

var enemy_type := "grunt"
var pattern := "straight"
var max_health := 1.0
var health := 1.0
var speed := 180.0
var contact_damage := 10.0
var resistance := 0.0
var points := 100
var fire_rate := 1.0
var burst := 1
var projectile_kind := "laser"
var can_fire := true
var fire_timer := 1.0
var elapsed := 0.0
var radius := 20.0
var difficulty_damage := 1.0
var state := 0
var state_timer := 0.0
var stop_x := 700.0
var escaping := false
var base_tint := Color.WHITE

@onready var sprite: Sprite2D = $Sprite2D
@onready var collision_shape: CollisionShape2D = $CollisionShape2D

func setup(type: String, movement_pattern: String, difficulty: Dictionary, firing: bool) -> void:
	enemy_type = type if CONFIG.ENEMIES.has(type) else "grunt"
	pattern = movement_pattern
	can_fire = firing
	var data: Dictionary = CONFIG.ENEMIES[enemy_type]
	max_health = float(data["health"]) * float(difficulty["enemy_health"])
	health = max_health
	speed = float(data["speed"])
	contact_damage = float(data["power"]) * float(difficulty["enemy_damage"])
	difficulty_damage = float(difficulty["enemy_damage"])
	resistance = float(data["resistance"])
	points = int(round(float(data["points"]) * float(difficulty["score"])))
	fire_rate = float(data["fire_rate"])
	burst = int(data["burst"])
	projectile_kind = str(data["projectile"])
	radius = 22.0 * float(data["size"])
	stop_x = randf_range(520.0, 900.0)
	fire_timer = randf_range(0.4, 1.4) / maxf(fire_rate, 0.1)
	if is_node_ready():
		_apply_visuals()

func _ready() -> void:
	add_to_group("enemy")
	_apply_visuals()

func _apply_visuals() -> void:
	if not is_instance_valid(sprite) or not is_instance_valid(collision_shape):
		return
	var data: Dictionary = CONFIG.ENEMIES[enemy_type]
	var path := "res://assets/sprites/enemies_3d/%s.png" % str(data["sprite"])
	if ResourceLoader.exists(path):
		sprite.texture = load(path)
	else:
		path = "res://assets/sprites/enemies/enemy_%s.png" % enemy_type
		if ResourceLoader.exists(path):
			sprite.texture = load(path)
	base_tint = data["tint"] as Color
	sprite.modulate = base_tint
	sprite.rotation = float(data["sprite_rotation"])
	sprite.flip_h = bool(data["flip"])
	var visual_size := radius * 2.4
	if sprite.texture:
		sprite.scale = Vector2.ONE * (visual_size / maxf(sprite.texture.get_width(), 1.0))
	var shape := CircleShape2D.new()
	shape.radius = radius
	collision_shape.shape = shape

func _process(delta: float) -> void:
	elapsed += delta
	_update_movement(delta)
	_update_firing(delta)
	_update_special_visuals()
	if position.x < -100.0 or position.y < -100.0 or position.y > CONFIG.HEIGHT + 100.0:
		queue_free()
	queue_redraw()

func _update_movement(delta: float) -> void:
	if escaping:
		position.x += 420.0 * delta
		return
	if enemy_type == "boss":
		if position.x > 1010.0:
			position.x -= speed * delta
		position.y = clampf(position.y + sin(elapsed * 0.9) * 38.0 * delta, 105.0, 425.0)
		return
	match pattern:
		"zigzag":
			position += Vector2(-speed, sin(elapsed * 4.0) * speed * 0.65) * delta
		"erratic", "phasing":
			position += Vector2(-speed * 0.85, sin(elapsed * 6.3 + position.x * 0.01) * speed * 0.55) * delta
		"v_formation", "formation", "minion":
			position += Vector2(-speed, sin(elapsed * 2.2) * 55.0) * delta
		"hover":
			if position.x > stop_x:
				position.x -= speed * delta
			position.y += sin(elapsed * 2.0) * 35.0 * delta
		"tank_assault", "elite_retreat":
			_update_assault(delta)
		"flanking":
			position += Vector2(-speed * 1.35, signf(265.0 - position.y) * speed * 0.22) * delta
		"rush":
			position.x -= speed * 1.65 * delta
		_:
			position.x -= speed * delta
	if enemy_type == "speeder":
		position.y += sin(elapsed * 12.0) * 2.2

func _update_assault(delta: float) -> void:
	if state == 0:
		position.x -= speed * delta
		if position.x <= stop_x:
			state = 1
			state_timer = randf_range(3.0, 7.0)
	elif state == 1:
		state_timer -= delta
		if state_timer <= 0.0:
			state = 2
	else:
		position.x += speed * 1.3 * delta
		if position.x > CONFIG.WIDTH + 100.0:
			queue_free()

func _update_firing(delta: float) -> void:
	if not can_fire:
		return
	var player := get_tree().get_first_node_in_group("player") as Node2D
	if not is_instance_valid(player) or not bool(player.get("active")):
		return
	fire_timer -= delta
	if fire_timer > 0.0:
		return
	fire_timer = 1.0 / maxf(fire_rate, 0.1)
	var base_direction := global_position.direction_to(player.global_position)
	for index in range(burst):
		var angle := 0.0
		if enemy_type in ["shield", "boss"]:
			angle = TAU * float(index) / float(maxi(1, burst))
		elif burst > 1:
			angle = lerpf(-0.28, 0.28, float(index) / float(burst - 1))
		projectile_requested.emit({
			"kind": projectile_kind, "position": global_position + base_direction * radius,
			"direction": base_direction.rotated(angle), "damage_scale": difficulty_damage,
			"from_player": false, "target": player,
		})

func _update_special_visuals() -> void:
	if enemy_type == "ghost":
		var phased := fmod(elapsed, 4.0) > 2.0
		modulate.a = 0.28 if phased else 1.0
	elif enemy_type == "shield":
		rotation = elapsed * 0.8

func take_damage(amount: float) -> void:
	health -= maxf(0.1, amount * (1.0 - resistance))
	if health <= 0.0:
		destroyed.emit(enemy_type, points, global_position)
		queue_free()
	else:
		var tween := create_tween()
		tween.tween_property(sprite, "modulate", Color.WHITE * 2.5, 0.04)
		tween.tween_property(sprite, "modulate", base_tint, 0.08)

func begin_escape() -> void:
	escaping = true
	can_fire = false

func _draw() -> void:
	if max_health > 1.0 and health < max_health:
		var width := radius * 2.0
		draw_rect(Rect2(-width * 0.5, -radius - 11.0, width, 4.0), Color(0.3, 0.04, 0.06, 0.9))
		draw_rect(Rect2(-width * 0.5, -radius - 11.0, width * clampf(health / max_health, 0.0, 1.0), 4.0), Color("62e77f"))
	if enemy_type == "shield":
		draw_arc(Vector2.ZERO, radius + 8.0, 0.0, TAU, 28, Color(0.2, 0.85, 1.0, 0.55), 3.0)
	elif enemy_type == "boss":
		draw_arc(Vector2.ZERO, radius + 10.0, 0.0, TAU, 40, Color(1.0, 0.1, 0.18, 0.55), 4.0)
		draw_arc(Vector2.ZERO, radius + 18.0, -elapsed, PI - elapsed, 24, Color(0.8, 0.05, 0.12, 0.38), 3.0)
	if get_meta("debug_hitbox", false):
		draw_arc(Vector2.ZERO, radius, 0.0, TAU, 32, Color(1.0, 0.3, 0.72, 0.9), 2.0)
