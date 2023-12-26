 /*
  *
  * LCD:                        LEDS: 
  * - GND -> GND                - RED -> DIGITAL 10
  * - VDD -> 5V                 - GREEN -> DIGITAL 9    
  * - VO -> DIGITAL 6           - BLUE -> DIGITAL 8
  * - RS -> DIGITAL 12          - YELLOW -> DIGITAL 7
  * - RW -> GND                 - 1 kohm rezistente pe leduri
  * - E -> DIGITAL 11
  * - D4 -> DIGITAL 5           BUTTONS:
  * - D5 -> DIGITAL 4           - btn0 -> ANALOG 0 (used as digital)
  * - D6 -> DIGITAL 3           - btn1 -> ANALOG 1
  * - D7 -> DIGITAL 2           - btn2 -> ANALOG 2
  * - BLA -> 5V                 - btn3 -> ANALOG 3
  * - BLK -> GND
  *
*/

#include <LiquidCrystal.h>
#include <string.h>

const int contrast = 75;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

enum game_state_enum {IDLE, RUNNING, GAME_OVER, GAME_WON};

game_state_enum game_state = IDLE;
const int red_led = 10, green_led = 9, blue_led = 8, yellow_led = 7;
const int btn0 = A0, btn1 = A1, btn2 = A2, btn3 = A3, btn_start = A4;
int high_score = 0, current_score = 0, game_started = 0;

const String high_score_str_dummy = "HIGH SCORE: ";
const String current_score_str_dummy = "YOUR SCORE: ";
const String game_over_str = "GAME OVER     ";
const String game_won_str = "GAME WON      ";
String high_score_str, current_score_str;

bool sequence_was_displayed, sequence_was_generated, sequence_not_guessed, game_over_displayed, game_won_displayed;
int sequence[20], guessed_sequence[20], current_seq_index = 1, current_guessed_index = 0;
int prev_idle_millis, crt_idle_millis;
int prev_over_millis, crt_over_millis;
int prev_won_millis, crt_won_millis;

void setup() {
  randomSeed(analogRead(5));

  //leds
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  pinMode(yellow_led, OUTPUT);

  //butons
  pinMode(btn0, INPUT);
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(btn3, INPUT);
  pinMode(btn_start, INPUT);

  //lcd
  high_score_str = high_score_str_dummy;
  current_score_str = current_score_str_dummy;
  high_score_str.concat(high_score);
  current_score_str.concat(current_score);
  analogWrite(6, contrast);
  lcd.begin(16, 2);
  lcd.setCursor(1, 0);
  lcd.print(high_score_str);
  lcd.setCursor(1, 1);
  lcd.print(current_score_str);
}

void loop() {
  set_game_state();
  switch(game_state){
    case IDLE:
      game_idle();
    break;
    case RUNNING:
      game_running();
    break;
    case GAME_OVER:
      game_over();
    break;
    case GAME_WON:
      game_won();
    break;
    default:
      game_idle();
    break;
  };
}

void generate_sequence(){
  for(int i = 0; i < 20; ++i){
    sequence[i] = random(4);
  }
}

void fill_sequence(){
  for(int i = 0; i < 20; ++i){
    guessed_sequence[i] = -1;
  }
}

void display_sequence(int sequence[], int index){
  for(int i = 0; i < index; i++){
    switch(sequence[i]){
      case 0:
        digitalWrite(red_led, HIGH);
        delay(500);
        digitalWrite(red_led, LOW);
        delay(500);
      break;
      case 1:
        digitalWrite(green_led, HIGH);
        delay(500);
        digitalWrite(green_led, LOW);
        delay(500);
      break;
      case 2:
        digitalWrite(blue_led, HIGH);
        delay(500);
        digitalWrite(blue_led, LOW);
        delay(500);
      break;
      default:
        digitalWrite(yellow_led, HIGH);
        delay(500);
        digitalWrite(yellow_led, LOW);
        delay(500);
      break;
    }
  }
}

bool match_sequences(int seq1[], int seq2[], int index){
  for(int i = 0; i < index; i++){
    if(seq1[i] != seq2[i]){
      return false;
    }
  }
  return true;
}

void set_game_state(){
  if(digitalRead(btn_start) == HIGH && game_state == IDLE){
    game_state = RUNNING;
    delay(1000);
  }else if(digitalRead(btn_start) == HIGH && game_state != IDLE){
    game_state = IDLE;
    sequence_was_displayed = false;
    sequence_was_generated = false; 
    sequence_not_guessed = false;
    game_over_displayed = false;
    game_won_displayed = false;
    current_score = 0;
    current_score_str = current_score_str_dummy;
    current_seq_index = 1;
    current_guessed_index = 0;
    current_score_str.concat(current_score);
    current_score_str.concat(" ");
    lcd.setCursor(1, 1);
    lcd.print(current_score_str);
    delay(1000);
  }else if(sequence_not_guessed){
    game_state = GAME_OVER;
  }else if(current_score == 5){
    game_state = GAME_WON;
  }
}

void game_idle(){
  crt_idle_millis = millis();
  if(crt_idle_millis - prev_idle_millis >= 1000){
    if(digitalRead(red_led) == LOW && digitalRead(blue_led) == LOW){ 
      digitalWrite(red_led, HIGH);
      digitalWrite(green_led, LOW);
      digitalWrite(blue_led, HIGH);
      digitalWrite(yellow_led, LOW);
    }else if(digitalRead(red_led) == HIGH && digitalRead(blue_led) == HIGH){ 
      digitalWrite(red_led, LOW);
      digitalWrite(green_led, HIGH);
      digitalWrite(blue_led, LOW);
      digitalWrite(yellow_led, HIGH);
    }else{
      digitalWrite(red_led, LOW);
      digitalWrite(green_led, LOW);
      digitalWrite(blue_led, LOW);
      digitalWrite(yellow_led, LOW);
    }
    prev_idle_millis = crt_idle_millis;
  }
}

