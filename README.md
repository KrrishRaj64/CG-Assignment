# CG-Assignment

Refactored OpenGL Chaplin animation into a modular multi-file layout.

## Project Structure

- `main.cpp`: Application entry point and GLUT bootstrap.
- `world/`: Shared state, scene update logic, and app callbacks.
- `objects/`: Character and machine model drawing.
- `assets/`: Reusable primitives and world/environment rendering helpers.

## Build

```bash
g++ main.cpp world/*.cpp objects/*.cpp assets/*.cpp -o chaplin -lGL -lGLU -lglut -lm
```

## Run

```bash
./chaplin
```
