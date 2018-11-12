void printReadyMessage(void) {
  lcd.init();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" ^ ^ ^ ^ ^ ^ ^");
  lcd.setCursor(0,1);
  lcd.print(" Ready to Scan");
}

void printVerifyMessage(void) {
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Verifying Card");
}

void printVerify2Message(void) {
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Verifying Card");
  lcd.setCursor(1,1);
  lcd.print("AUTHORIZING...");
}

void printWelcomeMessage(const char* name, const char* message) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(name);
  lcd.setCursor(0,1);
  lcd.print(message);
}

void printServerFailMessage(void) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BADGE ERROR");
}

void printFailMessage(const char* name, const char* message) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(name);
  lcd.setCursor(0,1);
  lcd.print(message);
}
