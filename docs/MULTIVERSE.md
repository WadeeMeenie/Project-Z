# Project-Z Multiverse

## Vision

Project-Z is being developed as an Ocarina of Time-based multiverse adventure. The Ocarina of Time engine and visual language remain the foundation, while additional incarnations of Link and timeline-specific gameplay are introduced as new game content.

## First playable milestone

The first implementation milestone is **two playable Links in one OoT world**:

- Hero of Time (existing OoT Link)
- Hero of Twilight (new playable character slot)

The player will eventually be able to switch between them during gameplay. The switch must preserve the active character's position/state and cleanly hand control to the other character.

## Architecture rules

1. Do not replace or damage the original Link actor implementation until the new system is proven.
2. Keep the original OoT Link as the baseline/default character.
3. Represent each multiverse Link as a character profile layered over the existing player/actor systems where possible.
4. Add new behavior incrementally: identity/profile -> model/assets -> switching -> unique abilities -> story/portal system.
5. Every gameplay change must be isolated in its own commit so regressions are easy to identify.
6. Do not add assets from other commercial Zelda releases to the repository. New assets should be original, properly licensed, or supplied by the project owner for local use.

## Planned character IDs

| ID | Character | Base game |
| --- | --- | --- |
| 0 | Hero of Time | Ocarina of Time |
| 1 | Hero of Termina | Majora's Mask |
| 2 | Hero of Twilight | Twilight Princess |
| 3 | Hero of the Sky | Skyward Sword |
| 4 | Hero of the Wild | Breath of the Wild |
| 5 | Hero of the Kingdom | Tears of the Kingdom |

## Planned progression

### Phase 1 — Foundation

- [x] Create isolated `modding` branch
- [x] Document multiverse architecture
- [ ] Establish a clean baseline build using a legally obtained compatible base ROM

### Phase 2 — Two-Link prototype

- [ ] Add character/profile state
- [ ] Add Hero of Twilight character slot
- [ ] Add a temporary OoT-compatible placeholder appearance
- [ ] Add player switching
- [ ] Preserve position/health/state across switches
- [ ] Validate switching in a normal OoT scene

### Phase 3 — Multiverse systems

- [ ] Portal actor/effect
- [ ] Timeline arrival sequence
- [ ] Party/follower representation
- [ ] Six-Link character roster
- [ ] Character-specific abilities

### Phase 4 — Story

- [ ] Timeline fracture event
- [ ] Multiple Ganondorf encounters
- [ ] Timeline-specific dungeons/events
- [ ] Final convergence and multiverse Ganondorf encounter

## Technical constraint

The zeldaret/oot project is a work-in-progress decompilation. The project documentation notes that some areas can change substantially and that some ROM regions are not yet straightforward to shift. Therefore, the prototype should favor small, reversible changes over a large rewrite.
