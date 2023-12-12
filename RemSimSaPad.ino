/* File name: RemSimSaPad.ino */ 

/**
 * The program is inspired by the Simon Says game.
 *
 * 4 LEDs (red, green, blue and yellow) are placed in a cross-like shape on a surface.
 * The program shows a random pattern of these LEDs (e.g. left, left, top, bottom, top)
 * Players have to replicate the pattern.
 * If players have replicated the pattern, they win. Otherwise, they lose
 * 
 * Pattern length can be set via a program constant.
 * 
 * A 16x2 LCD display guides the players during the game stages.
 * Game stages are: pattern memorization, pattern reproduction, result and game reset.
 * 
 * Players can input the pattern using an infrared remote or a joystick.
 *Thanks to these two input modalities, the game can be played in single- or multi-player modes both!
 **/

/**
 * Name of the authors: Daniele Solombrino, 1743111, solombrino.1743111@studenti.uniroma1.it
 * 
 * Some parts of the code have reworked from open source blogs.
 * Joystick-LED coordination original code from: https://www.italiantechproject.it/tutorial-arduino/joystick
 * LCD without potentiometer original code from: https://www.instructables.com/Arduino-Interfacing-With-LCD-Without-Potentiometer/
 * LCD with    potentiometer original code from: https://techzeero.com/arduino-tutorials/display-potentiometer-readings-on-lcd-display/ 
 **/

/* Date of creation: 08/12/2023 */



// --- BEGIN game constants --- //


/* How many elements should the player memorize */ 
# define SEQ_LEN 3

/**
 * Stores the random sequence of LEDs that the player is supposed to memorize
 * In the LEDs array defined before, each element of the array stores the Arduino PWM pin
 * responsible for lighting the LED up
 * So, by randomly putting indexes of that LEDs array in this array, we are effectively
 * creating a random sequence of the LEDs
 * Looping over this array and using it to index the LEDs array, we can then actually 
 * light the random sequence
 **/ 
int rand_LED_seq_idx[SEQ_LEN];

/* The delay between one LED of the sequence and the following one */
# define LED_SEQ_DELAY_MS 1000

/* Stores player's sequence of LEDs */
int player_LED_seq[SEQ_LEN];

/* Indexes player's sequence of LEDs */
int player_LED_seq_idx = 0;

/* Message to show when the system is initializing the game */
#define INIT_GAME_TOP_MSG "Init game..."
#define INIT_GAME_BOTTOM_MSG "Please wait :)"

/* Message to show when the player is supposed to look at the sequence to memorize */
#define LOOK_AT_SEQ_TOP_MSG "Look at the"
#define LOOK_AT_SEQ_BOTTOM_MSG "sequence :)"

/* Message to show when the player is supposed to reproduce the sequence */
#define REP_SEQ_TOP_MSG "Reproduce the"
#define REP_SEQ_BOTTOM_MSG "sequence :)"

/* Message to show when the player wins */
#define PLAYER_WINS_TOP_MSG "Player wins! :)"
#define PLAYER_WINS_BOTTOM_MSG " "

/* Message to show when the player loses */
#define GAME_WINS_TOP_MSG "Game wins! :("
#define GAME_WINS_BOTTOM_MSG " "


// --- END game constants --- //


// --- ### --- //


// --- BEGIN Joystick definitions --- //


/* Joystick X axis data is read through Arduino board Analog A0 pin */
#define JOYSTICK_X A0

/* Joystick Y axis data is read through Arduino board Analog A1 pin */
#define JOYSTICK_Y A1

/* Joystick press data is read through Arduino board Analog A2 pin */
#define JOYSTICK_BUTTON A2


// --- END Joystick definitions --- //


// --- ### --- //


// --- BEGIN LEDs definitions --- //


/* Red LED is on Arduino board PWM pin 3 */
#define LED_RED 3

/* Blue LED is on Arduino board PWM pin 4 */
#define LED_BLUE 4

/* Green LED is on Arduino board PWM pin 6 */
#define LED_GREEN 6

/* Yellow LED is on Arduino board PWM pin 7 */
#define LED_YELLOW 7

/* Number of LEDs */
#define NUM_LEDS 4

