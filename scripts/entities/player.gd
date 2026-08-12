class_name PlayerShip
extends Area2D

signal projectile_requested(config: Dictionary)
signal died
signal stats_changed
signal revived

enum WeaponMode { SINGLE, DOUBLE, SPREAD, RAPID, CHARGE, DUAL }
enum EnergyMode { OFFENSIVE, DEFENSIVE }

const WEAPON_NAMES := ["Single", "Double", "Spread", "Rapid", "Charge", "Dual"]
const MODEL_PATH := "res://assets/models/player_ship.glb"
const CONFIG := preload("res://scripts/core/game_config.gd")
const NEUTRAL_VISUAL_BANK := -PI * 0.5
const MAX_VISUAL_BANK_DELTA := PI * 0.5
const BANK_RESPONSE := 7.5
const SHIELD_ALPHA := 0.10
const DEFENSIVE_SPECIAL_ALPHA := 0.15
const SHIELD_RADIUS := 9.5
const DEFENSIVE_SPECIAL_RADIUS := 10.2

var max_health := 100.0
var health := 100.0
var max_shield := 25.0
var shield := 25.0
var max_energy := 100.0
var energy := 100.0
var energy_mode := EnergyMode.OFFENSIVE
var special_active := false
var weapon_mode := WeaponMode.SINGLE
var weapon_powerups := 0
var fire_timer := 0.0
var charge_level := 0.0
var was_shooting := false
var last_damage_time := -10.0
var last_energy_depletion := -10.0
var invulnerable := false
var active := true
var bank := NEUTRAL_VISUAL_BANK
var _elapsed := 0.0
var _model_pivot: Node3D
var _model_bank: Node3D
var _shield_sphere: MeshInstance3D
var _defensive_polyhedron: MeshInstance3D
var _defensive_polyhedron_edges: MeshInstance3D

func _ready() -> void:
	add_to_group("player")
	area_entered.connect(_on_area_entered)
	_build_model_view()
	stats_changed.emit()

func _process(delta: float) -> void:
	if not active:
		return
	_elapsed += delta
	fire_timer = maxf(0.0, fire_timer - delta)
	_handle_modes()
	_handle_movement(delta)
	_handle_energy(delta)
	_handle_regeneration(delta)
	_handle_weapon(delta)
	_update_defense_visuals(delta)
	queue_redraw()

func _handle_modes() -> void:
	for index in range(6):
		if Input.is_physical_key_pressed(KEY_1 + index):
			weapon_mode = index as WeaponMode
	if Input.is_physical_key_pressed(KEY_Q) and not get_meta("q_down", false):
		set_meta("q_down", true)
		var ratio := shield / maxf(max_shield, 1.0)
		energy_mode = EnergyMode.DEFENSIVE if energy_mode == EnergyMode.OFFENSIVE else EnergyMode.OFFENSIVE
		max_shield = 50.0 if energy_mode == EnergyMode.DEFENSIVE else 25.0
		shield = ratio * max_shield
		stats_changed.emit()
	elif not Input.is_physical_key_pressed(KEY_Q):
		set_meta("q_down", false)
	if Input.is_physical_key_pressed(KEY_R) and not get_meta("r_down", false):
		set_meta("r_down", true)
		weapon_mode = ((weapon_mode + 1) % WeaponMode.size()) as WeaponMode
		stats_changed.emit()
	elif not Input.is_physical_key_pressed(KEY_R):
		set_meta("r_down", false)

func _handle_movement(delta: float) -> void:
	var input := Vector2.ZERO
	input.x = float(Input.is_physical_key_pressed(KEY_D) or Input.is_key_pressed(KEY_RIGHT)) - float(Input.is_physical_key_pressed(KEY_A) or Input.is_key_pressed(KEY_LEFT))
	input.y = float(Input.is_physical_key_pressed(KEY_S) or Input.is_key_pressed(KEY_DOWN)) - float(Input.is_physical_key_pressed(KEY_W) or Input.is_key_pressed(KEY_UP))
	input = input.normalized()
	position += input * 300.0 * delta
	position.x = clampf(position.x, 34.0, CONFIG.WIDTH - 34.0)
	position.y = clampf(position.y, CONFIG.HUD_TOP + 34.0, CONFIG.HUD_BOTTOM - 34.0)
	set_visual_bank(input.y, false, delta)
	rotation = 0.0

