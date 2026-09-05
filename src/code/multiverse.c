#include "multiverse.h"

static MultiverseHero sActiveHero = MULTIVERSE_HERO_TIME;

static const char* sHeroNames[MULTIVERSE_HERO_MAX] = {
    "Hero of Time", "Hero of Termina", "Hero of Twilight", "Hero of the Sky", "Hero of the Wild", "Hero of the Kingdom",
};

void Multiverse_Reset(void) {
    sActiveHero = MULTIVERSE_HERO_TIME;
}

void Multiverse_SelectNext(void) {
    sActiveHero++;
    if (sActiveHero >= MULTIVERSE_HERO_MAX) {
        sActiveHero = MULTIVERSE_HERO_TIME;
    }
}

void Multiverse_SelectPrevious(void) {
    if (sActiveHero == MULTIVERSE_HERO_TIME) {
        sActiveHero = MULTIVERSE_HERO_MAX - 1;
    } else {
        sActiveHero--;
    }
}

MultiverseHero Multiverse_GetActiveHero(void) {
    return sActiveHero;
}

const char* Multiverse_GetHeroName(MultiverseHero hero) {
    if (hero < MULTIVERSE_HERO_TIME || hero >= MULTIVERSE_HERO_MAX) {
        return "Unknown Hero";
    }

    return sHeroNames[hero];
}
