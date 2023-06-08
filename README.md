# ONRL - _One Night RogueLike_

In the interest of providing Sunday an example of my gamedev work, I'm going to vomit up a quick and dirty roguelike. I'm going to use C++ and SFML, because I'm familiar with them.

SFML mostly because it'll be easy to make some guarantees w.r.t. color and sizes.

----

TODO:

    - [ ] Make a window.
    - [ ] Make a player, let it walk around.
    - [ ] Make a map, let the player walk around in it.
    - [ ] Make a monster, let it walk around (brownian?)
    - [ ] Time Systems
        - [ ] Make a turn clock
        - [ ] Entities act when $SPEED ticks have passed.
    - [ ] Combat
        - [ ] Player can attack monster
        - [ ] Monster can attack player
        - [ ] Monster can die
        - [ ] Player can die
    - [ ] Items
        - [ ] Player can pick up items
        - [ ] Player can drop items
        - [ ] Player can use items
        - [ ] Items can be weapons
        - [ ] Items can be armor
        - [ ] Items can be consumables
    - [ ] Experience & levels?
        - [ ] Player can gain experience
        - [ ] Player can level up
        - [ ] Player can gain stats