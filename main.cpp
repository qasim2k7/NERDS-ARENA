#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <cstring>
#include <ctime>

using namespace std;

const int SCREEN_WIDTH = 1920; 
const int SCREEN_HEIGHT = 945; 

// Game States
enum GameState {
    STATE_INPUT_NAME,   //0
    STATE_MENU,         //1
    STATE_TOPIC,        //2
    STATE_DIFFICULTY,   //3
    STATE_QUIZ,         //4
    STATE_LEADERBOARD,  //5
    STATE_GAME_OVER,    //6
    STATE_EXIT          //7
};

// Difficulty Timers (in seconds)
const int DIFF_EASY = 30; 
const int DIFF_MEDIUM = 20; 
const int DIFF_HARD = 15; 

const Color COL_BG          = { 0x00, 0x22, 0x4D, 0xFF };
const Color COL_MENU_TEXT   = RAYWHITE;
const Color COL_HEAD_TEXT   = RAYWHITE;
const Color COL_MENU_BOX    = { 0xFF, 0x20, 0x4E, 0xFF };
const Color COL_TIMER       = RED;
const Color COL_INPUT_BG    = RAYWHITE;
const Color COL_INPUT_TEXT  = { 0x00, 0x22, 0x4D, 0xFF };

const string TOPICS[] = { "Movies", "Web Series", "Anime", "Politics", "Error Finding", "Vloggers", "Celebrities", "Only Swifties", "BTS ARMY HERE" };
const string FILES[] = { "movies.txt", "webseries.txt", "anime.txt", "politics.txt", "error.txt", "vloggers.txt", "celebrities.txt", "swifty.txt", "bts.txt" };


// Structure for Quiz Data
struct Question {
    string text;                    //Question statement 
    string originalOptions[4];      //This array stores original mcq options as in the files
    char originalCorrectOption;     //Original correct option as in the files
    string shuffledOptions[4];      //This array Stores shuffled options
    char currentCorrectOption;      //Correct option after shuffling
};

//Structure For leaderboard which stores each players name and highest score

struct PlayerScore {
    string name;
    int score;
};

GameState currentState = STATE_INPUT_NAME;      
int menuSelection = 0;
int topicSelection =0;
int currentScore = 0;
int difficultyTime = DIFF_EASY;
int currentTopicIndex = -1;

float timer = 0.0f;
vector<Question> currentQuizQuestions;
int currentQuestionIndex = 0;
int selectedOption = 0;

string currentPlayerName = "";
char nameBuffer[32] = { 0 };
int nameLetterCount = 0;

string feedbackMessage = "";
float feedbackTimer = 0.0f;
int consecutiveCorrect = 0;
int consecutiveWrong = 0;

Font fontHeading, fontMenu, fontBody;

Music backgroundMusic;
Sound correctSound;
Sound wrongSound;

//Slogans which appears in game in typing effect
//Actual slogans
string slogan1 = "Identify Yourself.";
string slogan2 = "We Promise Not To Tell Your MOM You're a GEEK.";
string slogan3 = "Entering this arena may cause immediate realization that you aren't actually that smart.";
string slogan4 = "WARNING:";
//Index of slogans
int slogan1Index = 0;
int slogan2Index = 0;
int slogan3Index = 0;
int slogan4Index = 0;
//Set Time and Speed of typing effect
float typeTimer = 0.0f;
const float TYPE_SPEED = 0.04f; 

//Declaration of all user defined functions

//Main Window Funcions
void InitGame();
void UpdateGame();
void DrawGame();

//Game controlling functions for each state/part
void UpdateInputName();
void UpdateMenu();
void UpdateTopic();
void UpdateDifficulty();
void UpdateQuiz();
void UpdateGameOver();

//Game drawing and GUI functions
void DrawInputName();
void DrawMenu();
void DrawTopic();
void DrawDifficulty();
void DrawQuiz();
void DrawLeaderboard();
void DrawGameOver();
void DrawTextCentered(Font font, const char* text, float x, float y, float fontSize, float spacing, Color tint);

// Quiz Logic and Working functions
void StartQuiz(int topicIndex);
void NextQuestion();
void HandleAnswer(bool correct, bool timeout);
void ShuffleOptions(Question& q);

// File handling and Utility Functions
vector<Question> LoadQuestions(const string& filename);
void SaveScore(string name, int score);
vector<PlayerScore> LoadLeaderboard();
string Trim(const string& str);

