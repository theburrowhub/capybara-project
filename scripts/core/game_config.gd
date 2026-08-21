class_name GameConfig
extends RefCounted

const WIDTH := 1200.0
const HEIGHT := 600.0
const HUD_TOP := 30.0
const HUD_BOTTOM := 500.0
const PLAY_RECT := Rect2(0.0, HUD_TOP, WIDTH, HUD_BOTTOM - HUD_TOP)
const MAX_ENEMIES := 30
const MAX_PROJECTILES := 200

const PLAYER_SHIP_ORDER := ["vindicator", "sting", "goliat"]

const PLAYER_SHIPS := {
	"vindicator": {
		"id": "vindicator", "name": "VINDICATOR", "role": "BALANCED STRIKER",
		"description": "Balanced armor, firepower and recovery. No critical weakness.",
		"model": "res://assets/models/player_ship.glb", "model_scale": 1.0,
		"accent": Color("ff445e"), "dark_accent": Color("7a1f35"),
		"move_speed": 300.0, "max_health": 100.0, "base_shield": 25.0, "max_energy": 100.0,
		"damage_multiplier": 1.0, "shield_regen_rate": 2.0, "energy_regen_rate": 2.0,
		"shield_regen_delay": 5.0, "energy_regen_delay": 5.0,
		"radar": [3, 3, 3, 3, 3],
	},
	"sting": {
		"id": "sting", "name": "STING", "role": "RAPID INTERCEPTOR",
		"description": "Extreme speed and rapid bar recovery. Hull, shield and firepower run light.",
		"model": "res://assets/models/player_ship_sting.glb", "model_scale": 0.485185,
		"accent": Color("35a7ff"), "dark_accent": Color("184f81"),
		"move_speed": 450.0, "max_health": 70.0, "base_shield": 15.0, "max_energy": 100.0,
		"damage_multiplier": 0.70, "shield_regen_rate": 4.0, "energy_regen_rate": 4.0,
		"shield_regen_delay": 2.5, "energy_regen_delay": 2.5,
		"radar": [5, 2, 2, 2, 5],
	},
	"goliat": {
		"id": "goliat", "name": "GOLIAT", "role": "HEAVY DREADNOUGHT",
		"description": "Overwhelming firepower, hull and shield. Movement and recovery are brutally slow.",
		"model": "res://assets/models/player_ship_goliat.glb", "model_scale": 0.608025,
		"accent": Color("5de071"), "dark_accent": Color("1d7a45"),
		"move_speed": 180.0, "max_health": 180.0, "base_shield": 60.0, "max_energy": 100.0,
		"damage_multiplier": 2.25, "shield_regen_rate": 0.5, "energy_regen_rate": 0.5,
		"shield_regen_delay": 10.0, "energy_regen_delay": 10.0,
		"radar": [1, 5, 5, 5, 1],
	},
}

const ENEMY_ORDER := [
	"grunt", "tank", "speeder", "zigzag", "bomber",
	"shield", "swarm", "elite", "ghost", "boss"
]

const ENEMIES := {
	"grunt": {"name": "Grunt", "health": 1.0, "speed": 210.0, "power": 10.0, "size": 0.80, "resistance": 0.00, "points": 100, "fire_rate": 1.0, "burst": 1, "projectile": "laser", "sprite": "enemy_model_01", "sprite_rotation": 0.0, "flip": false, "tint": Color(1.20, 0.72, 0.72)},
	"tank": {"name": "Tank", "health": 20.0, "speed": 82.0, "power": 30.0, "size": 1.55, "resistance": 0.20, "points": 300, "fire_rate": 0.60, "burst": 3, "projectile": "missile", "sprite": "enemy_model_03", "sprite_rotation": 0.0, "flip": false, "tint": Color(1.08, 1.10, 1.25)},
	"speeder": {"name": "Speeder", "health": 1.0, "speed": 380.0, "power": 15.0, "size": 0.70, "resistance": 0.00, "points": 150, "fire_rate": 3.0, "burst": 3, "projectile": "laser", "sprite": "enemy_model_02", "sprite_rotation": 0.0, "flip": true, "tint": Color(1.30, 1.15, 0.65)},
	"zigzag": {"name": "Zigzag", "health": 2.0, "speed": 185.0, "power": 20.0, "size": 0.90, "resistance": 0.10, "points": 180, "fire_rate": 1.5, "burst": 2, "projectile": "plasma", "sprite": "enemy_model_01", "sprite_rotation": 0.0, "flip": false, "tint": Color(0.72, 1.25, 0.88)},
	"bomber": {"name": "Bomber", "health": 6.0, "speed": 125.0, "power": 40.0, "size": 1.30, "resistance": 0.10, "points": 350, "fire_rate": 0.80, "burst": 1, "projectile": "energy_orb", "sprite": "enemy_model_04", "sprite_rotation": 0.0, "flip": false, "tint": Color(1.20, 0.72, 1.28)},
	"shield": {"name": "Shield", "health": 7.0, "speed": 145.0, "power": 25.0, "size": 1.10, "resistance": 0.30, "points": 250, "fire_rate": 1.2, "burst": 6, "projectile": "plasma", "sprite": "enemy_model_03", "sprite_rotation": 0.0, "flip": false, "tint": Color(0.68, 1.20, 1.32)},
	"swarm": {"name": "Swarm", "health": 1.0, "speed": 250.0, "power": 8.0, "size": 0.55, "resistance": 0.00, "points": 80, "fire_rate": 2.0, "burst": 1, "projectile": "laser", "sprite": "enemy_model_02", "sprite_rotation": 0.0, "flip": true, "tint": Color(1.32, 0.88, 0.52)},
	"elite": {"name": "Elite", "health": 5.0, "speed": 205.0, "power": 35.0, "size": 1.15, "resistance": 0.20, "points": 400, "fire_rate": 2.0, "burst": 4, "projectile": "plasma", "sprite": "enemy_model_03", "sprite_rotation": 0.0, "flip": false, "tint": Color(1.30, 1.16, 0.68)},
	"ghost": {"name": "Ghost", "health": 2.0, "speed": 175.0, "power": 22.0, "size": 1.00, "resistance": 0.40, "points": 220, "fire_rate": 1.0, "burst": 2, "projectile": "plasma", "sprite": "enemy_model_01", "sprite_rotation": 0.0, "flip": false, "tint": Color(0.78, 0.84, 1.30)},
	"boss": {"name": "Boss", "health": 1000.0, "speed": 72.0, "power": 50.0, "size": 3.20, "resistance": 0.30, "points": 5000, "fire_rate": 2.5, "burst": 8, "projectile": "missile", "sprite": "enemy_model_04", "sprite_rotation": 0.0, "flip": false, "tint": Color(1.30, 0.62, 0.68)},
}

