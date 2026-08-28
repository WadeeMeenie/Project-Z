#ifndef MULTIVERSE_H
#define MULTIVERSE_H

#include "ultra64.h"

/**
 * Playable heroes represented by the Project-Z multiverse system.
 *
 * The first implementation deliberately keeps this independent from the
 * Player struct. This lets us establish the timeline/character state without
 * duplicating the very large Link actor or coupling the system to rendering.
 */
typedef enum MultiverseHero {
    MULTIVERSE_HERO_TIME = 0,
    MULTIVERSE_HERO_TERMINA,
    MULTIVERSE_HERO_TWILIGHT,
    MULTIVERSE_HERO_SKY,
    MULTIVERSE_HERO_WILD,
    MULTIVERSE_HERO_KINGDOM,
    MULTIVERSE_HERO_MAX,
} MultiverseHero;

void Multiverse_Reset(void);
void Multiverse_SelectNext(void);
void Multiverse_SelectPrevious(void);
MultiverseHero Multiverse_GetActiveHero(void);
const char* Multiverse_GetHeroName(MultiverseHero hero);

#endif
