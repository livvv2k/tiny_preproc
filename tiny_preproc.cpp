// Copyright (C) 2019 smiley3 <ml.smiley3@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "tiny_pp.h"

#include <iostream>
#include <sstream>
#include <vector>

void CTinyPreprocessor::AddDefines(const std::string& macroName, const std::string& macroValue)
{
    defines.emplace(macroName, macroValue);
}

bool CTinyPreprocessor::ExpandMacros(const std::string& word, std::string& out)
{
    std::vector<std::string> words;
    words.push_back(word);
    for (const auto& kv : defines)
    {
        size_t startPos = word.find(kv.first);
        if (startPos != std::string::npos && !IS_ASCII_LETTER(word[startPos - 1]) && !IS_ASCII_LETTER(word[startPos + kv.first.size()]))
        {
            words.clear();
            words.push_back(word.substr(0, startPos));
            words.push_back(kv.second);
            words.push_back(word.substr(startPos + kv.first.size()));
            break;
        }
    }
    
    for (const std::string& str : words)
        out.append(str);

    if (out == word)
        return false;
    return true;
}

std::string CTinyPreprocessor::Preprocess(const std::string& code)
{
    std::string out;

    std::string temp;
    std::vector<std::string> lines;
    std::istringstream ss(code);
    while (std::getline(ss, temp))
        lines.push_back(temp);

    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::string line = lines[i];

        if (line[0] != '#')
        {
            std::string newCode;
            ExpandMacros(line, newCode);
            while (true)
            {
                std::string nnCode;
                if (!ExpandMacros(newCode, nnCode))
                    break;
                newCode = nnCode;
            }
            out.append(newCode);
            out.append("\n");
            continue;
        }
        
        std::string directiveName = line.substr(0, line.find(' '));

        if (directiveName == "#include")
        {
            size_t startPos = line.find("\"") + 1;
            size_t endPos = line.find("\"", startPos);
            std::string fileName = line.substr(startPos, endPos - startPos);
            std::string newCode = Preprocess(openFile(fileName));
            out.append(newCode);
            out.append("\n");
        }
        else if (directiveName == "#define")
        {
            size_t startPos = line.find(" ");
            size_t endPos = line.find(" ", startPos + 1);
            std::string macroName = line.substr(startPos + 1, endPos - startPos - 1);
            std::string macroValue = line.substr(endPos + 1);
            AddDefines(macroName, macroValue);
        }
        else if (directiveName == "#ifdef")
        {
            bool notDef = true;
            std::string defName = line.substr(7);
            std::map<std::string, std::string>::iterator it = defines.find(defName);
            notDef = it == defines.end();
            if (notDef)
            {
                size_t encounteredIfs = 1; // To find the right #endif
                for (size_t j = i + 1; j < lines.size(); ++j)
                {
                    std::string newline = lines[j];
                    std::string subDirective = newline.substr(0, newline.find(' '));
                    if (subDirective == "#if" || subDirective == "#ifndef")
                    {
                        encounteredIfs++;
                    }

                    if (subDirective == "#elif")
                    {
                        for (size_t k = j + 1; k < lines.size(); ++k)
                        {
                            if (lines[k].substr(0, lines[k].find(' ')) == "#endif")
                            {
                                j = k - 1;
                                break;
                            }
                            out.append(lines[k]);
                            out.append("\n");
                        }
                    }

                    if (subDirective == "#endif")
                    {
                        encounteredIfs--;

                        if (encounteredIfs == 0)
                        {
                            i = j;
                            break;
                        }
                    }
                }
            }
            else
                continue;
        }
        else if (directiveName == "#endif")
        {
            continue; // Should absolutely not happen
        }
    }

    return out;
}

int main()
{
    CTinyPreprocessor tpp;
    tpp.AddDefines("TEST_MACRO_2", "\"test2\"");
    tpp.AddDefines("TEST_MACRO", "\"test\"");
    std::string str = openFile("file.cpp");
    std::cout << tpp.Preprocess(str);
    return 0;
}
