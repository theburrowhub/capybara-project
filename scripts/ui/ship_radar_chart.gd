class_name ShipRadarChart
extends Control

const LABELS := ["SPEED", "HULL", "SHIELD", "FIREPOWER", "RECOVERY"]
const BALANCED_STATS := [3.0, 3.0, 3.0, 3.0, 3.0]

var target_stats: Array[float] = [3.0, 3.0, 3.0, 3.0, 3.0]
var display_stats: Array[float] = [3.0, 3.0, 3.0, 3.0, 3.0]
var accent := Color("ff445e")
var show_balanced_reference := false

func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	set_process(true)
	queue_redraw()

func set_profile(values: Array, color: Color, compare_balanced: bool) -> void:
	for index in range(5):
		target_stats[index] = float(values[index])
	accent = color
	show_balanced_reference = compare_balanced
	queue_redraw()

func _process(delta: float) -> void:
	var changed := false
	for index in range(display_stats.size()):
		var next_value := lerpf(display_stats[index], target_stats[index], minf(1.0, delta * 9.0))
		if absf(next_value - target_stats[index]) < 0.01:
			next_value = target_stats[index]
		if not is_equal_approx(next_value, display_stats[index]):
			display_stats[index] = next_value
			changed = true
	if changed:
		queue_redraw()

func _draw() -> void:
	var center := Vector2(size.x * 0.50, size.y * 0.49)
	var radius := minf(size.x * 0.30, size.y * 0.34)
	var grid_color := Color(0.34, 0.84, 1.0, 0.22)
	var axis_color := Color(0.34, 0.84, 1.0, 0.16)

	for ring in range(5, 0, -1):
		var ring_values := [float(ring), float(ring), float(ring), float(ring), float(ring)]
		var ring_points := _points_for(ring_values, center, radius)
		if ring == 5:
			draw_colored_polygon(ring_points, Color(0.15, 0.55, 0.78, 0.025))
		draw_polyline(_closed(ring_points), grid_color, 1.0, true)

	for index in range(5):
		var angle := -PI * 0.5 + TAU * float(index) / 5.0
		draw_line(center, center + Vector2.from_angle(angle) * radius, axis_color, 1.0, true)

	if show_balanced_reference:
		var reference := _points_for(BALANCED_STATS, center, radius)
		for index in range(reference.size()):
			_draw_dashed_segment(reference[index], reference[(index + 1) % reference.size()])

	var value_points := _points_for(display_stats, center, radius)
	draw_colored_polygon(value_points, Color(accent.r, accent.g, accent.b, 0.31))
	draw_polyline(_closed(value_points), accent, 2.5, true)
	for point in value_points:
		draw_circle(point, 3.2, Color("edfaff"))
		draw_arc(point, 4.4, 0.0, TAU, 12, accent, 1.4, true)

	var font := ThemeDB.fallback_font
	var font_size := 10
	for index in range(5):
		var angle := -PI * 0.5 + TAU * float(index) / 5.0
		var label_center := center + Vector2.from_angle(angle) * (radius + 19.0)
		var text_size := font.get_string_size(LABELS[index], HORIZONTAL_ALIGNMENT_LEFT, -1.0, font_size)
		var baseline := label_center + Vector2(-text_size.x * 0.5, text_size.y * 0.34)
		draw_string(font, baseline, LABELS[index], HORIZONTAL_ALIGNMENT_LEFT, -1.0, font_size, Color("86a5d0"))

func _points_for(values: Array, center: Vector2, radius: float) -> PackedVector2Array:
	var points := PackedVector2Array()
	for index in range(5):
		var angle := -PI * 0.5 + TAU * float(index) / 5.0
		points.append(center + Vector2.from_angle(angle) * radius * float(values[index]) / 5.0)
	return points

func _closed(points: PackedVector2Array) -> PackedVector2Array:
	var result := points.duplicate()
	if not result.is_empty():
		result.append(result[0])
	return result

func _draw_dashed_segment(start: Vector2, finish: Vector2) -> void:
	var color := Color(0.93, 0.98, 1.0, 0.42)
	var length := start.distance_to(finish)
	if length <= 0.0:
		return
	var direction := start.direction_to(finish)
	var offset := 0.0
	while offset < length:
		var dash_end := minf(offset + 4.0, length)
		draw_line(start + direction * offset, start + direction * dash_end, color, 1.3, true)
		offset += 8.0