func set_visual_bank(vertical_direction: float, immediate := false, delta := 0.0) -> void:
	var target := NEUTRAL_VISUAL_BANK + clampf(vertical_direction, -1.0, 1.0) * MAX_VISUAL_BANK_DELTA
	bank = target if immediate else lerpf(bank, target, minf(1.0, delta * BANK_RESPONSE))
	if is_instance_valid(_model_bank):
		_model_bank.rotation.x = bank

func _handle_energy(delta: float) -> void:
	special_active = Input.is_physical_key_pressed(KEY_E) and energy > 0.0
	if special_active:
		var drain := 40.0 if energy_mode == EnergyMode.OFFENSIVE else 20.0
		energy = maxf(0.0, energy - drain * delta)
		if energy <= 0.0:
			special_active = false
			last_energy_depletion = _elapsed

func _handle_regeneration(delta: float) -> void:
	if shield < max_shield and _elapsed - last_damage_time > 5.0:
		var regen := 2.0
		if energy_mode == EnergyMode.DEFENSIVE and is_equal_approx(energy, max_energy):
			regen *= 2.0
		shield = minf(max_shield, shield + regen * delta)
	if energy < max_energy and _elapsed - last_energy_depletion > 5.0 and not special_active:
		energy = minf(max_energy, energy + 2.0 * delta)

func _handle_weapon(delta: float) -> void:
	var shooting := Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT) or Input.is_physical_key_pressed(KEY_SPACE)
	if weapon_mode == WeaponMode.CHARGE:
		if shooting:
			charge_level = minf(1.0, charge_level + delta * 1.5)
		elif was_shooting and charge_level >= 0.2:
			_fire_charge()
			charge_level = 0.0
	else:
		charge_level = 0.0
		if shooting and fire_timer <= 0.0:
			_fire_current_mode()
	was_shooting = shooting

func _fire_current_mode() -> void:
	var cooldown := 0.06 if weapon_mode == WeaponMode.RAPID else 0.12
	fire_timer = cooldown
	match weapon_mode:
		WeaponMode.DOUBLE:
			_request_shot(Vector2(30.0, -10.0), 0.0)
			_request_shot(Vector2(30.0, 10.0), 0.0)
		WeaponMode.SPREAD:
			for angle in [-0.20, 0.0, 0.20]:
				_request_shot(Vector2(30.0, 0.0), angle)
		WeaponMode.DUAL:
			_request_shot(Vector2(30.0, 0.0), 0.0)
			_request_shot(Vector2(-30.0, 0.0), PI)
		_:
			_request_shot(Vector2(30.0, 0.0), 0.0)

func _fire_charge() -> void:
	var bullet_count := clampi(int(charge_level * 9.0) + 2, 2, 11)
	var spread := lerpf(0.35, 1.15, charge_level)
	for index in range(bullet_count):
		var ratio := 0.5 if bullet_count == 1 else float(index) / float(bullet_count - 1)
		_request_shot(Vector2(30.0, 0.0), lerpf(-spread * 0.5, spread * 0.5, ratio), 1.0 + charge_level * 2.0)

func _request_shot(offset: Vector2, angle: float, charge_multiplier := 1.0) -> void:
	var power_multipliers := [1.0, 1.5, 2.0, 2.5]
	var damage: float = float(power_multipliers[clampi(weapon_powerups, 0, 3)]) * charge_multiplier
	if energy_mode == EnergyMode.OFFENSIVE and special_active:
		damage *= 2.0
	projectile_requested.emit({
		"kind": "player_bullet", "position": global_position + offset.rotated(rotation),
		"direction": Vector2.RIGHT.rotated(rotation + angle), "damage": damage,
		"from_player": true, "target": null,
	})

func take_damage(amount: float) -> void:
	if not active or invulnerable:
		return
	if energy_mode == EnergyMode.DEFENSIVE and special_active:
		amount *= 0.25
	last_damage_time = _elapsed
	var shield_damage := minf(shield, amount)
	shield -= shield_damage
	health -= amount - shield_damage
	if health <= 0.0:
		if weapon_powerups > 0:
			weapon_powerups -= 1
			health = max_health * 0.20
			shield = max_shield * 0.50
			energy = 0.0
			last_energy_depletion = _elapsed
			revived.emit()
		else:
			health = 0.0
			active = false
			visible = false
			died.emit()
	stats_changed.emit()

