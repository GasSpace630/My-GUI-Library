// Run Using this : g++ main.cpp   -o game   -I$HOME/raylib/src   -L$HOME/raylib/src   -lraylib   -lm -ldl -lpthread   -lX11

// Stuff todo: setup the base control class for every ui elements!!!

#include "raylib.h"
#include "string"


// Base UI class 'Control'
class Control {
    protected:
    Vector2 position = Vector2{0, 0};
    bool visible = true;
    bool enabled = true;

    public:
    virtual ~Control() = default;

    virtual void Update() {}
    virtual void Draw() {}

    virtual void setPosition(int xPosition, int yPosition) {position = Vector2{(float)xPosition, (float)yPosition};}
    Vector2 getPosition() const {return position;}

    void setVisible(bool visibility) {visible = visibility;}
    bool isVisible() const {return visible;}
};
// for base classes with size (width and height)
class RectControl : public Control{
    protected:
    Vector2 size = Vector2{0, 0};

    public:
    void setSize(int width, int height) {size = Vector2{(float)width, (float)height};}
    Vector2 getSize() const {return size;}
    
    // MAYBE: add a function to return the Rectnagle
};

// Used as a Background for Ui elements
class Panel : public RectControl{
    public:
    //Vector2 position = Vector2{0, 0};
    //Vector2 size = Vector2{0, 0};
    Color color = WHITE;
    Vector2 padding = Vector2{0, 0};

    void Draw() {
        // TODO: Add rounded corners
        DrawRectangle(position.x, position.y, size.x + padding.x, size.y + padding.y, color);
    }
};

// Display Text inside the window
class Label : public Control{
    private:
    std::string text = "";
    int fontSize = 16;
    // Vector2 position;
    Color color = BLACK;
    Panel background;

    public:
    bool bgActive = true; // Whether to show BG or not

    explicit Label(const std::string newText, int xPosition, int yPosition, int newFontSize)
    : text(newText), fontSize(newFontSize){
        position = Vector2{(float)xPosition, (float)yPosition};
        rePositionBg();
    }

    void setText(std::string newText) {
        text = newText;
        rePositionBg();
    }
    void setFontSize(int newFontSize) {
        fontSize = newFontSize;
        rePositionBg();
    }
    void setPosition(int xPosition, int yPosition) override {
        Control::setPosition(xPosition, yPosition);
        //position = newPosition;
        //background.position = position;
        rePositionBg();
    }
    void setBgPadding(int widthPad, int heightPad) {
        background.padding = Vector2{(float)widthPad, (float)heightPad};
        rePositionBg();
    }
    void setTextColor(Color newColor) {
        color = newColor;
    }
    void setBgColor(Color newColor) {
        background.color = newColor;
    }

    void Draw() {
        if (!visible) {return;}
        if (bgActive) {
            background.Draw();
            DrawText(text.c_str(), position.x + background.padding.x/2, position.y + background.padding.y/2, fontSize, color);
        }
        else {
            DrawText(text.c_str(), position.x, position.y, fontSize, color);
        }
    }

    private:
    // used to set the size and position of BG correctly 
    void rePositionBg() {
        background.setPosition(position.x, position.y);
        //background.position = position;
        background.setSize((float)MeasureText(text.c_str(), fontSize), (float)fontSize);
        //background.size = {
        //    (float)MeasureText(text.c_str(), fontSize),
        //    (float)fontSize
        //};
    }
};

// A button to press
class Button : public RectControl{
    private:
    Label label;
    Panel background;

    public:
    bool pressed = false;
    bool hovered = false;

    Button(std::string btnText, int xPosition, int yPosition)
    : label(btnText, xPosition, yPosition, 16)
    {
        setPosition(xPosition, yPosition);
    }

    void Draw() {
        background.Draw();
        label.Draw();
    }
};

// The Mainstuff
int main(void) {
    int windowWidth = 960;
    int windowHeight = 540;
    
    InitWindow(windowWidth, windowHeight, "My GUI Library");
    SetTargetFPS(60);

    // Testin stuff
    // The Label
    Label testLbl = Label("This is a Test Label Object", 10, 10, 16);
    testLbl.setTextColor(BLUE);
    testLbl.setBgColor(BLACK);
    testLbl.setText("dkjcskjgdck");
    testLbl.setFontSize(20);
    testLbl.setPosition(100, 30);
    testLbl.setBgPadding(30, 40);

    // The Button
    Button testBtn = Button("This is a test Button", 20, 200);

    while(!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        const char* text = "My GUI Library";
        int textSize = 20;
        int textWidth = MeasureText(text, textSize);
        DrawText(text, (windowWidth / 2 - textWidth / 2), windowHeight/2, textSize, GRAY);

        testLbl.Draw();
        testBtn.Draw();
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
