# 📦 LivreGameJam2025

**Livre Game Jam Entry 🎮** — Our project for **Livre Game Jam 2025!**  
Written entirely in **C** with Raylib, featuring a custom build system — because who needs *Make* or *CMake*, am I right? 😄

---

## 🚀 How to compile the build system

All you need is a C compiler installed.  
Compile the build system with:

```bash
gcc build.c -o build
````

Done! Now you have your own build executable (`./build`).

---

## 🕹️ How to run the game (native version)

### 1. Build dependencies (vendors)

```bash
./build vendors
```

### 2. Build the game

```bash
./build game
```

### 3. Build and run the game directly

```bash
./build game run
```

---

## 🌐 Running the game on the Web (WebAssembly)

You can also run it in your browser! Make sure you have the Emscripten SDK installed and configured in your environment. Take a look [here](https://github.com/emscripten-core/emsdk) for it

### 1. Build dependencies for the web

```bash
./build vendors web
```

### 2. Build the game for the web

```bash
./build game web
```

### 3. Build and run the game in the browser

```bash
./build game web run
```

```
```

