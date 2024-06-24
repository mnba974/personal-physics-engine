
#include <SFML/Graphics.hpp>
#include <iostream>
#include "utilities.h"
#include <windows.h>
#include <vector>


sf::Color pickcolor(int i);

int main()
{

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(1300, 800), "Hello World", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    std::vector<object*> objs;
    std::vector<object*> consts;



    object obj1(vec2(650, 200));
    object obj2(vec2(550, 200));
    object obj3(vec2(450, 200));
    object obj4(vec2(450, 300));
    linker link(&obj1, &obj2, 100);
    linker link2(&obj2, &obj3, 100);
    linker link3(&obj3, &obj4, 100);
    obj1.anchor = true;
    obj4.anchor = true;
    objs.emplace_back(&obj1);
    objs.emplace_back(&obj2);
    objs.emplace_back(&obj3);
    objs.emplace_back(&obj4);
    objs.emplace_back(&link);
    objs.emplace_back(&link2);
    objs.emplace_back(&link3);

    rectContainer con;
    con = rectContainer();
    consts.emplace_back(&con);
    renderer rend(objs, consts, &window);

    bool running = false;
    bool spawning = false;

    interaction inter;

    sf::Clock clck;

    int i = 0;
    int j = 0;
    int color = 0;
    int coutint = 0;


    std::vector<char> selection = { 'b','s','l' };
    int selectorint = 0;
    int selectorlenght = selection.size();
    int frequency = 60;  //per second
    frequency = 60 / frequency;
    object* spawner;


    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) running = !running;
                if (event.key.code == sf::Keyboard::S) spawning = !spawning;
                if (event.key.code == sf::Keyboard::R) rend.objects.clear();
                if (event.key.code == sf::Keyboard::N) {
                    selectorint = (selectorint + 1) % selectorlenght;
                    inter.linkerobj = NULL;
                    inter.linkerprogression = 0;
                    std::cout << selection[selectorint];
                }
            }
        }
        if (spawning && running) {
            if (i % frequency == 0) {
                for (int var = 0; var < 5;var++) {
                    spawner = new object(vec2(150, 100 + 15 * var));
                    spawner->prev_pos = vec2(146, 99 + 15 * var);
                    spawner->color = pickcolor(color);
                    color = (color + 1) % (6 * 255);
                    rend.objects.emplace_back(spawner);
                }
                i = 0;


            }
            i += 1;
        }



        window.clear(sf::Color(169, 166, 174, 0.8));

        if (running) rend.update(1.0 / 60, 8);
        rend.render();
        inter.mainloop(rend, selection[selectorint]);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
            std::cout << 1 / clck.getElapsedTime().asSeconds() << ' ' << rend.objects.size() << "\n";
        }

        clck.restart();
        window.display();


    }
    return 0;
}


sf::Color pickcolor(int i) {
    int r, g, b;
    r = 0;
    g = 0;
    b = 0;
    int s = i / 255;
    switch (s) {
    case 0:
        r = 255;
        g = i;
        break;
    case 1:
        r = 255 - (i % 255);
        g = 255;
        break;
    case 2:
        b = i % 255;
        g = 255;
        break;
    case 3:
        b = 255;
        g = 255 - (i % 255);
        break;
    case 4:
        b = 255;
        r = (i % 255);
        break;
    case 5:
        r = 255;
        b = 255 - (i % 255);
        break;
    }

    return sf::Color(r, g, b);
}
