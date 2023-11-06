#ifndef AGENT_H
#define AGENT_H
#include <Arduino.h>
#include <ArduinoJson.h>

extern DynamicJsonDocument beliefsJsonDoc;

class Action {
  private:
    int pinM1;
    int pinM2;

  public:
    Action(int pinM1, int pinM2);

    void Right();
    void Left();
    void Front();
    void desligarM1();
    void desligarM2();

    void setLeftStatus(const char* status);
    void setFrontStatus(const char* status);
    void setRightStatus(const char* status);
};

#endif