/**
 * Store LEDs in an array.
 * This way, if we generate a random sequence of numbers in [0, NUM_LEDS)
 * we can effectively have a random sequence of LEDs!
 * */ 
int LEDs[] = {LED_RED, LED_BLUE, LED_GREEN, LED_YELLOW};

/* Time (in milliseconds) to keep the LED on, when blinking it */
#define LED_BLINK_TIME_MS 500 

/**
 * Setup method to configure Arduino board pins 3, 4, 6 and 7 as output pins.
 * This is needed to control them via code, since they are responsible for 
 * turning LEDs on or off
 * */
void setup_LEDs_board_pins() {
  /* Red LED */
  pinMode(LED_RED, OUTPUT);

  /* Blue LED */
  pinMode(LED_BLUE, OUTPUT);

  /* Yellow LED */
  pinMode(LED_YELLOW, OUTPUT);

  /* Green LED */
  pinMode(LED_GREEN, OUTPUT);
}

/**
 * Generates random LED sequence.
 * 
 * Generating a random LED sequence really amounts to generating a random sequence
 * of indexes for the LEDs array.
 **/
void generate_rand_LED_sequence() {
  
  /* Seed the random number generator with an analog pin value */
  randomSeed(analogRead(A0));

  /* Print debug message to Serial interface to have an idea where we currenly are in the execution */
  Serial.println("Random LEDs");

  /**
   * LEDs are stored in the LEDs array
   * So, if we generate a random sequence of numbers in [0, NUM_LEDS)
   * we can use these numbers to randomly index the LEDs array
   * effectively creating a random sequence of LEDs
   * */
  for (int i = 0; i < SEQ_LEN; i++) {
    rand_LED_seq_idx[i] = random(NUM_LEDS);

    /* Print generated random index to Serial interface */
    Serial.println(rand_LED_seq_idx[i]);
  }
}

/* Plays a randomly-generated LED sequence */
int play_LED_sequence() {

  generate_rand_LED_sequence(); 

  /* Light the random LED sequence up */
  for (int i = 0; i < SEQ_LEN; i++) {

    blink_LED(LEDs[rand_LED_seq_idx[i]], LED_SEQ_DELAY_MS);

    /* Delay the blink of next LED a little bit to allow player to memorize current LED */
    delay(LED_SEQ_DELAY_MS);
  } 

}

/**
 * Blinks a given LED
 * 
 * Parameters:
 * int LED_board_pin --> Arduino board pin the LED to blink is connected to
 * int blink_time_ms --> milliseconds to keep the LED on, when blinking it
 * */
void blink_LED(int LED_board_pin, int blink_time_ms) {
  digitalWrite(LED_board_pin, true);

  delay(blink_time_ms);

  digitalWrite(LED_board_pin, false);
}

/**
 * Stores player LED choice, when the original random sequence has to be reproduced
 * 
 * Parameters:
 * int player_LED_choice --> what LED did the player choose
*/
void store_player_LED_choice(int player_LED_choice) {

  /* Storing player choice at the current sequence index */
  player_LED_seq[player_LED_seq_idx] = player_LED_choice;

  /* Moving on to the next sequence index */
  player_LED_seq_idx++;
}


// --- END LEDs definitions --- //


// --- ### --- ///


// --- BEGIN LCD definitions --- //


/* Load LCD library */
#include <LiquidCrystal.h> 

/* Stores LCD contrast value */
# define LCD_CONTRAST 50

/* Store LCD pin --> Arduino PWM pin mappings */
# define RS 13
# define E 5
# define D4 11
# define D5 10
# define D6 9
# define D7 8
# define V0 12

/* Init LCD controller object, using 4-data pin control mode */
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);  

/* Setup LCD screen */
void setup_LCD() {
  analogWrite(V0, LCD_CONTRAST);
  lcd.begin(16, 2);
}

