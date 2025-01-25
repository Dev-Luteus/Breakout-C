#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <stdbool.h>

#define MAX_LEADERBOARD_ENTRIES 10
#define LEADERBOARD_FILE "leaderboard.dat"

#define LB_FONT_SIZE 30
#define LB_PADDING 20

typedef struct {
    char date[20];       // YYYY-MM-DD - HH:MM:SS
    int score;
    int maxCombo;
} LeaderboardEntry;

typedef struct {
    LeaderboardEntry entries[MAX_LEADERBOARD_ENTRIES];
    int count;
} Leaderboard;

// Core functions
Leaderboard InitLeaderboard(void);
bool SaveLeaderboard(const Leaderboard* leaderboard);
bool LoadLeaderboard(Leaderboard* leaderboard);
void AddLeaderboardEntry(Leaderboard* leaderboard, int score, int maxCombo);
void DrawLeaderboardScreen(const Leaderboard* leaderboard, int screenWidth, int screenHeight);

#endif