// INT MAIN 

int main() {
    InitGame();

    while (!WindowShouldClose() && currentState != STATE_EXIT) {
        UpdateMusicStream(backgroundMusic);
      

        UpdateGame();
        DrawGame();
    }

    UnloadSound(correctSound);
    UnloadSound(wrongSound);
    UnloadMusicStream(backgroundMusic);
    CloseAudioDevice();
 

    UnloadFont(fontHeading);
    UnloadFont(fontMenu);
    UnloadFont(fontBody);

    CloseWindow();
    return 0;
}


void InitGame() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NERDS ARENA");
    SetTargetFPS(60);

    InitAudioDevice();
    backgroundMusic = LoadMusicStream("bgmusic.mp3");
    correctSound = LoadSound("Assets/correct.mp3");
    wrongSound   = LoadSound("Assets/wrong.mp3");
    SetSoundVolume(correctSound, 1.2f);
    SetSoundVolume(wrongSound, 1.2f);
    SetMusicVolume(backgroundMusic, 0.5f); 
    PlayMusicStream(backgroundMusic);

    fontHeading = LoadFont("Assets/PressStart2P-Regular.ttf");
    fontMenu = LoadFont("Assets/RussoOne-Regular.ttf");
    fontBody = LoadFont("Assets/PTSerif-Regular.ttf");

    // Reset global indices
    slogan1Index = 0;
    slogan2Index = 0;
    slogan3Index = 0;
    slogan4Index = 0;
    typeTimer = 0.0f;
}


void UpdateGame() {
    switch (currentState) {
        case STATE_INPUT_NAME: UpdateInputName(); break;
        case STATE_MENU: UpdateMenu(); break;
        case STATE_TOPIC: UpdateTopic(); break;
        case STATE_DIFFICULTY: UpdateDifficulty(); break;
        case STATE_QUIZ: UpdateQuiz(); break;
        case STATE_LEADERBOARD:
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) currentState = STATE_MENU;
            break;
        case STATE_GAME_OVER: UpdateGameOver(); break;
        default: break;
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(COL_BG);

    if (currentState != STATE_QUIZ) {
        DrawTextCentered(fontHeading, "NERDS ARENA", SCREEN_WIDTH/2, 50, 46, 2, GREEN);  
    }

    switch (currentState) {
        case STATE_INPUT_NAME: DrawInputName(); break;
        case STATE_MENU: DrawMenu(); break;
        case STATE_TOPIC: DrawTopic(); break;
        case STATE_DIFFICULTY: DrawDifficulty(); break;
        case STATE_QUIZ: DrawQuiz(); break;
        case STATE_LEADERBOARD: DrawLeaderboard(); break;
        case STATE_GAME_OVER: DrawGameOver(); break;
        default: break;
    }


    EndDrawing();
}

void UpdateInputName() {
    typeTimer += GetFrameTime();
    if (typeTimer >= TYPE_SPEED) {
        typeTimer = 0.0f;
        if (slogan1Index < (int)slogan1.length()) slogan1Index++;
        else if (slogan2Index < (int)slogan2.length()) slogan2Index++;
        else if (slogan4Index < (int)slogan4.length()) slogan4Index++;
        else if (slogan3Index < (int)slogan3.length()) slogan3Index++;
    }

    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (nameLetterCount < 31)) {
            nameBuffer[nameLetterCount++] = (char)key;
            nameBuffer[nameLetterCount] = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (nameLetterCount > 0) nameBuffer[--nameLetterCount] = '\0';
    }
    if (IsKeyPressed(KEY_ENTER) && nameLetterCount > 0) {
        currentPlayerName = string(nameBuffer);
        currentState = STATE_MENU;
        menuSelection = 0;
    }
}

void UpdateMenu() {
    if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection - 1 + 4) % 4;
    if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 4;

    if (IsKeyPressed(KEY_ENTER)) {
        if (menuSelection ==0 ) { 
            currentState = STATE_TOPIC;
        }
        else if(menuSelection==1) {
            currentState=STATE_LEADERBOARD;
        }
        else if (menuSelection == 2) { 
            currentState = STATE_INPUT_NAME;
        } 
        else { 
            currentState = STATE_EXIT;
        }
    }
    }

