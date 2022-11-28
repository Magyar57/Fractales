#include <SFML/Graphics.hpp>

#include "FractaleWindows.hpp"

#include <iostream>

int main() {
	std::cout << "FractalesUI launched" << std::endl;
	
    // Fenêtre principale
    PlanComplexeWindow plan_window(sf::VideoMode(720, 720));

    while (plan_window.isOpen()) {
        plan_window.update();
        plan_window.drawWindow();
        plan_window.displayWindow();
    }

	return 0;
}