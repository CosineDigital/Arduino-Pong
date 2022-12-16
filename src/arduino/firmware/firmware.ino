//Code:
// C++ code
//

//Setting up the pin # variables
constexpr int P1UpPin = 8;
constexpr int P1DownPin = 7;
constexpr int ResetPin = 6;
constexpr int P2UpPin = 5;
constexpr int P2DownPin = 4;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(P1UpPin, INPUT);
  pinMode(P1DownPin, INPUT);
  pinMode(ResetPin, INPUT);
  pinMode(P2UpPin, INPUT);
  pinMode(P2DownPin, INPUT);
  
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

int i = 0;
void loop()
{
  char data{};

  // data structure:
  //  0   0   0   1   1   1   1   1
  //              reset leftup  leftd rigthup rightdown
  data |= digitalRead(P2UpPin) << 0;
  data |= digitalRead(P2DownPin) << 1;
  data |= digitalRead(P1UpPin) << 2;
  data |= digitalRead(P1DownPin) << 3;
  data |= digitalRead(ResetPin) << 4;

  i = 0b01010101;

  Serial.write(i);
}
