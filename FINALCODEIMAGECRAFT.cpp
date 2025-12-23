#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<ctime>
#include<iomanip>
using namespace std;
struct Save {
    string playername;
    int points;
    int currentlevel;

};
Save mysave;
int level = 1;
int points = 0;
int levels[5] = { 0,25,60,100,150};
// Function declarations
void applyGreenFilter(vector<unsigned char>& pixels);
void applyRedFilter(vector<unsigned char>& pixels);
void applyBlueFilter(vector<unsigned char>& pixels);
void applyCustomFilter(vector<unsigned char>& pixels, int addR, int addG, int addB);
void gaussianBlurPass(vector<unsigned char>& pixels, int w, int h);
void sharpen(vector<unsigned char>& pixels, int w, int h);
void downscale(vector<unsigned char>& pixels, int w, int h, int scale,
    vector<unsigned char>& small, int& smallW, int& smallH);
void upscale(const vector<unsigned char>& small, int smallW, int smallH,
    int scale, vector<unsigned char>& big, int origW, int origH);
void display(int level);
int updateLevel();
int main();
void askGreen();
void askRed();
void askBlue();
void askBlur();
void askSharp();
void askRandom();
void askCustom();
bool validFilter(int level, int choice);

void savegame(){
    const char* userProfile = getenv("USERPROFILE");  // try windows first
    if (userProfile == nullptr) {
        userProfile = getenv("HOME");  // next tries linux/mac
    }

  
    string home(userProfile);
    string imagecraftfolder = home + "\\Desktop\\ImageCraft-Minahil-Fatima-main\\ImageCraft\\User's photos\\";
    string savefilepath= imagecraftfolder + "progress.txt";
    mysave.points = points;
    mysave.currentlevel = level;
    ofstream savefile(savefilepath);
  
    savefile << mysave.playername << endl;
    savefile << mysave.currentlevel << endl;
    savefile << mysave.points << endl;
    savefile.close();
    cout << "Game saved successfully!" << endl;

}
bool loadgame() {
    
    const char* userProfile = getenv("USERPROFILE");  // try windows first
    if (userProfile == nullptr) {
        userProfile = getenv("HOME");  // next tries linux/mac
    }


    string home(userProfile);
    string imagecraftfolder = home + "\\Desktop\\ImageCraft-Minahil-Fatima-main\\ImageCraft\\User's photos\\";
    string savefilepath = imagecraftfolder + "progress.txt";
    ifstream loadfile(savefilepath);

    if (!loadfile) {
        cout << "No save file found in ImageCraft folder." << endl;
        cout << "Location checked: " << savefilepath<< endl;
        return false;
    }
    else{
        getline(loadfile, mysave.playername);
        loadfile >> mysave.currentlevel;
        loadfile >> mysave.points;
        loadfile.close();
        points = mysave.points;
        level = mysave.currentlevel;
        return true;
    }
 
}
  

// Function Definitions 
void applyGreenFilter(vector<unsigned char>& pixels) {
    for (size_t i = 0; i < pixels.size(); i += 3) {
        int newGreen = pixels[i + 1] + 50;
        if (newGreen > 255) newGreen = 255;
        else if (newGreen < 0) newGreen = 0;
        pixels[i + 1] = static_cast<unsigned char>(newGreen);
    }
}
void applyRedFilter(vector<unsigned char>& pixels) {
    for (size_t i = 0; i < pixels.size(); i += 3) {
        int newRed = pixels[i] + 100;
        if (newRed > 255) newRed = 255;
        else if (newRed < 0) newRed = 0;
        pixels[i] = static_cast<unsigned char>(newRed);
    }
}

void applyBlueFilter(vector<unsigned char>& pixels) {
    for (size_t i = 0; i < pixels.size(); i += 3) {
        int newBlue = pixels[i + 2] + 100;
        if (newBlue > 255) newBlue = 255;
        else if (newBlue < 0) newBlue = 0;
        pixels[i + 2] = static_cast<unsigned char>(newBlue);
    }
}

