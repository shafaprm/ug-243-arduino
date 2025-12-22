#include "Telemetry.h"
#include "Config.h"
#include "Comm.h"
#include "Drive.h"
#include <ArduinoJson.h>
#include <Arduino.h>

namespace Telemetry {

static unsigned long lastTelemMs = 0;

static const char* dirName(int pwmSigned) {
  if (abs(pwmSigned) < PWM_DEADBAND) return "STOP";
  return (pwmSigned > 0) ? "F" : "R";
}

void sendBoot() {
  if (!TELEMETRY_ON) return;

  StaticJsonDocument<128> out;
  out["stat"] = "boot";

  serializeJson(out, Serial);
  Serial.println();
}

void tick(bool timeout, bool safe) {
  if (!TELEMETRY_ON) return;

  const unsigned long now = millis();
  if (now - lastTelemMs < TELEMETRY_MS) return;
  lastTelemMs = now;

  const bool parseErr = Comm::consumeParseErrorFlag();

  const char* stat;
  if (parseErr) stat = "parse_err";
  else if (timeout) stat = "timeout";
  else if (safe) stat = "safe";
  else stat = "ok";

  StaticJsonDocument<256> out;
  out["stat"] = stat;
  out["timeout"] = timeout;
  out["mode"] = Comm::getMode();
  out["estop"] = Comm::getEstop();

  out["th"] = Comm::getTh();
  out["st"] = Comm::getSt();
  out["rx"] = Comm::getRx();
  out["ry"] = Comm::getRy();

  out["tL"] = Drive::getTargetL();
  out["tR"] = Drive::getTargetR();
  out["oL"] = Drive::getOutL();
  out["oR"] = Drive::getOutR();

  out["dirL"] = dirName(Drive::getOutL());
  out["dirR"] = dirName(Drive::getOutR());
  out["age"]  = (unsigned long)Comm::ageMs();

  serializeJson(out, Serial);
  Serial.println();
}

} // namespace Telemetry
