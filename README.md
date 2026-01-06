# Mario Game in CSFML
A simplistic Mario clone created with C and CSFML (a C SFML binding). I made it as a semester project for my programming course during the first semester. 

Mario can move left, right, and jump throughout the game, and Goomba, an enemy, can move simply by following Mario to the left or right in relative to its own position.

## Preview

![A screenshot showing the game running.](/images/preview.png)

## Build and Run

You must compile the program yourself in order to run the game. Make sure your system has CSFML installed before proceeding.

To install CSFML on Linux, simply run the following command:

    sudo apt-get install libcsfml-dev

The CSFML library files (.so format) will be installed and configured automatically in the appropriate location (i.e., /usr/lib/x86_64-linux-gnu/).

For Windows and other systems, you can figure out on your own how to download and setup the CSFML library. I wouldn't bother explaining their setup process.

After successfully installing CSFML, you can follow the following steps:

1. Run the following command:

        git clone https://github.com/Shaheer-Zeb/mario-in-csfml.git
   
   Make sure that you have git installed on your system.
2. cd into the folder:

        cd mario-in-csfml
   
3. Run the following command to link and compile all the files:

        gcc main.c -o mario -lcsfml-window -lcsfml-audio -lcsfml-system -lcsfml-graphics functions.c
   
   An executable named "mario" will be created.
5. Run the executeable:

        ./mario
   