void applyCustomFilter(vector<unsigned char>& pixels, int addR, int addG, int addB) {
    for (size_t i = 0; i < pixels.size(); i += 3) {
        int newRed = pixels[i] + addR;
        int newGreen = pixels[i + 1] + addG;
        int newBlue = pixels[i + 2] + addB;

        // limit values
        if (newRed > 255) newRed = 255;
        else if (newRed < 0) newRed = 0;

        if (newGreen > 255) newGreen = 255;
        else if (newGreen < 0) newGreen = 0;

        if (newBlue > 255) newBlue = 255;
        else if (newBlue < 0) newBlue = 0;

        pixels[i] = static_cast<unsigned char>(newRed);
        pixels[i + 1] = static_cast<unsigned char>(newGreen);
        pixels[i + 2] = static_cast<unsigned char>(newBlue);
    }
}
void gaussianBlurPass(vector<unsigned char>& pixels, int w, int h) {
    float kernel[5][5] = {
        { 1 / 273.0f,  4 / 273.0f,  7 / 273.0f,  4 / 273.0f,  1 / 273.0f },
        { 4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f,  4 / 273.0f },
        { 7 / 273.0f, 26 / 273.0f, 41 / 273.0f, 26 / 273.0f,  7 / 273.0f },
        { 4 / 273.0f, 16 / 273.0f, 26 / 273.0f, 16 / 273.0f,  4 / 273.0f },
        { 1 / 273.0f,  4 / 273.0f,  7 / 273.0f,  4 / 273.0f,  1 / 273.0f }
    };

    vector<unsigned char> out(pixels.size());

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float rSum = 0, gSum = 0, bSum = 0;
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    int nx = x + kx;
                    int ny = y + ky;

                    if (nx < 0 || nx >= w || ny < 0 || ny >= h)
                        continue;

                    float weight = kernel[ky + 2][kx + 2];
                    int idx = (ny * w + nx) * 3;

                    rSum += pixels[idx] * weight;
                    gSum += pixels[idx + 1] * weight;
                    bSum += pixels[idx + 2] * weight;
                }
            }

            int outIndex = (y * w + x) * 3;
            out[outIndex] = (unsigned char)rSum;
            out[outIndex + 1] = (unsigned char)gSum;
            out[outIndex + 2] = (unsigned char)bSum;
        }
    }

    pixels = out;
}
void sharpen(vector<unsigned char>& pixels, int w, int h) {
    vector<unsigned char> out(pixels.size());

    int kernel[3][3] = {
        { 0, -1,  0 },
        {-1,  5, -1 },
        { 0, -1,  0 }
    };

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            float r = 0, g = 0, b = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int nx = x + kx;
                    int ny = y + ky;

                    int idx = (ny * w + nx) * 3;
                    int weight = kernel[ky + 1][kx + 1];

                    r += pixels[idx] * weight;
                    g += pixels[idx + 1] * weight;
                    b += pixels[idx + 2] * weight;
                }
            }

            int outIdx = (y * w + x) * 3;
            int rInt = (int)r;
            int gInt = (int)g;
            int bInt = (int)b;

            if (rInt > 255) rInt = 255; else if (rInt < 0) rInt = 0;
            if (gInt > 255) gInt = 255; else if (gInt < 0) gInt = 0;
            if (bInt > 255) bInt = 255; else if (bInt < 0) bInt = 0;

            out[outIdx] = (unsigned char)rInt;
            out[outIdx + 1] = (unsigned char)gInt;
            out[outIdx + 2] = (unsigned char)bInt;
        }
    }

    pixels = out;
}
void downscale(vector<unsigned char>& pixels, int w, int h, int scale,
    vector<unsigned char>& small, int& smallW, int& smallH) {

    smallW = w / scale;
    smallH = h / scale;
    small.resize(smallW * smallH * 3);

    for (int y = 0; y < smallH; y++) {
        for (int x = 0; x < smallW; x++) {
            int srcX = x * scale;
            int srcY = y * scale;

            if (srcX >= w) srcX = w - 1;
            if (srcY >= h) srcY = h - 1;
            int srcIdx = (srcY * w + srcX) * 3;
            int dstIdx = (y * smallW + x) * 3;

            small[dstIdx] = pixels[srcIdx];
            small[dstIdx + 1] = pixels[srcIdx + 1];
            small[dstIdx + 2] = pixels[srcIdx + 2];
        }
    }
}
void upscale(const vector<unsigned char>& small, int smallW, int smallH,
    int scale, vector<unsigned char>& big, int origW, int origH) {

    big.resize(origW * origH * 3);

    for (int y = 0; y < origH; y++) {
        for (int x = 0; x < origW; x++) {
            int srcX = x / scale;
            int srcY = y / scale;

            if (srcX >= smallW) srcX = smallW - 1;
            if (srcY >= smallH) srcY = smallH - 1;
            int srcIdx = (srcY * smallW + srcX) * 3;
            int dstIdx = (y * origW + x) * 3;

            big[dstIdx] = small[srcIdx];
            big[dstIdx + 1] = small[srcIdx + 1];
            big[dstIdx + 2] = small[srcIdx + 2];
        }
    }
}


