# Extractable Components

No existing component should be uploaded as a reusable Superdesign DraftComponent.

The project uses Godot-native runtime nodes rather than HTML/React components. Converting these scripts to Petite-Vue would create a parallel implementation instead of extracting a reusable source component. Pass the real GDScript, scenes, and design-system context directly to a design draft.

Patterns available as source context:

- Persistent menu shell from `scripts/core/main.gd`.
- Animated background from `scripts/ui/menu_background.gd`.
- Arcade selector frame from `scripts/ui/ship_selection_panel.gd`.
- Live GLB viewport from `scripts/ui/ship_showcase_preview.gd`.
- Area/radar chart from `scripts/ui/ship_radar_chart.gd`.
