//---------------- By Giorgio Padoan - Venice Italy. To be used freely ------------------
//------------------ Library Software for the use of the IC adf4351 ---------------------

#ifndef ADF4351_h
#define ADF4351_h 
// library interface description
#include <Arduino.h>

/*********************** Gestione registri **********************/

class ADF4351_
{
public:   
     int dBm; 
     uint64_t Fu;
     int64_t off_set_PLL;
     void begin(void);
     int imposta_f_dbm(int64_t Fu, int dBm);
     void scansione(int64_t f1, int64_t f2, int64_t passo); 
  private: 
     void invia(int n_byte);
};

extern ADF4351_ ADF4351;

#endif