/**
 * Prints a message in the LCD screen
 * 
 * Parameters:
 * String top_row_msg --> String containing the msg to display in LCD top row
 * String bottom_row_msg --> String containing the msg to display in LCD bottom row
*/
void print_msg_LCD(String top_row_msg, String bottom_row_msg) {
  
  /* Clean LCD display from previous messages */
  lcd.clear();
  
  /* Set LCD cursor at the start of the top row */
  lcd.setCursor(0, 0);
  /* Write top_row_msg to LCD top row */
  lcd.print(top_row_msg);

  /* Set LCD cursor at the start of the bottom row */
  lcd.setCursor(0, 1);
  /* Write top_row_msg to LCD bottom row */
  lcd.print(bottom_row_msg);
}


// --- END LCD constants --- //


// --- ### --- //


// --- BEGIN Infrared remote controller definitions --- //

#include <IRremote.h>

/* Arduino board PWM pin used to receive data from the infrared remote controller */
#define IR_REMOTE A3

/* Infrared remote controlled button values --> remote button mappings */

/* Blue LED (left LED) can be turned on using either the rewind back or 4 number in the remote */
#define REWIND_BACK 0x44
#define KEY_4 0x8

/* Red LED (top LED) can be turned on using either the vol + or 2 number in the remote */
#define VOL_PLUS 0x46
#define KEY_2 0x18

/* Yellow LED (right LED) can be turned on using either the vol + or 2 number in the remote */
#define FORWARD 0x43
#define KEY_6 0x5A

/* Green LED (bottom LED) can be turned on using either the vol + or 2 number in the remote */
#define VOL_MINUS 0x15
#define KEY_8 0x52

/* Game can be reset by pressing the power buttom on the remote*/
#define KEY_POWER 0x45

/* Gets command from the remote infrared controller */
int get_ir_remote_button_value() {
  
  /**
   * Stores the command received from the infrared remote control .
   * 
   * command attribute is a hexadecimal value that uniquely identifies the 
   * button that has been pressed on the remote
   * */ 
  int ir_command = IrReceiver.decodedIRData.command;

  /* Print raw data from the infrared remote to the Serial interface for debug purposes */
  Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); 
  IrReceiver.printIRResultShort(&Serial); 
  Serial.println();
  Serial.println();

  /* Set the infrared receiver to receive next value that gets pressed in the remote */
  IrReceiver.resume();

  return ir_command;
}

// --- END Infrared remote controller definitions --- //


// --- ### --- //

#define RESET_MSG_DELAY_MS 10000

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void reset_game() {

}

// --- BEGIN Arduino definitions --- //

# define BAUD_RATE 9600

// Setup method, it runs only once when the device gets turned on
void setup() {

  // Initialize serial communication with the Arduino board using the default Baud rate of 9600
  Serial.begin(BAUD_RATE);

  IrReceiver.begin(IR_REMOTE);

  // Setting LCD parameters. See method comments for more details!
  setup_LCD();

  print_msg_LCD(INIT_GAME_TOP_MSG, INIT_GAME_BOTTOM_MSG);
  
  // Setting LEDs Arduino board pins to output mode. See method comments for more details!
  setup_LEDs_board_pins();

  // Set the pin mode for the Joystick press button to "pull up" mode
  pinMode(JOYSTICK_BUTTON, INPUT_PULLUP);

  print_msg_LCD(LOOK_AT_SEQ_TOP_MSG, LOOK_AT_SEQ_BOTTOM_MSG);

  // Show the light sequence to the player
  play_LED_sequence();

  print_msg_LCD(REP_SEQ_TOP_MSG, REP_SEQ_BOTTOM_MSG);

}

