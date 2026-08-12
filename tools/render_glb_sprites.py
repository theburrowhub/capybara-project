"""Render banked transparent PNG sprites from the bundled enemy GLB sources.

Run with Blender, not regular Python:
  blender --background --python tools/render_glb_sprites.py -- \
    --input assets/models/enemies --output assets/sprites/enemies_3d
"""

from __future__ import annotations

import argparse
import math
import sys
from pathlib import Path

import bpy
from mathutils import Vector


BANK_ANGLES = (90.0, 67.5, 45.0, 22.5, 0.0, -22.5, -45.0, -67.5, -90.0)
MODEL_YAWS = {
    "enemy_model_03": 90.0,
    "enemy_model_04": 90.0,
}


def arguments() -> argparse.Namespace:
    raw = sys.argv[sys.argv.index("--") + 1 :] if "--" in sys.argv else []
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--size", default=384, type=int)
    return parser.parse_args(raw)


def reset_scene() -> None:
    bpy.ops.wm.read_factory_settings(use_empty=True)


def world_bounds(objects: list[bpy.types.Object]) -> tuple[Vector, Vector]:
    points = [obj.matrix_world @ Vector(corner) for obj in objects for corner in obj.bound_box]
    minimum = Vector((min(point.x for point in points), min(point.y for point in points), min(point.z for point in points)))
    maximum = Vector((max(point.x for point in points), max(point.y for point in points), max(point.z for point in points)))
    return minimum, maximum


def point_camera(camera: bpy.types.Object, target: Vector) -> None:
    camera.rotation_euler = (target - camera.location).to_track_quat("-Z", "Y").to_euler()


def add_area_light(name: str, location: tuple[float, float, float], energy: float, color: tuple[float, float, float], target: Vector) -> None:
    data = bpy.data.lights.new(name=name, type="AREA")
    data.energy = energy
    data.shape = "DISK"
    data.size = 8.0
    data.color = color
    light = bpy.data.objects.new(name, data)
    bpy.context.scene.collection.objects.link(light)
    light.location = location
    point_camera(light, target)


def render_model(source: Path, output_dir: Path, size: int) -> None:
    reset_scene()
    bpy.ops.import_scene.gltf(filepath=str(source))
    meshes = [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]
    if not meshes:
        raise RuntimeError(f"No mesh found in {source}")

    minimum, maximum = world_bounds(meshes)
    center = (minimum + maximum) * 0.5
    footprint = max(maximum.x - minimum.x, maximum.z - minimum.z)

    orientation_pivot = bpy.data.objects.new("OrientationPivot", None)
    bpy.context.scene.collection.objects.link(orientation_pivot)
    orientation_pivot.location = center
    for root in [obj for obj in bpy.context.scene.objects if obj != orientation_pivot and obj.parent is None]:
        world_transform = root.matrix_world.copy()
        root.parent = orientation_pivot
        root.matrix_world = world_transform
    pivot = bpy.data.objects.new("BankPivot", None)
    bpy.context.scene.collection.objects.link(pivot)
    pivot.location = center
    orientation_pivot.parent = pivot
    orientation_pivot.location = Vector((0.0, 0.0, 0.0))
    orientation_pivot.rotation_euler.z = math.radians(MODEL_YAWS.get(source.stem, 0.0))

    camera_data = bpy.data.cameras.new("SpriteCamera")
    camera_data.type = "ORTHO"
    camera_data.ortho_scale = footprint * 1.32
    camera = bpy.data.objects.new("SpriteCamera", camera_data)
    bpy.context.scene.collection.objects.link(camera)
    camera.location = center + Vector((0.0, max(20.0, footprint * 4.0), 0.0))
    point_camera(camera, center)
    bpy.context.scene.camera = camera

    add_area_light("Key", tuple(center + Vector((-footprint, footprint * 2.0, footprint))), 900.0, (0.72, 0.88, 1.0), center)
    add_area_light("Rim", tuple(center + Vector((footprint, footprint, -footprint))), 700.0, (1.0, 0.20, 0.12), center)
    add_area_light("TopFill", tuple(center + Vector((0.0, footprint * 3.0, 0.0))), 1600.0, (0.88, 0.94, 1.0), center)

    scene = bpy.context.scene
    scene.render.engine = "BLENDER_EEVEE"
    scene.render.resolution_x = size
    scene.render.resolution_y = size
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"
    scene.render.image_settings.color_mode = "RGBA"
    scene.render.film_transparent = True
    scene.render.image_settings.color_depth = "8"
    scene.view_settings.look = "AgX - Medium High Contrast"
    scene.view_settings.exposure = 1.15
    scene.render.resolution_percentage = 100
    scene.render.film_transparent = True
    for frame, bank_angle in enumerate(BANK_ANGLES):
        pivot.rotation_euler.x = math.radians(bank_angle)
        destination = output_dir / f"{source.stem}_bank_{frame:02d}.png"
        scene.render.filepath = str(destination)
        bpy.ops.render.render(write_still=True)
        print(f"Rendered {source.name} at {bank_angle:g}° -> {destination.name}")


def main() -> None:
    args = arguments()
    input_dir = args.input.resolve()
    output_dir = args.output.resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    sources = sorted(input_dir.glob("*.glb"))
    if not sources:
        raise SystemExit(f"No GLB files found in {input_dir}")
    for source in sources:
        render_model(source, output_dir, args.size)


if __name__ == "__main__":
    main()
