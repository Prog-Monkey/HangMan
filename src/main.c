#include "../include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
Documentation: https://www.raylib.com/cheatsheet/cheatsheet.html
Template fixed up!
*/

struct Rect {
  int x;
  int y;
  int width;
  int height;
  Color color;
};

struct Circle {
  int x;
  int y;
  int radius;
};

struct Text {
  int invisible;
  char text;
  int x;
  int y;
  int fontSize;
  Color color;
};

// Reads a random word from "words.txt". 
// Caps length at 9 letters so it fits neatly across your 1000px screen!
void GetRandomWord(char *outputBuffer) {
    FILE *file = fopen("words.txt", "r");
    if (file == NULL) {
        strcpy(outputBuffer, "apple"); // Safe fallback if file is missing
        return;
    }

    char line[64];
    int validWordCount = 0;

    // First pass: Count how many words fit our length constraint (3 to 9 characters)
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        size_t len = strlen(line);
        if (len >= 3 && len <= 9) {
            validWordCount++;
        }
    }

    if (validWordCount == 0) {
        strcpy(outputBuffer, "hangman");
        fclose(file);
        return;
    }

    int targetWordIndex = rand() % validWordCount;

    // Second pass: Jump back to the start and retrieve that specific word
    rewind(file);
    int currentValidIndex = 0;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        size_t len = strlen(line);
        if (len >= 3 && len <= 9) {
            if (currentValidIndex == targetWordIndex) {
                strcpy(outputBuffer, line);
                break;
            }
            currentValidIndex++;
        }
    }

    fclose(file);
}

// Check if a pressed letter is in the selected word
int input(char letter, const char selectedWord[], int length) {
  for (int i = 0; i < length; i++) {
    if (letter == selectedWord[i]) {
      return 1;
    }
  }
  return 0;
}

