/*
   пример скетча для устранения дребезга кнопки.
   Кнопка сичтается нажатой или НЕ нажатой если она находится в одном из этих состояний 10 (countStateToSwitch=10) и более циклов считывания подряд
*/

#define BTN_RIGHT_PIN 8

enum BtnState {  //возможные состояния кнопок
  None = 1,
  Pressed = 2,
  Released = 3,
  Front = 4,
  Fall = 5
};

struct Btn {    // структура кнопки
  byte pin = 0;
  int countState = 0;             // счетчик считываний 
  int countStateToSwitch = 10;
  BtnState stableState = None;   // текущее устойчивое состояние кнопки
  BtnState prevState = None;     // предыдущее считаное состояние кнопки

  void init(byte btnpin) {    // функция для инициализации (установки исходного рабочего состояния) кнопки (типа как конструктор у класса)
    pin = btnpin;
    pinMode(pin, INPUT_PULLUP);
  }

  BtnState check() {
    BtnState newState;    // считанное новое состояние кнопки
    if (digitalRead(pin))   // читаем пин
      newState = Released;  // кнопка отпущена (возможно дребезжит)
    else
      newState = Pressed;   // кнопка нажата (возможно дребезжит)

    // условно стабильные состояния кнопки в котором она не может находиться более одного цикла проверки
    if (stableState == Front)     // если пришел Front - то он обязан смениться на Pressed
      stableState = Pressed;
    else if (stableState == Fall) // если пришел Fall - то он обязан смениться на Released
      stableState = Released;

    if (newState != prevState)  // если изменилось состояние то
      countState = 0;           // обнуляем счетчик и начинаем новый отсчет
    else {
      countState++;             // если состояние не меняется то инкрементим счетчик
      if (countState == countStateToSwitch && stableState != newState) {  // если насчитали нужное количество одиноковых считываний то меняем стабильное состояние 
        if (newState == Pressed)     // переход из одного стабильного состояния в другое происходит через состояния Front или Fall
          stableState = Front;
        else
          stableState = Fall;
      }
      else if (countState > countStateToSwitch) { // Если ничего не меняется и счетчик больше 10 то
        countState--;                             // декризим счетчик чтобы он не переполнился.
      }
    }
    
    prevState = newState;
    return stableState;                           // Всегда возвращаем  стабильное (отфильтрованное от дребезга) состояние
  }
};

Btn btnRight;

void setup() {
  Serial.begin(115200);
  btnRight.init(BTN_RIGHT_PIN);     // инициализация кнопки
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  BtnState bs = btnRight.check();   // получаем стабильное состояние 
  if (bs == Front || bs == Fall) {  // приход фронта и спада обозначаем миганием светодиода
    byte pause = 50 + 50 * (bs == Fall);
    for (int i = 0; i < 6; i++)
    {
      digitalWrite( LED_BUILTIN, i % 2);
      delay(pause);
    }
  }
  else
    digitalWrite( LED_BUILTIN, (bs == Pressed));   устойчивые стосояния нажатия и отпускания соответсвенно показываем светодиодом

  Serial.print(bs);
  Serial.print(" ");
  Serial.print(btnRight.countState);
  Serial.println(" ");
}
