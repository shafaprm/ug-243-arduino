// #include "Comm.h"
// #include "Config.h"
// #include <ArduinoJson.h>
// #include <string.h>

// namespace Comm {

// static char lineBuf[768];
// static int  lineLen = 0;

// static unsigned long lastCmdMs = 0;

// static char modeBuf[8] = "safe";
// static bool estop = true;

// static float th = 0.0f;
// static float st = 0.0f;

// static float rx = 0.0f;
// static float ry = 0.0f;

// static bool firePending = false;
// static bool parseErrFlag = false;

// static inline float clampf(float x, float lo, float hi) {
//   if (x < lo) return lo;
//   if (x > hi) return hi;
//   return x;
// }

// static bool parseSetCommand(const char* jsonLine) {
//   // StaticJsonDocument<1024> doc;
//   // DeserializationError err = deserializeJson(doc, jsonLine);
//   // if (err) return false;

//   // const char* cmd = doc["cmd"] | "";
//   // if (strcmp(cmd, "set") != 0) return false;
//   StaticJsonDocument<1024> doc;
//   DeserializationError err = deserializeJson(doc, jsonLine);

//   if (err) {
//     return false;   // error nyata
//   }

//   const char* cmd = doc["cmd"] | "";
//   if (strcmp(cmd, "set") != 0) {
//     return true;    // JSON valid tapi bukan command
//   }

//   const char* m = doc["mode"] | "safe";
//   strncpy(modeBuf, m, sizeof(modeBuf) - 1);
//   modeBuf[sizeof(modeBuf) - 1] = '\0';

//   estop = doc["estop"] | true;

//   JsonVariant drive = doc["drive"];
//   th = drive["th"] | 0.0f;
//   st = drive["st"] | 0.0f;

//   JsonVariant turret = doc["turret"];
//   rx = turret["rx"] | 0.0f;
//   ry = turret["ry"] | 0.0f;

//   // fire event: latched, consumed once
//   bool fire = turret["fire"] | false;
//   if (fire) firePending = true;

//   // clamp safety (extra)
//   th = clampf(th, -1.0f, 1.0f);
//   st = clampf(st, -1.0f, 1.0f);
//   rx = clampf(rx, -1.0f, 1.0f);
//   ry = clampf(ry, -1.0f, 1.0f);

//   lastCmdMs = millis();
//   return true;
// }

// void setup() {
//   Serial.begin(BAUD);
//   lastCmdMs = millis();
// }

// // void poll() {
// //   while (Serial.available()) {
// //     char ch = (char)Serial.read();

// //     if (ch == '\n') {
// //       // trim
// //       while (lineLen > 0 && (lineBuf[lineLen - 1] == '\r' || lineBuf[lineLen - 1] == ' ' || lineBuf[lineLen - 1] == '\t')) {
// //         lineLen--;
// //       }
// //       lineBuf[lineLen] = '\0';

// //       // if (lineLen > 0 && lineBuf[0] == '{') {
// //       //   // only attempt parse for likely set command to reduce noise
// //       //   if (strstr(lineBuf, "\"cmd\"") != nullptr && strstr(lineBuf, "\"set\"") != nullptr) {
// //       //     if (!parseSetCommand(lineBuf)) {
// //       //       parseErrFlag = true;
// //       //     }
// //       //   }
// //       // }
// //       if (lineLen > 0 &&
// //           lineBuf[0] == '{' &&
// //           lineBuf[lineLen - 1] == '}') {

// //         if (strstr(lineBuf, "\"cmd\"") &&
// //             strstr(lineBuf, "\"set\"")) {

// //           if (!parseSetCommand(lineBuf)) {
// //             parseErrFlag = true;
// //           }
// //         }
// //       }

// //       lineLen = 0;
// //     } else {
// //       if (lineLen < (int)sizeof(lineBuf) - 1) {
// //         lineBuf[lineLen++] = ch;
// //       } else {
// //         // overflow -> reset buffer, flag error
// //         lineLen = 0;
// //         parseErrFlag = true;
// //       }
// //     }
// //   }
// // }

// void poll() {
//   while (Serial.available()) {
//     char ch = (char)Serial.read();

//     if (ch == '\n') {
//       // trim CR / spasi
//       while (lineLen > 0 &&
//             (lineBuf[lineLen - 1] == '\r' ||
//              lineBuf[lineLen - 1] == ' '  ||
//              lineBuf[lineLen - 1] == '\t')) {
//         lineLen--;
//       }

//       lineBuf[lineLen] = '\0';

//       // biarkan ArduinoJson yang validasi
//       if (lineLen > 0 && lineBuf[0] == '{') {
//         if (!parseSetCommand(lineBuf)) {
//           parseErrFlag = true;
//         }
//       }

