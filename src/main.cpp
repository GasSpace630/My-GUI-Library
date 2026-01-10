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

// Used as a Background to hold Ui elements
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
    int textSize = 0;
    Color color = BLACK;
    Panel background;

    public:
    bool bgVisible = true; // Whether to show BG or not

    explicit Label(const std::string newText, int xPosition, int yPosition, int newFontSize)
    : text(newText), fontSize(newFontSize){
        position = Vector2{(float)xPosition, (float)yPosition};
        textSize = MeasureText(text.c_str(), fontSize);
        rePositionBg();
    }

    void setText(std::string newText) {
        text = newText;
        textSize = MeasureText(text.c_str(), fontSize);
        rePositionBg();
    }
    std::string getText() const {return text;}

    void setFontSize(int newFontSize) {
        fontSize = newFontSize;
        rePositionBg();
    }
    int getFontSize() {return fontSize;}
    int getTextSize() {return textSize;}

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
        background.setSize((float)textSize, (float)fontSize);
    }
};

// A button to press
class Button : public RectControl{
    private:
    Label label;
    Panel background;
    // Default button colors
    Color normalColor = GRAY;
    Color hoveredColor = LIGHTGRAY;
    Color pressedColor = DARKGRAY;

    public:
    bool pressed = false;
    bool hovered = false;

    Button(std::string btnText, int xPosition, int yPosition, int xPadding, int yPadding)
    : label(btnText, xPosition, yPosition, 16)
    {
        setPosition(xPosition, yPosition);
        label.setText(btnText);
        // to center text
        float textWidth = label.getTextSize() + xPadding; 
        float textHeight = 16 + yPadding;
        float centerX = position.x + (textWidth / 2.0f) + (xPadding/2.0f);
        float centerY = position.y + (textHeight / 2.0f) + (yPadding/2.0f);
        // --- Default values
        background.setPosition(position.x, position.y);
        background.color = GRAY;
        background.setSize(textWidth, textHeight);
        label.bgVisible = false;
        label.setPosition(centerX - (textWidth/2), centerY - (textHeight/2));
    }

    // Chnages the position of the button (panel and text)
    void setPosition(int xPosition, int yPosition) override{
        Control::setPosition(xPosition, yPosition);
        background.setPosition(position.x, position.y);
        int textXPosition = position.x + (background.getSize().x/2.0f) - MeasureText(label.getText().c_str(), label.getFontSize())/2.0f;
        int textYPosition = position.y + (background.getSize().y/2.0f) - (label.getFontSize()/2.0f);
        label.setPosition(textXPosition, textYPosition);
    }

    // Pressing and hovering
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
    testBtn.setPosition(300, 10);

    Label titleLbl = Label("My GUI Library!", 0, 0, 20);
    titleLbl.setPosition(GetScreenWidth()/2.0f - titleLbl.getTextSize()/2.0f, GetScreenHeight()/2.0f - titleLbl.getFontSize()/2.0f);
    titleLbl.setTextColor(GRAY);

    while(!WindowShouldClose()) {

        testBtn.Update();

        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        titleLbl.Draw();
        testLbl.Draw();
        testBtn.Draw();
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