const PROJECTILES := {
	"player_bullet": {"speed": 600.0, "damage": 1.0, "radius": 4.0, "lifetime": 2.2, "homing": 0.0, "color": Color("ffe45e")},
	"laser": {"speed": 500.0, "damage": 10.0, "radius": 4.0, "lifetime": 3.0, "homing": 0.0, "color": Color("ff4545")},
	"plasma": {"speed": 300.0, "damage": 20.0, "radius": 7.0, "lifetime": 4.0, "homing": 0.30, "color": Color("64c8ff")},
	"missile": {"speed": 280.0, "damage": 50.0, "radius": 8.0, "lifetime": 8.0, "homing": 0.70, "color": Color("ff9d24")},
	"energy_orb": {"speed": 150.0, "damage": 35.0, "radius": 11.0, "lifetime": 10.0, "homing": 0.0, "color": Color("d24cff")},
}

const DROP_RATES := {
	"grunt": [10.0, 4.0, 0.5, 12.0],
	"tank": [20.0, 20.0, 8.0, 20.0],
	"speeder": [8.0, 7.0, 1.0, 15.0],
	"zigzag": [12.0, 10.0, 2.5, 12.0],
	"bomber": [15.0, 15.0, 5.0, 18.0],
	"shield": [10.0, 30.0, 4.0, 14.0],
	"swarm": [6.0, 3.0, 0.2, 10.0],
	"elite": [22.0, 22.0, 7.0, 22.0],
	"ghost": [18.0, 12.0, 3.0, 16.0],
	"boss": [0.0, 0.0, 0.0, 100.0],
}

