//---------------- By Giorgio Padoan - Venice Italy. To be used freely ------------------//
//------------------ Library Software for the use of the IC adf4351 ---------------------//

/*********************** Impostazione dei parametri ****************************

- Si scheglie il PDF, in genere sui 100KHz = Fquarzo/R da cui si ricava R da
  impostare
- Porre "feedback signal" a "Divided"
- Ora in base al passo si sceglie il MOD -> Passo =PDF/MOD
  MOD= PDF/Passo.
- Si verificano le impostaz.ni variando l'Output con le frecce

*******************************************************************************/


#include <ADF4351_VHF.h>

#define DATA  7      //24 !!! per generatore con Arduino Zero      
#define CLK   6      //26    
#define LE    5      //28     
#define CE    4      //30          
// Fcristallo=25M; R=250; -> PDF=Fq/R=100000;  (Output Signal: 70-3900MHz)                  
#define PDF 100000 
// Passo da 100Hz 
#define Passo 100   // Passo  


byte reg_byte[4];

/*********************** Scrittura registri ADF4351  **************************/
 
void ADF4351_::begin(void)
{
pinMode(CE,OUTPUT);       //  CE Alto (abilitato, puo' essere eliminato collegandolo al +5)
digitalWrite(CE, HIGH); 
pinMode(LE,OUTPUT);       //  LE basso
digitalWrite(LE, LOW);  
pinMode(DATA,OUTPUT);     //  Data basso
digitalWrite(DATA, LOW); 
pinMode(CLK,OUTPUT);      //  Clk basso 
digitalWrite(CLK, LOW);
  //  LE basso pronto per invio dati

//------------------------- Setup dei registri ---------------------------------
//------------------------------------------------------------------------------

                                  // 01011000 00000000 00000101  
// Registro 5 - Ind. 101
reg_byte[0]=B00000101;   //  00000 101      Riservati [3,18]  anche [19,21]
reg_byte[1]=B00000000;   //  00000000
reg_byte[2]=B01011000;   //  01 011 000     LD pin [22,23] = 01 -> Digital Lock detect (led)
reg_byte[3]=B00000000;   //  00000000       Riservati
invia(4);
                                 // 00110000 00010000 00111100
// Registro 4 - Ind. 100
reg_byte[0]=B00100100;   //  001 11 100     OUT POWER [3,4] = 5dBm
reg_byte[1]=B00010000;   //  0001 0000      Band select clock divider [12,19];  
reg_byte[2]=B01000000;   //  0 011 0000     RF Divider [20,22];  Feedback Select;  [23] (Foundamental/Divided)
reg_byte[3]=B00000000;   //  00000000       Riservati
invia(4);
                                 // 100 10110011
// Registro 3 - ind.011
reg_byte[0]=B10110011;   //  10110 011     Clock Divider Value [3,14] = 150
reg_byte[1]=B00000100;   //  0 0000100
reg_byte[2]=B00000000;   //  // Feedback Sel. =0 (Divided)
reg_byte[3]=B00000000;   //   ;

/*                                 // 00111110 10001110 01000010
// Registro 2 - Ind. 010
reg_byte[0]=B01000010;   //  01000 010
reg_byte[1]=B10001110;   //  10 0 0111 0       Charge Pump  [9,12]
reg_byte[2]=B00010110;   //  00010110          R counter    [14,23] = 90
reg_byte[3]=B00000000;   //  0 00 000 00       Low Noise ON [29,30]
invia(4);
                               // 00001000 00000000 10000000 01000001
// Registro 1 - Ind. 001            
reg_byte[0]=B10000001;   //   10000 001         MOD=2000  (Step=100000/2000 = 50Hz)
reg_byte[1]=B10111110;   //   1 0111110         
reg_byte[2]=B00000000;   //   00000000          Phase=1
reg_byte[3]=B00001000;   //   000 01 000        Prescaler=1 (8/9)
invia(4);
*/ 

// Registro 2 - Ind. 010
reg_byte[0]=B01000010;   //  01000 010
reg_byte[1]=B10001110;   //  10 0 0111 0       Charge Pump  [9,12]
reg_byte[2]=B00111110;   //  00010110          R counter    [14,23] = 250  (PDF=Fq/R -> PDF=25M/250=100KHz)
reg_byte[3]=B00000000;   //  0 00 000 00       Low Noise ON [29,30]
invia(4);

// Registro 1 - Ind. 001            
reg_byte[0]=B01000001;   //   01000 001         MOD=1000  [3,14]  (Step=PDF/MOD=100000/1000 = 100Hz)
reg_byte[1]=B10011111;   //   1 0011111         
reg_byte[2]=B00000000;   //   00000000          Phase=1
reg_byte[3]=B00001000;   //   000 01 000        Prescaler=1 (8/9)
invia(4);
                                 // 00000100 11100010 00000000 00001000
// Registro 0 - Ind. 000
reg_byte[0]=B00000000;   //  00000 000          FRAC=0 [3,14] -> range da 0 a 3   
reg_byte[1]=B00000000;   //  0 0000000
reg_byte[2]=B11100010;   //  11100010           INT=2500 [15,30] -> range da 2500 a 5000
reg_byte[3]=B00000100;   //  0 0000100          0 al [31] non utilizzato
invia(4);
}

