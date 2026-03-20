// SPDX-License-Identifier: GPL-2.1-only
/*
CVO De Verdieping - Gevorderden - 2025
Marc Van Endert
*/

#ifndef __DrukknopTType_H__
#define __DrukknopTType_H__
class Drukknop {
public:
  enum DrukknopToestand {
    Losgelaten,           //Laag = 0,
    Indrukken,            //StijgendeFlank,
    Loslaten,             //DalendeFlank
    Ingedrukt,            //Hoog
    Wachten
  };
  
private:
  const int Pin;
  int waarde;
  int vorigeWaarde;
  int denderTime;
  int mode;
  unsigned long buttonTime;
  unsigned long lastDebounceTime = 0;
  bool wachtOpDebounce = false;
  int checkWaarde = 0;
  DrukknopToestand toestand;
  void updateKnop();

public:
  Drukknop(int p);
  Drukknop(int p, int mode);
  DrukknopToestand getToestand();
  bool toestandIs(DrukknopToestand waarde);
  bool toestandIs(DrukknopToestand waarde, bool update);
  void leesKnop();
  void setDenderTime(unsigned long ms);
  String toString(Drukknop::DrukknopToestand toestand);
};
#endif