int main() {
  InitWindow(1000, 1000, "HangMan");
  
  // Try loading window icon safely
  Image icon = LoadImage("img/icon.png");
  if (icon.data != NULL) {
      SetWindowIcon(icon);
  }
  
  srand(time(NULL)); 

  char selectedWord[32];       
  GetRandomWord(selectedWord);  
  int wordLength = (int)strlen(selectedWord);

  int dashWidth = 50;
  int dashHeight = 10;
  int wordIncrement = dashWidth + 30;
  int wordHeight = 700;

  // Max word length is 9, so safe fixed-size buffers of 10 elements are perfect here
  struct Rect rectangles[10];
  struct Text letters[10];

  // Helper to initialize the visual structures for the active word
  void InitWordVisuals(int len, const char *word, struct Rect rects[], struct Text lets[]) {
    int wordDistance = 0;
    struct Rect tempRect;
    struct Text tempLetter;

    for (int i = 0; i < len; i++) {
      wordDistance += wordIncrement;
      if (word[i] == ' ') {
        tempLetter.text = ' ';
        tempRect.color = GRAY;
      } else {
        tempLetter.text = word[i];
        tempRect.color = BLACK;
      }
      tempLetter.invisible = 1;
      tempRect.x = wordDistance;
      tempLetter.x = wordDistance + 10; // slightly shifted so letters sit nicely on top
      tempRect.y = wordHeight;
      tempLetter.y = wordHeight - 70;
      tempRect.width = dashWidth;
      tempRect.height = dashHeight;
      tempLetter.color = BLUE; // Changed to BLUE so it's easier to see on a white background
      tempLetter.fontSize = 80;
      lets[i] = tempLetter;
      rects[i] = tempRect;
    }
  }

  InitWordVisuals(wordLength, selectedWord, rectangles, letters);

  struct Rect drawTempRect;
  struct Text drawTempLetter;
  char tempStr[2];
  int key;
  int result;
  int wrongGuesses = 0;
  int gameOver = 0; // 0 = playing, 1 = lost, 2 = won
  char endMsg[] = "YOU LOST! THE WORD WAS:";
  char charGuesses[20];

  // Stickman positions
  Rectangle stickArm1 = {250, 280, 20, 100};
  Rectangle stickArm2 = {250, 290, 20, 100};
  Rectangle stickLeg1 = {250, 350, 20, 100};
  Rectangle stickLeg2 = {250, 360, 20, 100};
  Rectangle stickBody = {250, 280, 30, 120};
  Rectangle rope = {250, 200, 15, 80};
  struct Circle head = {265, 280, 30};

  Color stickManColor = BLACK;
  Rectangle restartButton = {(float)GetRenderWidth()/2 - 70, (float)GetRenderHeight()/2 - 100, 140, 50};
  Color restartColor = GREEN;

  snprintf(charGuesses, sizeof(charGuesses), "%d", 6 - wrongGuesses);
  SetExitKey(KEY_NULL);

  while (!WindowShouldClose()) {
    Vector2 mousePos = GetMousePosition();
    key = GetCharPressed();

    // GAMEPLAY INPUT LOOP
    while (key > 0 && gameOver == 0) {
      // Normalize letters to lowercase to make the game easier to play
      if (key >= 'A' && key <= 'Z') {
        key = key + 32; 
      }

      if (key >= 'a' && key <= 'z') {
        result = input(key, selectedWord, wordLength);
        if (result == 1) {
          // Reveal correctly guessed letters
          for (int i = 0; i < wordLength; i++) {
            if (key == letters[i].text) {
              letters[i].invisible = 0;
            }
          }

          // Check Win Condition: Are there any hidden characters left?
          int stillHasHidden = 0;
          for (int i = 0; i < wordLength; i++) {
            if (letters[i].invisible == 1 && selectedWord[i] != ' ') {
              stillHasHidden = 1;
              break;
            }
          }
          if (!stillHasHidden) {
            gameOver = 2; // Won
          }

        } else {
          wrongGuesses++;
          if (wrongGuesses >= 6) {
            gameOver = 1; // Lost
          }
          snprintf(charGuesses, sizeof(charGuesses), "%d", 6 - wrongGuesses);
        }
      }
      key = GetCharPressed();
    }

    // DRAWING STEP
    BeginDrawing();
    ClearBackground(WHITE);

    // Draw Core Game Stats
    DrawText("Grey Lines are spaces. By Suga: github.com/Prog-Monkey", 50, 960, 30, BLACK);
    DrawText("Guesses Left: ", 50, 0, 40, BLACK);
    DrawText(charGuesses, 340, 0, 40, BLACK);

    // Draw Dashes and Revealed Letters
    for (int i = 0; i < wordLength; i++) {
      drawTempRect = rectangles[i];
      drawTempLetter = letters[i];
      tempStr[0] = drawTempLetter.text;
      tempStr[1] = '\0';

      DrawRectangle(drawTempRect.x, drawTempRect.y, drawTempRect.width,
                    drawTempRect.height, drawTempRect.color);

      if (drawTempLetter.invisible == 0) {
        DrawText(tempStr, drawTempLetter.x, drawTempLetter.y,
                 drawTempLetter.fontSize, drawTempLetter.color);
      }
    }

    // Draw the Gallows
    DrawRectangle(10, 200, 20, 500, BLACK);
    DrawRectangle(10, 200, 280, 20, BLACK);
    DrawRectangle(rope.x, rope.y, rope.width, rope.height, BROWN);

    // Draw the Stickman incrementally based on wrong guesses
    if (wrongGuesses >= 1) {
      DrawCircle(head.x, head.y, head.radius, stickManColor);
    }
    if (wrongGuesses >= 2) {
      DrawRectangle(stickBody.x, stickBody.y, stickBody.width, stickBody.height, stickManColor);
    }
    if (wrongGuesses >= 3) {
      DrawRectanglePro(stickArm1, (Vector2){0, 0}, 50, stickManColor);
    }
    if (wrongGuesses >= 4) {
      DrawRectanglePro(stickArm2, (Vector2){0, 0}, -50, stickManColor);
    }
    if (wrongGuesses >= 5) {
      DrawRectanglePro(stickLeg1, (Vector2){0, 0}, 50, stickManColor);
    }
    if (wrongGuesses >= 6) {
      DrawRectanglePro(stickLeg2, (Vector2){0, 0}, -50, stickManColor);
    }

    // GAME OVER SCREENS / RESTART BUTTON
    if (gameOver != 0) {
      // Draw Restart Button
      if (CheckCollisionPointRec(mousePos, restartButton)) {
        restartColor = BLUE;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          // Reset game values safely
          wrongGuesses = 0;
          gameOver = 0;
          snprintf(charGuesses, sizeof(charGuesses), "%d", 6 - wrongGuesses);
          
          // Fetch new word and recalculate length
          GetRandomWord(selectedWord);
          wordLength = (int)strlen(selectedWord);

          // Re-init graphics structures
          InitWordVisuals(wordLength, selectedWord, rectangles, letters);
        }
      } else {
        restartColor = GREEN;
      }

      DrawRectangleRec(restartButton, restartColor);
      DrawText("Restart", restartButton.x + 15, restartButton.y + 10, 30, WHITE);

      if (gameOver == 1) {
        DrawText(endMsg, 50, 450, 40, RED);
        DrawText(selectedWord, 50, 510, 50, DARKGRAY);
      }
      if (gameOver == 2) {
        DrawText("You've Won!", GetRenderWidth()/2 - 130, 450, 50, LIME);
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}