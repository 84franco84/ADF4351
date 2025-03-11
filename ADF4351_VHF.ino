//---------------- By Giorgio Padoan - Venice Italy. To be used freely ------------------//
//------------------- Main Software for the use of the IC adf4351 -----------------------//

#include <ADF4351_VHF.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8,9,10,11,12,13);     // UNO

// FRef=100KHz;  With quarz da 9MHz -> R=90; PDF=100K; MOD=2000; Step of 50Hz=100K/2000

void setup() 
  { 
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("- START -");
  ADF4351.begin();
  //AD8369.begin();  
  }
  
int64_t f; int dbm;

void loop() 
  {
  //while(true){};
  delay(5);
  lcd.clear();
  //f=100000000;  Massimo valore di clk accettato dal AD9951
  f=220000000;
  dbm=2;         // Scelte: -4,-1, 2, 5dBm ->  (0,2V; 0,28; 0,4; 0,56V)
  //
  //if (ADF4351.imposta_f_dbm(f,dbm)==0)
  if (ADF4351.imposta_f_dbm(f,dbm)==1)
    {
    lcd.print("F = "+String((uint32_t)f/1000)+"Khz");
    lcd.setCursor(0,1);
    lcd.print("Out = "+String(dbm));
    }
  else
    lcd.print("Errore!");
  //
  // ADF4351.scansione(200000000,1000000000,1000000);
  while(true){};
  }
