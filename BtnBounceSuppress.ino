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

  //-------------------------- функция для инициализации (установки исходного рабочего состояния) кнопки (типа как конструктор у класса)
  void init(byte btnpin) {    
    pin = btnpin;
    pinMode(pin, INPUT_PULLUP);
  }

  //---------------------------- Функция Check возвращает одно из состояний Pressed, Released, Front, Fall.
  BtnState check() {
    BtnState instantState;    // состояние кнопки на текущий момент
    if (digitalRead(pin))   // читаем пин
      instantState = Released;  // кнопка отпущена (возможно дребезжит)
    else
      instantState = Pressed;   // кнопка нажата (возможно дребезжит)

    // Front и Fall - переходные состояния в котором кнопка может находиться только один цикл проверки
    // после чего состояние безусловно меняется на Pressed или Released соответственно.
    if (stableState == Front)
      stableState = Pressed;
    else if (stableState == Fall)
      stableState = Released;

    if (instantState != prevState)  // если изменилось состояние то
      countState = 0;           // обнуляем счетчик и начинаем новый отсчет
    else {
      countState++;             // если состояние не меняется то инкрементим счетчик
      if (countState == countStateToSwitch && stableState != instantState) {  // если насчитали нужное количество одиноковых считываний то меняем стабильное состояние
        if (instantState == Pressed)     // переход из одного стабильного состояния в другое происходит через состояния Front или Fall
          stableState = Front;
        else
          stableState = Fall;
      }
      else if (countState > countStateToSwitch) { // Если ничего не меняется и счетчик больше 10 то
        countState--;                             // декризим счетчик чтобы он не переполнился.
      }
    }

    prevState = instantState;
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
    digitalWrite( LED_BUILTIN, (bs == Pressed));  // устойчивые стосояния нажатия и отпускания соответсвенно показываем светодиодом

  Serial.print(bs);
  Serial.print(" ");
  Serial.print(btnRight.countState);
  Serial.println(" ");
}
