class_name ExplosionEffect
extends Node2D

var color := Color.WHITE
var size := 24.0
var elapsed := 0.0
var lifetime := 0.55
var particles: Array[Dictionary] = []

func setup(effect_color: Color, effect_size: float) -> void:
	color = effect_color
	size = maxf(8.0, effect_size)
	for index in range(clampi(int(size * 0.65), 8, 34)):
		var angle := randf() * TAU
		particles.append({"direction": Vector2.from_angle(angle), "speed": randf_range(size * 1.6, size * 4.2), "weight": randf_range(1.5, 4.5)})

func _process(delta: float) -> void:
	elapsed += delta
	if elapsed >= lifetime:
		queue_free()
		return
	queue_redraw()

func _draw() -> void:
	var progress := elapsed / lifetime
	draw_circle(Vector2.ZERO, size * progress, Color(color, (1.0 - progress) * 0.28))
	draw_arc(Vector2.ZERO, size * (0.4 + progress), 0.0, TAU, 32, Color(color, 1.0 - progress), maxf(1.0, size * 0.08))
	for particle in particles:
		var point: Vector2 = particle["direction"] * float(particle["speed"]) * elapsed
		draw_circle(point, float(particle["weight"]) * (1.0 - progress), Color(color, 1.0 - progress))
