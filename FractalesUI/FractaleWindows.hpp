#pragma once

#include <SFML/Graphics.hpp>

#define DEFAULT_SCALE 2.0 // [(-2,-2) ; (2,2)]

typedef struct z_window_scope {
    double xMin;
    double yMin;
    double xMax;
    double yMax;

    void setToDefault() {
        xMin = -1 * DEFAULT_SCALE; // From (xMin, yMin)
        yMin = -1 * DEFAULT_SCALE;
        xMax = DEFAULT_SCALE; // To (xMax, yMax)
        yMax = DEFAULT_SCALE;
    }

} windowScope;

static const enum direction {
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3
} directions ;

class FractaleWindow : public sf::RenderWindow {
    protected: int nbPixel;

    public: FractaleWindow(sf::VideoMode v, const sf::String& titre_fenetre);

    public: virtual void update() = 0;
    private: virtual void handleEvents() = 0; // called by update()

    public: virtual void drawWindow() = 0;
    public: virtual void displayWindow() = 0;
    
};

class SettingsWindow : public FractaleWindow {
    private: bool is_visible;
    private: sf::CircleShape cercle;

    public: static const int default_width = 150;
    public: static const int default_height = 300;

    public: SettingsWindow(sf::VideoMode v);

    public: void update();
    private: void handleEvents();
    public: bool isVisible();
    public: void changeVisibility();

    public: void drawWindow();
    public: void displayWindow();

};

class PlanComplexeWindow : public FractaleWindow {
    private: SettingsWindow settings_window;
    private: int width;
    private: int height;
    
    private: sf::Vector2i previous_mouse_position;

    private: windowScope scope;
    private: bool scope_updated = true;

    private: int** current_fractale_results; // if (fractale_scope_updated == true), current_fractale_results has to be updated

    private: sf::Image fractale_image; // edit pixel by pixel
    private: sf::Texture fractale_texture; // load fractale_image to be drawn in...
    private: sf::RectangleShape fractale_holder; // ... the rectangle shape to draw the final generated image

    public: PlanComplexeWindow(sf::VideoMode v);
    public: ~PlanComplexeWindow();

    public: void update();
    private: void handleEvents();
    private: void storeLastMousePosition();
    private: void moveScope(direction d);
    private: void moveScopeFromMouse();
    private: void zoomScope(int times = 1);

    public: void drawWindow();
    public: void displayWindow();

};