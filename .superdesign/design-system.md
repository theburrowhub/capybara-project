# Capybara Project — Ship Selection Design System

## Product context

Capybara Project is a music-driven horizontal shoot ’em up built in Godot 4. Its fixed logical viewport is 1200 × 600. The requested screen is a new pre-flight ship selector reached from START MISSION; gameplay must begin only after the pilot confirms Vindicator, Sting, or Goliat.

## Visual direction

Build a confident 1990s arcade-selection screen layered onto the existing dark space-flight UI. It should feel like a high-end cabinet attract screen: sharp techno framing, raster-grid details, subtle CRT scanlines, stepped highlights, cyan navigation telemetry, and loud ship-specific accents. Preserve the current app’s disciplined navy/cyan foundation; do not turn it into a generic neon synthwave page.

The screen must be immediately readable at 1200 × 600. Favor rectangular and chamfered geometry over rounded web cards. Use controlled glow, not foggy gradients.

## Layout

- Keep the existing persistent header at upper left: eyebrow, CAPYBARA PROJECT title, and subtitle.
- Keep the animated starfield/navigation background and persistent controls footer.
- Use a wide selector stage from roughly y=138 to y=548.
- Top of selector stage: title “SELECT YOUR SHIP”, current index (01/03), and three horizontally arranged roster tabs.
- Primary body: a large selected-ship visual on the left; on the right, a five-axis filled radar/area chart and short profile summary.
- Bottom strip: exact stat labels/ratings plus clear BACK and DEPLOY actions.
- Keyboard-first: left/right changes ship, Enter deploys, Escape returns. Mouse buttons should work too.
- The selected ship must dominate; unselected options remain visible but subdued.
- Never start gameplay on the first click from the main menu. START MISSION opens selection; DEPLOY confirms the current ship and starts gameplay.

## Ship identities and comparison data

Use five radar axes in this exact order: SPEED, HULL, SHIELD, FIREPOWER, RECOVERY. Plot on a 1–5 scale so tradeoffs are unmistakable.

- VINDICATOR · red · role BALANCED STRIKER · stats 3 / 3 / 3 / 3 / 3. Existing initial ship.
- STING · blue · role RAPID INTERCEPTOR · stats 5 / 2 / 2 / 2 / 5. Substantially faster; lower endurance and firepower; bars recover quickly.
- GOLIAT · green · role HEAVY DREADNOUGHT · stats 1 / 5 / 5 / 5 / 1. Very slow; overwhelming firepower; substantially stronger shield and hull; recovery is painfully slow.

Use accent colors:
- Vindicator red: #FF445E; dark red: #7A1F35.
- Sting blue: #35A7FF; bright blue: #5FD7FF.
- Goliat green: #5DE071; dark green: #1D7A45.

The selected ship’s accent colors the roster cursor, ship frame, chart polygon, numeric ratings, and DEPLOY button. Keep health/shield/energy semantic colors recognizable in gameplay.

## Base tokens from the existing game

- Background: #020613.
- Panel surface: rgba(6, 14, 31, 0.94).
- Panel border: rgba(33, 140, 199, 0.58).
- Primary cyan: #45D6FF; bright cyan: #56D7FF.
- Main text: #EDFAFF; section text: #E8F9FF.
- Secondary text: #7692B7; control label: #86A5D0.
- Energy yellow: #FFE45E.
- Shadows: black at 55%, up to 18 px.
- Base spacing unit: 4 px; common gaps: 8, 10, 16, 24, 32.
- Existing title scale: 44 px; section heading 22 px; body/button 16 px; labels 13–15 px.

## Typography

Use only resilient system fonts:
- Display/ship name: “Arial Black”, Impact, sans-serif; uppercase, tight tracking.
- Telemetry, labels, buttons, numbers: “Courier New”, ui-monospace, monospace.
- Supporting copy: Arial, Helvetica, sans-serif.

Do not load or invent decorative web fonts.

## Components

- Selector stage: deep navy translucent panel with 1 px cyan technical frame, clipped/chamfered corners, internal grid marks, and a restrained inset glow.
- Roster tabs: 3 equal cells showing index, ship name, role. Selected tab gets a bright ship-color top bar and cursor chevrons; inactive tabs use muted navy/blue.
- Ship viewport: true ship imagery centered over a circular targeting reticle and horizontal blueprint grid. Use no placeholder icon if actual ship imagery is supplied.
- Radar chart: five concentric polygon rings, labeled axes, muted cyan grid, selected ship-color fill at 30–40% opacity, bright 2 px outline, visible vertices. Show Vindicator’s balanced dotted reference polygon behind Sting/Goliat when useful.
- Stat rows: label, five rectangular pips, and a short plain-language tradeoff.
- DEPLOY: strong ship-color fill or outlined cabinet button with double border and a pulsing focus caret.
- BACK: secondary cyan outline.
- Focus: unmistakable white text, selected accent border, and side chevrons.

## Motion

- Screen entrance: 180–240 ms stepped horizontal wipe.
- Ship switch: 120–180 ms scan/glitch transition plus chart polygon interpolation.
- Selected ship: very slow float/roll, no more than 4 px and 2 degrees.
- DEPLOY focus cursor: 700 ms pulse.
- Respect a reduced-motion path by collapsing transitions to simple opacity changes.

## Content rules

- Keep labels terse and uppercase.
- Avoid paragraphs longer than two short lines.
- Do not use emoji.
- Do not use rounded SaaS cards, soft pastel gradients, glassmorphism blobs, or generic dashboard widgets.
- Use ONLY the fonts, colors, spacing, and component styles defined here. Do not introduce any fonts, colors, or visual styles not in this design system.
