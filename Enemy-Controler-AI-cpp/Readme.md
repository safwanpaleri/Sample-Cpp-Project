# Enemy Controller AI cpp

This is the implementation of an AI controller for an enemy character in a game. The code is written in C++ using the Unreal Engine API.

The BeginPlay() function waits for one second and then gets a reference to the player character and the character script attached to the AI controller.

The Tick() function is called every frame and checks if the AI controller has been activated by the player character. If it has not been activated, it checks if the player character is in front of the enemy character. If the player character is in front of the enemy character, the enemy character is activated.

If the enemy character is activated, the AI controller gets the location of the enemy character and the player character. It then checks if the player character is within the attack range of the enemy character. If the player character is not within the attack range, the AI controller moves the enemy character towards the player character or initiates a ranged attack depending on a random number. If the player character is within the attack range, the AI controller initiates a melee attack.

The IsWithinRange() function calculates the distance between two locations and returns whether they are within a certain range.

The RangedAttack() function handles the ranged attack of the enemy character. If the enemy character is not already doing a ranged attack and the player character is not dead, the function initiates the ranged attack and decreases the speed of the player character. If the enemy character is already doing a ranged attack, the function decreases the health of the player character and increases the magnitude of the damage taken by the player character over time. If the player character moves out of range of the ranged attack, the function resets the speed of the player character and the magnitude of the damage taken.

The MeleeAttack() function handles the melee attack of the enemy character. If the enemy character is not already attacking or hurt, the function gives the player character a danger sense warning and initiates the attack. After half a second, the function returns the time to normal.
