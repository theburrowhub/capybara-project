class_name ShipShowcasePreview
extends TextureRect

const CONFIG := preload("res://scripts/core/game_config.gd")

var current_ship_id := "vindicator"
var rotation_speed := 0.42
var viewport: SubViewport
var model_pivot: Node3D
var active_model: Node3D
var accent_light: OmniLight3D

func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	expand_mode = TextureRect.EXPAND_IGNORE_SIZE
	stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
	_build_viewport()
	show_ship(current_ship_id)
	set_process(true)

func _process(delta: float) -> void:
	if is_instance_valid(model_pivot):
		model_pivot.rotation.y = wrapf(model_pivot.rotation.y + rotation_speed * delta, 0.0, TAU)

func show_ship(ship_id: String) -> void:
	current_ship_id = ship_id if CONFIG.PLAYER_SHIPS.has(ship_id) else "vindicator"
	if not is_node_ready() or not is_instance_valid(model_pivot):
		return
	if is_instance_valid(active_model):
		active_model.free()
		active_model = null
	var profile := CONFIG.player_ship(current_ship_id)
	var resource := load(str(profile["model"]))
	if resource == null:
		return
	active_model = resource.instantiate() as Node3D
	if active_model == null:
		return
	active_model.name = "ShowcaseModel"
	active_model.scale = Vector3.ONE * float(profile["model_scale"])
	model_pivot.add_child(active_model)
	if is_instance_valid(accent_light):
		accent_light.light_color = profile["accent"]

func _build_viewport() -> void:
	viewport = SubViewport.new()
	viewport.name = "ShipShowcaseViewport"
	viewport.size = Vector2i(720, 360)
	viewport.transparent_bg = true
	viewport.render_target_update_mode = SubViewport.UPDATE_ALWAYS
	viewport.world_3d = World3D.new()
	add_child(viewport)
	texture = viewport.get_texture()

	model_pivot = Node3D.new()
	model_pivot.name = "RotatingShip"
	model_pivot.rotation.y = -0.35
	viewport.add_child(model_pivot)

	var key := DirectionalLight3D.new()
	key.name = "KeyLight"
	key.rotation_degrees = Vector3(-52.0, -32.0, -12.0)
	key.light_energy = 2.6
	viewport.add_child(key)

	accent_light = OmniLight3D.new()
	accent_light.name = "AccentLight"
	accent_light.position = Vector3(-9.0, 9.0, 9.0)
	accent_light.omni_range = 42.0
	accent_light.light_energy = 5.0
	accent_light.light_color = Color("ff445e")
	viewport.add_child(accent_light)

	var rim := OmniLight3D.new()
	rim.name = "RimLight"
	rim.position = Vector3(10.0, 5.0, -9.0)
	rim.omni_range = 40.0
	rim.light_energy = 4.2
	rim.light_color = Color("70cfff")
	viewport.add_child(rim)

	var camera := Camera3D.new()
	camera.name = "ShowcaseCamera"
	camera.projection = Camera3D.PROJECTION_ORTHOGONAL
	camera.size = 23.5
	camera.position = Vector3(0.0, 12.5, 25.0)
	viewport.add_child(camera)
	camera.look_at(Vector3(0.0, 0.5, 0.0), Vector3.UP)
	camera.current = true