void game_over(){
  crt_over_millis = millis();
  if(crt_over_millis - prev_over_millis >= 1000){
    if(digitalRead(red_led) == LOW && digitalRead(blue_led) == LOW && digitalRead(green_led) == LOW && digitalRead(yellow_led) == LOW){ 
      digitalWrite(red_led, HIGH);
      digitalWrite(green_led, HIGH);
      digitalWrite(blue_led, HIGH);
      digitalWrite(yellow_led, HIGH);
    }else if(digitalRead(red_led) == HIGH && digitalRead(blue_led) == HIGH && digitalRead(green_led) == HIGH && digitalRead(yellow_led) == HIGH){ 
      digitalWrite(red_led, LOW);
      digitalWrite(green_led, LOW);
      digitalWrite(blue_led, LOW);
      digitalWrite(yellow_led, LOW);
    }else{
      digitalWrite(red_led, LOW);
      digitalWrite(green_led, LOW);
      digitalWrite(blue_led, LOW);
      digitalWrite(yellow_led, LOW);
    }
    prev_over_millis = crt_over_millis;
  }
  if(!game_over_displayed){
    game_over_displayed = true;
    lcd.setCursor(1,1);
    lcd.print(game_over_str);
  }
}

void game_won(){
  crt_won_millis = millis();
  if(crt_won_millis - prev_won_millis >= 1000){
    if(digitalRead(red_led) == LOW && digitalRead(yellow_led) == LOW){ 
      digitalWrite(red_led, HIGH);
      digitalWrite(green_led, LOW);
      digitalWrite(blue_led, LOW);
      digitalWrite(yellow_led, HIGH);
    }else if(digitalRead(red_led) == HIGH && digitalRead(yellow_led) == HIGH){ 
      digitalWrite(red_led, LOW);
      digitalWrite(green_led, HIGH);
      digitalWrite(blue_led, HIGH);
      digitalWrite(yellow_led, LOW);
    }else{
      digitalWrite(red_led, LOW);
      digitalWrite(green_led, LOW);
      digitalWrite(blue_led, LOW);
      digitalWrite(yellow_led, LOW);
    }
    prev_won_millis = crt_won_millis;
  }
  if(!game_won_displayed){
    game_won_displayed = true;
    lcd.setCursor(1,1);
    lcd.print(game_won_str);
  }
}

bool only_this_button_pressed(int crt_btn, int other_btn0, int other_btn1, int other_btn2){
  if(digitalRead(crt_btn) == HIGH && digitalRead(other_btn0) == LOW && 
     digitalRead(other_btn1) == LOW && digitalRead(other_btn2) == LOW){
      return true;
     }
  return false;
}

void game_running(){
  if(!sequence_was_displayed){
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, LOW);
    digitalWrite(blue_led, LOW);
    digitalWrite(yellow_led, LOW);
    delay(1000);

    if(!sequence_was_generated){
      generate_sequence();
      sequence_was_generated = true;
    }

    fill_sequence();
    display_sequence(sequence, current_seq_index);
    sequence_was_displayed = true;
    delay(1000);

  }else if(current_seq_index > current_guessed_index){
    if(only_this_button_pressed(btn0, btn1, btn2, btn3)){
      digitalWrite(red_led, HIGH);
      guessed_sequence[current_guessed_index] = 0;
      current_guessed_index++;
      sequence_not_guessed = !match_sequences(sequence, guessed_sequence, current_guessed_index);
      delay(250);
    }
    digitalWrite(red_led, LOW);

    if(only_this_button_pressed(btn1, btn0, btn2, btn3)){
      digitalWrite(green_led, HIGH);
      guessed_sequence[current_guessed_index] = 1;
      current_guessed_index++;
      sequence_not_guessed = !match_sequences(sequence, guessed_sequence, current_guessed_index);
      delay(250);
    }
    digitalWrite(green_led, LOW);

    if(only_this_button_pressed(btn2, btn0, btn1, btn3)){
      digitalWrite(blue_led, HIGH);
      guessed_sequence[current_guessed_index] = 2;
      current_guessed_index++;
      sequence_not_guessed = !match_sequences(sequence, guessed_sequence, current_guessed_index);
      delay(250);
    }
    digitalWrite(blue_led, LOW);

    if(only_this_button_pressed(btn3, btn0, btn1, btn2)){
      digitalWrite(yellow_led, HIGH);
      guessed_sequence[current_guessed_index] = 3;
      current_guessed_index++;
      sequence_not_guessed = !match_sequences(sequence, guessed_sequence, current_guessed_index);
      delay(250);
    }
    digitalWrite(yellow_led, LOW);

  }else if(current_seq_index == current_guessed_index){
    sequence_not_guessed = !match_sequences(sequence, guessed_sequence, current_seq_index);
    if(!sequence_not_guessed){
      current_seq_index++;
      sequence_was_displayed = false;
      current_guessed_index = 0;
      current_score++;
      if(current_score > high_score){
        high_score = current_score;
        lcd.setCursor(1, 0);
        high_score_str = high_score_str_dummy;
        high_score_str.concat(high_score);
        lcd.print(high_score_str);
      }
      lcd.setCursor(1, 1);
      current_score_str = current_score_str_dummy;
      current_score_str.concat(current_score);
      lcd.print(current_score_str);
    }
    return;
  }
}
