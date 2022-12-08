//////////////////////////////////////////////////////////////////////////////
//0 программа: стартове меню.
//1 программа: режим ліхтар 
//2 программа: режим підігрів 50% 
//3 программа: режим підігрів 100%
//4 программа: режим сушки. 100% + запись в епром(при відключеніта включені продовжить роботу)
////////////////////////////////////////////////////////////////////////////////////////////






#include <EEPROM.h>                     //бібліотека для взаємодії з єпромом

#define button 12                       //пін кнопки
#define ZUME 11                         //пін зума
#define Hot_meet 10                     //пін живлення устілок
#define LedStart 9                      // пін стартового(нульового) меню
#define LedLih 8                        // пін меню 1 та він же ліхтар
#define Led1 7                          // пін диода меню 2
#define Led2 6                          // пін диода меню 3
#define Led3 5                          // пін диода меню 4


bool btnFlag = false;                  //флаг стану кнопки
uint32_t btnTimer = 0;                 // час нажатя кнопки
bool btnFlagIgnor = true;              //ігнор меню після вибору та окінченю тайменгу
int menu_val = 0;                      //стан меню(переход по меню)
uint32_t timerMenu = 0;                //таймер - стан пердстарту програми
bool flagTimerStart = true;            //флаг передстарту таймера що вище
uint32_t timeBlink = 0;                //таймер для мигання леду
bool eeprom;                            // для епрому 
bool flagZume = true;                  //флаг для запуску зуму



void setup() {                                                  //цикл setup

  Serial.begin(9600);                       //активація порту для отладки

  pinMode(ZUME, OUTPUT);                    //активація пінов
  pinMode(Hot_meet, OUTPUT);
  pinMode(LedStart, OUTPUT);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(button, INPUT_PULLUP);            //пин кнопки в режими подтягивающего резистора

 //EEPROM.write(1, 0);                      //одноразово(первиний запис в епром)

  if(eeprom = EEPROM.read(1)){              //первіряє що в епромі по адресу??
     menu_val = 4;
  }
}


