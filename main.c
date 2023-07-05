#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>

#define BUTTON 16
#define BUTTON2 19
#define BUTTON3 17
#define LED 13
#define LED2 9

// debouncer variables
static const int DEBOUNCE = 500; // debouncer delay
static bool pressed = false;
static int32_t alarm_id = 0;

// grid variables
char grid_var[10] = {'0', '1', '2', '3', '4',
                     '5', '6', '7', '8', '9'}; // grid numbers can change
char grid_var_default[10] = {'0', '1', '2', '3', '4',
                             '5', '6', '7', '8', '9'}; // default grid numbers
int restricted[10] = {12, 12, 12, 12, 12,
                      12, 12, 12, 12, 12}; // restricted grid numbers
char player_var[2] = {'X', 'O'};           // two players

int grid_var_default2[10] = {
    '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9'}; // default grid numbers for reset

static int grid_idx = 0; // grid index
int restricted_idx = 0;  // restricted grid index

int rstr = 12;  // current restricted variable
int player = 0; // state 0, 1 is player. state 3 is reset
int turns = 0;
bool print_enable = false; // because we only want
int result =
    0; // 1 means player O has won, 2 means player X has won, 3 means TIE

// function to check win
bool checkWin() {

  bool status;
  int count = 0;

  for (int i = 0; i < 9; i++) {
    if (grid_var_default[i] == 'O' || grid_var_default[i] == 'X') {
      count = count + 1;
    }
  }

  // checking if player 'O' has won
  if ((grid_var_default[0] == 'O' && grid_var_default[4] == 'O' &&
       grid_var_default[8] == 'O') ||
      (grid_var_default[2] == 'O' && grid_var_default[4] == 'O' &&
       grid_var_default[6] == 'O') ||
      (grid_var_default[3] == 'O' && grid_var_default[4] == 'O' &&
       grid_var_default[5] == 'O') ||
      (grid_var_default[1] == 'O' && grid_var_default[4] == 'O' &&
       grid_var_default[7] == 'O') ||
      (grid_var_default[0] == 'O' && grid_var_default[3] == 'O' &&
       grid_var_default[6] == 'O') ||
      (grid_var_default[2] == 'O' && grid_var_default[5] == 'O' &&
       grid_var_default[8] == 'O') ||
      (grid_var_default[0] == 'O' && grid_var_default[1] == 'O' &&
       grid_var_default[2] == 'O')) {
    printf("\nPLAYER O HAS WON!");
    printf("\nGame is restarting..");
    result = 1;

    status = true;
  }

  // checking if player 'X' has won
  else if ((grid_var_default[0] == 'X' && grid_var_default[4] == 'X' &&
            grid_var_default[8] == 'X') ||
           (grid_var_default[2] == 'X' && grid_var_default[4] == 'X' &&
            grid_var_default[6] == 'X') ||
           (grid_var_default[3] == 'X' && grid_var_default[4] == 'X' &&
            grid_var_default[5] == 'X') ||
           (grid_var_default[1] == 'X' && grid_var_default[4] == 'X' &&
            grid_var_default[7] == 'X') ||
           (grid_var_default[0] == 'X' && grid_var_default[3] == 'X' &&
            grid_var_default[6] == 'X') ||
           (grid_var_default[2] == 'X' && grid_var_default[5] == 'X' &&
            grid_var_default[8] == 'X') ||
           (grid_var_default[0] == 'X' && grid_var_default[1] == 'X' &&
            grid_var_default[2] == 'X')) {

    printf("\nPLAYER X HAS WON!");
    printf("\nGAME is restarting...");
    result = 2;

    status = true;
  }

  else if (count == 9) {
    printf("\n NO ONE WON");
    result = 3;
    status = true;
  }

  else {
    status = false;
  }

  return status;
}

void blink_seq(int *player) {
  switch (*player) {
  case 1:
    gpio_put(LED, 0);
    gpio_put(LED2, 1);
    sleep_ms(800);
    gpio_put(LED2, 0);
    sleep_ms(800);
    gpio_put(LED2, 1);
    sleep_ms(800);
    gpio_put(LED2, 0);
    sleep_ms(800);
    gpio_put(LED2, 1);
    sleep_ms(800);
    gpio_put(LED2, 0);
    break;

  case 2:
    gpio_put(LED2, 0);
    gpio_put(LED, 1);
    sleep_ms(800);
    gpio_put(LED, 0);
    sleep_ms(800);
    gpio_put(LED, 1);
    sleep_ms(800);
    gpio_put(LED, 0);
    sleep_ms(800);
    gpio_put(LED, 1);
    sleep_ms(800);
    gpio_put(LED, 0);
    break;

  case 3:
    gpio_put(LED2, 1);
    gpio_put(LED, 1);
    sleep_ms(800);
    gpio_put(LED2, 0);
    gpio_put(LED, 0);
    sleep_ms(800);
    gpio_put(LED2, 1);
    gpio_put(LED, 1);
    sleep_ms(800);
    gpio_put(LED2, 0);
    gpio_put(LED, 0);
    sleep_ms(800);
    gpio_put(LED2, 1);
    gpio_put(LED, 1);
    sleep_ms(800);
    gpio_put(LED2, 0);
    gpio_put(LED, 0);
    break;

  default:
    gpio_put(LED, 0);
    gpio_put(LED2, 0);
  }
}

