class_name PowerUp
extends Area2D

signal collected(kind: String)

const COLORS := {
	"energy": Color("ffd84d"), "shield": Color("47d7ff"),
	"hull": Color("ff5577"), "weapon": Color("a978ff"),
}
const SYMBOLS := {"energy": "E", "shield": "S", "hull": "+", "weapon": "W"}

var kind := "energy"
var lifetime := 10.0
var elapsed := 0.0
var player: Node2D

func setup(powerup_kind: String) -> void:
	kind = powerup_kind if COLORS.has(powerup_kind) else "energy"

func _ready() -> void:
	add_to_group("powerup")
	collision_layer = 16
	collision_mask = 1
	area_entered.connect(_on_area_entered)

func _process(delta: float) -> void:
	elapsed += delta
	lifetime -= delta
	if lifetime <= 0.0:
		queue_free()
		return
	if not is_instance_valid(player):
		player = get_tree().get_first_node_in_group("player") as Node2D
	if is_instance_valid(player) and global_position.distance_to(player.global_position) < 80.0:
		position += global_position.direction_to(player.global_position) * 220.0 * delta
	else:
		position.x -= 120.0 * delta
	rotation += delta * 1.8
	if position.x < -40.0:
		queue_free()
	queue_redraw()

func _on_area_entered(area: Area2D) -> void:
	if area.is_in_group("player"):
		collected.emit(kind)
		if area.has_method("collect_powerup"):
			area.collect_powerup(kind)
		queue_free()

func _draw() -> void:
	var color: Color = COLORS[kind]
	var pulse := 1.0 + sin(elapsed * 7.0) * 0.12
	draw_circle(Vector2.ZERO, 18.0 * pulse, Color(color, 0.15))
	draw_circle(Vector2.ZERO, 12.0 * pulse, Color(color, 0.92))
	draw_arc(Vector2.ZERO, 15.0 * pulse, 0.0, TAU, 24, Color.WHITE, 2.0)
	draw_string(ThemeDB.fallback_font, Vector2(-6.0, 6.0), SYMBOLS[kind], HORIZONTAL_ALIGNMENT_CENTER, 12.0, 16, Color("081128"))