void UpdateTopic() {
    if (IsKeyPressed(KEY_UP)) topicSelection = (topicSelection -1 + 9) % 9;
    if (IsKeyPressed(KEY_DOWN)) topicSelection = (topicSelection +1) %9;

    if (IsKeyPressed(KEY_ENTER)){
        if (topicSelection==0){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
        if (topicSelection==1){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
        if (topicSelection==2){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
        if (topicSelection==3){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
        if (topicSelection==4){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
        if (topicSelection==5){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
        if (topicSelection==6){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
        if (topicSelection==7){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
        if (topicSelection==8){
            currentTopicIndex = topicSelection;
            currentState = STATE_DIFFICULTY;
        }
    }
    if(IsKeyPressed(KEY_ONE)) {
                currentState = STATE_MENU;
            }

}

void UpdateDifficulty() {
    if (IsKeyPressed(KEY_UP)) menuSelection = (menuSelection - 1 + 3) % 3;
    if (IsKeyPressed(KEY_DOWN)) menuSelection = (menuSelection + 1) % 3;

    if (IsKeyPressed(KEY_ENTER)) {
        if (menuSelection == 0) difficultyTime = DIFF_EASY;
        if (menuSelection == 1) difficultyTime = DIFF_MEDIUM;
        if (menuSelection == 2) difficultyTime = DIFF_HARD;
        StartQuiz(currentTopicIndex);
    }
    if (IsKeyPressed(KEY_ONE)) {
        currentState = STATE_MENU;
        menuSelection = currentTopicIndex;
    }
}

void UpdateQuiz() {
    if (IsKeyPressed(KEY_ONE)) {
        SaveScore(currentPlayerName, currentScore);
        currentState = STATE_MENU;
        return;
    }

    if (feedbackMessage != "") {
        feedbackTimer -= GetFrameTime();
        if (feedbackTimer <= 0) {
            feedbackMessage = "";
            NextQuestion();
        }
        return;
    }

    timer -= GetFrameTime();
    if (timer <= 0) {
        HandleAnswer(false, true); // Timeout
    }

    if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + 4) % 4;
    if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 4;

    if (IsKeyPressed(KEY_ENTER)) {
        char chosen = 'A' + selectedOption;
        char correct = currentQuizQuestions[currentQuestionIndex].currentCorrectOption;
        HandleAnswer(chosen == correct, false);
    }
}

void UpdateGameOver() {
    if (IsKeyPressed(KEY_ENTER)) {
        nameLetterCount = 0;
        memset(nameBuffer, 0, 32);
        currentPlayerName = "";

        currentState = STATE_INPUT_NAME;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        currentState = STATE_EXIT;
    }
}


void DrawInputName() {
    string vis1 = slogan1.substr(0, slogan1Index);
    string vis2 = slogan2.substr(0, slogan2Index);
    string vis3 = slogan3.substr(0, slogan3Index);
    string vis4 = slogan4.substr(0, slogan4Index);

    // TOP SECTION: Slogans 1 & 2
    DrawTextCentered(fontBody, vis1.c_str(), SCREEN_WIDTH / 2, 160, 37, 2, YELLOW);
    DrawTextCentered(fontBody, vis2.c_str(), SCREEN_WIDTH / 2, 200, 37, 2, YELLOW);

    // CENTER SECTION: Name Input
    DrawTextCentered(fontMenu, "ENTER YOUR NAME", SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 60, 34, 2, WHITE);
    Rectangle r = { SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 20, 400, 50 };
    DrawRectangleRec(r, COL_INPUT_BG);
    DrawRectangleLinesEx(r, 2, COL_INPUT_TEXT);
    DrawTextEx(fontMenu, nameBuffer, { r.x + 10, r.y + 10 }, 30, 2, COL_INPUT_TEXT);
    
    if ((int)(GetTime() * 2) % 2 == 0) {
        Vector2 textSize = MeasureTextEx(fontMenu, nameBuffer, 30, 2);
        DrawRectangle(r.x + 10 + textSize.x, r.y + 10, 2, 30, COL_INPUT_TEXT);
    }

    // BOTTOM SECTION
    DrawTextCentered(fontHeading, vis4.c_str(), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 190, 30, 2, RED);
    DrawTextCentered(fontBody, vis3.c_str(), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 250, 34, 2, YELLOW);
}

void DrawMenu() {
    DrawTextCentered(fontHeading, "MAIN MENU", SCREEN_WIDTH/2, 160, 30, 2, COL_HEAD_TEXT);

    const char* menuOptions[] = {
        "TOPICS","VIEW LEADERBOARD","EDIT NAME", "EXIT"
    };

    for (int i = 0; i < 4; i++) {
        Rectangle r = { SCREEN_WIDTH/2 - 230, 300 + (float)i * 105, 450, 75 };
        Color boxColor = (i == menuSelection) ? WHITE : COL_MENU_BOX;
        Color textColor = (i == menuSelection) ? COL_MENU_BOX : COL_MENU_TEXT;

        DrawRectangleRounded(r, 0.3f, 10, boxColor);
        DrawTextCentered(fontMenu, menuOptions[i], r.x + r.width/2, r.y + r.height/2, 30, 2, textColor);
    }
    
}

void DrawTopic(){
    const char* topicOptions[] = {
        TOPICS[0].c_str(), TOPICS[1].c_str(), TOPICS[2].c_str(), TOPICS[3].c_str(), TOPICS[4].c_str(), TOPICS[5].c_str(),TOPICS[6].c_str(),TOPICS[7].c_str(),TOPICS[8].c_str() 
    };

    for (int i = 0; i<9; i++){
        Rectangle r = { SCREEN_WIDTH/2 - 230, 120 +(float)i * 90, 450, 75};
        Color boxColor = (i == topicSelection) ? WHITE : COL_MENU_BOX;
        Color textcolor = (i == topicSelection) ? COL_MENU_BOX: COL_MENU_TEXT;

        DrawRectangleRounded(r, 0.3f, 10,boxColor);
        DrawTextCentered(fontMenu, topicOptions[i], r.x + r.width/2, r.y + r.height/2, 30, 2, textcolor);
    }
    DrawTextCentered(fontMenu, "PRESS 1 TO RETURN TO MENU", SCREEN_WIDTH-1730, SCREEN_HEIGHT - 50, 24, 2, COL_HEAD_TEXT);
}

void DrawDifficulty() {
    DrawTextCentered(fontMenu, "SELECT DIFFICULTY", SCREEN_WIDTH/2, 150, 46, 2, WHITE);
    DrawTextCentered(fontHeading, TextFormat("TOPIC: %s", TOPICS[currentTopicIndex].c_str()), SCREEN_WIDTH/2, 220, 24, 2, GREEN);

    const char* options[] = { "EASY (30s)", "MEDIUM (20s)", "HARD (15s)" };

    for (int i = 0; i < 3; i++) {
        Rectangle r = { SCREEN_WIDTH/2 - 230, 300 + (float)i * 105, 450, 75 };
        Color boxColor = (i == menuSelection) ? WHITE : COL_MENU_BOX;
        Color textColor = (i == menuSelection) ? COL_MENU_BOX : COL_MENU_TEXT;

        DrawRectangleRounded(r, 0.3f, 10, boxColor);
        DrawTextCentered(fontMenu, options[i], r.x + r.width/2, r.y + r.height/2, 30, 2, textColor);
    }
    DrawTextCentered(fontMenu, "PRESS 1 TO RETURN TO MENU", SCREEN_WIDTH/2, SCREEN_HEIGHT - 50, 28, 2, COL_HEAD_TEXT);
}

void DrawQuiz() {
    DrawTextCentered(fontHeading, TOPICS[currentTopicIndex].c_str(), SCREEN_WIDTH/2, 50, 40, 2, COL_MENU_TEXT);

    Rectangle timerBox = { SCREEN_WIDTH - 150, 20, 130, 50 };
    DrawRectangleRounded(timerBox, 0.3f, 10, COL_INPUT_BG);
    DrawTextCentered(fontMenu, TextFormat("%.1f", timer), timerBox.x + timerBox.width/2, timerBox.y + timerBox.height/2, 25, 2, COL_TIMER);

    DrawTextEx(fontMenu, TextFormat("Q: %d / %d", currentQuestionIndex + 1, currentQuizQuestions.size()), { 10.0f, 20.0f }, 30.0f, 2.0f, RAYWHITE);
    DrawTextEx(fontMenu, TextFormat("SCORE: %d", currentScore), { 10.0f, 100.0f }, 30.0f, 2.0f, RAYWHITE);

    if (currentQuestionIndex < currentQuizQuestions.size()) {
        Question& q = currentQuizQuestions[currentQuestionIndex];

        DrawTextCentered(fontMenu, q.text.c_str(), SCREEN_WIDTH/2, 200, 40, 2, WHITE);

        for (int i = 0; i < 4; i++) {
            Rectangle r = { SCREEN_WIDTH/2 - 300, 300 + (float)i * 150, 550, 80 };
            DrawRectangleRec(r, (i == selectedOption) ? WHITE : COL_MENU_BOX);

            DrawTextEx(fontMenu, TextFormat("%c. %s", 'A' + i, q.shuffledOptions[i].c_str()), { r.x + 25, r.y + 30 }, 30, 2, (i == selectedOption) ? COL_MENU_BOX : COL_MENU_TEXT);
        }
    }

    if (feedbackMessage != "") {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, { 0, 0, 0, 200 });
        DrawTextCentered(fontHeading, feedbackMessage.c_str(), SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 40, 2, YELLOW);
    }

    DrawTextCentered(fontMenu, "PRESS 1 TO RETURN TO MENU", SCREEN_WIDTH-1730, SCREEN_HEIGHT - 50, 24, 2, COL_HEAD_TEXT);
}

void DrawLeaderboard() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COL_BG);
    DrawTextCentered(fontHeading, "LEADERBOARD", SCREEN_WIDTH/2, 50, 40, 2, COL_HEAD_TEXT);

    vector<PlayerScore> scores = LoadLeaderboard();

    float yStart = 150;

    for (size_t i = 0; i < scores.size() && i < 10; ++i) {
        const auto& s = scores[i];
        string line = TextFormat("%zu. %s %d", i + 1, s.name.c_str(), s.score);

        Color c = COL_MENU_TEXT;
         if (i == 0) c = GREEN;
         else if (i == 1) c = YELLOW;
         else if (i == 2) c = PURPLE;

        DrawTextEx(fontMenu, line.c_str(), { SCREEN_WIDTH/2 - 200, yStart }, 34, 2, c);

        yStart += 50;
    }
    DrawTextCentered(fontMenu, "PRESS ENTER TO RETURN TO MAIN MENU", SCREEN_WIDTH/2, SCREEN_HEIGHT - 50, 28, 2, COL_HEAD_TEXT);
}

void DrawGameOver() {
    DrawTextCentered(fontHeading, "GAME OVER", SCREEN_WIDTH/2, 200, 50, 2, WHITE);
    DrawTextCentered(fontMenu, TextFormat("FINAL SCORE: %d", currentScore), SCREEN_WIDTH/2, 300, 40, 2, COL_HEAD_TEXT);
    if (currentScore>=100)
    {
        DrawTextCentered(fontMenu, TextFormat("Excelent !!! Sir Talha Is Proud Of You"),SCREEN_WIDTH/2,420,36,2,COL_MENU_BOX);
    }
    else if (currentScore<=50)
    {
        DrawTextCentered(fontMenu, TextFormat("Have some shame !!!! And Be prepared next time"), SCREEN_WIDTH/2,420,36,2,COL_MENU_BOX);
    }

    DrawTextCentered(fontMenu, "PRESS ENTER FOR NEW PLAYER", SCREEN_WIDTH/2, 600, 30, 2, WHITE);
    DrawTextCentered(fontMenu, "PRESS ESC TO EXIT GAME", SCREEN_WIDTH/2, 650, 30, 2, WHITE);
}

void DrawTextCentered(Font font, const char* text, float x, float y, float fontSize, float spacing, Color tint) {
    Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
    DrawTextEx(font, text, { x - size.x / 2, y - size.y / 2 }, fontSize, spacing, tint);
}

void StartQuiz(int topicIndex) {
    currentQuizQuestions = LoadQuestions(FILES[topicIndex]);

    srand(time(0));
    shuffle(currentQuizQuestions.begin(), currentQuizQuestions.end(), default_random_engine(rand()));

    if (currentQuizQuestions.size() > 20) {
        currentQuizQuestions.resize(20);
    }

    for (auto& q : currentQuizQuestions) {
        ShuffleOptions(q);
    }

    currentQuestionIndex = 0;
    currentScore = 0;
    consecutiveCorrect = 0;
    consecutiveWrong = 0;
    timer = (float)difficultyTime;
    selectedOption = 0;
    feedbackMessage = "";
    currentState = STATE_QUIZ;
}

void HandleAnswer(bool correct, bool timeout) {
    if (timeout) {
        PlaySound(wrongSound);
        feedbackMessage = "TIME'S UP! [BAD]";
        consecutiveCorrect = 0;
        consecutiveWrong++;
    } else if (correct) {
        PlaySound(correctSound);
        currentScore += 10;
        consecutiveCorrect++;
        consecutiveWrong = 0;
        feedbackMessage = "CORRECT! [GOOD]";

        if (consecutiveCorrect == 3) {
            currentScore += 50;
            feedbackMessage = "STREAK BONUS! +50! [GREAT]";
            consecutiveCorrect = 0;
        }
    } else {
         PlaySound(wrongSound);
        feedbackMessage = "WRONG! [BAD]";
        consecutiveCorrect = 0;
        consecutiveWrong++;

        if (consecutiveWrong == 3) {
            currentScore -= 50;
            feedbackMessage = "PENALTY! -50 [PENALTY]";
            consecutiveWrong = 0;
        }
    }
    feedbackTimer = 1.5f;
}

void NextQuestion() {
    currentQuestionIndex++;
    if (currentQuestionIndex >= currentQuizQuestions.size()) {
        SaveScore(currentPlayerName, currentScore);
        currentState = STATE_GAME_OVER;
    } else {
        timer = (float)difficultyTime;
        selectedOption = 0;
    }
}

void ShuffleOptions(Question& q) {
    vector<pair<int, string>> options;
    for (int i = 0; i < 4; ++i) {
        options.push_back({i, q.originalOptions[i]});
    }

    shuffle(options.begin(), options.end(), default_random_engine(rand()));

    char originalCorrect = q.originalCorrectOption;
    for (int i = 0; i < 4; ++i) {
        q.shuffledOptions[i] = options[i].second;

        if (options[i].first == (originalCorrect - 'A')) {
            q.currentCorrectOption = (char)('A' + i);
        }
    }
}

vector<Question> LoadQuestions(const string& filename) {
    vector<Question> questions;
    ifstream file("Assets/" + filename);
    string line;
    string accumulator = "";

    if (!file.is_open()) return questions;

    while (getline(file, line)) {
        if (line.empty()) continue;
        accumulator += line;

        int pipes = 0;
        for (char c : accumulator) if (c == '|') pipes++;

        if (pipes >= 5) {
            stringstream ss(accumulator);
            string segment;
            vector<string> parts;

            while(getline(ss, segment, '|')) parts.push_back(Trim(segment));

            if (parts.size() >= 6) {
                Question q;
                q.text = parts[0];
                q.originalOptions[0] = parts[1];
                q.originalOptions[1] = parts[2];
                q.originalOptions[2] = parts[3];
                q.originalOptions[3] = parts[4];
                q.originalCorrectOption = parts[5][0];
                questions.push_back(q);
            }
            accumulator = "";
        } else {
            accumulator += " ";
        }
    }
    file.close();
    return questions;
}

void SaveScore(string name, int score) {
    if (name.empty()) return;
    ofstream file("Assets/leaderboard.txt", ios::app);
    file << name << "|" << score<< "\n";
    file.close();
}

vector<PlayerScore> LoadLeaderboard() {
    vector<PlayerScore> scores;
    ifstream file("Assets/leaderboard.txt");
    string line;

    vector<PlayerScore> uniqueScores;

    while (getline(file, line)) {
        size_t delim = line.find('|');
        if (delim != string::npos) {
            PlayerScore ps;
            ps.name = line.substr(0, delim);
            try {
                ps.score = stoi(line.substr(delim + 1));
            } catch (const exception& e) {
                continue;
            }
            scores.push_back(ps);
        }
    }
    file.close();

    for(const auto& newScore : scores) {
        bool found = false;
        for(auto &existing : uniqueScores) {
            if(existing.name == newScore.name) {
                if(newScore.score > existing.score) existing.score = newScore.score;
                found = true;
                break;
            }
        }
        if(!found) uniqueScores.push_back(newScore);
    }

    sort(uniqueScores.begin(), uniqueScores.end(), [](const PlayerScore& a, const PlayerScore& b) {
        return a.score > b.score;
    });

    return uniqueScores;
}

string Trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}