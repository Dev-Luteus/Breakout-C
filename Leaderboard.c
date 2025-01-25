#include <Leaderboard.h>
#include <stdio.h>
#include <raylib.h>
#include <time.h>

Leaderboard InitLeaderboard(void)
{
    Leaderboard leaderboard = {0};
    LoadLeaderboard(&leaderboard);

    return leaderboard;
}

bool SaveLeaderboard(const Leaderboard* leaderboard)
{
    FILE* file = fopen(LEADERBOARD_FILE, "wb");

    if (!file)
    {
        printf("Failed to open leaderboard file\n");
        return false;
    }

    // Here we're writing our leaderboard struct in bytes to our save file
    // size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
    size_t written = fwrite(leaderboard, sizeof(Leaderboard), 1, file);
    fclose(file);

    return written == 1;
}

bool LoadLeaderboard(Leaderboard* leaderboard)
{
    FILE* file = fopen(LEADERBOARD_FILE, "rb");

    if (!file)
    {
        return false;
    }

    size_t read = fread(leaderboard, sizeof(Leaderboard), 1, file);
    fclose(file);

    return read == 1;
}

void AddLeaderboardEntry(Leaderboard* leaderboard, int score, int maxCombo)
{
    time_t now;
    time(&now);
    struct tm* timeinfo = localtime(&now);

    LeaderboardEntry newEntry =
    {
        .score = score,
        .maxCombo = maxCombo
    };

    // Here we're formatting the date and time to a string
    strftime(newEntry.date, sizeof(newEntry.date), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Find new entry position: sort by score
    int insertPos = 0;

    while (insertPos < leaderboard->count && insertPos < MAX_LEADERBOARD_ENTRIES &&
           leaderboard->entries[insertPos].score > score)
    {
        insertPos++;
    }

    // If Score too low and board full
    if (insertPos >= MAX_LEADERBOARD_ENTRIES)
    {
        return;
    }

    // Compare scores to sort them!
    for (int i = (leaderboard->count < MAX_LEADERBOARD_ENTRIES - 1 ?
              leaderboard->count : MAX_LEADERBOARD_ENTRIES - 1);
              i > insertPos; i--)
    {
        leaderboard->entries[i] = leaderboard->entries[i - 1];
    }

    // Insert new entry
    leaderboard->entries[insertPos] = newEntry;
    if (leaderboard->count < MAX_LEADERBOARD_ENTRIES)
    {
        leaderboard->count++;
    }

    // Save after updating!
    SaveLeaderboard(leaderboard);
}

void DrawLeaderboardScreen(const Leaderboard* leaderboard, int screenWidth, int screenHeight)
{
    // Calculate total height needed for the leaderboard
    const int rowSpacing = LB_FONT_SIZE + 20;
    const int totalRows = MAX_LEADERBOARD_ENTRIES;
    const int headerSpacing = LB_PADDING * 2;
    const int titleSpacing = LB_PADDING * 4;

    int totalHeight =
        titleSpacing +                              // Space above title
        (LB_FONT_SIZE * 2) +                       // Title height
        headerSpacing +                            // Space between title and headers
        LB_FONT_SIZE +                            // Headers height
        headerSpacing +                            // Space between headers and entries
        (rowSpacing * totalRows);                 // All rows height

    // Calculate starting Y position to center everything
    int startY = (screenHeight - totalHeight) / 2;

    // Title
    const char* title = "LEADERBOARD";
    int titleFontSize = LB_FONT_SIZE * 2;
    int titleWidth = MeasureText(title, titleFontSize);

    int currentY = startY;

    DrawText(title,
        screenWidth/2 - titleWidth/2,
        currentY,
        titleFontSize,
        WHITE);

    // Update currentY for headers
    currentY += titleSpacing + titleFontSize;

    // Calculate header texts and their widths
    const char* headerRank = "RANK";
    const char* headerScore = "SCORE";
    const char* headerCombo = "MAX COMBO";

    int rankWidth = MeasureText(headerRank, LB_FONT_SIZE);
    int scoreWidth = MeasureText(headerScore, LB_FONT_SIZE);
    int comboWidth = MeasureText(headerCombo, LB_FONT_SIZE);

    // Calculate column positions from center
    const int columnSpacing = LB_PADDING * 10;
    const int centerX = screenWidth/2;

    // Define column centers - adjusted for 3 columns
    const int rankX = centerX - columnSpacing;
    const int scoreX = centerX;
    const int comboX = centerX + columnSpacing;

    // Draw headers
    DrawText(headerRank, rankX - rankWidth/2, currentY, LB_FONT_SIZE, GRAY);
    DrawText(headerScore, scoreX - scoreWidth/2, currentY, LB_FONT_SIZE, GRAY);
    DrawText(headerCombo, comboX - comboWidth/2, currentY, LB_FONT_SIZE, GRAY);

    // Update currentY for entries
    currentY += headerSpacing + LB_FONT_SIZE;

    // Main draw loop - always draw 10 rows
    for (int i = 0; i < MAX_LEADERBOARD_ENTRIES; i++)
    {
        Color color = (i == 0) ? GOLD : (i == 1) ? LIGHTGRAY : (i == 2) ? BROWN : WHITE;

        // Rank column
        char rank[4];
        sprintf(rank, "#%d", i + 1);
        int currentRankWidth = MeasureText(rank, LB_FONT_SIZE);
        DrawText(rank, rankX - currentRankWidth/2, currentY, LB_FONT_SIZE, color);

        if (i < leaderboard->count)
        {
            const LeaderboardEntry* entry = &leaderboard->entries[i];

            // Score
            char score[32];
            sprintf(score, "%d", entry->score);
            int currentScoreWidth = MeasureText(score, LB_FONT_SIZE);
            DrawText(score, scoreX - currentScoreWidth/2, currentY, LB_FONT_SIZE, color);

            // Combo
            char combo[32];
            sprintf(combo, "x%d", entry->maxCombo);
            int currentComboWidth = MeasureText(combo, LB_FONT_SIZE);
            DrawText(combo, comboX - currentComboWidth/2, currentY, LB_FONT_SIZE, color);
        }
        else
        {
            // Empty rows
            const char* dots = "...";
            int dotsWidth = MeasureText(dots, LB_FONT_SIZE);

            DrawText(dots, scoreX - dotsWidth/2, currentY, LB_FONT_SIZE, DARKGRAY);
            DrawText(dots, comboX - dotsWidth/2, currentY, LB_FONT_SIZE, DARKGRAY);
        }

        currentY += rowSpacing;
    }

    // Instructions at bottom
    const char* instructions = "Press Q to return to menu";
    int instrWidth = MeasureText(instructions, LB_FONT_SIZE);

    DrawText(instructions,
        screenWidth/2 - instrWidth/2,
        screenHeight - LB_PADDING * 5,
        LB_FONT_SIZE,
        GRAY);
}