func collect_powerup(kind: String) -> void:
	match kind:
		"energy": energy = minf(max_energy, energy + max_energy * 0.20)
		"shield": shield = max_shield
		"hull": health = minf(max_health, health + max_health * 0.20)
		"weapon": weapon_powerups = mini(3, weapon_powerups + 1)
	stats_changed.emit()

func weapon_name() -> String:
	return WEAPON_NAMES[weapon_mode]

func energy_mode_name() -> String:
	return "OFFENSIVE" if energy_mode == EnergyMode.OFFENSIVE else "DEFENSIVE"

func _on_area_entered(area: Area2D) -> void:
	if area.is_in_group("enemy"):
		take_damage(float(area.get("contact_damage")) if area.get("contact_damage") != null else 10.0)
		if area.has_method("take_damage") and str(area.get("enemy_type")) != "boss":
			area.take_damage(99999.0)

func _build_model_view() -> void:
	var model_resource := load(MODEL_PATH)
	if model_resource == null:
		_add_fallback_sprite()
		return
	var viewport := SubViewport.new()
	viewport.name = "Ship3DViewport"
	viewport.size = Vector2i(180, 180)
	viewport.transparent_bg = true
	viewport.render_target_update_mode = SubViewport.UPDATE_ALWAYS
	viewport.world_3d = World3D.new()
	add_child(viewport)
	_model_pivot = Node3D.new()
	viewport.add_child(_model_pivot)
	_model_bank = Node3D.new()
	_model_pivot.add_child(_model_bank)
	_model_bank.rotation.x = bank
	var model: Node = model_resource.instantiate()
	_model_bank.add_child(model)
	_build_defense_geometry()
	_model_pivot.rotation_degrees = Vector3(0.0, -90.0, 0.0)
	var light := DirectionalLight3D.new()
	light.rotation_degrees = Vector3(-50.0, -35.0, 0.0)
	light.light_energy = 2.2
	viewport.add_child(light)
	var fill := OmniLight3D.new()
	fill.position = Vector3(-8.0, 10.0, 8.0)
	fill.omni_range = 40.0
	fill.light_energy = 5.0
	fill.light_color = Color("70cfff")
	viewport.add_child(fill)
	var camera := Camera3D.new()
	camera.projection = Camera3D.PROJECTION_ORTHOGONAL
	camera.size = 25.0
	camera.position = Vector3(0.0, 24.0, 0.0)
	camera.rotation_degrees = Vector3(-90.0, 0.0, 0.0)
	viewport.add_child(camera)
	camera.current = true
	var sprite := Sprite2D.new()
	sprite.name = "ModelSprite"
	sprite.texture = viewport.get_texture()
	sprite.scale = Vector2(0.58, 0.58)
	sprite.rotation = -PI * 0.5
	add_child(sprite)
	_update_defense_visuals()

func _build_defense_geometry() -> void:
	_shield_sphere = MeshInstance3D.new()
	_shield_sphere.name = "ShieldSphere"
	var sphere := SphereMesh.new()
	sphere.radius = SHIELD_RADIUS
	sphere.height = SHIELD_RADIUS * 2.0
	sphere.radial_segments = 32
	sphere.rings = 16
	_shield_sphere.mesh = sphere
	_shield_sphere.material_override = _transparent_energy_material(Color(0.12, 0.92, 1.0, SHIELD_ALPHA))
	_shield_sphere.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_model_bank.add_child(_shield_sphere)

	var polyhedron_data := _icosahedron_data(DEFENSIVE_SPECIAL_RADIUS)
	_defensive_polyhedron = MeshInstance3D.new()
	_defensive_polyhedron.name = "DefensiveSpecialIcosahedron"
	_defensive_polyhedron.mesh = _icosahedron_faces(polyhedron_data["vertices"], polyhedron_data["faces"])
	_defensive_polyhedron.material_override = _transparent_energy_material(Color(0.30, 1.0, 0.68, DEFENSIVE_SPECIAL_ALPHA))
	_defensive_polyhedron.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_defensive_polyhedron.set_meta("face_count", 20)
	_model_bank.add_child(_defensive_polyhedron)

	_defensive_polyhedron_edges = MeshInstance3D.new()
	_defensive_polyhedron_edges.name = "DefensiveSpecialEdges"
	_defensive_polyhedron_edges.mesh = _icosahedron_edges(
		polyhedron_data["vertices"], polyhedron_data["faces"],
		_transparent_energy_material(Color(0.45, 1.0, 0.78, DEFENSIVE_SPECIAL_ALPHA))
	)
	_defensive_polyhedron_edges.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_model_bank.add_child(_defensive_polyhedron_edges)