void display(int level) {
    if (level == 1) {
        cout << "\033[94m1. Blue Filter\033[0m" << endl;
        cout << "\033[31m2. Red Filter\033[0m" << endl;
        cout << "\033[32m3. Green Filter\033[0m" << endl;
    }
    else if (level == 2) {
        cout << "\033[94m1. Blue Filter\033[0m" << endl;
        cout << "\033[31m2. Red Filter\033[0m" << endl;
        cout << "\033[32m3. Green Filter\033[0m" << endl;
        cout << "\033[35m4. Custom RGB Filter\033[0m" << endl;
    }
    else if (level == 3) {
        cout << "\033[94m1. Blue Filter\033[0m" << endl;
        cout << "\033[31m2. Red Filter\033[0m" << endl;
        cout << "\033[32m3. Green Filter\033[0m" << endl;
        cout << "\033[35m4. Custom RGB Filter\033[0m" << endl;
        cout << "\033[36m5. Blur Filter\033[0m" << endl;
    }
    else if (level == 4) {
        cout << "\\033[94m1. Blue Filter\033[0m" << endl;
        cout << "\033[31m2. Red Filter\033[0m" << endl;
        cout << "\033[32m3. Green Filter\033[0m" << endl;
        cout << "\033[35m4. Custom RGB Filter\033[0m" << endl;
        cout << "\033[36m5. Blur Filter\033[0m" << endl;
        cout << "\033[33m6. Sharpen Filter\033[0m" << endl;
    }
    else if (level == 5) {
        cout << "\033[94m1. Blue Filter\033[0m" << endl;
        cout << "\033[31m2. Red Filter\033[0m" << endl;
        cout << "\033[32m3. Green Filter\033[0m" << endl;
        cout << "\033[35m4. Custom RGB Filter\033[0m" << endl;
        cout << "\033[36m5. Blur Filter\033[0m" << endl;
        cout << "\033[33m6. Sharpen Filter\033[0m" << endl;
        cout << "\033[91m7. Guess the filter!\033[0m" << endl;
    }
}
bool validFilter(int level, int choice) {
    // Check level-based restrictions
    if (level == 1 && choice > 3) {
        cout << "\033[31mThis filter is locked! Complete Level 1 to unlock more filters.\033[0m" << endl;
        return false;
    }
    else if (level == 2 && choice > 4) {
        cout << "\033[31mThis filter is locked! Reach Level 3 to unlock Blur filter.\033[0m" << endl;
        return false;
    }
    else if (level == 3 && choice > 5) {
        cout << "\033[31mThis filter is locked! Reach Level 4 to unlock Sharpen filter.\033[0m" << endl;
        return false;
    }
    else if (level == 4 && choice > 6) {
        cout << "\033[31mThis filter is locked! Reach Level 5 to unlock mystery filter.\033[0m" << endl;
        return false;
    }
    cout << "\033[3mProcessing....This may take a while.\033[0m" << endl;
    return true;
}

