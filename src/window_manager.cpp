#include "../app/window_manager.h"

void WindowManager::Start() {
    sf::RenderWindow win(sf::VideoMode(win_size.x, win_size.y), "TuringMachineSimulator", sf::Style::Default);

    win.setActive(false);
    bool close = false;
    auto displayer = std::async([&] () {
        win.setVerticalSyncEnabled(true);
        while (true) {
            if (close) {
                win.close();
                return;
            }
            win.clear(surface);
            win.draw(tab_);
            win.display();
        }
    });

    sf::Event event;
    while (win.isOpen() && win.waitEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed: {
                close = true;
                displayer.wait();
                break;
            }
            case sf::Event::Resized: {
                win_size = {(float) event.size.width, (float) event.size.height};
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                win.setView(sf::View(visibleArea));
                break;
            }
            default: {
                tab_.ProcessEvent(event);
                break;
            }
        }
    }
}
