// Run Using this : g++ main.cpp   -o game   -I$HOME/raylib/src   -L$HOME/raylib/src   -lraylib   -lm -ldl -lpthread   -lX11

#include "raylib.h"
#include "string"

// Used as a Background for Ui elements
class Panel {
    public:
    Vector2 position = Vector2{0, 0};
    Vector2 size = Vector2{0, 0};
    Color color = WHITE;
    Vector2 padding = Vector2{0, 0};

    void Draw() {
        DrawRectangle(position.x, position.y, size.x + padding.x, size.y + padding.y, color);
    }
};
// Display Text inside the window
class Label {
    private:
    std::string text = "";
    int textSize = 16;
    Vector2 position;
    Color color = BLACK;
    Panel background;

    public:
    bool bgActive = true; // Whether to show BG or not

    Label(std::string newText, int xPosition, int yPosition, int newTextSize)
    : text(newText), position{(float)xPosition, (float)yPosition}, textSize(newTextSize){
        rePositionBg();
    }

    void setText(std::string newText) {
        text = newText;
        rePositionBg();
    }
    void setTextSize(int newTextSize) {
        textSize = newTextSize;
        rePositionBg();
    }
    void setPosition(int xPosition, int yPosition) {
        position = Vector2{(float)xPosition, (float)yPosition};
        background.position = position;
    }
    void setBgPadding(int widthPad, int heightPad) {
        background.padding = Vector2{(float)widthPad, (float)heightPad};
    }
    void setTextColor(Color newColor) {
        color = newColor;
    }
    void setBgColor(Color newColor) {
        background.color = newColor;
    }

    void Draw() {
        if (bgActive) {
            background.Draw();
            DrawText(text.c_str(), position.x + background.padding.x/2, position.y + background.padding.y/2, textSize, color);
        }
        else {
            DrawText(text.c_str(), position.x, position.y, textSize, color);
        }
    }

    private:
    // used to set the size and position of BG correctly 
    void rePositionBg() {
        background.position = position;
        background.size = {
            (float)MeasureText(text.c_str(), textSize),
            (float)textSize
        };
    }
};

int main(void) {
    int windowWidth = 960;
    int windowHeight = 540;

    InitWindow(windowWidth, windowHeight, "My GUI Library");
    SetTargetFPS(60);

    Label testLbl = Label("This is a Test Label Object", 10, 10, 16);
    testLbl.setTextColor(BLUE);
    testLbl.setBgColor(BLACK);
    testLbl.setText("dkjcskjgdck");
    testLbl.setTextSize(20);
    testLbl.setPosition(100, 20);
    testLbl.setBgPadding(30, 40);

    while(!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        const char* text = "My GUI Library";
        int textSize = 20;
        int textWidth = MeasureText(text, textSize);
        DrawText(text, (windowWidth / 2 - textWidth / 2), windowHeight/2, textSize, GRAY);

        testLbl.Draw();
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
