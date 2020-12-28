//carduino.ino
#include "Carduino+Main.h"

Carduino_Main *main;

void setup() {
    main = new Carduino_Main();
}

void loop() {
    delay(50);
    main->runLoop();
}
