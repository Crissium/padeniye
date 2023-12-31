# Note

I discovered this project when I was cleaning up my old files. I wrote this game in May 2022, when I was a first-year university student. Much of the code was adapted from Gnome's _Quadrapassel_, and the style is not good. But it indeed works and works well, its AI beating that of KDE's Tetris clone by a very large margin. Anyway, it is an interesting project, so I decided to clean it up a little, removing some hard-coded media paths and silly Russian sentences (the commissar shoots you upon failure :), and publish it here.

![demo](/demo.gif)

---

# Description

Classic Tetris game developed with SFML-2.

__Special Feature__: AI, quite short-sighted, though.

# Controls

- The arrow keys: These do what you think they do
- R: (Re-)start game
- A: An AI move
- Q: Quit and save current game to the working directory
- S: Toggle 'shadow'

# Build

```
$ make
```

This game depends on SFML 2.5 and you need a C++17-compliant compiler to build.

Note: The paths of the font file and images used for rendering is hard-coded.
