#include "ASSP.h"
#include "SSBasicUtilities.h"
#include "names.h"

#define TRIES 50
#define DEVICES 29
#define PORTS 4

// Checks if device is on the specified port.
// Runs for the number of tries specified or until a message is fetched.
// Returns true if it is, false otherwise.
boolean isDeviceOnPort(const char name[], int port) {
  boolean test = false;
  SerialShield.setMessageSender("szg2");
  SerialShield.setMessageDestination(name);
  SerialShield.setMessageContent("ping");

  while (!SerialShield.sendASSPMessage(port)) {
  }

  for (int i = 0; i < TRIES; i++) {
    test = SerialShield.fetchASSPMessage();
    if (test) {
      return true;
    }
  }
  return false;
}

// Returns the time it takes for the given device to respond on the given port.
// Returns '0' if device doesn't respond after all tries.
unsigned long getTimeToDevice(const char name[], int port) {
  long starttime = millis();

  // send and try fetching message using the previously defined function
  if (isDeviceOnPort(name, port)) {

    long stoptime = millis();
    return stoptime - starttime;
  }
  else {
    return 0;
  }
}


// returns the port number for the device
// returns 0 if device is not responding/not on port
int getPortForDevice(const char name[]) {
  for (int port = 1; port < PORTS + 1; port++) {
    // check if device is on 'this' port
    if (isDeviceOnPort(name, port)) {
      return port;
    }
  }
  return 0;
}

// helper function to print the data calculated/obtained in task4
void printData(int port[], int fastest[], int slowest[], unsigned long fastest_time[], unsigned long slowest_time[]) {
  Serial.print("Port");
  Serial.print("    ");
  Serial.print("Devices");
  Serial.print("    ");
  Serial.print("Fastest");
  Serial.print("    ");
  Serial.print("Time");
  Serial.print("    ");
  Serial.print("Slowest");
  Serial.print("    ");
  Serial.println("Time");

  // print the data for all ports with whitespace padding
  for (int i = 0; i < 4; i++) {

    // print port number.
    Serial.print(i + 1);
    Serial.print("       ");

    // print devices for this port.
    Serial.print(port[i]);
    Serial.print("          ");

    // print the name of the fastest device if set.
    if (fastest[i] == -1) {
      Serial.print("--");
    }
    else {
      Serial.print(names[fastest[i]]);
    }

    // print the time of the fastest device.
    Serial.print("        ");
    Serial.print(fastest_time[i]);
    if (fastest_time[i] > 999) {
      Serial.print("    ");
    }
    else {
      Serial.print("     ");
    }

    // do the same for the slowest device and its time.
    if (slowest[i] == -1) {
      Serial.print("--");
    }
    else {
      Serial.print(names[slowest[i]]);
    }

    // print the time of the slowest device.
    Serial.print("        ");
    Serial.println(slowest_time[i]);
  }
}

// Implementation for task 1.
// goes through all the names in the names array and prints whether they are connected to port 1.
void task1() {
  Serial.print("Device  ");
  Serial.println("  Port1?");
  for (int x = 0; x < DEVICES; x++) {
    Serial.print(names[x]);
    if (isDeviceOnPort(names[x], 1)) {
      Serial.print("       ");
      Serial.println("YES");
    }
    else {
      Serial.print("       ");
      Serial.println("NO");
    }
  }
}

// Implementation for task 2.
// goes through all the names in the list, and prints the resulting time if they are connected to port 4, and “--” otherwise.
void task2() {
  Serial.print("Device  ");
  Serial.println("  Port4Time?");

  for (int x = 0; x < DEVICES; x++) {
    Serial.print(names[x]);
    int time_to_device = getTimeToDevice(names[x], 4);
    Serial.print("       ");
    if (time_to_device == 0) {
      Serial.println("--");
    }
    else {
      Serial.println(time_to_device);
    }
  }
}

// Implementation for task 3.
// loops through all the names and prints which port they are connected to, or “--” if they are not connected.
void task3() {
  Serial.print("Device  ");
  Serial.println("  Port");
  for (int x = 0; x < DEVICES; x++) {
    Serial.print(names[x]);
    int port = getPortForDevice(names[x]);
    if ( port == 0) {
      Serial.print("       ");
      Serial.println("--");
    }
    else {
      Serial.print("       ");
      Serial.println(port);
    }
  }
}

// Implementation for task 4.
// Finds which port a device is on using getPortForDevice function, get the time of the device's response.
// Increments the number of devices for each device on a port, sets the fastest and slowest device with its time and prints the data.
void task4() {
  int port[] = {0, 0, 0, 0};

  // the name 'id' / 'index' number for each port's fastest and slowest devices.
  int fastest[4] = { -1, -1, -1, -1}; // -1 shows that the port's device is unset.
  int slowest[4] = { -1, -1, -1, -1};

  // the times for the fastest and slowest devices on each port, 0 indexed
  unsigned long fastest_time[4] = { 0, 0, 0, 0};
  unsigned long slowest_time[4] = { 0, 0, 0, 0};

  // go through each device, get the port number, the response time and update the fastest and slowest devices (and their time) for each port.
  for (int i = 0; i < DEVICES; i++) {
    int portNumber = getPortForDevice(names[i]);

    // if device is connected to a port
    if (portNumber > 0) {
      unsigned long currTime = getTimeToDevice(names[i], portNumber);

      // increment number of devices for this port
      port[portNumber - 1]++;

      // if the fastest device is unset for this port or the time for this device is faster than the time of the fastest device set.
      // make it this device and also set the fastest time.
      if (fastest[portNumber - 1] == -1 || currTime < fastest_time[portNumber - 1]) {
        fastest_time[portNumber - 1] = currTime;
        fastest[portNumber - 1] = i;
      }

      // do the same for slowest device as well.
      // This makes sure that if a port has only one device connected, it will be the fastest AND the slowest.
      if (slowest[portNumber - 1] == -1 || currTime > slowest_time[portNumber - 1]) {
        slowest_time[portNumber - 1] = currTime;
        slowest[portNumber - 1] = i;
      }
    }
  }

  // print a table with whitespace padding
  printData(port, fastest, slowest, fastest_time, slowest_time);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  SerialShield.setEmuMode(6);
  //task1();
  //task2();
  //task3();
  task4();
}

void loop() {
  // put your main code here, to run repeatedly:

}
