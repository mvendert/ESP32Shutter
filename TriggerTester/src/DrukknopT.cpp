// SPDX-License-Identifier: GPL-2.1-only

/*
CVO De Verdieping - Gevorderden - 2025
Marc Van Endert
*/
#include <Arduino.h>
#include "DrukknopT.h"

Drukknop::Drukknop(int p, int modeIn)
  : Pin(p) {
  mode = modeIn;
  if ((modeIn != INPUT) && (modeIn != INPUT_PULLUP) && (modeIn != INPUT_PULLDOWN)) {
    mode = INPUT;
  }
  pinMode(p, mode);
  denderTime = 10;
  buttonTime = 0;
  toestand = DrukknopToestand::Wachten;
};

Drukknop::Drukknop(int p)
  : Drukknop(p, INPUT){};

void Drukknop::leesKnop() {
  updateKnop();
  if (mode == INPUT_PULLUP) {
    waarde != waarde;  //=(waarde == LOW) ? HIGH : LOW;
  }  
  if (wachtOpDebounce) {
    toestand = DrukknopToestand::Wachten;
  } else {
    if (waarde == HIGH and vorigeWaarde == LOW) {
      if (mode == INPUT) {
        toestand = DrukknopToestand::Indrukken;
      } else {
        toestand = DrukknopToestand::Loslaten;
      }
    }
    if (waarde == LOW and vorigeWaarde == LOW) {
      if (mode == INPUT) {
        toestand = DrukknopToestand::Losgelaten;
      } else {
        toestand = DrukknopToestand::Ingedrukt;
      }
    }
    if (waarde == HIGH and vorigeWaarde == HIGH) {
      if (mode == INPUT) {
        toestand = DrukknopToestand::Ingedrukt;
      } else {
        toestand = DrukknopToestand::Losgelaten;
      }
    }
    if (waarde == LOW and vorigeWaarde == HIGH) {
      if (mode == INPUT) {
        toestand = DrukknopToestand::Loslaten;
      } else {
        toestand = DrukknopToestand::Indrukken;
      }
    }
  }
  /*
  Serial.print(Pin);
  Serial.print("->");
  switch (toestand) {
    case DrukknopToestand::Losgelaten:
      Serial.println("Losgelaten");
      break;
    case DrukknopToestand::Indrukken:
      Serial.println("Indrukken");
      break;
    case DrukknopToestand::Loslaten:
      Serial.println("Loslaten");
      break;
    case DrukknopToestand::Ingedrukt:
      Serial.println("Ingedrukt");
      break;
    case DrukknopToestand::Wachten:
      Serial.println("Wachten");
      break;
    default:
      Serial.println("Oeps...");
  }
  */
}

void Drukknop::updateKnop() {
  //Deze code is veel eenvoudiger, maar vereist in de mainloop minimaal een delay(1)
  //anders werkt het niet.
  //unsigned long currentTime = millis();
  //if (currentTime > buttonTime) {
  //  buttonTime = currentTime + denderTime;
  //  vorigeWaarde = waarde;
  //  waarde = digitalRead(Pin);
  //}

  //Deze code vereist GEEN delay, maar dan heb je wel een nieuwe toestand van de knop.
  // Tijdens het debouncen loopt de code gewoon door, maar de toestand van de knop is dan 'wachten'.
  unsigned long currentTime = millis();
  int nieuweWaarde = digitalRead(Pin);
  if (!wachtOpDebounce) {
    if (nieuweWaarde != vorigeWaarde) {
      lastDebounceTime = currentTime;
      wachtOpDebounce = true;
      checkWaarde = nieuweWaarde;
      return;
    };
    vorigeWaarde = waarde;
    waarde = checkWaarde;
  };
  if (wachtOpDebounce) {
    if (currentTime - lastDebounceTime > denderTime) {
      wachtOpDebounce = false;
      vorigeWaarde = waarde;
      waarde = checkWaarde;
      return;
    }
  }  
}

void Drukknop::setDenderTime(unsigned long ms) {
  denderTime = ms;
}

bool Drukknop::toestandIs(DrukknopToestand waarde) {
  bool tt;
  return (toestand == waarde);
}
bool Drukknop::toestandIs(DrukknopToestand waarde, bool update) {
  if (update) {
    updateKnop();
  }
  return toestandIs(waarde);
}

Drukknop::DrukknopToestand Drukknop::getToestand() {
  return toestand;
}

String Drukknop::toString(Drukknop::DrukknopToestand toestand) {
  switch (toestand) {
    case DrukknopToestand::Losgelaten:
      return "Losgelaten";
    case DrukknopToestand::Indrukken:
      return "Indrukken";
    case DrukknopToestand::Loslaten:
      return "Loslaten";
    case DrukknopToestand::Ingedrukt:
      return "Ingedrukt";
    case DrukknopToestand::Wachten:
      return "Wachten";
    default:
      return "Oeps...";
  }
}