void loop() {                                                         //основний цикл, у ньому слухаем кнопку та вибираємо кейс меню
  
  bool btnState = !digitalRead(button);            // інвертуємо бо по нажатю приходить 0

  
  
  if(flagZume){                                                                           //ЗУМ

    if(funTimerStart(4200)){                        //видає три коротких звука
      funTimeBlink(ZUME, 700);

      Serial.println("зум on");                    //отладка

    }else{
      digitalWrite(ZUME, LOW);                    //відключає зум
      flagZume = false;

      Serial.println("зум off");                  //отладка
    }
  }

  if (btnState && !btnFlag && millis() - btnTimer > 100) {    // обробка кнопки 
    btnTimer = millis();
     
    if(btnFlagIgnor){                                         //перевірка на ігнор меню
      if(++menu_val > 4){                                     //перерахунок меню
      menu_val = 1;
      }
    }

    btnFlag = true;
    flagTimerStart = true;
    //Serial.println("button cklik");                         //отладка
  }

  if (btnState && btnFlag &&  millis() - btnTimer > 5000) {   // обработка по зажаттю 
    btnTimer = millis();
    menu_val = 0;                                             // переход/зброс меню
    
    //Serial.println("Button hold");                          // отладка
  }

  if (!btnState && btnFlag && millis() - btnTimer > 100) {    // обробка по відпуску кнопки
    btnTimer = millis();
    btnFlag = false;
    //Serial.println("Button NOcklic");                        //отладка
  }


  switch (menu_val){                                           //свитч кейс меню
    case 0:
      menuZero();
      break;
    case 1:
       if(btnFlagIgnor)menuOne();
      break;
    case 2:
      if(btnFlagIgnor)menuTwo();
      break;
    case 3:
       if(btnFlagIgnor)menuThree();
      break;
    case 4:
       if(btnFlagIgnor)menuFour();
      break;
  }
}


  void menuZero(){                                                            //меню 0 (стартовое)
    Serial.println("Chek menu 0");                //отладка
    
    digitalWrite(LedLih, LOW);                   //відключаемо усе
    digitalWrite(Led1, LOW);                  
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    digitalWrite(Hot_meet, LOW);

    funTimeBlink(LedStart, 1000);              
    btnFlagIgnor = true;                          //отпускає ігнор нажаття
                         
   
    
    if(eeprom){                                    //обнуляє епром
        EEPROM.write(1, 0);
        //Serial.println("епром 0");                  //отладка
      }
  }

  void menuOne(){                                                                     //меню 1

    digitalWrite(Led3, LOW);
    digitalWrite(Hot_meet, LOW);
    
    if(funTimerStart(10000)){                   
      funTimeBlink(LedStart, 500);
      digitalWrite(LedLih, HIGH);               //вкл ліхтар

      //Serial.println("Chek menu 1.1");          //отладка
    
    }else{
      btnFlagIgnor = false;      
      digitalWrite(LedStart, LOW);
      digitalWrite(LedLih, HIGH); 
     
     // Serial.println("Chek menu 1.1");          //отладка
    }                                                                 
    
  }        



  void menuTwo(){                                                                         //меню 2

    digitalWrite(LedLih, LOW);                    //відключаємо попередній індикатор

  
    if(funTimerStart(10000)){                     //проходить 10 сек і переходить до режиму фіксації програми. Щоб з нього війти треба зажати кнопку на 7 сек.
      funTimeBlink(LedStart, 500);                //моргає червоний
      funTimeBlink(Led1, 500);                    //моргає зелений


      //Serial.println("Chek menu 2.1");            //отладка

    }else{
      btnFlagIgnor = false;                       //флаг игнору преходу по меню
      digitalWrite(LedStart, LOW);                //викл зелений
      digitalWrite(Led1, HIGH);                   //вкл красний показує тим що перейшов до роботи
      analogWrite(Hot_meet, 125);                 //вкл підігрів на 50%

      //Serial.println("Chek menu 2.2");            //отладка
    }
  }

                                                                                
  void menuThree(){                                                                         // меню 3
       
    digitalWrite(Led1, LOW);

    if(funTimerStart(10000)){
      funTimeBlink(LedStart, 500);
      funTimeBlink(Led2, 500);

      //Serial.println("Chek menu 3.1");                // отладка

    }else{
      btnFlagIgnor = false;
      digitalWrite(LedStart, LOW); 
      digitalWrite(Led2, HIGH); 
      analogWrite(Hot_meet, 255);

      //Serial.println("Chek menu 3.2");               // отладка
    }
  }
 

 void menuFour(){                                                                             //меню 4

    digitalWrite(Led2, LOW);


    if(funTimerStart(10000)){
      funTimeBlink(LedStart, 500);
      funTimeBlink(Led3, 500);

      //Serial.println("Chek menu 4.1");                   // отладка

    }else{
      btnFlagIgnor = false;

      if(!eeprom){                                      //Запись в епром
        EEPROM.write(1, 1);
      }

      digitalWrite(LedStart, LOW); 
      digitalWrite(Led3, HIGH); 
      analogWrite(Hot_meet, 255); 
      
      //Serial.println("Chek menu 4.2");                 // отладка
    }
    
}




bool funTimerStart (int period){                                      //функція приймає період і по закінченю відає false

    if(flagTimerStart){                                               // ! флаг та таймер у глобальних 
      timerMenu = millis();
      flagTimerStart = !flagTimerStart;                               //робіть запис поточного часу та блокуе до нового нажатя
    }

    if(millis() - timerMenu < period){
      return(true);
    }else{
      return(false);
    }   
}
 
void funTimeBlink (int pin, int period){                          //У функцію пріходять два параметри це - номер піна та інтервал приривання в секундах ! таймер в глобальних
  if(millis() - timeBlink > period){                           //таймер на міліс
    timeBlink = millis();                                     
    digitalWrite(pin, !digitalRead(pin));                     //Включає або виключає. Сам чітає, а потім інвертує. 

    Serial.print(digitalRead(pin) ? "On> " : "Off>"); Serial.println(period);    // отладка 
  }
}










