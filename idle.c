#include "idle.h"

const char *idle_msgs[] = {
    "Spravna krychle tuhne rychle.",
    "Nevstupovat, vstupuje samo.",
    "Skokana kdyz jeste zije - ujima se policie",
    "All your base are belong to us.",
    "Rozum a chtic nemohou byt trvale v opozici.",
    "Pica pice puci klice.",
    "Nasral franta na trabanta.",
    "Na zacatku bylo slovo. To slovo bylo mrdat.",
    "Segmentation fault (core dumped).",
    "Floating point exception.",
    "Banik pyco!",
    "Racek racku posral packu.",
    "Doplnuj smajly, nebo ti odeberu licenci to pouzivat!",
    "Hej, tohle je posledni idle zprava, tzn. (rand() + 1) % idle_msgs == 0, mazec!"
};

const int idle_msgs_count = sizeof(idle_msgs)/sizeof(*idle_msgs);