func _transparent_energy_material(color: Color) -> StandardMaterial3D:
	var material := StandardMaterial3D.new()
	material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	material.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	material.albedo_color = color
	material.emission_enabled = true
	material.emission = Color(color.r, color.g, color.b)
	material.emission_energy_multiplier = 1.25
	return material

func _icosahedron_data(radius: float) -> Dictionary:
	var golden_ratio := (1.0 + sqrt(5.0)) * 0.5
	var vertices := PackedVector3Array([
		Vector3(-1.0, golden_ratio, 0.0), Vector3(1.0, golden_ratio, 0.0),
		Vector3(-1.0, -golden_ratio, 0.0), Vector3(1.0, -golden_ratio, 0.0),
		Vector3(0.0, -1.0, golden_ratio), Vector3(0.0, 1.0, golden_ratio),
		Vector3(0.0, -1.0, -golden_ratio), Vector3(0.0, 1.0, -golden_ratio),
		Vector3(golden_ratio, 0.0, -1.0), Vector3(golden_ratio, 0.0, 1.0),
		Vector3(-golden_ratio, 0.0, -1.0), Vector3(-golden_ratio, 0.0, 1.0),
	])
	for index in range(vertices.size()):
		vertices[index] = vertices[index].normalized() * radius
	var faces := PackedInt32Array([
		0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
		1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
		3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
		4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1,
	])
	return {"vertices": vertices, "faces": faces}

func _icosahedron_faces(vertices: PackedVector3Array, faces: PackedInt32Array) -> ArrayMesh:
	var face_vertices := PackedVector3Array()
	for vertex_index in faces:
		face_vertices.append(vertices[vertex_index])
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = face_vertices
	var mesh := ArrayMesh.new()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	return mesh

func _icosahedron_edges(vertices: PackedVector3Array, faces: PackedInt32Array, material: Material) -> ImmediateMesh:
	var mesh := ImmediateMesh.new()
	var unique_edges := {}
	mesh.surface_begin(Mesh.PRIMITIVE_LINES, material)
	for face_offset in range(0, faces.size(), 3):
		for edge_offset in range(3):
			var first := faces[face_offset + edge_offset]
			var second := faces[face_offset + (edge_offset + 1) % 3]
			var edge_key := "%d:%d" % [mini(first, second), maxi(first, second)]
			if unique_edges.has(edge_key):
				continue
			unique_edges[edge_key] = true
			mesh.surface_add_vertex(vertices[first])
			mesh.surface_add_vertex(vertices[second])
	mesh.surface_end()
	return mesh

func _update_defense_visuals(delta := 0.0) -> void:
	if not is_instance_valid(_shield_sphere):
		return
	var defensive_special := energy_mode == EnergyMode.DEFENSIVE and special_active
	_shield_sphere.visible = shield > 0.0 and not defensive_special
	_defensive_polyhedron.visible = defensive_special
	_defensive_polyhedron_edges.visible = defensive_special
	if defensive_special and delta > 0.0:
		_defensive_polyhedron.rotation.y += delta * 0.45
		_defensive_polyhedron_edges.rotation.y = _defensive_polyhedron.rotation.y

func _add_fallback_sprite() -> void:
	var sprite := Sprite2D.new()
	sprite.texture = load("res://assets/sprites/player_ship.png")
	sprite.scale = Vector2(0.9, 0.9)
	add_child(sprite)

func _draw() -> void:
	var pulse := 1.0 + sin(_elapsed * 8.0) * 0.12
	draw_circle(Vector2(-34.0, -9.0), 7.0 * pulse, Color(0.25, 0.8, 1.0, 0.28))
	draw_circle(Vector2(-34.0, 9.0), 7.0 * pulse, Color(0.25, 0.8, 1.0, 0.28))
	if charge_level > 0.0:
		draw_arc(Vector2.ZERO, 46.0, -PI * 0.5, -PI * 0.5 + TAU * charge_level, 32, Color("ffe45e"), 4.0)
	if get_meta("debug_hitbox", false):
		draw_rect(Rect2(-26.0, -18.0, 52.0, 36.0), Color(0.2, 1.0, 0.4, 0.85), false, 2.0)