void loop() {

  // if the index is smaller than the sequence length
  // then we have to capture the input, because they player has not competed the game yet
  if (player_LED_seq_idx < SEQ_LEN) {

    // read X, Y and pressure coordinates from the Joystick
    int x = analogRead(JOYSTICK_X);
    int y = analogRead(JOYSTICK_Y);
    int button = !digitalRead(JOYSTICK_BUTTON);

    if (button) {
      resetFunc();
    }

    // input mode is infrared remote
    if (IrReceiver.decode()) {

      int ir_command = get_ir_remote_button_value();

      switch (ir_command) {

        case REWIND_BACK:
        case KEY_4:

          store_player_LED_choice(LED_BLUE);

          blink_LED(LED_BLUE, LED_BLINK_TIME_MS);

          break;

        case VOL_PLUS:
        case KEY_2:

          store_player_LED_choice(LED_RED);

          blink_LED(LED_RED, LED_BLINK_TIME_MS);

          break;

        case FORWARD:
        case KEY_6:

          store_player_LED_choice(LED_YELLOW);

          blink_LED(LED_YELLOW, LED_BLINK_TIME_MS);

          break;

        case VOL_MINUS:
        case KEY_8:
          
          store_player_LED_choice(LED_GREEN);

          blink_LED(LED_GREEN, LED_BLINK_TIME_MS);

          break;
        
        case KEY_POWER:

          lcd.setCursor(0, 0);
          lcd.print("Resetting...");
          lcd.setCursor(0, 1);
          lcd.print("        ");

          delay(RESET_MSG_DELAY_MS); 

          resetFunc();

          break;
      }
    }

    // input mode is joystick
    else if (!(x > 505 && x < 520 && y > 505 && y < 520)) {

      Serial.print("X: " + String(x));
      Serial.print(",\tY:" + String(y));
      Serial.println(",\tP: " + String(button));

      if (x == 515 && y == 1023) {
        store_player_LED_choice(LED_BLUE);

        blink_LED(LED_BLUE, LED_BLINK_TIME_MS);
      }

      if (x == 0 && y == 512) {
        store_player_LED_choice(LED_RED);

        blink_LED(LED_RED, LED_BLINK_TIME_MS);
      }

      if (x == 1023 && y == 512) {
        store_player_LED_choice(LED_GREEN);

        blink_LED(LED_GREEN, LED_BLINK_TIME_MS);
      }

      if (x == 515 && y == 0) {
        store_player_LED_choice(LED_YELLOW);

        blink_LED(LED_YELLOW, LED_BLINK_TIME_MS);
      }
    }
  } 
  
  // otherwise, if the index is greater or equal to sequence length
  // then the player has made all his moves, so we have to check whether they were correct or not!
  else {

    int game_and_player_match = true;

    for (int i = 0; i < SEQ_LEN; i++) {

      int game_seq_el = LEDs[rand_LED_seq_idx[i]];
      int player_seq_el = player_LED_seq[i];
      Serial.println("G: " + String(game_seq_el) + " P: " + String(player_seq_el));

      if (game_seq_el != player_seq_el) {
        game_and_player_match = false;
      }

    }

    if (game_and_player_match) {
      Serial.println("PLAYER WINNER!");

      print_msg_LCD(PLAYER_WINS_TOP_MSG, PLAYER_WINS_BOTTOM_MSG);

      for (int i = 0; i < 1000; i++) {
        blink_LED(LED_GREEN, 100);

        delay(100);

        if (IrReceiver.decode()) {

          // store the raw data received from the infrared remote, for debug purposes
          int ir_remote_button_value = IrReceiver.decodedIRData.decodedRawData;
          
          // store the actual command received from the infrared receiver
          int ir_command = IrReceiver.decodedIRData.command;

          IrReceiver.resume(); // Enable receiving of the next value

          if (ir_command == KEY_POWER) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Resetting...");
            lcd.setCursor(0, 1);
            lcd.print("        ");

            delay(RESET_MSG_DELAY_MS); 

            resetFunc();
          }


        }
      }

      resetFunc();

    } 
    else {
      Serial.println("Game winner :(");

      print_msg_LCD(GAME_WINS_TOP_MSG, GAME_WINS_BOTTOM_MSG);

      for (int i = 0; i < 1000; i++) {
        blink_LED(LED_RED, 100);

        delay(250);

        if (IrReceiver.decode()) {

          // store the raw data received from the infrared remote, for debug purposes
          int ir_remote_button_value = IrReceiver.decodedIRData.decodedRawData;
          
          // store the actual command received from the infrared receiver
          int ir_command = IrReceiver.decodedIRData.command;

          IrReceiver.resume(); // Enable receiving of the next value

          if (ir_command == KEY_POWER) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Resetting...");
            lcd.setCursor(0, 1);
            lcd.print("        ");
            delay(RESET_MSG_DELAY_MS); 
            resetFunc();
          }
        }
      }
    }

    Serial.println();
    Serial.println();


  }


  delay(500);
}

// --- END Arduino definitions --- //


// --- ### --- //
