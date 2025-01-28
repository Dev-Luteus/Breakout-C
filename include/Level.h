#ifndef LEVEL_H
#define LEVEL_H

#include <Game.h>

#define LEVEL_BONUS_MULTIPLIER 100
#define SCORE_BONUS_MULTIPLIER 0.25f

void DrawLevelComplete(Game game);
void LoadNextLevel(Game* game);
void CalculateLevelProgression(int currentLevel, float* speedIncrease, float* widthDecrease);
void InitializeLevel(Game* game, int level);
int CalculateLevelBonus(int level, int currentScore);

#endif //LEVEL_H