// print out grid
void grid() {
  printf("\n");
  printf("\n");
  printf("     |     |     \n");
  printf("  %c  |  %c  |  %c \n", grid_var[0], grid_var[1], grid_var[2]);

  printf("_____|_____|_____\n");
  printf("     |     |     \n");

  printf("  %c  |  %c  |  %c \n", grid_var[3], grid_var[4], grid_var[5]);

  printf("_____|_____|_____\n");
  printf("     |     |     \n");

  printf("  %c  |  %c  |  %c \n", grid_var[6], grid_var[7], grid_var[8]);

  printf("     |     |     \n\n");
  printf("\n");
}

// button functions
int64_t enable_button(alarm_id_t alarm_id, void *user_data) {
  pressed = false;
  return 0;
}

void button_callback(uint gpio, uint32_t events) {

  if (pressed) {
    cancel_alarm(alarm_id);
  }

  else {

    pressed = true;

    if (gpio == BUTTON) {           // select grid
      print_enable = !print_enable; // only print once
      ++grid_idx;

      if (grid_idx > 8) {
        grid_idx = 0;
        grid_var[8] = '8';
      }

    }

    else if (gpio == BUTTON2) { // confirm user entry
      // printf("restricted %d\n", rstr);

      if (grid_var_default[grid_idx] == 'O' ||
          grid_var_default[grid_idx] == 'X') {
        printf("\ninvalid move!");
        player = player;
      } else {
        rstr = grid_idx;
        restricted[restricted_idx++] = grid_idx; // store index
        player = !player; // switch between player 1 and 2
        grid_idx = 0;
        turns++;

        grid_var_default[rstr] = player_var[player];
        printf("\nof player : %c\n", player_var[!player]);
        printf("\nnow turn-->player number %c", player_var[!player]);
      }

      if (checkWin() == true) {
        player = 3;
      }
    }

    else if (gpio == BUTTON3) { // reset game
      player = 3;
    }
  }

  alarm_id = add_alarm_in_ms(DEBOUNCE, enable_button, NULL, false);
}

int main() {
  stdio_init_all();

  // printf("HELLO!");
  sleep_ms(50);

  gpio_init(LED); // initialize LEDs
  gpio_init(LED2);

  gpio_init(BUTTON); // initialize button
  gpio_init(BUTTON2);
  gpio_init(BUTTON3);

  gpio_set_dir(BUTTON, false);  // set direction to input
  gpio_set_dir(BUTTON2, false); // set direction to input
  gpio_set_dir(BUTTON3, false); // set direction to input

  gpio_set_dir(LED, true);
  gpio_set_dir(LED2, true);

  gpio_pull_down(BUTTON);  // pull down resistor
  gpio_pull_down(BUTTON2); // pull down resistor
  gpio_pull_down(BUTTON3); // pull down resistor

  gpio_set_irq_enabled_with_callback(
      BUTTON, 0x04, 1, button_callback); // attach interrupt to button
  gpio_set_irq_enabled(BUTTON2, GPIO_IRQ_EDGE_FALL,
                       true); // attach intr to button 2
  gpio_set_irq_enabled(BUTTON3, GPIO_IRQ_EDGE_FALL,
                       true); // attach intr to button 2

  grid(); // print out grid

  while (true) {
    tight_loop_contents();
    sleep_ms(500);

    switch (player) {
    case 0: // PLAYER 1
      gpio_put(LED, 0);
      gpio_put(LED2, 1);

      if (print_enable) {
        // check if value is equal to any value in array
        grid_var[grid_idx] = 'O';
        if (grid_idx > 0) {
          grid_var[grid_idx - 1] = grid_var_default[grid_idx - 1];
        }
        grid();
        print_enable = !print_enable;
      }

      break;

    case 1: // PLAYER 2

      gpio_put(LED, 1);
      gpio_put(LED2, 0);
      if (print_enable) {
        grid_var[grid_idx] = 'X';
        if (grid_idx > 0) {
          printf("%d and %d\n", grid_idx, rstr);
          grid_var[grid_idx - 1] = grid_var_default[grid_idx - 1];
        }
        grid();
        print_enable = !print_enable;
      }

      break;

    case 3: // CASE 3

      printf("\nGAME RESET..");
      blink_seq(&result);
      result = 0;

      for (int i = 0; i < 10; i++) {
        grid_var[i] = grid_var_default2[i];
        grid_var_default[i] = grid_var_default2[i];
        restricted[i] = 12;
        printf("%d", i);
      }

      grid_idx = 0;
      restricted_idx = 0;
      rstr = 12;
      print_enable = false;

      player = 0;

      break;

    default:
      printf("invalid");
    }
  }
}