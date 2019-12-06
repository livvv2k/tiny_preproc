// Copyright (C) 2019 smiley3 <ml.smiley3@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <string>
#include <fstream>
#include <streambuf>
#include <map>

// ASCII codes allowed in var names
bool IS_ASCII_LETTER(char c)
{
    // underscore and numbers
    if (c == 95 || (c < 58 && c > 47))
        return true;
    if (c > 90)
        c -= 32;
    if (c < 65 || c > 90)
        return false;
    return true;
}

std::string openFile(const std::string& fileName)
{
    std::ifstream ifs(fileName);
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    return content;
}

class CTinyPreprocessor
{
public:
    std::string Preprocess(const std::string& code);
    void AddDefines(const std::string& macroName, const std::string& macroValue);
private:
    std::map<std::string, std::string> defines;
    bool ExpandMacros(const std::string& word, std::string& out);
};
