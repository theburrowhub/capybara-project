extends Node2D

const CONFIG := preload("res://scripts/core/game_config.gd")

var stars: Array[Dictionary] = []
var elapsed := 0.0

func _ready() -> void:
	var generator := RandomNumberGenerator.new()
	generator.seed = 0x5EEDCA9
	for index in range(90):
		stars.append({
			"position": Vector2(generator.randf_range(0.0, CONFIG.WIDTH), generator.randf_range(0.0, CONFIG.HEIGHT)),
			"size": generator.randf_range(0.7, 2.2), "phase": generator.randf_range(0.0, TAU),
		})

func _process(delta: float) -> void:
	elapsed += delta
	for star in stars:
		var position: Vector2 = star["position"]
		position.x -= float(star["size"]) * 4.0 * delta
		if position.x < -4.0:
			position.x = CONFIG.WIDTH + 4.0
		star["position"] = position
	queue_redraw()

func _draw() -> void:
	draw_rect(Rect2(0.0, 0.0, CONFIG.WIDTH, CONFIG.HEIGHT), Color("020613"))
	for band in range(8):
		var band_color := Color(0.02, 0.08 + band * 0.006, 0.16 + band * 0.012, 0.14)
		draw_circle(Vector2(940.0, 280.0), 520.0 - band * 52.0, band_color)
	for star in stars:
		var alpha := 0.42 + sin(elapsed * 1.8 + float(star["phase"])) * 0.18
		draw_circle(star["position"], float(star["size"]), Color(0.65, 0.86, 1.0, alpha))
	# Navigation motif: a stylized flight vector behind the menu card.
	var path := PackedVector2Array([Vector2(70, 470), Vector2(250, 420), Vector2(410, 455), Vector2(630, 360), Vector2(850, 405), Vector2(1130, 260)])
	draw_polyline(path, Color(0.13, 0.65, 0.9, 0.16), 2.0, true)
	for point in path:
		draw_circle(point, 5.0, Color(0.25, 0.85, 1.0, 0.24))
