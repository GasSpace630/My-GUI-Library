#include "raylib.h"
#include "iostream"
#include "memory"
#include "vector"
#include "string"
#include <algorithm>
#include <endian.h>
#include <memory>
#include <pthread.h>
#include <string>
#include <vector>


// For borders , margins and stuff
struct Edges {
    float left, right, top, bottom;

    static Edges All(float v) {
        return Edges{v,v,v,v};
    }

    static Edges Symmatric(float horizontal, float vertical) {
        return Edges{horizontal, horizontal, vertical, vertical};
    }
};

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

    // Layout stuff
    Edges padding;
    Edges border;
    Edges margin;

    public:
    void setSize(int width, int height) {
        size = Vector2{(float)width, (float)height};
    }

    Vector2 getSize() const {return size;}

    // padding
    void setPadding(const Edges& paddingEdges) {padding = paddingEdges;}
    void setPadding(float all) {padding = Edges::All(all);}
    void setPadding(float horizontal, float vertical) {padding = Edges::Symmatric(horizontal, vertical);}
    Edges getPadding() {return padding;}

    //border
    void setBorder(const Edges& borderEdges) {border = borderEdges;}
    void setBorder(float all) {border = Edges::All(all);}
    void setBorder(float horizontal, float vertical) {border = Edges::Symmatric(horizontal, vertical);}
    Edges getBorder() {return border;}

    //margin
    void setMargin(const Edges& marginEdges) {margin = marginEdges;}
    void setMargin(float all) {margin = Edges::All(all);}
    Edges getMargin() {return margin;}

    // The actual rect
    Rectangle getOuterRect() {
        Vector2 wp = getWorldPosition();
        return {wp.x, wp.y, size.x, size.y};
    }

    // for layout stuff
    Rectangle getLayoutRect() {
        Rectangle r = getOuterRect();
        return {
            r.x - margin.left,
            r.y - margin.top,
            r.width + margin.left + margin.right,
            r.height + margin.top + margin.bottom
        };
    }

    Rectangle getContentRect() {
        Rectangle r = getOuterRect();
        float contentW = r.width  - (padding.left + padding.right) - (border.left + border.right);
        float contentH = r.height - (padding.top  + padding.bottom) - (border.top  + border.bottom);
        // Avoid negative values
        contentW = std::max(0.0f, contentW);
        contentH = std::max(0.0f, contentH);

        return {
            r.x + padding.left + border.left,
            r.y + padding.top + border.top,
            contentW,
            contentH
        };
    }
};

// Display Text inside the window
class Label : public Control{
    private:
    std::string text = "";
    Font font = GetFontDefault();
    float spacing = 1.0f;
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

    Vector2 getTextBounds() const {
        return MeasureTextEx(font, text.c_str(), fontSize, spacing);
    }


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

// To get them label text related functions
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
    private:
    Color color = WHITE;
    Color borderColor = WHITE;

    public:

    void setColor(Color newColor) {
        color = newColor;
    }

    Color getColor() {return color;}

    void setBorderColor(Color newColor) {
        borderColor = newColor;
    }

    Color getBorderColor() {return borderColor;}

    void Draw() override {
    Rectangle outer = getOuterRect();
    Rectangle content = getContentRect();

    // background ONLY
    DrawRectangleRec(content, color);

    // border inside outer rect
    Edges b = getBorder();

    if (b.left > 0)
        DrawRectangle(outer.x, outer.y, b.left, outer.height, borderColor);
    if (b.right > 0)
        DrawRectangle(outer.x + outer.width - b.right, outer.y, b.right, outer.height, borderColor);
    if (b.top > 0)
        DrawRectangle(outer.x, outer.y, outer.width, b.top, borderColor);
    if (b.bottom > 0)
        DrawRectangle(outer.x, outer.y + outer.height - b.bottom, outer.width, b.bottom, borderColor);

    Control::Draw();
}

};

// A button to press
// FIXME: test clips through the button, maybe problem in drawing label test
class Button : public RectControl, public TextElement {
private:
    // Default colors
    Color normalColor  = GRAY;
    Color hoveredColor = LIGHTGRAY;
    Color pressedColor = DARKGRAY;

    // States
    bool hovered = false;
    bool pressed = false;

public:
    Button(const std::string& text) {

        setPadding(Edges::All(6));

        auto lbl = std::make_unique<Label>(text, 16);
        label = lbl.get();
        addChild(std::move(lbl));

        reCalcLayout();
    }

   void reCalcLayout() {
        Vector2 text = label->getTextBounds();

        // Avoid negative or very small size
        float minW = 60;
        float minH = 30;

        float w = std::max(minW, text.x + padding.left + padding.right);
        float h = std::max(minH, text.y + padding.top  + padding.bottom);

        setSize(w, h);

        Rectangle content = getContentRect();
        
        // getting the Center position
        float x = content.x + (content.width  - text.x) / 2.0f;
        float y = content.y + (content.height - text.y) / 2.0f;

        label->setPosition(x, y);
    }


    void Update() override {
        Vector2 mouse = GetMousePosition();
        Rectangle r = getOuterRect();

        hovered = CheckCollisionPointRec(mouse, r);

        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            pressed = true;
        }
        if (pressed && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            pressed = false;
        }

        Control::Update();
    }

    void Draw() override {

        Color bg =
            pressed ? pressedColor :
            hovered ? hoveredColor :normalColor;

        Rectangle r = getOuterRect();
        DrawRectangleRec(r, bg);

        Control::Draw(); // draw label
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
    testPanel -> setSize(400, 200);
    testPanel -> setBorderColor(BLUE);
    testPanel -> setBorder(6, 6);
    testPanel -> setColor(RAYWHITE);

    // the Label
    auto testLbl = std::make_unique<Label>("Hello", 16);
    testLbl -> setText("sjdhcsjhcjshvdj");
    testLbl -> setPosition(10, 10);
    testPanel -> addChild(std::move(testLbl));
    testLbl = nullptr;

    // The Button
    auto testBtn = std::make_unique<Button>("The test Button");
    testBtn -> setPosition(40, 40);
    testBtn -> setTextColor(ORANGE);
    testPanel -> addChild(std::move(testBtn));
    testBtn = nullptr;

    // ========================================================
    // Title Label
    auto titleLbl = std::make_unique<Label>("My GUI Library!", 20);
    titleLbl -> setPosition(testPanel->getSize().x/2.0 - (titleLbl->getTextSize()/2.0), testPanel->getSize().y/2.0 - (titleLbl->getFontSize()));
    titleLbl -> setTextColor(BLUE);
    testPanel -> addChild(std::move(titleLbl));
    titleLbl = nullptr;

    while(!WindowShouldClose()) {

        testPanel -> Update();

        BeginDrawing();
        
        ClearBackground(BLACK);

        testPanel -> Draw();
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
