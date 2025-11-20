# AntsRemake
This project aims to recreate the old Microsoft Ants game from the 90s-early 2000.  
The main goal is to make it playable on modern systems without depending on external tools.  
Secondary goals include expanding the game with more content, adding quality-of-life features, fixing old bugs, adding support for more than 4 players.  

# Modifying / adding new content
Inside the Data folder are a few script-like files:
* food.txt: The food database, containing data for every kind of food, like the amount of points it gives, the size of the collision box, animations frames, and such.
* obstacles.txt: Data for solid objects.
* animations.txt: allow modifiying a few animations.

# Compiling
The project is developed in C++ using my custom game library [Alegrinho](https://github.com/Spacecpp/Alegrinho). Run the IDE of your choice and create a project and setup the search paths to point to the library source and pass -lAlegrinho to the linker.  
Currently it was compiled only on Windows using MinGW. I'm not sure if it is ready for other compilers/platforms.