// ----------------------------- MAIN ---------------------
int main() {
    srand(time(NULL));

    if (loadgame()) {
        cout << "\n";
        cout << "\033[36m================================================================================\033[0m" << endl;
        cout << "\033[36m|                                                                              |\033[0m" << endl;
        cout << "\033[36m|                        \033[1m\033[96mWELCOME BACK TO IMAGECRAFT\033[0m\033[36m                            |\033[0m" << endl;
        cout << "\033[36m|                                                                              |\033[0m" << endl;
        cout << "\033[36m================================================================================\033[0m" << endl;
        cout << "\033[36m|                                                                              |\033[0m" << endl;
        cout << "\033[36m|                              \033[92mPlayer:\033[0m        \033[1m\033[32m" << mysave.playername << "\033[0m\033[36m                          |\033[0m" << endl;
        cout << "\033[36m|                              \033[92mLevel:\033[0m         \033[1m\033[32m" << mysave.currentlevel << "\033[0m\033[36m                                |\033[0m" << endl;
        cout << "\033[36m|                              \033[92mTotal Points:\033[0m  \033[1m\033[32m" << mysave.points << "\033[0m\033[36m                              |\033[0m" << endl;
        cout << "\033[36m|                                                                              |\033[0m" << endl;
        cout << "\033[36m================================================================================\033[0m" << endl;
        cout << "\n";


    }
    else {
        cout << "\033[31mNew player detected!\033[0m" << endl;
        cout << "\033[95mPlease enter your name : \033[0m" << endl;
        getline(cin, mysave.playername);
        mysave.currentlevel = 1;
        mysave.points = 0;
        cout << "\033[93mWelcome to ImageCraft " << mysave.playername << "!\033[0m" << endl;
    }


    // Do not touch -> this is to get user's profile
    const char* userProfile = getenv("USERPROFILE");  // try windows first
    if (userProfile == nullptr) {
        userProfile = getenv("HOME");  // next tries linux/mac
    }

    if (userProfile == nullptr) {
        cout << "ERROR: Could not detect home directory." << endl;
        return 1; //not found in both windows or linux/mac
    }
    string home(userProfile);
    string userFolder = home + "\\Desktop\\ImageCraft-Minahil-Fatima-main\\ImageCraft\\User's photos\\";
    string filteredFolder = home + "\\Desktop\\ImageCraft-Minahil-Fatima-main\\ImageCraft\\Filtered photos\\";

    while (level <= 5) {
        cout << "\nWhat would you like to do?" << endl;
        cout << "\033[36m1. Process an image\033[0m" << endl;
        cout << "\033[31m2. Exit\033[0m" << endl;
        cout << "\033[33m3. Save Progress\033[0m" << endl;
        int option;
        cin >> option;
        if (option == 2)
            break; // the while loop is exited
        else if (option == 3) {
            mysave.currentlevel = level;
            mysave.points = points;
            savegame();
            continue;
      
        }



        string inFileName, outFileName;
        cout << "\033[91mEnter INPUT filename (example: input.ppm): \033[0m";
        cin >> inFileName;
        cout << "\033[91mEnter OUTPUT filename (example: result.ppm): \033[0m";
        cin >> outFileName;

        string inputPath = userFolder + inFileName;
        string outputPath = filteredFolder + outFileName;

        // OPEN AND VALIDATE IMAGE FIRST
        ifstream image(inputPath, ios::binary);
        if (!image) {
            cout << "\033[31mERROR: Cannot open file: \033[0m" << inputPath << endl;
            cout << "\033[31mMake sure the file exists in the 'User's photos' folder.\033[0m" << endl;
            return 1;
        }

        // READ IMAGE DATA
        string type, width, height, RGB;
        image >> type >> width >> height >> RGB;
        image.get();

        int w = stoi(width);
        int h = stoi(height);

        vector<unsigned char> pixels(w * h * 3);
        image.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
        image.close();

        // NOW GET USER CHOICE
        cout << "Choose a filter:\033[" << endl;
        display(level);

        int choice;
        cin >> choice;
        // Variables for custom filter
        int addR = 0, addG = 0, addB = 0;

        bool allowed = validFilter(level, choice);
        if (!allowed)
            continue;

        // ** APPLY FILTER 
        switch (choice) {
        case 1:
            applyBlueFilter(pixels);
            break;
        case 2:
            applyRedFilter(pixels);
            break;
        case 3:
            applyGreenFilter(pixels);
            break;
        case 4: {
            cout << "\033[35m--Custom Filter Selected--\033[0m" << endl;
            cout << "\033[31mEnter Red Adjustment(-255 to 255): \033[0m";
            cin >> addR;
            cout << "\033[92mEnter Green Adjustment(-255 to 255): \033[0m";
            cin >> addG;
            cout << "\033[94mEnter Blue Adjustment(-255 to 255): \033[0m";
            cin >> addB;
            applyCustomFilter(pixels, addR, addG, addB);
        }
              break;
        case 5:
        {
            int scale = 10;
            int origW = w;
            int origH = h;

            vector<unsigned char> small;
            int smallW, smallH;

            downscale(pixels, origW, origH, scale, small, smallW, smallH);

            for (int i = 0; i < 5; i++)
                gaussianBlurPass(small, smallW, smallH);

            vector<unsigned char> big;
            upscale(small, smallW, smallH, scale, big, origW, origH);

            pixels = big;
            w = origW;
            h = origH;
        }
        break;
        case 6:
            sharpen(pixels, w, h);
            break;
        case 7: {
            cout << "033[91mGuess the filter!033[0m" << endl;
            int filter = (rand() % 5) + 1;
            void (*fxnpntr1)(vector <unsigned char>&);
            void (*fxnpntr2)(vector <unsigned char>&, int, int);
            if (filter == 1) {
                fxnpntr1 = applyBlueFilter;
                fxnpntr1(pixels);
            }
            if (filter == 2) {
                fxnpntr1 = applyRedFilter;
                fxnpntr1(pixels);
            }
            if (filter == 3) {
                fxnpntr1 = applyGreenFilter;
                fxnpntr1(pixels);
            }
            if (filter == 4) {
                fxnpntr2 = gaussianBlurPass;
                fxnpntr2(pixels, w, h);
            }
            if (filter == 5) {
                fxnpntr2 = sharpen;
                fxnpntr2(pixels, w, h);
                break;
            }
        }
        default:
            cout << "Invalid choice!" << endl;
            continue;
        }
        // SAVE OUTPUT
        ofstream newimage(outputPath, ios::binary);
        newimage << "P6\n" << w << " " << h << "\n255\n";
        newimage.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
        newimage.close();
        cout << "Processed successfully.\nSaved to: " << outputPath << endl;
        if (choice == 1)
            askBlue();
        if (choice == 2)
            askRed();
        if (choice == 3)
            askGreen();
        if (choice == 4)
            askCustom();
        if (choice == 5)
            askBlur();
        if (choice == 6)
            askSharp();
        if (choice == 7)
            askRandom();
    }

    cout << "\033[95mThank you for playing ImageCraft ~ **** \033[0m" << endl;
    cout << "\033[95m==================== *WE HOPE YOU ENJOYED THE EXPERIENCE* =================\n\033[0m";
    return 0;
}

