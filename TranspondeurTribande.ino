 

 /////////////////  DATA PIN CONF ////////////
//  11  2  3  9  4  5   -  6  7  8  10
  byte BF6M=2;
  byte BF2M=4;
  byte BF70CM=9;
  byte BFRRF=13;
  
  byte PTT6M=3;
  byte PTT2M=5;
  byte PTT70CM=11;
  byte PTTRRF=12;
   
  byte DATAPLL6M=6;
  byte CLKPLL6M=7;
  byte CSPLL6M=8; 

  byte tonepin = 10;

  
   // ANA In //
  byte SQL6M=0;
  byte SQL2M=1;
  byte SQL70CM=2;
  byte SQLRRF=3;

/////////////////// VARIABLES //////////////
  int RX6m =0;
  int RX2m =0;
  int RX70cm =0;
  int RXRRF =1024;
  int RXANA2M =0;
  int ton = 0;
        
      ////  Partie PLL 6M ////
  byte TX6M = false; // Flag de passage 6 m en emission
  int  cpt=0;
  byte Etat = true;
  long freqTX = 50537500; // Hz
  long freqRX = 50537500; // Hz
  long FI = 10700000; // Hz
  int buffers; // 16 BITS
  int R;      // 16 BITS
  int M;      // 16 BITS
  int A;      // 16 BITS
  long calc;
  
      //// VARIABLE 
  int TX = 10;
  unsigned long TOTMAX = 420000; // Antie bavard à 7mn 420s
  unsigned long TOT ; // compteur pour faire delta avec millis
  int ABVOIERX = 10; // Anti bavard voies RX

void setup() {

  // Pinout
  Serial.begin(115200);

    pinMode(BF6M, OUTPUT); // 4066 BF 6m
    pinMode(BF2M, OUTPUT); // 4066 BF 2m
    pinMode(BF70CM, OUTPUT); // 4066 BF 70cm
    pinMode(BFRRF, OUTPUT); // 4066 BF RRF
    
    pinMode(PTTRRF, OUTPUT); // PTT rrf
    pinMode(PTT6M, OUTPUT); // PTT 6m
    pinMode(PTT2M, OUTPUT); // PTT 2m
    pinMode(PTT70CM, OUTPUT); // PTT 70cm

    pinMode (DATAPLL6M, OUTPUT);  // DATA vert
    pinMode (CLKPLL6M, OUTPUT);  // CLK orange
    pinMode (CSPLL6M, OUTPUT);  // blanc CS
    
//init
    digitalWrite (BF6M, LOW);  
    digitalWrite (BF2M, LOW);
    digitalWrite (BF70CM, LOW);
    digitalWrite (BFRRF, LOW);
    digitalWrite (PTT6M, LOW);   
    digitalWrite (PTT2M, LOW);
    digitalWrite (PTT70CM, LOW); 
 PLLTRX6M();
 Serial.println("init ok"); 
}

