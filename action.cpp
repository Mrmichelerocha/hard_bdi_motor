#include "agent.h"

Action::Action(int pinM1, int pinM2) {
    this->pinM1 = pinM1;
    this->pinM2 = pinM2;
    pinMode(pinM1, OUTPUT);
    pinMode(pinM2, OUTPUT);
    desligarM1();
    desligarM2();   
}

void Action::Left() {
    analogWrite(pinM1, 300);   
    delay(520);
    analogWrite(pinM1, 0);   
    setLeftStatus("OK");
    delay(200);
}

void Action::Right() {
    analogWrite(pinM2, 300);   
    delay(650);
    analogWrite(pinM2, 0);
    setRightStatus("OK");
    delay(200);
}

void Action::Front() {
    analogWrite(pinM1, 250); // Define a potência do motor M1
    analogWrite(pinM2, 300); // Define a potência do motor M2
    delay(500); // Mantém a potência por 1 segundo
    analogWrite(pinM1, 0); // Desliga o motor M1
    analogWrite(pinM2, 0); // Desliga o motor M2
    setFrontStatus("OK");
    delay(100);
}


void Action::desligarM1() {
    analogWrite(pinM1, 0);
}

void Action::desligarM2() {
    analogWrite(pinM2, 0);
}

void Action::setFrontStatus(const char* status) {
    beliefsJsonDoc["Front"] = status;
    // Você pode chamar handleGetBeliefs() aqui se necessário
}

void Action::setRightStatus(const char* status) {
    beliefsJsonDoc["Right"] = status;
    // Você pode chamar handleGetBeliefs() aqui se necessário
}

void Action::setLeftStatus(const char* status) {
    beliefsJsonDoc["Left"] = status;
    // Você pode chamar handleGetBeliefs() aqui se necessário
}