void askGreen() {
    int qchoice;
    cout << "\033[91mWhich RGB value is increased in GREEN filter?\033[0m" << endl;
    cout << "1. R\n2. G\n3. B\n";
    cin >> qchoice;
    switch (qchoice) {
    case 1:
        cout << "\033[31mWrong Answer.\033[0m" << endl;
        break;
    case 2:
        cout << "\033[92mCorrect!\n\033[0m";
        points += 10;
        updateLevel();
        break;
    case 3:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    default:
        cout << "\033[31mInvalid answer.\033[0m" << endl;
        break;
    }
}
int updateLevel() {
    int oldlevel = level;
    for (int i = 5; i >= 1; i--) {
        if (points >= levels[i - 1]) {
            level = i;
            cout << "\033[95mLEVEL: \033[0m" << level;
            break;
        }
    }
    if (level != oldlevel) {
        mysave.currentlevel=level;
}
    mysave.points = points;
    return level;
}
void askRed() {
    int qchoice;
    cout << "\033[91mA RED filter emphasizes which color wavelengths?\033[0m" << endl;
    cout << "1. Short Wavelengths (blue/violet)\n2. Medium wavelengths (green/yellow)\n3. Long wavelengths (red/orange)\n";
    cin >> qchoice;
    switch (qchoice) {
    case 1:
        cout << "\033[31mWrong Answer.\033[0m" << endl;
        break;
    case 2:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    case 3:
        cout << "\033[92mCorrect!\n\033[0m";
        points += 10;
        mysave.points = points;
        updateLevel();
        break;
    default:
        cout << "\033[31mInvalid answer.\033[0m" << endl;
        break;
    }
}
void askBlue() {
    int qchoice;
    cout << "\033[91mWhat does increasing the BLUE concentration do to an image?\033[0m" << endl;
    cout << "1. Makes the image warmer\n";
    cout << "2. Makes the image cooler / blue-tinted\n";
    cout << "3. Makes the image darker\n";
    cin >> qchoice;

    switch (qchoice) {
    case 1:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    case 2:
        cout << "\033[92mCorrect!\n\033[0m";
        points += 10;
        mysave.points = points;
        updateLevel();
        break;
    case 3:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    default:
        cout << "\033[31mInvalid answer.\n\033[0m";
        break;
    }
}
void askCustom() {
    int qchoice;
    cout << "\033[91mIn the RGB color model, what happens when you add equal values to R, G, and B?\033[0m" << endl;
    cout << "1. Changes image hue but not brightness\n";
    cout << "2. Makes image uniformly brighter or darker\n";
    cout << "3. Increases image contrast\n";
    cin >> qchoice;

    switch (qchoice) {
    case 1:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    case 2:
        cout << "\033[92mCorrect!\n\033[0m";
        points += 30;
        mysave.points = points;
        updateLevel();
        break;
    case 3:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    default:
        cout << "\033[31mInvalid answer.\n\033[0m";
        break;
    }
}

