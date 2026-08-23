class_name GameProjectile
extends Area2D

const CONFIG := preload("res://scripts/core/game_config.gd")

signal impacted(at: Vector2, color: Color, size: float)

var kind := "laser"
var direction := Vector2.LEFT
var speed := 500.0
var damage := 10.0
var radius := 4.0
var lifetime := 3.0
var homing := 0.0
var from_player := false
var target: Node2D
var elapsed := 0.0
var wave_phase := 0.0
var base_direction := Vector2.LEFT
var trail: Array[Vector2] = []
var projectile_color := Color.WHITE

@onready var sprite: Sprite2D = $Sprite2D
@onready var collision_shape: CollisionShape2D = $CollisionShape2D

func setup(config: Dictionary) -> void:
	kind = str(config.get("kind", "laser"))
	var definition: Dictionary = CONFIG.PROJECTILES.get(kind, CONFIG.PROJECTILES["laser"])
	direction = (config.get("direction", Vector2.LEFT) as Vector2).normalized()
	base_direction = direction
	speed = float(definition["speed"]) * float(config.get("speed_scale", 1.0))
	damage = float(config.get("damage", definition["damage"])) * float(config.get("damage_scale", 1.0))
	radius = float(definition["radius"]) * float(config.get("radius_scale", 1.0))
	lifetime = float(definition["lifetime"])
	homing = float(definition["homing"])
	from_player = bool(config.get("from_player", false))
	target = config.get("target") as Node2D
	projectile_color = config.get("color", definition["color"]) as Color
	if is_node_ready():
		_apply_setup()

func _ready() -> void:
	add_to_group("player_projectile" if from_player else "enemy_projectile")
	area_entered.connect(_on_area_entered)
	_apply_setup()

func _apply_setup() -> void:
	if not is_instance_valid(sprite):
		return
	var texture_kind := "laser" if kind == "player_bullet" else kind
	var texture_path := "res://assets/sprites/projectiles/projectile_%s.png" % texture_kind
	if ResourceLoader.exists(texture_path):
		sprite.texture = load(texture_path)
	if sprite.texture:
		sprite.scale = Vector2.ONE * ((radius * 3.0) / maxf(sprite.texture.get_width(), 1.0))
	sprite.modulate = projectile_color
	rotation = direction.angle()
	var shape := CircleShape2D.new()
	shape.radius = radius
	collision_shape.shape = shape
	if from_player:
		collision_layer = 4
		collision_mask = 2
	else:
		collision_layer = 8
		collision_mask = 1

func _process(delta: float) -> void:
	elapsed += delta
	lifetime -= delta
	if lifetime <= 0.0:
		queue_free()
		return
	trail.push_front(global_position)
	if trail.size() > 7:
		trail.pop_back()
	if homing > 0.0 and is_instance_valid(target):
		var desired := global_position.direction_to(target.global_position)
		direction = direction.slerp(desired, minf(1.0, homing * delta * 2.5)).normalized()
	if kind == "energy_orb":
		wave_phase += delta * 4.0
		var perpendicular := Vector2(-base_direction.y, base_direction.x)
		position += (direction * speed + perpendicular * sin(wave_phase) * 65.0) * delta
	else:
		position += direction * speed * delta
	rotation = direction.angle()
	if position.x < -120.0 or position.x > CONFIG.WIDTH + 120.0 or position.y < -120.0 or position.y > CONFIG.HEIGHT + 120.0:
		queue_free()
	queue_redraw()

func _on_area_entered(area: Area2D) -> void:
	if from_player and area.is_in_group("enemy") and area.has_method("take_damage"):
		area.take_damage(damage)
		_hit()
	elif not from_player and area.is_in_group("player") and area.has_method("take_damage"):
		area.take_damage(damage)
		_hit()

func _hit() -> void:
	impacted.emit(global_position, projectile_color, radius)
	queue_free()

func _draw() -> void:
	for index in range(1, trail.size()):
		var local_point := to_local(trail[index])
		var alpha := (1.0 - float(index) / float(trail.size())) * 0.35
		draw_circle(local_point, maxf(1.0, radius * (1.0 - float(index) / float(trail.size()))), Color(projectile_color, alpha))
	if kind == "player_bullet":
		draw_circle(Vector2.ZERO, radius * 1.8, Color(projectile_color, 0.22))
