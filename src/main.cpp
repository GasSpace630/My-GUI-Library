#include "raylib.h"
#include "memory"
#include "vector"
#include "string"
#include <algorithm>
#include <memory>
#include <vector>

// Base UI class 'Control'
class Control {
    protected:
    Vector2 position = Vector2{0, 0};
    bool visible = true;
    bool enabled = true;

    Control* parent = nullptr; // parent
    std::vector<std::unique_ptr<Control>> children; // children

    public:
    virtual ~Control() = default;

    // Add a control object as a child to this object
    void addChild(std::unique_ptr<Control> child) {
        if (!child) return;
        child -> parent = this;
        children.push_back(std::move(child));
    }
    // Rmove a child
    // the 'i' may get out of range if not carefull
    void removeChild(Control* child) {
        for (auto i = children.begin(); i != children.end(); ++i) {
            if (i->get() == child) {
                (*i)->parent = nullptr;
                children.erase(i); // DESTROYS child
                return;
            }
        }
    }

    Control* getParent() const {return parent;}

    virtual void setPosition(int x, int y) {
        position = Vector2{(float)x, (float)y};
    }
    Vector2 getPosition() const {return position;}

    void setVisible(bool visibility) {visible = visibility;}
    bool isVisible() const {return visible;}

    virtual void Update() {
        for (auto& child : children)
            child -> Update();
    }
    virtual void Draw() {
        if (!visible) {return;}
        for (auto& child : children)
            child -> Draw();
    }
};
// for base classes with size (width and height)
class RectControl : public Control{
    protected:
    Vector2 size = Vector2{0, 0};

    public:
    void setSize(int width, int height) {
        size = Vector2{(float)width, (float)height};
    }
    // Get the size Vector2
    Vector2 getSize() const {return size;}
    // Get the bounding rectangle object
    Rectangle getRect() const {return {position.x, position.y, size.x, size.y};}
};

// Used as a Background to hold Ui elements
class Panel : public RectControl{
    public:
    Color color = WHITE;
    Vector2 padding = Vector2{0, 0};

    void setpadding(int x, int y) {
        padding = Vector2{(float)x, (float)y};
    }

    void Draw() override{
        // TODO: Add rounded corners
        DrawRectangle(position.x, position.y, size.x + padding.x, size.y + padding.y, color);
        Control::Draw();
    }
};

// Display Text inside the window
class Label : public Control{
    private:
    std::string text = "";
    int fontSize = 16;
    int textSize = 0;
    Color color = BLACK;

    public:

    explicit Label(const std::string newText, int newFontSize)
    : text(newText), fontSize(newFontSize){
        textSize = MeasureText(text.c_str(), fontSize);
    }

    void setText(std::string newText) {
        text = newText;
        textSize = MeasureText(text.c_str(), fontSize);
    }
    std::string getText() const {return text;}

    void setFontSize(int newFontSize) {
        fontSize = newFontSize;
    }
    int getFontSize() {return fontSize;}

    int getTextSize() {return textSize;}

    void setPosition(int x, int y) override {
        Control::setPosition(x, y);
    }
    void setTextColor(Color newColor) {
        color = newColor;
    }

    void Draw() override{
        if (!visible) {return;}
        DrawText(text.c_str(), position.x, position.y, fontSize, color);
    }
};

// A button to press
// TODO: Refactor for parent-child system.
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

    Button(std::string btnText, int x, int y, int xPadding, int yPadding)
    : label(btnText, 16)
    {
        setPosition(x, y);
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
        label.setPosition(centerX - (textWidth/2), centerY - (textHeight/2));
    }

    // Chnages the position of the button (panel and text)
    void setPosition(int x, int y) override{
        Control::setPosition(x, y);
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
    // the Panel
    auto testPanel = std::make_unique<Panel>();
    testPanel -> setPosition(10, 10);
    testPanel -> setSize(200, 80);
    // the Label
    auto testLbl = std::make_unique<Label>("Hello", 16);
    testLbl -> setPosition(10, 10);
    testPanel -> addChild(std::move(testLbl));
    testLbl = nullptr;

    // The Button
    Button testBtn = Button("The test Button", 60, 200, 10, 10);
    testBtn.setPosition(300, 10);

    auto titleLbl = std::make_unique<Label>("My GUI Library!", 20);
    titleLbl -> setPosition(GetScreenWidth()/2.0f - titleLbl -> getTextSize()/2.0f, GetScreenHeight()/2.0f - titleLbl -> getFontSize()/2.0f);
    titleLbl -> setTextColor(GRAY);

    while(!WindowShouldClose()) {

        testBtn.Update();

        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        titleLbl -> Draw();
        testBtn.Draw();

        testPanel -> Draw();
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
