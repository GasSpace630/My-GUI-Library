#include "raylib.h"
#include "iostream"
#include "memory"
#include "vector"
#include "string"
#include <algorithm>
#include <memory>
#include <pthread.h>
#include <string>
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

    Vector2 getWorldPosition() {
        if (parent) {
            Vector2 parentPos = parent -> getWorldPosition();
            return Vector2{parentPos.x + position.x, parentPos.y + position.y};
        }
        else {
            return position;
        }
    }

    void setVisibility(bool visibility) {visible = visibility;}
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

    Vector2 getSize() const {return size;}

    Rectangle getRect() {return {getWorldPosition().x, getWorldPosition().y, size.x, size.y};}
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

    void setText(const std::string& newText) {
        text = newText;
        textSize = MeasureText(text.c_str(), fontSize);
    }

    std::string getText() const {return text;}

    void setFontSize(int newFontSize) {
        fontSize = newFontSize;
    }
    int getFontSize() const {return fontSize;}

    int getTextSize() const {return textSize;}

    void setPosition(int x, int y) override {
        Control::setPosition(x, y);
    }

    void setTextColor(Color newColor) {
        color = newColor;
    }

    Color getTextColor() {return color;}

    void Draw() override{
        if (!visible) {return;}

        DrawText(text.c_str(), getWorldPosition().x, getWorldPosition().y, fontSize, color);
        Control::Draw();
    }
};

// To get them label functions
class TextElement {
protected:
    Label* label = nullptr;

public:
    void setText(const std::string& newText) {
        if (label) {label -> setText(newText);}
    }

    std::string getText() {
        return label ? label -> getText() : "";
    }

    void setFontSize(int newFontSize) {
        if (label) {label -> setFontSize(newFontSize);}
    }

    int getFontSize() {
        return label ? label -> getFontSize() : 0;
    }

    int getTextSize() {
        return label ? label -> getTextSize() : 0;
    }

    void setTextColor(Color newColor) {
        if (label) {label -> setTextColor(newColor);}
    }

    Color getTextColor() {
        return label ? label -> getTextColor() : BLACK;// return black if no color is set
    }
};


// Used as a Background to hold Ui elements
class Panel : public RectControl{
    public:
    Color color = WHITE;
    Vector2 padding = Vector2{0, 0};

    void setColor(Color newColor) {
        color = newColor;
    }

    void setPadding(int xPadding, int yPadding) {
        padding = Vector2{(float)xPadding, (float)yPadding};
    }
    Vector2 getPadding() {return padding;}

    Rectangle getRect() {
        float x = getWorldPosition().x;
        float y = getWorldPosition().y;
        float w = getSize().x + padding.x;
        float h = getSize().y + padding.y;
        return Rectangle{x, y, w, h};
    }

    void Draw() override{
        // TODO: Add rounded corners
        DrawRectangle(getWorldPosition().x, getWorldPosition().y, size.x + padding.x, size.y + padding.y, color);
        Control::Draw();
    }
};

// A button to press
// TODO: Refactor for parent-child system.
class Button : public RectControl, public TextElement{
    private:
    Label* label;            // Text
    Panel* panel;              // Background
    // Default button colors
    Color normalColor = GRAY;
    Color hoveredColor = LIGHTGRAY;
    Color pressedColor = DARKGRAY;

    public:
    bool pressed = false;
    bool hovered = false;

    Button(std::string text){
        auto pnl = std::make_unique<Panel>();
        panel = pnl.get();
        addChild(std::move(pnl));

        auto lbl = std::make_unique<Label>(text, 16);
        label = lbl.get();
        addChild(std::move(lbl));

        panel -> setPadding(4, 4);
        panel -> setColor(normalColor);

        reCalcLayout();
    }

    // Chnages the position of the button (panel and text)
    void setPosition(int x, int y) override{
        Control::setPosition(x, y);
    }

    void setPadding(int x, int y) {
        panel -> setPadding(x, y);
        reCalcLayout();
    }

    Vector2 getPadding() {return panel -> padding;}

    void reCalcLayout() {
        float textWidth = label -> getTextSize();
        float textHeight = label -> getFontSize();

        float width = textWidth + panel -> getPadding().x * 2;
        float height = textHeight + panel -> getPadding().y * 2;

        setSize(width, height);

        panel -> setPosition(0, 0); // Local position
        panel -> setSize(width, height);

        label -> setPosition((width - textWidth) / 2.0f, (height - textHeight) / 2.0f);
    }

    // Pressing and hovering
    void Update() override{
        Vector2 mousePosition = GetMousePosition();
        Rectangle bgRect = panel -> getRect();

        hovered = CheckCollisionPointRec(mousePosition, bgRect);
        if (hovered) {
            panel -> color = hoveredColor;
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                panel -> color = pressedColor;
                pressed = true;
            }
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                panel -> color = hoveredColor;
                pressed = false;
            }
        }
        else {
            panel -> color = normalColor;
            hovered = false;
            pressed = false;
        }
        Control::Update();
    }

    void Draw() override{
        Control::Draw();
    }
};


// The Mainstuff
int main(void) {
    int windowWidth = 960;
    int windowHeight = 540;
    
    InitWindow(windowWidth, windowHeight, "My GUI Library");
    SetTargetFPS(60);

    // ========================================================
    // Testin stuff
    //
    // the Panel
    auto testPanel = std::make_unique<Panel>();
    testPanel -> setPosition(10, 90);
    testPanel -> setSize(400, 340);
    testPanel -> setColor(BLUE);

    // the Label
    auto testLbl = std::make_unique<Label>("Hello", 16);
    testLbl -> setText("sjdhcsjhcjshvdj");
    testLbl -> setPosition(10, 10);
    testPanel -> addChild(std::move(testLbl));
    testLbl = nullptr;

    // The Button
    auto testBtn = std::make_unique<Button>("The test Button");
    testBtn -> setPosition(40, 40);
    testBtn -> setPadding(10, 10);
    testBtn -> setText("std::string &newText");
    testPanel -> addChild(std::move(testBtn));
    testBtn = nullptr;

    // ========================================================

    auto titleLbl = std::make_unique<Label>("My GUI Library!", 20);
    titleLbl -> setPosition(GetScreenWidth()/2.0f - titleLbl -> getTextSize()/2.0f, GetScreenHeight()/2.0f - titleLbl -> getFontSize()/2.0f);
    titleLbl -> setTextColor(GRAY);

    while(!WindowShouldClose()) {

        testPanel -> Update();

        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        titleLbl -> Draw();

        testPanel -> Draw();
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
