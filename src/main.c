#include "../include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*
Documentation: https://www.raylib.com/cheatsheet/cheatsheet.html
Use this to learn how to init window, draw, etc. Template by Suga!
*/
int *wordLengthG;

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
        // Strip trailing newline characters safely
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

    // Pick a random index among the valid words
    // Make sure you have called `srand(time(NULL));` at the start of main()!
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
int input(char letter, char letters[]) {
  for (int i = 0; i < *wordLengthG; i++) {
    if (letter == letters[i]) {
      return 1;
    }
  }
  return 0;
};
int main() {
  // Code here
  InitWindow(1000, 1000, "HangMan");

  srand(time(NULL)); // <-- ADD THIS

  char selectedWord[32];       // <-- CHANGE TO THIS
  GetRandomWord(selectedWord);  // <-- ADD THIS to load the first word
  size_t wordULength = strlen(selectedWord);
  int wordLength = (int)wordULength;
  wordLengthG = &wordLength;
  int dashWidth = 50;
  int dashHeight = 10;
  int distances[wordLength];
  int wordDistance = 0;
  int wordIncrement = dashWidth + 30;
  int wordHeight = 700;
  int dashDistance = 100;
  struct Rect rectangles[wordLength];
  struct Rect tempRect;
  struct Text letters[100];
  struct Text tempLetter;
  for (int i = 0; i < wordLength; i++) {
    wordDistance += wordIncrement;
    if (selectedWord[i] == ' ') {
      tempLetter.text = ' ';
      tempRect.color = GRAY;
    } else {
      tempLetter.text = selectedWord[i];
      tempRect.color = BLACK;
    }
    tempLetter.invisible = 1;
    tempRect.x = wordDistance;
    tempLetter.x = wordDistance;
    tempRect.y = wordHeight;
    tempLetter.y = wordHeight - 70;
    tempRect.width = dashWidth;
    tempRect.height = dashHeight;
    tempLetter.color = BEIGE;
    tempLetter.fontSize = 80;
    letters[i] = tempLetter;
    rectangles[i] = tempRect;
  }
  struct Rect drawTempRect;
  struct Text drawTempLetter;
  char tempStr[2];
  int key;
  char newKey;
  int result;
  int wrongGuesses = 0;
  int gameOver = 0;
  char endMsg[] = "YOU LOST THE WORD WAS ";
  char charGuesses[20]; // Buffer to hold the string
  // 220 is height of the post
  int offset = 50;
  Rectangle stickArm1 = {250, 280, 20, 100};
  Rectangle stickArm2 = {250, 290, 20, 100};
  Rectangle stickLeg1 = {250, 350, 20, 100};
  Rectangle stickLeg2 = {250, 360, 20, 100};
  Rectangle stickBody = {250, 280, 30, 120};
  Rectangle rope = {250, 200, 15, 80};
  struct Circle head = {265, 280, 30};
  Vector2 mousePos;
  Color stickManColor = BLACK;
  Rectangle restartButton = {(float)GetRenderWidth()/2,(float)GetRenderHeight()/2,140,50};
  Color restartColor=GREEN;
  // Convert int to string
  snprintf(charGuesses, sizeof(charGuesses), "%d", 6 - wrongGuesses);


  SetExitKey(KEY_NULL);
  while (!WindowShouldClose()) {
    mousePos=GetMousePosition();
    key = GetCharPressed();
    while (key > 0 && gameOver == 0) {
      // Check if the character is a letter (A-Z or a-z)
      if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z')) {
        result = input(key, selectedWord);
        if (result == 1) {
          for (int i = 0; i < wordLength; i++) {
            if (key == letters[i].text) {
              letters[i].invisible = 0;
            }
            
          }
          for (int i = 0; i < wordLength; i++){
            if(letters[i].invisible==0){
              gameOver=2;
            }
            else{
              gameOver=0;
              break;
            }
          }
        } else if (wrongGuesses < 6) {
          wrongGuesses++;
          if (wrongGuesses == 6) {
            gameOver++;
          }
          snprintf(charGuesses, sizeof(charGuesses), "%d", 6 - wrongGuesses);
        }

        // TraceLog(LOG_INFO, "Letter pressed: %c", (char)key);
      }

      // Get next character in the queue
      key = GetCharPressed();
    }

    BeginDrawing();
    ClearBackground(WHITE);
    if(gameOver !=0){
    DrawRectangle(restartButton.x, restartButton.y, restartButton.width, restartButton.height, restartColor);
    DrawText("Restart", restartButton.x+10, restartButton.y, 30, BLACK);
    if(CheckCollisionPointRec(mousePos, restartButton)){
      restartColor=BLUE;
      if(IsMouseButtonPressed(0)){
      // 1. Reset your game trackers
    wrongGuesses = 0;
    gameOver = 0;
    wordDistance = 0;
      // Convert int to string
    snprintf(charGuesses, sizeof(charGuesses), "%d", 6 - wrongGuesses);
    

    // 2. Re-hide and reset all the letters and dashes
    for (int i = 0; i < wordLength; i++) {
        wordDistance += wordIncrement;
        
        if (selectedWord[i] == ' ') {
            letters[i].text = ' ';
            rectangles[i].color = GRAY;
        } else {
            letters[i].text = selectedWord[i];
            rectangles[i].color = BLACK;
        }
        
        letters[i].invisible = 1;         // Hide the letters again
        rectangles[i].x = wordDistance;   // Reset positions
        letters[i].x = wordDistance;
        rectangles[i].y = wordHeight;
        letters[i].y = wordHeight - 70;
    }
  }

    }  
    if (gameOver == 1) {
      DrawText(endMsg, 50, 300, 50, BLACK);
      DrawText(selectedWord, GetRenderWidth()/2-70, 350, 70, RED);
    }
    if (gameOver == 2) {
      DrawText("You've Won!", 400, 300, 50, BLACK);
    }
    
  }
    DrawText("Grey Lines are spaces. By Suga", 50, 960, 30, BLACK);

    DrawText("Guesses Left: ", 50, 0, 40, BLACK);
    DrawText(charGuesses, 340, 0, 40, BLACK);

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
    DrawRectangle(10, 200, 20, 500 + dashHeight, BLACK);
    DrawRectangle(10, 200, 280, 20, BLACK);
    DrawRectangle(rope.x, rope.y, rope.width, rope.height, BROWN);
    if (wrongGuesses <= 5) {
      DrawCircle(head.x, head.y, head.radius, stickManColor);
    }
    if (wrongGuesses <= 4) {
      DrawRectangle(stickBody.x, stickBody.y, stickBody.width, stickBody.height,
                    stickManColor);
    }
    if (wrongGuesses <= 3) {
      DrawRectanglePro(stickArm2, (Vector2){0, 0}, -50, stickManColor);
    }
    if (wrongGuesses <= 2) {
      DrawRectanglePro(stickArm1, (Vector2){0, 0}, 50, stickManColor);
    }
    if (wrongGuesses <= 1) {
      DrawRectanglePro(stickLeg2, (Vector2){0, 0}, -50, stickManColor);
    }
    if (wrongGuesses <= 0) {
      DrawRectanglePro(stickLeg1, (Vector2){0, 0}, 50, stickManColor);
    }
    EndDrawing();
  }
  return 0;
}
