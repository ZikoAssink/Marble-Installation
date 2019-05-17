#define trigPin 3
#define echoPin 2
#define MAX_READINGS 40

double duration, distance, prev_distance, avg_distance;
double differences[MAX_READINGS];
double sum;
short count;

//float alpha; // factor to tune


void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

//  alpha = 0.2;
  count = 0;
  sum = 0;
  prev_distance = 0;
}

void loop() {
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;

  Serial.print(distance);
  Serial.print(" ");
//  avg_distance = alpha * distance + (1-alpha) * avg_distance;
//  Serial.println(avg_distance);

  // Measure difference between consecutive readings
  differences[count] = abs(distance - prev_distance);

  // Store measured distance in a variable
  prev_distance = distance;

  sum += differences[count];
  count += 1;

  // Print the average difference of the last "count" readings
  if (count == MAX_READINGS){
    Serial.println(sum/(count-1));
  }

  // Restart the computation after MAX_READINGS have been done
  if (count == MAX_READINGS){
    count = 0;
    sum = 0;
  }

  // Wait before reading the distance again
  delay(100);
}
