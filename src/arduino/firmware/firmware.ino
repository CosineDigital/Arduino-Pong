static constexpr int P1Up   = 9;
static constexpr int P1Down = 10;
static constexpr int Reset  = 11;
static constexpr int P2Up   = 12;
static constexpr int P2Down = 13;

void setup()
{
  pinMode(P1Up,   INPUT);
  pinMode(P1Down, INPUT);
  pinMode(P2Up,   INPUT);
  pinMode(P2Down, INPUT);
  pinMode(Reset,  INPUT);
  
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

static constexpr byte REQUEST_READY = 1 << 7;
static constexpr byte REQUEST_HOLD  = 1 << 6;

void loop()
{
  if (Serial.available() > 0) {
    // we have recieved a response from the computer
    byte request = Serial.read();

    if (request == REQUEST_READY) {
      // computer is ready for response, send an encoded byte
      byte response{};

      // data structure:
      //  0   0   0   1   1   1   1   1
      //              reset p1up  p1down  p2up  p2down
      response |= digitalRead(P1Up)   << 0;
      response |= digitalRead(P1Down) << 1;
      response |= digitalRead(P2Up)   << 2;
      response |= digitalRead(P2Down) << 3;
      response |= digitalRead(Reset)  << 4;

      // send a single byte to the computer if it's ready
      Serial.write(response);
    }
    else { //response == REQUEST_HOLD   
      // do nothing
    }
  }
}

// use serial event