void loop() {
   if (TX==8){
      TOT = millis();  // remet a zero le décallage millis si le a eu un coup d'antie bavard tout en gardant la voie désectivée en RX uniquement
   }
//  Serial.println(TOT); 
  RX6m = analogRead (SQL6M);
  delay(1);
  Serial.print("  RX6m=");
  Serial.print(RX6m);
  RX2m = analogRead (SQL2M);
  delay(1);
  Serial.print("  RX2m=");
  Serial.print(RX2m);
  RX70cm = analogRead (SQL70CM);
  delay(1);
  Serial.print("  RX70cm=");
  Serial.print(RX70cm);
  RXRRF = analogRead (SQLRRF);
  delay(1);
  Serial.print("  RXRRF=");
  Serial.print(RXRRF);
  Serial.println("");
  //// SI RX RRF ////
  if (RXRRF >= 300 && RXRRF <=900 && ABVOIERX != 0 && TX != 1 && TX != 2 && TX != 3 ){  // mettre une fourchette pour eviter de bloquer en cas de plantage de l orangePI ou de perte alim 5v
     TX=0;
     Serial.print("RRF");
     TX6M = true ;
     PLLTRX6M();
     digitalWrite (BFRRF, HIGH);  // redirection BF
     digitalWrite (BF6M, LOW);  // redirection BF
     digitalWrite (BF70CM, LOW);  // coupure BF
     digitalWrite (BF2M, LOW);    // coupure BF
     digitalWrite (PTT2M, HIGH);  
     digitalWrite (PTT70CM, HIGH);
     digitalWrite (PTT6M, HIGH);
     ton = 587;
  }

  //// SI RX 6M ////
  if (RX6m >= 825 && RX6m <= 1000 && ABVOIERX != 1 && TX != 0 && TX != 2 && TX != 3 ){
     digitalWrite (PTTRRF, HIGH);  // PTT RRF
     TX=1;
     Serial.print("6m");
     digitalWrite (BFRRF, LOW);    // coupure BF
     digitalWrite (BF6M, HIGH);  // redirection BF
     digitalWrite (BF70CM, LOW);  // coupure BF
     digitalWrite (BF2M, LOW);    // coupure BF
     digitalWrite (PTT2M, HIGH);  // passe en emission, detection RRF fait par le systeme
     digitalWrite (PTT70CM, HIGH);// passe en emission, idem
     ton = 783;
     delay(50);
  }
  //// SI RX 2M ////
  if (RX2m >= 600  && ABVOIERX != 2 && TX != 0 && TX != 1 && TX != 3 ){
     digitalWrite (PTTRRF, HIGH);  // PTT RRF
     TX=2;
     Serial.print("2m");
     digitalWrite (BF2M, HIGH);    // redirection BF
     digitalWrite (BF70CM, LOW);   // coupure BF
     digitalWrite (BF6M, LOW);     // coupure BF
     digitalWrite (BFRRF, LOW);    // coupure BF
     digitalWrite (PTT70CM, HIGH); // passe en emission
     TX6M = true ;                 //Idique que c est une emission
     PLLTRX6M();                   // sous programme pour PLL du 6m
     digitalWrite (PTT6M, HIGH);   // passe en emission
     ton = 1174;
     delay(50);
  }
    
  //// SI RX 70CM ////
  if (RX70cm >= 300 && ABVOIERX != 3 && TX != 0 && TX != 1 && TX != 2 ){
     digitalWrite (PTTRRF, HIGH);  // PTT RRF
     TX=3;
     Serial.print("70cm");
     digitalWrite (BF70CM, HIGH);  // redirection BF
     digitalWrite (BF6M, LOW);     // coupure BF
     digitalWrite (BF2M, LOW);     // coupure BF
     digitalWrite (BFRRF, LOW);    // coupure BF
     digitalWrite (PTT2M, HIGH);
     TX6M = true ;
     PLLTRX6M();
     digitalWrite (PTT6M, HIGH);
     ton = 1567;
     delay(50);
  }
     
     //// COUPURE ////  
   if ((RX70cm <= 300 && TX == 3) || (RX2m <= 512 &&  TX == 2) || (RX6m <= 450 &&  TX == 1) || (RXRRF <= 150 && TX == 0) || (RXANA2M <= 800 && TX == 4) || TX == 10){ 
     delay (300);
     digitalWrite (BF6M, LOW);     // coupure BF
     digitalWrite (BF2M, LOW);     // coupure BF
     digitalWrite (BF70CM, LOW);   // coupure BF
     digitalWrite (BFRRF, LOW);    // coupure BF
   //  tone(tonepin, ton, 100);
   //  delay (200);
     digitalWrite (PTT2M, LOW);   // coupure PTT
     digitalWrite (PTT70CM, LOW); // coupure PTT
     digitalWrite (PTT6M, LOW);    // coupure PTT
     digitalWrite (PTTRRF, LOW);  // LED eteinte
     if (TX6M == true){ ;
        TX6M = false ;
        PLLTRX6M();
        }
      Serial.println("STOP");      
     TX = 9;
   }
   
   if ((RX70cm <= 300 && ABVOIERX == 3) || (RX2m <= 512 &&  ABVOIERX == 2) || (RX6m <= 450 &&  ABVOIERX == 1) || (RXRRF <= 150 && ABVOIERX == 0) || (RXANA2M <= 800 && ABVOIERX == 4)){
     ABVOIERX = 9;
   }
   
   if (TX==9){
      TOT = millis();  // remet a zero le décallage millis si le relais n emet pas
   }
   
   if ((millis() - TOT ) >= TOTMAX){                 // TOT kill TX
     ABVOIERX = TX; // note quel voie est HS
     TX=8; // pour que les autres voies puissent reprendre
     digitalWrite (BF6M, LOW);     // coupure BF
     digitalWrite (BF2M, LOW);     // coupure BF
     digitalWrite (BF70CM, LOW);   // coupure BF
     digitalWrite (BFRRF, LOW);    // coupure BF
     delay (50);
     tone(tonepin, 1760, 50);
     delay (80);
     tone(tonepin, 1174, 50);
     delay (80);
     tone(tonepin, 880, 50);
     delay (80);
     tone(tonepin, 587, 50);
     delay (80);
     tone(tonepin, 440, 50);
     delay (80);
     digitalWrite (PTT2M, LOW);   // coupure PTT
     digitalWrite (PTT70CM, LOW); // coupure PTT
     digitalWrite (PTT6M, LOW);    // coupure PTT
     digitalWrite (PTTRRF, LOW);  // coupure PTT
     if (TX6M == true){ ;
        TX6M = false ;
        PLLTRX6M();
        }
   }
}


