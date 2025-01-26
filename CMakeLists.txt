cmake_minimum_required(VERSION 3.30)
project(RaylibGame C)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Add the include directory for headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the library directory for linking
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Add the executable // RaylibGame old name
add_executable(
        RaylibGame
        main.c Game.c
        Player.c
        Block.c BlocksManager.c
        Ball.c
        VectorMath.c
        PowerUp.c
        MainMenu.c
        include/Leaderboard.h
        Leaderboard.c
        include/Background.h
        Background.c
)

# Link Raylib library (and required Windows libraries)
target_link_libraries(RaylibGame raylib winmm)