//--------------- Aggiorna solo il Registro 0 e 4 ------------------------------

//----------------- FOUT=PDF *(INT + (FRAC/MOD)) ------------------------------ 
 
int ADF4351_::imposta_f_dbm(int64_t f, int dBm)
{ 
uint32_t fr,q,q1; byte m1,m2,m3; char n;
if (f==0)
   {
   for(n=0;n<=3;n++)
      reg_byte[n]=0;
   invia(4);
   return 0;
   }
// Da 100MHz a 4,4 GHZ
if (f<100000000)  
  f=100000000;
if (f>4400000000) 
   f=4400000000;
 
//--------------- Determinazione del quoziente di Divisione --------------------
//------------------------ del Registro 4 --------------------------------------
// Esegui finche non entra nell'intervallo [2200-4400] 
/*
reg_byte[0]=B00100100;   //  001 11 100     OUT POWER [3,4] = 5dBm
reg_byte[1]=B00010000;   //  0001 0000      Band select clock divider [12,19];  
reg_byte[2]=B01000000;   //  0 011 0000     RF Divider [20,22];  Feedback Select;  [23] (Foundamental/Divided)
reg_byte[3]=B00000000;   //  00000000       Riservati
*/

reg_byte[1]=B00010000;   //  0001 0000      Band select clock divider [12,19];

m1=1;
while(!((m1*f>=2200000000) && (m1*f<=4400000000))) 
   m1=2*m1;
switch  (m1)
	{
	case 1:  m2=0; break;
	case 2:  m2=1; break;
	case 4:  m2=2; break;
	case 8:  m2=3; break;
	case 16: m2=4; break;
	case 32: m2=5; break;
	case 64: m2=6; break;
  default: m2=100;
	}
m2=m2<<4;
reg_byte[2]=m2;          //  0 DDD 0000     RF Divider [20,22];  Feedback Select [23]=1 (Foundamental/Divided=0)
reg_byte[3]=B00000000;   //  00000000       Riservati

// Imposta l'ampiezza in dBm
switch (dBm)
	{
	case -4:  m1=0; break; // 0,2V, 0,28, 0,4, 0,56V
	case -1:  m1=1; break; // 0,28V
	case  2:  m1=2; break; // 0,4V
	case  5:  m1=3; break; // 0,56V
  default:  m1=3;
  }                                       
m1=m1<<3;
reg_byte[0]=B00100100 | m1;   //  001 dd 100     Registro n.4;  OUT POWER [3,4]
invia(4); 
 
//-------------------------- Calcolo N e Frac ----------------------------------
//-------------------------- del Registro 0 ------------------------------------ 
// Imposta il Frac
fr=f%PDF;            // Parte Frazionaria fr->12 bit [3,14]
fr=fr/Passo;
m1=(byte)(fr<<3);
reg_byte[0]=m1;           // FFFFF 000   (F=parte frazionaria, I=parte intera)
                          // I FFFFFFF
m2=(byte)(fr>>5);         // IIIIIIII
// Parte Intera INT       // 0 IIIIIII
q=f/PDF;                    
m1=B00000000;
if((q & 1)==1)
   m1=B10000000; 
// Imposta INT   
reg_byte[1]=m2 | m1;      // I FFFFFFF
q1=q>>1;
reg_byte[2]=(byte)q1;     // IIIIIIII
q1=q>>9;
reg_byte[3]=(byte)q1;     // (0) IIIIIII
//
invia(4);  
//------------------------------------------------------------------------------
return 1;
}

//---------------------------- Invia un byte -----------------------------------
void ADF4351_::invia(int n_byte)
{
byte bout; int n;
digitalWrite(LE, LOW); 
for (n=n_byte-1;n>=0;n--) // Invia n_byte
   {
   digitalWrite(CLK, LOW);
   bout=reg_byte[n]; 
   shiftOut(DATA,CLK,MSBFIRST,bout);
   digitalWrite(CLK, LOW); 
   }
digitalWrite(LE,HIGH);  
digitalWrite(DATA, LOW);
digitalWrite(CLK, LOW);
}
//------------------------------------------------------------------------------

void ADF4351_::scansione(int64_t f1, int64_t f2, int64_t passo)
{ 
int64_t n; 
// Ampiezza = 5dBm di default
for(n=f1;n<f2;n=n+passo)
  {
  imposta_f_dbm(n,5);
  delay(10);
  }
}

ADF4351_ ADF4351;
 