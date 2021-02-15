// A kanadai Vidrák tulajdona
#define V_MAX 185.0 // max sebesség [cm/s]
#define DEADBAND 0.2 // holtsáv (tapadási súrlódás legyőzéséhez szükséges pwm)

int pwm_pins[] = {3, 6, 10, 9}; // 0-3. motor EN
int direction_pins[][4] = { // 0-3. motor IN1, IN2
  {12, 2},
  {4, 5},
  {11, 13},
  {7, 8}
};
float motor_multipliers[] = {0.6, 1.0, 1.0, 0.7};

double v = 0.0; // [cm/s]
double angle = 0.0; // [fok]
double vk = 0.0; // [cm/s]
           
//0: negatív 1: pozitív
bool sign(double n){
  return  0 < n;
}

void motorout(int n, double v, double angle, double vk){ // motorindex, haladási sebesség [cm/s], haladás iránya [fok], kerületi sebesség [cm/s]
  double vx = -v * -sin(angle / 180.0 * M_PI); // [cm/s]
  double vy = -v * cos(angle / 180.0 * M_PI); // [cm/s]
 
  double motor_pwm = ((pow(-1, n % 2 + 1) * vk / V_MAX) + (pow(-1, n/2) * vy / V_MAX) + (pow(-1, 1 ^ bool(n % 3)) * vx / V_MAX)) * motor_multipliers[n];
  if (motor_pwm > 0.0) {
  motor_pwm += DEADBAND;
  }
  else if (motor_pwm < 0.0) {
  motor_pwm -= DEADBAND;
  }
  if (motor_pwm > 1.0) {
  motor_pwm = 1.0;
  }
  else if (motor_pwm < -1.0) {
  motor_pwm = -1.0;
  }
 
  digitalWrite(direction_pins[n][0], !sign(motor_pwm));
  digitalWrite(direction_pins[n][1], sign(motor_pwm));
  analogWrite(pwm_pins[n], int(abs(motor_pwm) * 255.0));
}

void setup() {
  Serial.begin(9600);
  for(int i = 0;  i < 4; i++){
  pinMode(direction_pins[i][0], OUTPUT);
  pinMode(direction_pins[i][1], OUTPUT);
  pinMode(pwm_pins[i], OUTPUT);
  }
}

void loop() {
  if (Serial.available() > 0) {
  String message = Serial.readStringUntil('#');
  if (message.length() == 7) {
    Serial.println(message);
    angle = double(message.substring(0, 3).toInt());
    v = double(message.substring(3, 6).toInt());
    int button = message.substring(6, 8).toInt();
    if (button == 2) {
      vk = 15.0;
    }
    else if (button == 4) {
      vk = -15.0;
    }
    else {
      vk = 0.0;
    }
  }
  }
 
  for (int i = 0; i < 4; i++){
  motorout(i, v, angle, vk);
  }
}

/*
  ^
1 | 0
2 | 3
*/
