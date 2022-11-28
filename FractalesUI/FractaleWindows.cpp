#include "FractaleWindows.hpp"

#include "CUDAShader.cuh"

#include <iostream>

// Implémentations de FractaleWindow
#pragma region FractaleWindow

FractaleWindow::FractaleWindow(sf::VideoMode v, const sf::String& titre_fenetre) : RenderWindow(v, titre_fenetre) {
    setVerticalSyncEnabled(true);
    this->nbPixel = v.width * v.height;
}

#pragma endregion

// Implémentations de SettingsWindow
#pragma region SettingsWindow

SettingsWindow::SettingsWindow(sf::VideoMode v = sf::VideoMode(SettingsWindow::default_width, SettingsWindow::default_height)) : FractaleWindow(v, "Paramètres de la fractale") {
    setVisible(false);
    this->is_visible = false;

    // UI des paramètres
    this->cercle = sf::CircleShape(100.f);
}

bool SettingsWindow::isVisible() {
    return this->is_visible;
}

void SettingsWindow::changeVisibility() {
    is_visible = !is_visible;
    setVisible(is_visible);
}

void SettingsWindow::update() {
    handleEvents();
}

void SettingsWindow::handleEvents() {
    sf::Event event;

    while (pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            changeVisibility();
    }
}

void SettingsWindow::drawWindow() {
    clear();
    draw(this->cercle);
}

void SettingsWindow::displayWindow() {
    display();
}

#pragma endregion

// Implémentations de PlanComplexeWindow
#pragma region PlanComplexeWindow

// Constructeurs

PlanComplexeWindow::PlanComplexeWindow(sf::VideoMode v) : FractaleWindow(v, "Projet Fractale") {
    this->width = v.width;
    this->height = v.height;
    
    this->scope.setToDefault();
    this->scope_updated = true;

    this->current_fractale_results = allocate_tab(width, height); // dll function
    
    fractale_image.create(v.width, v.height, sf::Color::Red);
    fractale_holder.setSize(sf::Vector2f((float)width, (float)height));
}

PlanComplexeWindow::~PlanComplexeWindow() {
    free_tab(this->current_fractale_results, width, height);
}

// Updates

void PlanComplexeWindow::update() {
    handleEvents();

    if (scope_updated) {
        new_calculate_Julia(0.285, 0.013, current_fractale_results, width, height, scope.xMin, scope.yMin, scope.xMax, scope.yMax); // Dll function (parallelized)
        sf::Color cur_color;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                cur_color = (current_fractale_results[i][j] == 1) ? sf::Color::Black : sf::Color::White;
                if (current_fractale_results[i][j] == -1) cur_color = sf::Color::Red;
                fractale_image.setPixel(i, j, cur_color); // A PARALLELISER OSKOUR CA RAME
            }
        }
        fractale_texture.loadFromImage(fractale_image);
        fractale_holder.setTexture(&fractale_texture);
        scope_updated = false; // we just ajusted the image to the new scope
    }

    storeLastMousePosition();

    settings_window.update();
}

