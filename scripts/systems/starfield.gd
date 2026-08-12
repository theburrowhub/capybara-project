class_name Starfield
extends Node2D

const CONFIG := preload("res://scripts/core/game_config.gd")

var stars: Array[Dictionary] = []
var scroll_speed := 2.0

func _ready() -> void:
	var generator := RandomNumberGenerator.new()
	generator.seed = 0xCA9BA4A
	for index in range(150):
		stars.append({
			"position": Vector2(generator.randf_range(0.0, CONFIG.WIDTH), generator.randf_range(CONFIG.HUD_TOP, CONFIG.HUD_BOTTOM)),
			"layer": 1 + index % 3,
			"twinkle": generator.randf_range(0.0, TAU),
		})

func _process(delta: float) -> void:
	for star in stars:
		var position: Vector2 = star["position"]
		position.x -= float(star["layer"]) * scroll_speed * 32.0 * delta
		if position.x < -4.0:
			position.x = CONFIG.WIDTH + 4.0
			position.y = randf_range(CONFIG.HUD_TOP, CONFIG.HUD_BOTTOM)
		star["position"] = position
	queue_redraw()

func _draw() -> void:
	for star in stars:
		var layer := int(star["layer"])
		var alpha := 0.35 + layer * 0.18 + sin(Time.get_ticks_msec() * 0.0015 + float(star["twinkle"])) * 0.08
		draw_circle(star["position"], float(layer) * 0.62, Color(0.65, 0.82, 1.0, alpha))
