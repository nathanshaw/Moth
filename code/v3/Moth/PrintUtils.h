#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <Arduino.h>


void printTab() {
  Serial.print("\t");
}
String frText(int d) {
  if (d == 0) {
    return " Front ";
  } else {
    return " Rear ";
  }
}

void dprintTab(int conditional) {
  Serial.print("\t");
}

void dprint(int conditional, String data) {
  if (conditional > 0) {
    Serial.print(data);
  }
}

void dprint(int conditional, int data) {
  if (conditional > 0) {
    Serial.print(data);
  }
}

void dprint(int conditional, unsigned long data) {
  if (conditional > 0) {
    Serial.print(data);
  }
}

void dprint(int conditional, long data) {
  if (conditional > 0) {
    Serial.print(data);
  }
}

void dprint(int conditional, unsigned int data) {
  if (conditional > 0) {
    Serial.print(data);
  }
}

void dprint(int conditional, double data) {
  if (conditional > 0) {
    Serial.print(data);
  }
}

void dprintln(int conditional) {
  if (conditional > 0) {
    Serial.println();
  }
}

void dprintln(int conditional, String data) {
  if (conditional > 0) {
    Serial.println(data);
  }
}

void dprintln(int conditional, int data) {
  if (conditional > 0) {
    Serial.println(data);
  }
}

void dprintln(int conditional, unsigned int data) {
  if (conditional > 0) {
    Serial.println(data);
  }
}

void dprintln(int conditional, double data) {
  if (conditional > 0) {
    Serial.println(data);
  }
}

void dprintln(int conditional, long data) {
  if (conditional > 0) {
    Serial.println(data);
  }
}

void dprintln(int conditional, unsigned long data) {
  if (conditional > 0) {
    Serial.println(data);
  }
}

void printTeensyDivide() {
  Serial.println("- - - - - - - - - - - - - - - - - - -\n");
}

void printMinorDivide() {
  Serial.println("-------------------------------------\n");
}

void printDivide() {
  Serial.println("|||||||||||||||||||||||||||||||||||||\n");
}

void printMajorDivide(String text) {
  Serial.println("|||||||||||||||||||||||||||||||||||||");
  if (text != "") {
    Serial.println(text);
  } else {
    Serial.println();
  };
  Serial.println("|||||||||||||||||||||||||||||||||||||\n");
}


void printDouble( double val, unsigned int precision) {
  // prints val with number of decimal places determine by precision
  // NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
  // example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

  Serial.print (int(val));  //prints the int part
  Serial.print("."); // print the decimal point
  unsigned int frac;
  if (val >= 0)
    frac = (val - int(val)) * precision;
  else
    frac = (int(val) - val ) * precision;
  int frac1 = frac;
  while ( frac1 /= 10 )
    precision /= 10;
  precision /= 10;
  while (  precision /= 10)
    Serial.print("0");

  Serial.println(frac, DEC);
}

#endif // PRINT_UTILS_H