const LEVELS := [
	{
		"number": 1, "name": "Initiation", "duration": 553.82, "target_score": 5000,
		"description": "Aprende los sistemas, resiste la escalada y derrota al jefe.",
		"music": "res://assets/audio/level1.mp3", "fire_from": 55.0, "boss_time": 427.0,
		"phases": [
			{"name": "Tutorial", "start": 0.0, "end": 55.0, "roster": ["grunt"], "interval": 6.0, "count": 1, "pattern": "straight"},
			{"name": "First Combat", "start": 55.0, "end": 87.0, "roster": ["grunt", "swarm"], "interval": 3.2, "count": 2, "pattern": "formation"},
			{"name": "Tank Assault", "start": 87.0, "end": 110.0, "roster": ["tank", "speeder", "grunt"], "interval": 3.8, "count": 1, "pattern": "tank_assault"},
			{"name": "Recovery", "start": 110.0, "end": 125.0, "roster": ["grunt"], "interval": 7.0, "count": 1, "pattern": "straight"},
			{"name": "Swarm Assault", "start": 125.0, "end": 150.0, "roster": ["swarm", "speeder"], "interval": 2.0, "count": 3, "pattern": "v_formation"},
			{"name": "Recovery", "start": 150.0, "end": 165.0, "roster": ["grunt"], "interval": 6.0, "count": 1, "pattern": "straight"},
			{"name": "Mixed Assault", "start": 165.0, "end": 195.0, "roster": ["grunt", "zigzag", "speeder", "bomber"], "interval": 2.8, "count": 2, "pattern": "erratic"},
			{"name": "Recovery", "start": 195.0, "end": 218.0, "roster": ["swarm"], "interval": 7.0, "count": 2, "pattern": "formation"},
			{"name": "Elite Encounter", "start": 218.0, "end": 245.0, "roster": ["elite", "speeder", "zigzag"], "interval": 3.1, "count": 1, "pattern": "elite_retreat"},
			{"name": "Recovery", "start": 245.0, "end": 265.0, "roster": ["grunt", "swarm"], "interval": 6.5, "count": 1, "pattern": "straight"},
			{"name": "Shield Assault", "start": 265.0, "end": 295.0, "roster": ["shield", "zigzag", "ghost"], "interval": 3.0, "count": 2, "pattern": "hover"},
			{"name": "Recovery", "start": 295.0, "end": 310.0, "roster": ["grunt"], "interval": 7.0, "count": 1, "pattern": "straight"},
			{"name": "Heavy Assault", "start": 310.0, "end": 345.0, "roster": ["tank", "bomber", "speeder", "swarm"], "interval": 2.8, "count": 2, "pattern": "tank_assault"},
			{"name": "Major Recovery", "start": 345.0, "end": 368.0, "roster": ["grunt"], "interval": 8.0, "count": 1, "pattern": "straight"},
			{"name": "Elite Assault", "start": 368.0, "end": 398.0, "roster": ["elite", "shield", "bomber"], "interval": 2.7, "count": 2, "pattern": "flanking"},
			{"name": "Final Recovery", "start": 398.0, "end": 415.0, "roster": ["swarm"], "interval": 6.0, "count": 2, "pattern": "formation"},
			{"name": "Pre-Boss Build", "start": 415.0, "end": 427.0, "roster": ["elite", "speeder", "bomber"], "interval": 2.0, "count": 2, "pattern": "rush"},
			{"name": "Boss Battle", "start": 427.0, "end": 517.0, "roster": ["swarm", "grunt"], "interval": 8.0, "count": 2, "pattern": "minion"},
			{"name": "Final Push", "start": 517.0, "end": 553.82, "roster": ["grunt", "swarm", "zigzag"], "interval": 5.0, "count": 2, "pattern": "straight"},
		]
	},
	{
		"number": 2, "name": "Escalation", "duration": 612.13, "target_score": 10000,
		"description": "Combate inmediato, presión constante y un último asalto extremo.",
		"music": "res://assets/audio/level2.mp3", "fire_from": 0.5, "boss_time": 510.0,
		"phases": [
			{"name": "Instant Action", "start": 0.0, "end": 0.5, "roster": ["speeder"], "interval": 0.4, "count": 3, "pattern": "rush"},
			{"name": "High Intensity", "start": 0.5, "end": 90.0, "roster": ["elite", "tank", "zigzag", "speeder", "shield", "bomber", "swarm"], "interval": 2.2, "count": 2, "pattern": "erratic"},
			{"name": "Continuous Combat", "start": 90.0, "end": 370.0, "roster": ["tank", "elite", "bomber", "ghost", "shield", "zigzag", "speeder", "swarm"], "interval": 1.9, "count": 2, "pattern": "flanking"},
			{"name": "Calm Recovery", "start": 370.0, "end": 480.0, "roster": ["grunt", "swarm", "speeder"], "interval": 5.0, "count": 1, "pattern": "straight"},
			{"name": "Extreme Intensity", "start": 480.0, "end": 510.0, "roster": ["elite", "tank", "shield", "bomber", "speeder", "swarm"], "interval": 1.4, "count": 3, "pattern": "rush"},
			{"name": "Boss Battle", "start": 510.0, "end": 600.0, "roster": ["grunt", "swarm", "elite"], "interval": 6.5, "count": 2, "pattern": "minion"},
			{"name": "Victory Lap", "start": 600.0, "end": 612.13, "roster": ["swarm", "grunt"], "interval": 5.0, "count": 2, "pattern": "formation"},
		]
	},
]

static func level(index: int) -> Dictionary:
	return LEVELS[clampi(index, 0, LEVELS.size() - 1)]

static func player_ship(ship_id: String) -> Dictionary:
	if PLAYER_SHIPS.has(ship_id):
		return PLAYER_SHIPS[ship_id]
	return PLAYER_SHIPS["vindicator"]

static func difficulty_scale(difficulty: int) -> Dictionary:
	const SCALES := [
		{"enemy_health": 0.80, "enemy_damage": 0.70, "spawn_rate": 0.85, "score": 0.80},
		{"enemy_health": 1.00, "enemy_damage": 1.00, "spawn_rate": 1.00, "score": 1.00},
		{"enemy_health": 1.35, "enemy_damage": 1.25, "spawn_rate": 1.20, "score": 1.35},
		{"enemy_health": 1.75, "enemy_damage": 1.55, "spawn_rate": 1.45, "score": 1.75},
	]
	return SCALES[clampi(difficulty, 0, SCALES.size() - 1)]
