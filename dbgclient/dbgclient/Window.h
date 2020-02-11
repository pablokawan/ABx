/**
 * Copyright 2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <string>
#include <functional>
#include <AB/CommonConfig.h>
#ifdef AB_WINDOWS
#include <Windows.h>
#endif

struct Point
{
    int x;
    int y;
};

class Window
{
public:
    enum ForeColor : short
    {
        // Foregrounds
#if defined(AB_UNIX)
        FG_BLACK = 30,
        FG_RED = 31,
        FG_GREEN = 32,
        FG_YELLOW = 33,
        FG_BLUE = 34,
        FG_MAGENTA = 35,
        FG_CYAN = 36,
        FG_LIGHTGREY = 37,
        FG_DEFAULT = 39,
        FG_DARKGREY = 90,
        FG_LIGHTRED = 91,
        FG_LIGHTGREEN = 92,
        FG_LIGHTYELLOW = 93,
        FG_LIGHTBLUE = 94,
        FG_LIGHTMAGENTA = 95,
        FG_LIGHTCYAN = 96,
        FG_WHITE = 97,
#elif defined(AB_WINDOWS)
        FG_BLACK = 0,
        FG_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        FG_RED = FOREGROUND_RED,
        FG_GREEN = FOREGROUND_GREEN,
        FG_YELLOW = FOREGROUND_GREEN | FOREGROUND_RED,
        FG_BLUE = FOREGROUND_BLUE,
        FG_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
        FG_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
#endif
    };
    enum BackColor : short
    {
        // Backgrounds
#if defined(AB_UNIX)
        BG_BLACK = 40,
        BG_RED = 41,
        BG_GREEN = 42,
        BG_YELLOW = 43,
        BG_BLUE = 44,
        BG_MAGENTA = 45,
        BG_CYAN = 46,
        BG_LIGHTGREY = 47,
        BG_DEFAULT = 49,
        BG_DARKGREY = 100,
        BG_LIGHTRED = 101,
        BG_LIGHTGREEN = 102,
        BG_LIGHTYELLOW = 103,
        BG_LIGHTBLUE = 104,
        BG_LIGHTMAGENTA = 105,
        BG_LIGHTCYAN = 106,
        BG_WHITE = 107,
#elif defined(AB_WINDOWS)
        BG_BLACK = 0,
        BG_RED = BACKGROUND_RED,
        BG_GREEN = BACKGROUND_GREEN,
        BG_YELLOW = BACKGROUND_GREEN | BACKGROUND_RED,
        BG_BLUE = BACKGROUND_BLUE,
        BG_MAGENTA = BACKGROUND_RED | BACKGROUND_BLUE,
        BG_CYAN = BACKGROUND_GREEN | BACKGROUND_BLUE,
        BG_WHITE = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
#endif
    };
private:
    bool running_{ false };
    Point pos_{ 0, 0 };
    ForeColor defForeColor_{ FG_WHITE };
    BackColor defBackColor_{ BG_BLACK };

    void HandleInput(char c);
public:
    Window();
    ~Window();
    char GetChar() const;
    void Loop();
    void Print(const Point& pos, const std::string& text);
    void Goto(const Point& pos);
    void Clear();
    void ShowCursor(bool visible);
    Point GetPosition() const;
    bool IsRunning() const { return running_; }
    void SetColor(ForeColor foreColor, BackColor backColor);
    void RestColor();
    ForeColor DefaultForeColor() const { return defForeColor_; }
    BackColor DefaultBackColor() const { return defBackColor_; }

    std::function<void(char& c)> onKey_;
};

