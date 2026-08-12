extends Node

const SAVE_PATH := "user://profile.json"
const DIFFICULTY_NAMES := ["EASY", "NORMAL", "HARD", "INSANE"]

var settings := {
	"sound_volume": 1.0,
	"music_volume": 0.5,
	"resolution": Vector2i(1200, 600),
	"fullscreen": false,
	"vsync": true,
	"difficulty": 1,
}

var high_scores: Array = []

const PRESET_NAMES := [
	["masktaro", "nomustyle", "kapover", "miyaborn", "ledesmactrl", "baertech", "yokoitech", "spencore", "regginator", "hulstcore"],
	["boonfatal", "barloggamer", "cliffyB", "molygod", "kondobeat", "mikahorror", "russoAI", "aonulink", "kamiRage", "uedadreamer"],
	["sakurush", "levinecore", "sonaknaka", "granYamauchi", "notchcode", "robertadream", "jadevision", "hennigstory", "iwapac", "iwataheart"],
	["shigmoto", "kojimaster", "carmatron", "romeroid", "simeierX", "gabenator", "yusonic", "simwright", "howardcore", "hirofinal"],
]

const PRESET_SCORES := [
	[63000, 62500, 61900, 61500, 60800, 60000, 59500, 58700, 58000, 57500],
	[69500, 68700, 68000, 67300, 66800, 66000, 65400, 65000, 64700, 63800],
	[78700, 77900, 76800, 75600, 74800, 73900, 72600, 71800, 70900, 70000],
	[100000, 96500, 94000, 92500, 91000, 90000, 87800, 85300, 82900, 80400],
]

func _ready() -> void:
	load_profile()
	apply_settings()

func load_profile() -> void:
	high_scores = _default_high_scores()
	if not FileAccess.file_exists(SAVE_PATH):
		save_profile()
		return
	var file := FileAccess.open(SAVE_PATH, FileAccess.READ)
	if file == null:
		return
	var parsed: Variant = JSON.parse_string(file.get_as_text())
	if not parsed is Dictionary:
		return
	var stored: Dictionary = parsed
	var stored_settings: Dictionary = stored.get("settings", {})
	for key: String in settings.keys():
		if stored_settings.has(key):
			settings[key] = stored_settings[key]
	if settings["resolution"] is Array:
		var resolution: Array = settings["resolution"]
		if resolution.size() >= 2:
			settings["resolution"] = Vector2i(int(resolution[0]), int(resolution[1]))
	if stored.get("high_scores", []) is Array and not stored["high_scores"].is_empty():
		high_scores = stored["high_scores"]

func save_profile() -> void:
	var serializable_settings := settings.duplicate(true)
	var resolution: Vector2i = settings["resolution"]
	serializable_settings["resolution"] = [resolution.x, resolution.y]
	var file := FileAccess.open(SAVE_PATH, FileAccess.WRITE)
	if file:
		file.store_string(JSON.stringify({"settings": serializable_settings, "high_scores": high_scores}, "\t"))

func apply_settings() -> void:
	var music_bus := AudioServer.get_bus_index("Music")
	var sfx_bus := AudioServer.get_bus_index("SFX")
	if music_bus >= 0:
		AudioServer.set_bus_volume_db(music_bus, linear_to_db(maxf(float(settings["music_volume"]), 0.001)))
	if sfx_bus >= 0:
		AudioServer.set_bus_volume_db(sfx_bus, linear_to_db(maxf(float(settings["sound_volume"]), 0.001)))
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_ENABLED if settings["vsync"] else DisplayServer.VSYNC_DISABLED)
	if bool(settings["fullscreen"]):
		DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_FULLSCREEN)
	else:
		DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_WINDOWED)
		DisplayServer.window_set_size(settings["resolution"])
		_center_window()

func scores_for(difficulty: int) -> Array:
	var filtered: Array = high_scores.filter(func(entry: Dictionary) -> bool: return int(entry.get("difficulty", 1)) == difficulty)
	filtered.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return int(a["score"]) > int(b["score"]))
	return filtered.slice(0, mini(10, filtered.size()))

func is_high_score(score: int, difficulty: int) -> bool:
	var scores := scores_for(difficulty)
	return scores.size() < 10 or score > int(scores[-1]["score"])

func add_high_score(player_name: String, score: int, difficulty: int) -> void:
	var safe_name := player_name.strip_edges().substr(0, 31)
	if safe_name.is_empty():
		safe_name = "Player"
	high_scores.append({"name": safe_name, "score": score, "difficulty": difficulty, "timestamp": int(Time.get_unix_time_from_system())})
	var kept: Array = []
	for level in range(4):
		kept.append_array(scores_for(level))
	high_scores = kept
	save_profile()

func _default_high_scores() -> Array:
	var result: Array = []
	for difficulty in range(4):
		for rank in range(10):
			result.append({"name": PRESET_NAMES[difficulty][rank], "score": PRESET_SCORES[difficulty][rank], "difficulty": difficulty, "timestamp": 0})
	return result

func _center_window() -> void:
	var screen := DisplayServer.window_get_current_screen()
	var usable := DisplayServer.screen_get_usable_rect(screen)
	var size := DisplayServer.window_get_size()
	DisplayServer.window_set_position(usable.position + (usable.size - size) / 2)