//       lineLen = 0;
//     }
//     else {
//       if (lineLen < (int)sizeof(lineBuf) - 1) {
//         lineBuf[lineLen++] = ch;
//       } else {
//         lineLen = 0;
//         parseErrFlag = true;
//       }
//     }
//   }
// }

// float getTh() { return th; }
// float getSt() { return st; }
// float getRx() { return rx; }
// float getRy() { return ry; }

// bool consumeFire() {
//   if (firePending) {
//     firePending = false;
//     return true;
//   }
//   return false;
// }

// bool getEstop() { return estop; }

// bool isTimeout() {
//   return (millis() - lastCmdMs) > CMD_TIMEOUT_MS;
// }

// bool isSafeMode() {
//   return strcmp(modeBuf, "safe") == 0;
// }

// const char* getMode() { return modeBuf; }

// unsigned long ageMs() {
//   return (unsigned long)(millis() - lastCmdMs);
// }

// bool consumeParseErrorFlag() {
//   bool v = parseErrFlag;
//   parseErrFlag = false;
//   return v;
// }

// } // namespace Comm
#include "Comm.h"
#include "Config.h"
#include <ArduinoJson.h>
#include <string.h>

namespace Comm {

// JSON command dari Pi itu pendek; 256 sudah cukup aman.
static char lineBuf[256];
static uint16_t lineLen = 0;

static unsigned long lastCmdMs = 0;

static char modeBuf[8] = "safe";
static bool estop = true;

static float th = 0.0f;
static float st = 0.0f;
static float rx = 0.0f;
static float ry = 0.0f;

static bool firePending = false;
static bool parseErrFlag = false;

static inline float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

static bool parseSetCommand(const char* jsonLine) {
  // 384 biasanya cukup untuk payload kamu; 256 juga sering cukup.
  StaticJsonDocument<384> doc;
  DeserializationError err = deserializeJson(doc, jsonLine);
  if (err) return false;

  const char* cmd = doc["cmd"] | "";
  if (strcmp(cmd, "set") != 0) return true; // valid JSON tapi bukan set

  const char* m = doc["mode"] | "safe";
  strncpy(modeBuf, m, sizeof(modeBuf) - 1);
  modeBuf[sizeof(modeBuf) - 1] = '\0';

  estop = doc["estop"] | true;

  JsonVariant drive = doc["drive"];
  th = drive["th"] | 0.0f;
  st = drive["st"] | 0.0f;

  JsonVariant turret = doc["turret"];
  rx = turret["rx"] | 0.0f;
  ry = turret["ry"] | 0.0f;

  bool fire = turret["fire"] | false;
  if (fire) firePending = true;

  th = clampf(th, -1.0f, 1.0f);
  st = clampf(st, -1.0f, 1.0f);
  rx = clampf(rx, -1.0f, 1.0f);
  ry = clampf(ry, -1.0f, 1.0f);

  lastCmdMs = millis();
  return true;
}

void setup() {
  Serial.begin(BAUD);
  lastCmdMs = millis();
}

void poll() {
  while (Serial.available() > 0) {
    char ch = (char)Serial.read();

    if (ch == '\n') {
      // trim CR / spasi / tab
      while (lineLen > 0 &&
            (lineBuf[lineLen - 1] == '\r' ||
             lineBuf[lineLen - 1] == ' '  ||
             lineBuf[lineLen - 1] == '\t')) {
        lineLen--;
      }
      lineBuf[lineLen] = '\0';

      if (lineLen > 0 && lineBuf[0] == '{') {
        if (!parseSetCommand(lineBuf)) {
          parseErrFlag = true;
        }
      }

      lineLen = 0;
    } else {
      if (lineLen < sizeof(lineBuf) - 1) {
        lineBuf[lineLen++] = ch;
      } else {
        // overflow → reset
        lineLen = 0;
        parseErrFlag = true;
      }
    }
  }
}

float getTh() { return th; }
float getSt() { return st; }
float getRx() { return rx; }
float getRy() { return ry; }

bool consumeFire() {
  if (firePending) { firePending = false; return true; }
  return false;
}

bool getEstop() { return estop; }

bool isTimeout() { return (millis() - lastCmdMs) > CMD_TIMEOUT_MS; }
bool isSafeMode() { return strcmp(modeBuf, "safe") == 0; }
const char* getMode() { return modeBuf; }

unsigned long ageMs() { return (unsigned long)(millis() - lastCmdMs); }

bool consumeParseErrorFlag() {
  bool v = parseErrFlag;
  parseErrFlag = false;
  return v;
}

} // namespace Comm
