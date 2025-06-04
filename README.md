1. **Clone project:**

   ```bash
   git clone https://github.com/your-username/your-repo-name.git
   cd your-repo-name
   ```

2. **Open the project in Xcode:**

   * Double-click the `.xcodeproj` file (or `.xcworkspace` if using CocoaPods).
   * Make sure your target is selected.

3. **Build the project:**

   * Select `Product > Build` (or press `Cmd + B`).

4. **Run the game:**

   * Click the play button in Xcode, or use `Cmd + R`.

#### ⚠️ Note:

Make sure SDL3 is properly installed with Homebrew:

```bash
brew install sdl3
```

And linked in your Xcode project:

* Add `/opt/homebrew/include/SDL3` to your Header Search Paths.
* Add `/opt/homebrew/lib` to your Library Search Paths.
* Link with `libSDL3.dylib`.

---

### 🖥️ **Instructions for Linux or Windows (CLI, no IDE)**

#### ✅ **To compile and run:**

Make sure you have **SDL3 installed** and compile using:

```bash
g++ main.cpp -o game -std=c++17 `sdl3-config --cflags --libs`
./game
```

If `sdl3-config` doesn't exist, manually include the SDL3 paths:

```bash
g++ main.cpp -o game -std=c++17 -I/path/to/SDL3/include -L/path/to/SDL3/lib -lSDL3
./game
```