void PlanComplexeWindow::handleEvents() {
    using sf::Event;
    using sf::Mouse, sf::Keyboard;
    using std::cout, std::endl;

    Event event;

    while (pollEvent(event)) {

        // Close window (and subwindows)
        if (event.type == Event::Closed) {
            settings_window.close();
            close();
        }
   
        if (hasFocus() && Mouse::isButtonPressed(Mouse::Left)) moveScopeFromMouse();                                // Move around using click
        if (event.type == Event::MouseWheelScrolled) zoomScope((int)event.mouseWheelScroll.delta);                  // Zoom or unzoom

        // Keyboard actions
        if (event.type == Event::KeyPressed) {
            
            // Move around using keys
            if (event.key.code == Keyboard::Z || event.key.code == Keyboard::Up) moveScope(direction::Up);          // Go up
            if (event.key.code == Keyboard::Q || event.key.code == Keyboard::Left) moveScope(direction::Left);      // Go left
            if (event.key.code == Keyboard::S || event.key.code == Keyboard::Down) moveScope(direction::Down);      // Go down
            if (event.key.code == Keyboard::D || event.key.code == Keyboard::Right) moveScope(direction::Right);    // Go right
            if (event.key.code == Keyboard::E || event.key.code == Keyboard::Add) zoomScope(1);                     // Flat zoom in (using numpad)
            if (event.key.code == Keyboard::A || event.key.code == Keyboard::Subtract) zoomScope(-1);               // Flat zoom out (using numpad)

            // Reset zoom level and offset (center to (0,0) )
            if (event.key.code == Keyboard::R || event.key.code == Keyboard::Numpad0) {
                scope.setToDefault();
                scope_updated = true;
            }

            // Display (or hide) settings window
            if (event.key.code == Keyboard::C) {
                settings_window.changeVisibility();                 // Display (or hide) settings window
                if (settings_window.isVisible()) requestFocus();    // Stay focused on main window
            }

        }

    }
}

/* Called after all (current frame) updates, to store previous mouse position for the next frame */
void PlanComplexeWindow::storeLastMousePosition() {
     sf::Vector2i mouse_pos_temp = sf::Mouse::getPosition(*this);
     previous_mouse_position.x = mouse_pos_temp.x;
     previous_mouse_position.y = mouse_pos_temp.y;
}

void PlanComplexeWindow::moveScope(direction d) {
    
    double pas_x = -15.0 * ((scope.xMax - scope.xMin) / width); // 10%
    double pas_y = -15.0 * ((scope.yMax - scope.yMin) / height); // 10%

    double shift_x = 0.0;
    double shift_y = 0.0;

    switch (d) {
        case 0: // up
            shift_y = pas_y;
            break;
        case 1: // down
            shift_y = -1.0 * pas_y;
            break;
        case 2: // left
            shift_x = pas_x;
            break;
        case 3: // right
            shift_x = -1.0 * pas_x;
            break;
        default:
            break;
    }
    
    scope.xMin += shift_x;
    scope.yMin += shift_y;
    scope.xMax += shift_x;
    scope.yMax += shift_y;
    scope_updated = true;
}

void PlanComplexeWindow::moveScopeFromMouse() {
    sf::Vector2i new_mouse_pos = sf::Mouse::getPosition(*this);

    double shift_x = new_mouse_pos.x - previous_mouse_position.x;
    double shift_y = new_mouse_pos.y - previous_mouse_position.y;
    shift_x *= -1; // il y a une erreur de signe quelque part dans le code; on la corrige ici comme ça
    shift_y *= -1;

    std::cout << "Shifting using mouse, shift:" << shift_x << "," << shift_y << std::endl;

    double pas_x = shift_x * ((scope.xMax - scope.xMin) / width);
    double pas_y = shift_y * ((scope.yMax - scope.yMin) / height);

    scope.xMin += pas_x;
    scope.yMin += pas_y;
    scope.xMax += pas_x;
    scope.yMax += pas_y;
    scope_updated = true;
}

void PlanComplexeWindow::zoomScope(int times) {
    if (times == 0) return;

    double pas_x = 15.0 * ((scope.xMax - scope.xMin) / width); // 10%
    double pas_y = 15.0 * ((scope.yMax - scope.yMin) / height); // 10%
    
    if (times < 0) {
        pas_x = -1.0 * pas_x;
        pas_y = -1.0 * pas_y;
        times = -1 * times;
    }

    for (int i = 0; i < times; i++) {
        scope.xMin += pas_x;
        scope.yMin += pas_y;
        scope.xMax -= pas_x;
        scope.yMax -= pas_y;
    }
    
    scope_updated = true;
}

// Drawings

void PlanComplexeWindow::drawWindow() {
    clear();
    draw(this->fractale_holder);
    settings_window.drawWindow();
}

void PlanComplexeWindow::displayWindow() {
    display();
    settings_window.displayWindow();
}

#pragma endregion