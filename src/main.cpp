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
    
    Rectangle getRect() const {return {position.x, position.y, size.x, size.y};}
};

// Used as a Background for Ui elements
class Panel : public RectControl{
    public:
    Color color = WHITE;
    Vector2 padding = Vector2{0, 0};

    void setpadding(int xPadding, int yPadding) {
        padding = Vector2{(float)xPadding, (float)yPadding};
    }

    void Draw() override{
        // TODO: Add rounded corners
        DrawRectangle(position.x, position.y, size.x + padding.x, size.y + padding.y, color);
    }
};

// Display Text inside the window
class Label : public Control{
    private:
    std::string text = "";
    int fontSize = 16;
    Color color = BLACK;
    Panel background;

    public:
    bool bgVisible = true; // Whether to show BG or not

    explicit Label(const std::string newText, int xPosition, int yPosition, int newFontSize)
    : text(newText), fontSize(newFontSize){
        position = Vector2{(float)xPosition, (float)yPosition};
        rePositionBg();
    }

    void setText(std::string newText) {
        text = newText;
        rePositionBg();
    }
    std::string getText() const {return text;}
    void setFontSize(int newFontSize) {
        fontSize = newFontSize;
        rePositionBg();
    }
    void setPosition(int xPosition, int yPosition) override {
        Control::setPosition(xPosition, yPosition);
        rePositionBg();
    }
    void setBgPadding(int xPadding, int yPadding) {
        background.setpadding(xPadding, yPadding);
        rePositionBg();
    }
    void setTextColor(Color newColor) {
        color = newColor;
    }
    void setBgColor(Color newColor) {
        background.color = newColor;
    }

    void Draw() override{
        if (!visible) {return;}
        if (bgVisible) {
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
        background.setSize((float)MeasureText(text.c_str(), fontSize), (float)fontSize);
    }
};

// A button to press
class Button : public RectControl{
    private:
    Label label;
    Panel background;
    Color normalColor = GRAY;
    Color hoveredColor = LIGHTGRAY;
    Color pressedColor = DARKGRAY;

    public:
    bool pressed = false;
    bool hovered = false;

    Button(std::string btnText, int xPosition, int yPosition, int xPadding, int yPadding)
    : label(btnText, xPosition, yPosition, 16)
    {
        // to center text
        float textWidth = MeasureText(btnText.c_str(), 16) + xPadding; 
        float textHeight = 16 + yPadding;
        float centerX = xPosition + (textWidth / 2.0f) + (xPadding/2.0f);
        float centerY = yPosition + (textHeight / 2.0f) + (yPadding/2.0f);
        // ---
        
        background.setPosition(xPosition, yPosition);
        background.color = GRAY;
        background.setSize(textWidth, textHeight);
        label.bgVisible = false;
        label.setPosition(centerX - (textWidth/2), centerY - (textHeight/2));
    }

    // void setTextPosition(int xPosition, int yPosition) {
    //     float textWidth = MeasureText(label.getText().c_str(), 16) + width;
    //     float textHeight = 16 + height;
    //     float centerX = xPosition + (textWidth / 2.0f) + (width/2.0f);
    //     float centerY = yPosition + (textHeight / 2.0f) + (height/2.0f);
    //     label.setPosition(centerX - (textWidth/2), centerY - (textHeight/2));
    // }

    void Update() override{
        Vector2 mousePosition = GetMousePosition();
        Rectangle bgRect = background.getRect();

        hovered = CheckCollisionPointRec(mousePosition, bgRect);
        if (hovered) {
            background.color = hoveredColor;
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                background.color = pressedColor;
                pressed = true;
            }
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                background.color = hoveredColor;
                pressed = false;
            }
        }
        else {
            background.color = normalColor;
            hovered = false;
            pressed = false;
        }
    }

    void Draw() override{
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
    Button testBtn = Button("The test Button", 60, 200, 10, 10);

    while(!WindowShouldClose()) {

        testBtn.Update();

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
