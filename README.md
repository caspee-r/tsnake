# Terminal Snake Game

A simple terminal-based Snake game written in C using the POSIX API (`unistd`, signals, etc.). This project implements the classic snake game logic with a basic user interface suitable for a Unix-like terminal environment.

## Requirements

- GCC (or any C compiler supporting POSIX)
- `make`

## Building

To compile the game, use the provided `Makefile`:

```bash
make
```

This will produce an executable named `tsnake`.

## Running

```bash
./tsnake
```

Controls:
- `WASD`: Move the snake
- `Ctrl+C`: Quit the game

## Known Issues / TODO

- No pause/resume functionality
- Snake speed not configurable via command-line args
- No sound or effects
- Resize handling is minimal

## License

This project is provided as-is with no warranty. You are free to modify and redistribute.
