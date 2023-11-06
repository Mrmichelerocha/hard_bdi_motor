#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <functional>
#include <map>
#include "agent.h"

typedef std::function<void()> ActionFunction;

std::map<String, ActionFunction> actionMap;

const char* ssid = "LIA";             // LIA  Apê 202 SCP
const char* password = "Lia321A@";  //Lia321A@  COMPIUTERHOUSE 87654321
const int ledPinM1 = 0;
const int ledPinM2 = 2;

Action action(ledPinM1, ledPinM2);

ESP8266WebServer server(80);

DynamicJsonDocument planJsonDoc(2000);
DynamicJsonDocument beliefsJsonDoc(2000);
DynamicJsonDocument desireJsonDoc(2000);
DynamicJsonDocument intentionJsonDoc(2000);

JsonObject planObject = planJsonDoc.to<JsonObject>();
JsonObject beliefsObject = beliefsJsonDoc.to<JsonObject>();
JsonArray desireArray = desireJsonDoc.to<JsonArray>();
JsonArray intentionArray = intentionJsonDoc.to<JsonArray>();

////////////////////////////////////CONEXÃO WIFI/////////////////////////////////////////////////////////////
void setUpWiFi() {
  // Conectar à rede Wi-Fi
  Serial.println();
  Serial.print("Conectando à rede ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado à rede WiFi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

////////////////////////////////////////////HANDLE PLAN////////////////////////////////////////////////////
void handleGetPlan() {
  // Converter o objeto JSON em uma string JSON
  String response;
  serializeJson(planJsonDoc, response);

  // Enviar a resposta para a página
  server.send(200, "application/json", response);
}

void handleSetPlan() {
  // Obter os valores do formulário
  String nameKey = server.arg("nameKey");
  String ctxKey = server.arg("ctxKey");
  String planKey = server.arg("planKey");

  // Atualizar as crenças com os valores do formulário
  JsonObject planName = planObject.createNestedObject(nameKey);
  DynamicJsonDocument contextJson(200);
  deserializeJson(contextJson, ctxKey);
  JsonObject contextObject = planName.createNestedObject("context");
  for (JsonPair keyValue : contextJson.as<JsonObject>()) {
    contextObject[keyValue.key()] = keyValue.value();
  }
  DynamicJsonDocument planJson(200);
  deserializeJson(planJson, planKey);
  JsonArray planArray = planName.createNestedArray("plan");
  for (JsonVariant value : planJson.as<JsonArray>()) {
    planArray.add(value);
  }

  String html = "<html><body><h1>Controle de Relé</h1>";
  html += "<form method='POST' action='/setplan'>";
  html += "<input type='text' name='nameKey' value=''>Name Plan Key<br>";
  html += "<input type='text' name='ctxKey' value=''>Ctx Key<br>";
  html += "<input type='text' name='planKey' value=''>Plan Key<br>";
  html += "<input type='submit' value='Enviar'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

JsonVariant getPlan(String goal) {
  if (planObject.containsKey(goal)) {  // verifica se existe meta nos planos
    Serial.print("existe um plano com a meta");
    Serial.println(goal);

    JsonObject goalPlan = planObject[goal].as<JsonObject>();  // pega a meta no plano
    Serial.print("pegou a meta no plano: ");
    serializeJson(goalPlan, Serial);
    Serial.println();

    JsonObject context = goalPlan["context"].as<JsonObject>();  // pega o contexto
    Serial.print("pega o contexto: ");
    serializeJson(context, Serial);
    Serial.println();


    for (JsonPair kv : context) {                           // percorre todos os elementos do contexto
      if (!beliefsObject.containsKey(kv.key().c_str()) ||   // verifica se não existe o contexto em beliefsObject ou
          beliefsObject[kv.key().c_str()] != kv.value()) {  // se bieliefObject[contexto] é diferente do contexto
        Serial.print("As Condições não foram satisfeitas");
        return JsonObject();  // retorna um objeto Json vazio se as condições não forem satisfeitas
      }
    }

    Serial.print("o plano que vai ser retornado a variavel é: ");

    JsonArray planArray = goalPlan["plan"].as<JsonArray>();  // obtém o array do plano
    serializeJson(planArray, Serial);                        // imprime o array JSON na saída serial
    Serial.println();
    return planArray;  // retorna o plano se todas as condições forem satisfeitas
  }

  return JsonObject();  // retorna um objeto Json vazio se o objetivo não for encontrado
}

////////////////////////////////////////////HANDLE BELIEF////////////////////////////////////////////////////
void handleGetBeliefs() {
  String response;  // converter o objeto JSON em uma string JSON
  serializeJson(beliefsJsonDoc, response);
  server.send(200, "application/json", response);  // enviar a resposta para a página
}

void handleSetBeliefs() {
  String beliefsKey = server.arg("beliefsKey");  // obter os valores do formulário
  String beliefsValue = server.arg("beliefsValue");
  beliefsObject[beliefsKey] = beliefsValue;  // atualizar as crenças com os valores do formulário

  String html = "<html><body><h1>Controle de Relé</h1>";
  html += "<form method='POST' action='/setbeliefs'>";
  html += "<input type='text' name='beliefsKey' value=''>Beliefs Key<br>";
  html += "<input type='text' name='beliefsValue' value=''>Beliefs Value<br>";
  html += "<input type='submit' value='Enviar'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

////////////////////////////////////////////HANDLE DESIRE////////////////////////////////////////////////////
void handleGetDesire() {
  String response;  // Converter o objeto JSON em uma string JSON
  serializeJson(desireJsonDoc, response);
  server.send(200, "application/json", response);  // Enviar a resposta para a página
}

void handleSetDesire() {
  String desireKey = server.arg("desireKey");  // Obter os valores do formulário
  desireArray.add(desireKey);                  // Atualizar as desire com os valores do formulário

  String html = "<html><body><h1>Controle de Relé</h1>";
  html += "<form method='POST' action='/setdesire'>";
  html += "<input type='text' name='desireKey' value=''>Desire Key<br>";
  html += "<input type='submit' value='Enviar'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

String getDesires() {
  if (desireArray.size() > 0) {                                        // verifica se desire é maior que zero
    String desire = desireArray[desireArray.size() - 1].as<String>();  // converter para uma string
    // tem que print o desire aqui
    Serial.println("getDesires d1 " + desire);
    desireArray.remove(desireArray.size() - 1);
    Serial.println("getDesires d2 " + desire);
    // depois aqui ver se ta removendo tudo. (se a desire estiver removendo pode ser que não tenha desire pra fazer)
    return desire;  // return desire
  }
  return "";
}

////////////////////////////////////////////HANDLE INTENTION////////////////////////////////////////////////////
void handleGetIntention() {
  String response;                                 // converter o objeto JSON em uma string JSON
  serializeJson(intentionJsonDoc, response);       // cria json
  server.send(200, "application/json", response);  // enviar a resposta para a página
}

void handleSetIntention() {
  String intentionKey = server.arg("intentionKey");  // obter os valores do formulário
  intentionArray.add(intentionKey);                  // atualizar as desire com os valores do formulário

  String html = "<html><body><h1>Controle de Relé</h1>";
  html += "<form method='POST' action='/setintention'>";
  html += "<input type='text' name='intentionKey' value=''>Intention Key<br>";
  html += "<input type='submit' value='Enviar'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

////////////////////////////////////////////PRS+BDI/////////////////////////////////////////////////////
void updateIntention() {
  String goal = getDesires();
  JsonVariant plan = getPlan(goal);
  if (plan.size() > 0) {  // plano diferente de zero
    Serial.print("o plano não está mais vazio ");
    Serial.println("a goal que está em updateintention é " + goal);
    serializeJson(plan, Serial);  // Imprime o JSON do objeto "plan" na saída serial
    Serial.println();
    Serial.print("o plano que tá em plan depois de getplan: ");
    for (size_t i = 0; i < plan.size(); i++) {  // percorre todos os elementos de plano
      JsonVariant value = plan[i];
      intentionArray.add(value);  // adiciona plano a intenção
    }
    executeIntention();  // executa a intenção
  }
}

void executeIntention() {
  while (intentionArray.size() > 0) {  // enquanto intenção for maior que zero
    Serial.println();
    Serial.println("as intentions não estão vazias");
    String next = intentionArray[intentionArray.size() - 1];  // pega a proxima intenção
    intentionArray.remove(intentionArray.size() - 1);
    Serial.println("intenção: " + next);
    JsonVariant nextPlan = getPlan(next);
    if (!nextPlan.isNull() && nextPlan.is<JsonArray>()) {  // verifica se é um plano
      Serial.println("Verificou que dentro de planos EXISTE nova intenção");
      JsonArray planArray = nextPlan.as<JsonArray>();
      for (JsonVariant value : planArray) {
        intentionArray.add(value);
      }
    } else if (actionMap.count(next) > 0) {  // verifica se é uma ação
      Serial.println("verificou que existe a ação no arquivo pra FAZER");
      actionMap[next]();  // executa a ação
    } else {
      Serial.println("Ação não pode ser executada");
    }
  }
}

////////////////////////////////////////////SET SERVER/////////////////////////////////////////////////////
void setUpWebServer() {
  server.on("/getplan", handleGetPlan);
  server.on("/getbeliefs", handleGetBeliefs);
  server.on("/getdesire", handleGetDesire);
  server.on("/getintention", handleGetIntention);

  server.on("/setplan", handleSetPlan);
  server.on("/setbeliefs", handleSetBeliefs);
  server.on("/setdesire", handleSetDesire);
  server.on("/setintention", handleSetIntention);

  server.begin();
  Serial.println("Servidor iniciado");
}

////////////////////////////////////////////SETUP/////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  delay(1500);
  WiFi.forceSleepWake();

  actionMap["Front"] = std::bind(&Action::Front, action);
  actionMap["Left"] = std::bind(&Action::Left, action);
  actionMap["Right"] = std::bind(&Action::Right, action);

  setUpWiFi();
  setUpWebServer();
}

////////////////////////////////////////////LOOP/////////////////////////////////////////////////////
void loop() {
  server.handleClient();

  updateIntention();
  delay(1000);
}
