// ---------------------------------------------------------------------------
// Before attempting to use this sketch, please read the "Help with 15 Sensors Example Sketch":
// https://bitbucket.org/teckel12/arduino-new-ping/wiki/Help%20with%2015%20Sensors%20Example%20Sketch
//
// This example code was used to successfully communicate with 15 ultrasonic sensors. You can adjust
// the number of sensors in your project by changing SONAR_NUM and the number of NewPing objects in the
// "sonar" array. You also need to change the pins for each sensor for the NewPing objects. Each sensor
// is pinged at 33ms intervals. So, one cycle of all sensors takes 495ms (33 * 15 = 495ms). The results
// are sent to the "oneSensorCycle" function which currently just displays the distance data. Your project
// would normally process the sensor results in this function (for example, decide if a robot needs to
// turn and call the turn function). Keep in mind this example is event-driven. Your complete sketch needs
// to be written so there's no "delay" commands and the loop() cycles at faster than a 33ms rate. If other
// processes take longer than 33ms, you'll need to increase PING_INTERVAL so it doesn't get behind.
// ---------------------------------------------------------------------------
#include <NewPing.h>

#define SONAR_NUM     1 // Number of sensors.
#define MAX_DISTANCE 250 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
#define ROUNDING_ENABLED false

#define LOOPS_NUM 7 // If the differecnes between readings are high for LOOPS_NUM times, the water is moving.

#define MAX_READINGS 30
const double MAX_DIFF = 3; // Maximum allowed difference between two readings (in cm)
const float MOVING_THRESHOLD = 1.1; // Difference between readings after which water is considered to be moving

double prev_distance[SONAR_NUM] = {0};
double differences[SONAR_NUM][MAX_READINGS] = {0}; // Matrix which stores all the processed data (=differences between readings per each sensor)
double sums[SONAR_NUM] = {0};
double avg_diff[SONAR_NUM] = {0};
short count = 0; // variable that keeps track of the number of readings
short jj = 0;
boolean water_moving[SONAR_NUM][LOOPS_NUM] = {false};
//

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
double distance[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(3, 2, MAX_DISTANCE) // Each sensor's trigger pin, echo pin, and max distance to ping.
//  NewPing(43, 44, MAX_DISTANCE)
//  NewPing(52, 53, MAX_DISTANCE)
};

void setup() {
  Serial.begin(115200);
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
}

void loop() {

  read_ultrasonic();
  // Other code that *DOESN'T* analyze ping results can go here.
}

void read_ultrasonic() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      distance[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    distance[currentSensor] = sonar[currentSensor].ping_result / (float)US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.

  // Increment "count" index only when the first sensor is processed.
  // The variable "count" is shared across the sensors, hence it needs to be updated only once per each loop().
  count += 1;
    
  for (uint8_t idx = 0; idx < SONAR_NUM; idx++) {
    // Measure difference between consecutive readings 
    // (new distance minus previously computed distance at the same sensor)
    differences[idx][count] = abs(distance[idx] - prev_distance[idx]);

    // Clip differences to a maximum value (in cm). 
    // For example, we might now that the maximum water oscillation is 5cm.
    if (differences[idx][count] > MAX_DIFF){
      differences[idx][count] = MAX_DIFF; 
      }
  
    // Store measured distance in a variable
    prev_distance[idx] = distance[idx];

    // Sum all the computed differences together (per each sensor) to compute the average
    sums[idx] += differences[idx][count];

    //    //    // DEBUG INFORMATION
//    Serial.print("prev_distance: "); Serial.println(prev_distance[idx]);
//    Serial.print("distance: "); Serial.println(distance[idx]);
//    Serial.print("Diff: "); Serial.println(differences[idx][count]);
//    Serial.print("Sum: "); Serial.println(sums[idx]);

    // Print the average difference of the last "count" readings
    if (count == MAX_READINGS-1){
      avg_diff[idx] = sums[idx]/(count-1);
      water_moving[idx][jj] = avg_diff[idx] > MOVING_THRESHOLD;

      Serial.print("Difference over "); Serial.print(count); Serial.print(" samples: ");
      Serial.println(avg_diff[idx]);

      boolean allTrue = true;
      for(short iii=0;iii<LOOPS_NUM;iii++){
          if(!water_moving[idx][iii]) //a value is false
              allTrue = false; //not all values in array are true
      }

      if (allTrue) {
        Serial.print("Water is moving according to sensor ");
        Serial.println(idx);
      }
      
      if (jj < LOOPS_NUM) {jj++;}
      else {jj=0;}
    }
  }
  
  // If processing the LAST ultrasonic sensor,
  // restart the computation after MAX_READINGS have been done.
  // Set both the "count" index and the "sums" which were stored to 0.
  if (count == MAX_READINGS-1){
    count = 0;
    memset(sums, 0, sizeof(sums));
  }
}