//////////////////////////
///// SOUS PROGRAMME /////
//////////////////////////

void PLLTRX6M() {
  if (TX6M != Etat){
    if (TX6M == false){
     digitalWrite (CLKPLL6M, HIGH);
     digitalWrite (CSPLL6M, HIGH);
     calc = (freqRX + FI);
     calc = (calc / 12500);
     delay(1);
     Completpll();
     digitalWrite (CSPLL6M, LOW);
    }
    if (TX6M == true){
     digitalWrite (CLKPLL6M, HIGH);
     digitalWrite (CSPLL6M, HIGH);
     calc = (freqTX / 12500);
     delay(1);
     Completpll();
     digitalWrite (CSPLL6M, LOW);
    }
  }
  delay (1);
  Etat = TX6M;
}

// -----------------------------

void Completpll(){
  //  9 bits  01 1101 1110  M = (Freq / 12500) / xx 
  //  7 bits  001 0000 A = (reste de M) * xx  
  M = int (calc / 40);          // calcul du registre M
  A = calc % M;         // calcul du registre A

// push datas A 
  cpt = 7 ;
//  Serial.print("A= ");
//  Serial.println(A);
  A = A << 4;
  while (cpt >= 1){
    buffers = A;
    buffers = (buffers & 0x400);
    mempll();
    A = A << 1;
    cpt --;
  }
  
// push datas M
  cpt = 10;
//  Serial.print("M= ");
//  Serial.println(M);
  M = M << 1;
  while (cpt >= 1){
    buffers = M;
    buffers = (buffers & 0x400);
    mempll();
    M= M << 1;
    cpt --;
  }
  
// push datas A
  R = 0x0100; // 0x0100 pour osc 6.4M et 0x0200 pour 12.8
  cpt = 11 ;
//  Serial.print("R= ");
//  Serial.println(R);
  while (cpt >= 1){
    buffers = R;  
    buffers = (buffers & 0x400);
    mempll();
    R = R << 1;
    cpt --;
  }
}

// -----------------------------
 
void mempll(){ // programme de vidage de la mémoire
  if (buffers==1024){
   digitalWrite (DATAPLL6M, HIGH);
  }
  else{
   digitalWrite (DATAPLL6M, LOW);
  }
    delay(1); digitalWrite (CLKPLL6M, LOW); 
    delay(1); digitalWrite (CLKPLL6M, HIGH); // VALIDE les data
}