void askBlur() {
    int qchoice;
    cout << "\033[91mWhat does a Gaussian blur do to an image?\033[0m" << endl;
    cout << "1. Sharpens edges\n";
    cout << "2. Smooths the image and reduces detail\n";
    cout << "3. Increases brightness\n";
    cin >> qchoice;

    switch (qchoice) {
    case 1:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    case 2:
        cout << "\033[92mCorrect!\n\033[0m";
        points += 30;
        mysave.points = points;
        updateLevel();
        break;
    case 3:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    default:
        cout << "\033[31mInvalid answer.\n\033[0m";
        break;
    }
}
void askSharp() {
    int qchoice;
    cout << "\033[91mWhat does a sharpen filter actually do to pixels?\033[0m" << endl;
    cout << "1. Blends neighboring pixels together\n";
    cout << "2. Increases contrast at edges\n";
    cout << "3. Converts the image to grayscale\n";
    cin >> qchoice;

    switch (qchoice) {
    case 1:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    case 2:
        cout << "\033[92mCorrect!\n\033[0m";
        points += 30;
        mysave.points = points;
        updateLevel();
        break;
    case 3:
        cout << "\033[31mWrong answer.\n\033[0m";
        break;
    default:
        cout << "\033[31mInvalid answer.\n\033[0m";
        break;
    }
}
void askRandom() {
    int qchoice;
    cout << "\033[91mWhich filter do you think was randomly applied?\033[0m" << endl;
    cout << "1. Blur\n";
    cout << "2. Sharpen\n";
    cout << "3. Color adjustment (Red/Green/Blue)\n";
    cin >> qchoice;

    if (qchoice >= 1 && qchoice <= 3) {
        cout << "\033[96mInteresting choice! The goal was to observe visual changes.\n\033[0m";
        points += 30;
        mysave.points = points;
        updateLevel();
    }
    else
        cout << "\033[31mInvalid answer.\n\033[0m";
}