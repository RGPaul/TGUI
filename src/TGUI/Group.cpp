/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group(const Group& groupToCopy) :
    globalFont(groupToCopy.globalFont)
    {
        // Copy all the objects
        for (unsigned int i=0; i<groupToCopy.m_EventManager.m_Objects.size(); ++i)
        {
            OBJECT* newObject = static_cast<OBJECT*>(groupToCopy.m_EventManager.m_Objects[i]->clone());
            m_EventManager.m_Objects.push_back(newObject);
            m_ObjName.push_back(groupToCopy.m_ObjName[i]);

            newObject->m_Parent = this;
            newObject->initialize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::~Group()
    {
        removeAllObjects();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group& Group::operator= (const Group& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            // Copy the font
            globalFont = right.globalFont;

            // Remove all the old objects
            removeAllObjects();

            // Copy all the objects
            for (unsigned int i=0; i<right.m_EventManager.m_Objects.size(); ++i)
            {
                OBJECT* newObject = static_cast<OBJECT*>(right.m_EventManager.m_Objects[i]->clone());
                m_EventManager.m_Objects.push_back(newObject);
                m_ObjName.push_back(right.m_ObjName[i]);

                newObject->m_Parent = this;
                newObject->initialize();
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Group::loadObjectsFromFile(const std::string filename)
    {
        // I wrote these defines to avoid having the same code over and over again
        #define CHECK_SHARED_PROPERTIES(name) \
            if (line.substr(0, 5).compare("size=") == 0) \
            { \
                Vector2f size; \
                if (extractVector2f(line.erase(0, 5), size) == false) \
                    goto LoadingFailed; \
              \
                name->setSize(size.x, size.y); \
            } \
            else if (line.substr(0, 6).compare("width=") == 0) \
            { \
                line.erase(0, 6); \
                name->setSize(static_cast<float>(atoi(line.c_str())), name->getScaledSize().y); \
            } \
            else if (line.substr(0, 7).compare("height=") == 0) \
            { \
                line.erase(0, 7); \
                name->setSize(name->getScaledSize().x, static_cast<float>(atoi(line.c_str()))); \
            } \
            else if (line.substr(0, 6).compare("scale=") == 0) \
            { \
                line.erase(0, 6); \
                Vector2f objScale; \
                if (extractVector2f(line, objScale)) \
                    name->setScale(objScale); \
                else \
                    goto LoadingFailed; \
            } \
            else if (line.substr(0, 9).compare("position=") == 0) \
            { \
                line.erase(0, 9); \
                Vector2f position; \
                if (extractVector2f(line, position)) \
                    name->setPosition(position); \
                else \
                    goto LoadingFailed; \
            } \
            else if (line.substr(0, 5).compare("left=") == 0) \
            { \
                line.erase(0, 5); \
                name->setPosition(static_cast<float>(atoi(line.c_str())), name->getPosition().y); \
            } \
            else if (line.substr(0, 4).compare("top=") == 0) \
            { \
                line.erase(0, 4); \
                name->setPosition(name->getPosition().x, static_cast<float>(atoi(line.c_str()))); \
            } \
            else if (line.substr(0, 11).compare("callbackid=") == 0) \
            { \
                line.erase(0, 11); \
                name->callbackID = atoi(line.c_str()); \
            }

        #define CHECK_FOR_QUOTES \
            if (line.empty() == true) \
                goto LoadingFailed; \
             \
            if ((line[0] == '"') && (line[line.length()-1] == '"')) \
            { \
                line.erase(0, 1); \
                line.erase(line.length()-1, 1); \
            } \
            else \
                goto LoadingFailed;

        #define CHECK_BOOL(boolean) \
            if (line.compare("true") == 0) \
                boolean = true; \
            else if (line.compare("false") == 0) \
                boolean = false; \
            else \
            { \
                if (atoi(line.c_str())) \
                    boolean = true; \
                else \
                    boolean = false; \
            }

         #define COMPARE_OBJECT(length, name, objectName, id) \
            if (line.substr(0, length).compare(name) == 0) \
            { \
                line.erase(0, length); \
              \
                if (line.empty() == false) \
                { \
                    CHECK_FOR_QUOTES \
                } \
              \
                extraPtr = static_cast<void*>(parentPtr.top()->add<objectName>(line)); \
                objectID = id + 1; \
                progress.push(0); \
            }

        #define START_LOADING_OBJECT \
            if (progress.top() == 0) \
            { \
                if (line.compare("{") == 0) \
                { \
                    progress.pop(); \
                    progress.push(1); \
                    break; \
                } \
                else \
                    goto LoadingFailed; \
            } \
            else \
            { \
                if (line.compare("}") == 0) \
                { \
                    objectID = parentID.top(); \
                    parentID.pop(); \
                    parentPtr.pop(); \
                    progress.pop(); \
                    break; \
                } \
            }

        // During the process some variables are needed to store what exactly was going on.
        std::stack<Group*> parentPtr;
        std::stack<unsigned int> parentID;
        std::stack<unsigned int> progress;
        unsigned int objectID = 0;
        void* extraPtr = NULL;
        bool multilineComment = false;

        std::vector<std::string> defineTokens;
        std::vector<std::string> defineValues;

        // Create a file object
        std::ifstream m_File;

        // Open the file
        m_File.open(filename.c_str(), std::ifstream::in);

        // Check if the file was not opened
        if (m_File.is_open() == false)
            return false;

        // Stop reading when we reach the end of the file
        while (!m_File.eof())
        {
            // Get the next line
            std::string line;
            std::getline(m_File, line);

            // Check if there is a multiline comment
            if (multilineComment)
            {
                // Search for an asterisk
                std::string::size_type commentPos = line.find('*');

                // Check if there is an asterisk
                if (commentPos != std::string::npos)
                {
                    // Make sure the asterisk is not the last character on the line
                    if (line.length() > commentPos + 1)
                    {
                        // Check if the next character is a slash
                        if (line[commentPos+1] == '/')
                        {
                            // Erase the first part of the line
                            line.erase(0, commentPos + 2);

                            // The multiline comment has been processed
                            multilineComment = false;

                            // Continue like normal
                            goto multilineCommentProcessed;
                        }
                        else // There is no end of the comment in this line
                            line = "";
                    }
                    else // There is no end of the comment in this line
                        line = "";
                }
                else // There is no end of the comment in this line
                    line = "";
            }
            else // There is no multiline comment
            {
              multilineCommentProcessed:

                // Search for a quote
                std::string::size_type quotePos1 = line.find('"');

                // Check if the quote was found or not
                if (quotePos1 == std::string::npos)
                {
                    // Remove all spaces and tabs from the whole line
                    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
                    line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
                    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

                    // Search for comments
                    std::string::size_type commentPos = line.find('/');

                    // Check if a slash was found
                    if (commentPos != std::string::npos)
                    {
                        // Make sure the slash is not the last character on the line
                        if (line.length() > commentPos + 1)
                        {
                            // Erase the comment (if there is one)
                            if (line[commentPos+1] == '/')
                                line.erase(commentPos);
                            else if (line[commentPos+1] == '*')
                            {
                                // Remove the rest of the line
                                line.erase(commentPos);

                                // From now on, everything is comment
                                multilineComment = true;
                            }
                            else // There is a slash in the middle of nowhere. It shouldn't be there.
                                goto LoadingFailed;
                        }
                        else // There is a slash on the end of the line. It shouldn't be there.
                            goto LoadingFailed;
                    }

                    // Convert the whole line to lowercase
                    for (unsigned int i = 0; i < line.length(); i++)
                    {
                        if ((line[i] > 64) && (line[i] < 91))
                            line[i] += 32;
                    }
                }
                else
                {
                    // Only remove spaces until the quote
                    line.erase(std::remove(line.begin(), line.begin() + quotePos1, ' '), line.begin() + quotePos1);

                    // Search for the quote again, because the position might have changed
                    quotePos1 = line.find('"');

                    // Only remove tabs until the quote
                    line.erase(std::remove(line.begin(), line.begin() + quotePos1, '\t'), line.begin() + quotePos1);

                    // Search for the quote again, because the position might have changed
                    quotePos1 = line.find('"');

                    // Search for comments
                    std::string::size_type commentPos = line.substr(0, quotePos1).find('/');

                    // Check if a slash was found
                    if (commentPos != std::string::npos)
                    {
                        // Erase the comment (if there is one)
                        if (line[commentPos+1] == '/')
                            line.erase(commentPos);
                        else if (line[commentPos+1] == '*')
                        {
                            // Remove the rest of the line
                            line.erase(commentPos);

                            // From now on, everything is comment
                            multilineComment = true;
                        }
                        else // There is a slash in the middle of nowhere. It shouldn't be there.
                            goto LoadingFailed;
                    }

                    // Search for the quote again, because the position might have changed
                    quotePos1 = line.find('"');

                    // The quote might have been behind the comment
                    if (quotePos1 != std::string::npos)
                    {
                        // Convert the part before the quote to lowercase
                        for (unsigned int i = 0; i < quotePos1; i++)
                        {
                            if ((line[i] > 64) && (line[i] < 91))
                                line[i] += 32;
                        }

                        // Search for a second quote
                        std::string::size_type quotePos2 = line.find('"', quotePos1 + 1);

                        // There must always be a second quote
                        if (quotePos2 == std::string::npos)
                            goto LoadingFailed;

                        // Remove all spaces and tabs after the quote
                        line.erase(std::remove(line.begin() + quotePos2, line.end(), ' '), line.end());
                        line.erase(std::remove(line.begin() + quotePos2, line.end(), '\t'), line.end());
                        line.erase(std::remove(line.begin() + quotePos2, line.end(), '\r'), line.end());

                        // Search for comments
                        commentPos = line.find('/', quotePos2 + 1);

                        // Check if a slash was found
                        if (commentPos != std::string::npos)
                        {
                            // Make sure the slash is not the last character on the line
                            if (line.length() > commentPos + 1)
                            {
                                // Erase the comment (if there is one)
                                if (line[commentPos+1] == '/')
                                    line.erase(commentPos);
                                else if (line[commentPos+1] == '*')
                                {
                                    // Remove the rest of the line
                                    line.erase(commentPos);

                                    // From now on, everything is comment
                                    multilineComment = true;
                                }
                                else // There is a slash in the middle of nowhere. It shouldn't be there.
                                    goto LoadingFailed;
                            }
                            else // There is a slash on the end of the line. It shouldn't be there.
                                goto LoadingFailed;
                        }

                        // Search for the quote again, because the position might have changed
                        quotePos2 = line.find('"', quotePos1 + 1);

                        // Search for backslashes between the quotes
                        std::string::size_type backslashPos = line.find('\\', quotePos1);

                        // Check if a backlash was found before the second quote
                        while (backslashPos < quotePos2)
                        {
                            // Check for special characters
                            if (line[backslashPos + 1] == 'n')
                            {
                                line[backslashPos] = '\n';
                                line.erase(backslashPos + 1, 1);
                                --quotePos2;
                            }
                            else if (line[backslashPos + 1] == 't')
                            {
                                line[backslashPos] = '\t';
                                line.erase(backslashPos + 1, 1);
                                --quotePos2;
                            }
                            else if (line[backslashPos + 1] == '\\')
                            {
                                line.erase(backslashPos + 1, 1);
                                --quotePos2;
                            }
                            else if (line[backslashPos + 1] == '"')
                            {
                                line[backslashPos] = '"';
                                line.erase(backslashPos + 1, 1);

                                // Find the next quote
                                quotePos2 = line.find('"', backslashPos + 1);

                                if (quotePos2 == std::string::npos)
                                    goto LoadingFailed;
                            }

                            // Find the next backslash
                            backslashPos = line.find('\\', backslashPos + 1);
                        }

                        // There may never be more than two quotes
                        if (line.find('"', quotePos2 + 1) != std::string::npos)
                            goto LoadingFailed;

                        // Convert the part behind the quote to lowercase
                        for (unsigned int i = quotePos2; i < line.length(); i++)
                        {
                            if ((line[i] > 64) && (line[i] < 91))
                                line[i] += 32;
                        }
                    }
                }
            }

            // Only continue when the line is not empty
            if (!line.empty())
            {
                // Check if something was defined
                if (defineTokens.empty() == false)
                {
                    // Loop through all tokens
                    for (unsigned int i=0; i<defineTokens.size(); ++i)
                    {
                        // Search for every token in the line
                        std::string::size_type tokenPos = line.find(defineTokens[i]);

                        // Check if a token was found
                        if (tokenPos != std::string::npos)
                        {
                            // Replace the token with the corresponding value
                            line.erase(tokenPos, defineTokens[i].length());
                            line.insert(tokenPos, defineValues[i]);
                        }
                    }
                }

                // What happens now depends on the process
                switch (objectID)
                {
                    case 0: // Done nothing yet
                    {
                        // Check if this is the first line
                        if (progress.empty())
                        {
                            // The first line should contain 'window' or 'define'
                            if (line.substr(0, 7).compare("window:") == 0)
                            {
                                objectID = 0;
                                progress.push(1);
                            }
                            else if (line.substr(0, 7).compare("define:") == 0)
                            {
                                line.erase(0, 7);

                                // Search the equals sign
                                std::string::size_type equalsSignPos = line.find('=');
                                if (equalsSignPos != std::string::npos)
                                {
                                    // Store the define
                                    defineTokens.push_back(line.substr(0, equalsSignPos));
                                    defineValues.push_back(line.erase(0, equalsSignPos + 1));
                                }
                                else // The equals sign is missing
                                    goto LoadingFailed;
                            }
                            else // The second line is wrong
                                goto LoadingFailed;
                        }
                        else // This is the second line
                        {
                            // The second line should contain "{"
                            if (line.compare("{") == 0)
                            {
                                objectID = tgui::window + 1;
                                progress.pop();
                            }
                            else // The second line is wrong
                                goto LoadingFailed;
                        }

                        break;
                    }
                    case tgui::window + 1: // The window was found
                    {
                        // Find out if the loading is done
                        if (line.compare("}") == 0)
                            goto LoadingSucceeded;

                        // The next object will have the window as its parent
                        parentID.push(tgui::window + 1);
                        parentPtr.push(this);

                        // The line doesn't contain a '}', so check what object it contains
                        COMPARE_OBJECT(6, "panel:", Panel, tgui::panel)
                        else COMPARE_OBJECT(6, "label:", Label, tgui::label)
                        else COMPARE_OBJECT(7, "button:", Button, tgui::button)
                        else COMPARE_OBJECT(7, "slider:", Slider, tgui::slider)
                        else COMPARE_OBJECT(8, "picture:", Picture, tgui::picture)
                        else COMPARE_OBJECT(8, "listbox:", Listbox, tgui::listbox)
                        else COMPARE_OBJECT(8, "editbox:", EditBox, tgui::editBox)
                        else COMPARE_OBJECT(8, "textbox:", TextBox, tgui::textBox)
                        else COMPARE_OBJECT(9, "checkbox:", Checkbox, tgui::checkbox)
                        else COMPARE_OBJECT(9, "combobox:", ComboBox, tgui::comboBox)
                        else COMPARE_OBJECT(10, "scrollbar:", Scrollbar, tgui::scrollbar)
                        else COMPARE_OBJECT(11, "loadingbar:", LoadingBar, tgui::loadingBar)
                        else COMPARE_OBJECT(11, "spinbutton:", SpinButton, tgui::spinButton)
                        else COMPARE_OBJECT(12, "radiobutton:", RadioButton, tgui::radioButton)
                        else COMPARE_OBJECT(12, "childwindow:", ChildWindow, tgui::childWindow)
                        else COMPARE_OBJECT(12, "spritesheet:", SpriteSheet, tgui::spriteSheet)
                        else COMPARE_OBJECT(15, "animatedbutton:", AnimatedButton, tgui::animatedButton)
                        else COMPARE_OBJECT(16, "animatedpicture:", AnimatedPicture, tgui::animatedPicture)
                        else // The line was wrong
                            goto LoadingFailed;

                        break;
                    }
                    case tgui::panel + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the panel back
                        tgui::Panel* panel = static_cast<tgui::Panel*>(extraPtr);

                        CHECK_SHARED_PROPERTIES(panel)
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            // Change the background color (black on error)
                            panel->backgroundColor = tgui::extractColor(line.erase(0, 16));
                        }
                        else if (line.substr(0, 16).compare("backgroundimage=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 16);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the image
                            panel->setBackgroundImage(line);
                        }
                        else
                        {
                            // All newly created objects must be part of the panel
                            parentID.push(tgui::panel + 1);
                            parentPtr.push(panel);

                            COMPARE_OBJECT(6, "panel:", Panel, tgui::panel)
                            else COMPARE_OBJECT(6, "label:", Label, tgui::label)
                            else COMPARE_OBJECT(7, "button:", Button, tgui::button)
                            else COMPARE_OBJECT(7, "slider:", Slider, tgui::slider)
                            else COMPARE_OBJECT(8, "picture:", Picture, tgui::picture)
                            else COMPARE_OBJECT(8, "listbox:", Listbox, tgui::listbox)
                            else COMPARE_OBJECT(8, "editbox:", EditBox, tgui::editBox)
                            else COMPARE_OBJECT(8, "textbox:", TextBox, tgui::textBox)
                            else COMPARE_OBJECT(9, "checkbox:", Checkbox, tgui::checkbox)
                            else COMPARE_OBJECT(9, "combobox:", ComboBox, tgui::comboBox)
                            else COMPARE_OBJECT(10, "scrollbar:", Scrollbar, tgui::scrollbar)
                            else COMPARE_OBJECT(11, "loadingbar:", LoadingBar, tgui::loadingBar)
                            else COMPARE_OBJECT(11, "spinbutton:", SpinButton, tgui::spinButton)
                            else COMPARE_OBJECT(12, "radiobutton:", RadioButton, tgui::radioButton)
                            else COMPARE_OBJECT(12, "childwindow:", ChildWindow, tgui::childWindow)
                            else COMPARE_OBJECT(12, "spritesheet:", SpriteSheet, tgui::spriteSheet)
                            else COMPARE_OBJECT(15, "animatedbutton:", AnimatedButton, tgui::animatedButton)
                            else COMPARE_OBJECT(16, "animatedpicture:", AnimatedPicture, tgui::animatedPicture)
                            else // The line was wrong
                                goto LoadingFailed;
                        }

                        break;
                    }
                    case tgui::label + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the label back
                        tgui::Label* label = static_cast<tgui::Label*>(extraPtr);

                        if (line.substr(0, 9).compare("autosize=") == 0)
                        {
                            label->setAutoSize(!!atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the text
                            label->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            label->setTextSize(atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            label->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            // Change the background color (black on error)
                            label->backgroundColor = tgui::extractColor(line.erase(0, 16));
                        }
                        else CHECK_SHARED_PROPERTIES(label)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::button + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the button back
                        tgui::Button* button = static_cast<tgui::Button*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the button
                            button->load(line);
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the caption
                            button->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            button->setTextSize(atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            button->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else CHECK_SHARED_PROPERTIES(button)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::slider + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the slider back
                        tgui::Slider* slider = static_cast<tgui::Slider*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the slider
                            slider->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            slider->setValue(atoi(line.erase(0, 6).c_str()));
                        }
                        else if (line.substr(0, 8).compare("minimum=") == 0)
                        {
                            slider->setMinimum(atoi(line.erase(0, 8).c_str()));
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            slider->setMaximum(atoi(line.erase(0, 8).c_str()));
                        }
                        else if (line.substr(0, 15).compare("verticalscroll=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Check if the value is true or false
                            bool vericalScroll;
                            CHECK_BOOL(vericalScroll)

                            slider->verticalScroll = vericalScroll;
                        }
                        else CHECK_SHARED_PROPERTIES(slider)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::picture + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the picture back
                        tgui::Picture* picture = static_cast<tgui::Picture*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("filename=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the picture
                            picture->load(line);
                        }
                        else CHECK_SHARED_PROPERTIES(picture)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::listbox + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the listbox back
                        tgui::Listbox* listbox = static_cast<tgui::Listbox*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 6).compare("width=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 6);

                            // Set the width
                            listbox->setSize(static_cast<float>(atof(line.c_str())), static_cast<float>(listbox->getSize().y));
                        }
                        else if (line.substr(0, 7).compare("height=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 7);

                            // Set the height
                            listbox->setSize(static_cast<float>(listbox->getSize().x), static_cast<float>(atof(line.c_str())));
                        }
                        else if (line.substr(0, 11).compare("itemheight=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 11);

                            // Set the item height
                            listbox->setItemHeight(atoi(line.c_str()));
                        }
                        else if (line.substr(0, 18).compare("scrollbarpathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 18);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the scrollbar
                            listbox->setScrollbar(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            tgui::Vector4u borders;
                            if (extractVector4u(line, borders))
                                listbox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                goto LoadingFailed;
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            listbox->setBackgroundColor(tgui::extractColor(line.erase(0, 16)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            listbox->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 24).compare("selectedbackgroundcolor=") == 0)
                        {
                            listbox->setSelectedBackgroundColor(tgui::extractColor(line.erase(0, 24)));
                        }
                        else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                        {
                            listbox->setSelectedTextColor(tgui::extractColor(line.erase(0, 18)));
                        }
                        else if (line.substr(0, 12).compare("bordercolor=") == 0)
                        {
                            listbox->setBorderColor(tgui::extractColor(line.erase(0, 12)));
                        }
                        else if (line.substr(0, 13).compare("maximumitems=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 13);

                            // Set the maximum items
                            listbox->setMaximumItems(atoi(line.c_str()));
                        }
                        else if (line.substr(0, 5).compare("item=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Add the item to the listbox
                            listbox->addItem(line);
                        }
                        else if (line.substr(0, 13).compare("selecteditem=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 13);

                            // The line must contain something
                            if (line.empty() == true)
                                goto LoadingFailed;

                            // Check if there are quotes
                            if ((line[0] == '"') && (line[line.length()-1] == '"'))
                            {
                                line.erase(0, 1);
                                line.erase(line.length()-1, 1);

                                // Select the item
                                listbox->setSelectedItem(line);
                            }
                            else // There were no quotes
                            {
                                // Select the item
                                listbox->setSelectedItem(atoi(line.c_str()));
                            }
                        }
                        else CHECK_SHARED_PROPERTIES(listbox)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::editBox + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the edit box back
                        tgui::EditBox* editBox = static_cast<tgui::EditBox*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the edit box
                            editBox->load(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            tgui::Vector4u borders;
                            if (extractVector4u(line, borders))
                                editBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                goto LoadingFailed;
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the text
                            editBox->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            editBox->setTextSize(atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            editBox->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                        {
                            // Change the selected text color (black on error)
                            editBox->setSelectedTextColor(tgui::extractColor(line.erase(0, 18)));
                        }
                        else if (line.substr(0, 28).compare("selectedtextbackgroundcolor=") == 0)
                        {
                            // Change the selected text background color (black on error)
                            editBox->setSelectedTextBackgroundColor(tgui::extractColor(line.erase(0, 28)));
                        }
                        else if (line.substr(0, 37).compare("unfocusedselectedtextbackgroundcolor=") == 0)
                        {
                            TGUI_OUTPUT("TGUI warning: EditBox no longer has a selection background color when unfocused.");
                        }
                        else if (line.substr(0, 20).compare("selectionpointcolor=") == 0)
                        {
                            // Change the selection pointer color (black on error)
                            editBox->selectionPointColor = tgui::extractColor(line.erase(0, 20));
                        }
                        else if (line.substr(0, 13).compare("passwordchar=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 13);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Make sure that the string is not empty
                            if (line.empty() == false)
                            {
                                // Set the password character
                                editBox->setPasswordChar(line[0]);
                            }
                            else // The string is empty
                                editBox->setPasswordChar('\0');
                        }
                        else if (line.substr(0, 20).compare("selectionpointwidth=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 20);

                            // Read the selection point width (0 and thus no selection point when it goes wrong)
                            editBox->selectionPointWidth = atoi(line.c_str());
                        }
                        else if (line.substr(0, 18).compare("maximumcharacters=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 18);

                            // Read the maximum characters (0 and thus no limit when it goes wrong)
                            editBox->setMaximumCharacters(atoi(line.c_str()));
                        }
                        else CHECK_SHARED_PROPERTIES(editBox)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::textBox + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the text box back
                        tgui::TextBox* textBox = static_cast<tgui::TextBox*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 6).compare("width=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 6);

                            // Set the width
                            textBox->setSize(static_cast<float>(atof(line.c_str())), static_cast<float>(textBox->getSize().y));
                        }
                        else if (line.substr(0, 7).compare("height=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 7);

                            // Set the height
                            textBox->setSize(static_cast<float>(textBox->getSize().x), static_cast<float>(atof(line.c_str())));
                        }
                        else if (line.substr(0, 18).compare("scrollbarpathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 18);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the scrollbar
                            textBox->setScrollbar(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            tgui::Vector4u borders;
                            if (extractVector4u(line, borders))
                                textBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                goto LoadingFailed;
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the text
                            textBox->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            textBox->setTextSize(atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            // Change the background color (black on error)
                            textBox->setBackgroundColor(tgui::extractColor(line.erase(0, 16)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            textBox->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                        {
                            // Change the selected text color (black on error)
                            textBox->setSelectedTextColor(tgui::extractColor(line.erase(0, 18)));
                        }
                        else if (line.substr(0, 28).compare("selectedtextbackgroundcolor=") == 0)
                        {
                            // Change the selected text background color (black on error)
                            textBox->setSelectedTextBackgroundColor(tgui::extractColor(line.erase(0, 28)));
                        }
                        else if (line.substr(0, 37).compare("unfocusedselectedtextbackgroundcolor=") == 0)
                        {
                            TGUI_OUTPUT("TGUI warning: TextBox no longer has a selection background color when unfocused.");
                        }
                        else if (line.substr(0, 12).compare("bordercolor=") == 0)
                        {
                            // Change the border color (black on error)
                            textBox->setBorderColor(tgui::extractColor(line.erase(0, 12)));
                        }
                        else if (line.substr(0, 20).compare("selectionpointcolor=") == 0)
                        {
                            // Change the selection pointer color (black on error)
                            textBox->selectionPointColor = tgui::extractColor(line.erase(0, 20));
                        }
                        else if (line.substr(0, 20).compare("selectionpointwidth=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 20);

                            // Read the selection point width (0 and thus no selection point when it goes wrong)
                            textBox->selectionPointWidth = atoi(line.c_str());
                        }
                        else if (line.substr(0, 18).compare("maximumcharacters=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 18);

                            // Read the maximum characters (0 and thus no limit when it goes wrong)
                            textBox->setMaximumCharacters(atoi(line.c_str()));
                        }
                        else CHECK_SHARED_PROPERTIES(textBox)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::checkbox + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the checkbox back
                        tgui::Checkbox* checkbox = static_cast<tgui::Checkbox*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the checkbox
                            checkbox->load(line);
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the caption
                            checkbox->setText(line);
                        }
                        else if (line.substr(0, 8).compare("checked=") == 0)
                        {
                            // Remove the first part of the string
                            line.erase(0, 8);

                            // Check if the value is true or false
                            bool checked;
                            CHECK_BOOL(checked)

                            // Check or uncheck the checkbox
                            if (checked)
                                checkbox->check();
                            else
                                checkbox->uncheck();
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            checkbox->setTextSize(atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            checkbox->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else CHECK_SHARED_PROPERTIES(checkbox)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::comboBox + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the combo box back
                        tgui::ComboBox* comboBox = static_cast<tgui::ComboBox*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the combo box
                            comboBox->load(line, comboBox->getSize().x, comboBox->getSize().y);
                        }
                        else if (line.substr(0, 6).compare("width=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 6);

                            // Set the width
                            comboBox->setSize(static_cast<float>(atoi(line.c_str())), static_cast<float>(comboBox->getSize().y));
                        }
                        else if (line.substr(0, 18).compare("scrollbarpathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 18);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the scrollbar
                            comboBox->setScrollbar(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            tgui::Vector4u borders;
                            if (extractVector4u(line, borders))
                                comboBox->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                goto LoadingFailed;
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            comboBox->setBackgroundColor(tgui::extractColor(line.erase(0, 16)));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            comboBox->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 24).compare("selectedbackgroundcolor=") == 0)
                        {
                            comboBox->setSelectedBackgroundColor(tgui::extractColor(line.erase(0, 24)));
                        }
                        else if (line.substr(0, 18).compare("selectedtextcolor=") == 0)
                        {
                            comboBox->setSelectedTextColor(tgui::extractColor(line.erase(0, 18)));
                        }
                        else if (line.substr(0, 12).compare("bordercolor=") == 0)
                        {
                            comboBox->setBorderColor(tgui::extractColor(line.erase(0, 12)));
                        }
                        else if (line.substr(0, 15).compare("itemstodisplay=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Set the nr of items to display
                            comboBox->setItemsToDisplay(atoi(line.c_str()));
                        }
                        else if (line.substr(0, 5).compare("item=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Add the item to the combo box
                            comboBox->addItem(line);
                        }
                        else if (line.substr(0, 13).compare("selecteditem=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 13);

                            // The line must contain something
                            if (line.empty() == true)
                                goto LoadingFailed;

                            // Check if there are quotes
                            if ((line[0] == '"') && (line[line.length()-1] == '"'))
                            {
                                line.erase(0, 1);
                                line.erase(line.length()-1, 1);

                                // Select the item
                                comboBox->setSelectedItem(line);
                            }
                            else // There were no quotes
                            {
                                // Select the item
                                comboBox->setSelectedItem(atoi(line.c_str()));
                            }
                        }
                        else CHECK_SHARED_PROPERTIES(comboBox)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::scrollbar + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the scrollbar back
                        tgui::Scrollbar* scrollbar = static_cast<tgui::Scrollbar*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the scrollbar
                            scrollbar->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            scrollbar->setValue(atoi(line.erase(0, 6).c_str()));
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            scrollbar->setMaximum(atoi(line.erase(0, 8).c_str()));
                        }
                        else if (line.substr(0, 9).compare("lowvalue=") == 0)
                        {
                            scrollbar->setLowValue(atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 15).compare("verticalscroll=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Check if the value is true or false
                            bool vericalScroll;
                            CHECK_BOOL(vericalScroll)

                            scrollbar->verticalScroll = vericalScroll;
                        }
                        else CHECK_SHARED_PROPERTIES(scrollbar)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::loadingBar + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the loading bar back
                        tgui::LoadingBar* loadingBar = static_cast<tgui::LoadingBar*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the loading bar
                            loadingBar->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            loadingBar->setValue(atoi(line.erase(0, 6).c_str()));
                        }
                        else if (line.substr(0, 8).compare("minimum=") == 0)
                        {
                            loadingBar->setMinimum(atoi(line.erase(0, 8).c_str()));
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            loadingBar->setMaximum(atoi(line.erase(0, 8).c_str()));
                        }
                        else CHECK_SHARED_PROPERTIES(loadingBar)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::spinButton + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the spin button back
                        tgui::SpinButton* spinButton = static_cast<tgui::SpinButton*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the spin button
                            spinButton->load(line);
                        }
                        else if (line.substr(0, 6).compare("value=") == 0)
                        {
                            spinButton->setValue(atoi(line.erase(0, 6).c_str()));
                        }
                        else if (line.substr(0, 8).compare("minimum=") == 0)
                        {
                            spinButton->setMinimum(atoi(line.erase(0, 8).c_str()));
                        }
                        else if (line.substr(0, 8).compare("maximum=") == 0)
                        {
                            spinButton->setMaximum(atoi(line.erase(0, 8).c_str()));
                        }
                        else if (line.substr(0, 15).compare("verticalscroll=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 15);

                            // Check if the value is true or false
                            bool vericalScroll;
                            CHECK_BOOL(vericalScroll)

                            spinButton->verticalScroll = vericalScroll;
                        }
                        else CHECK_SHARED_PROPERTIES(spinButton)
                        else // The line was wrong
                            goto LoadingFailed;

                        break;
                    }
                    case tgui::radioButton + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the radio button back
                        tgui::RadioButton* radioButton = static_cast<tgui::RadioButton*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the radio button
                            radioButton->load(line);
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the caption
                            radioButton->setText(line);
                        }
                        else if (line.substr(0, 8).compare("checked=") == 0)
                        {
                            // Remove the first part of the string
                            line.erase(0, 8);

                            // Check if the value is true or false
                            bool checked;
                            CHECK_BOOL(checked)

                            // Check or uncheck the radio button
                            if (checked)
                                radioButton->check();
                            else
                                radioButton->uncheck();
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            radioButton->setTextSize(atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            radioButton->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else CHECK_SHARED_PROPERTIES(radioButton)
                            else // The line was wrong
                                goto LoadingFailed;

                        break;
                    }
                    case tgui::childWindow + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the child window back
                        tgui::ChildWindow* child = static_cast<tgui::ChildWindow*>(extraPtr);

                        CHECK_SHARED_PROPERTIES(child)
                        else if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the child window
                            if (child->load(child->getSize().x, child->getSize().y, child->backgroundColor, line) == false)
                                goto LoadingFailed;
                        }
                        else if (line.substr(0, 16).compare("backgroundcolor=") == 0)
                        {
                            // Change the background color (black on error)
                            child->backgroundColor = tgui::extractColor(line.erase(0, 16));
                        }
                        else if (line.substr(0, 16).compare("backgroundimage=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 16);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the image
                            child->setBackgroundImage(line);
                        }
                        else if (line.substr(0, 8).compare("borders=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 8);

                            // Get the borders
                            tgui::Vector4u borders;
                            if (extractVector4u(line, borders))
                                child->setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
                            else
                                goto LoadingFailed;
                        }
                        else if (line.substr(0, 13).compare("transparency=") == 0)
                        {
                            child->setTransparency(static_cast<unsigned char>(atoi(line.erase(0, 13).c_str())));
                        }
                        else if (line.substr(0, 15).compare("titlebarheight=") == 0)
                        {
                            child->setTitlebarHeight(atoi(line.erase(0, 15).c_str()));
                        }
                        else if (line.substr(0, 7).compare("layout=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 7);

                            // Check what the layout is
                            if (line.compare("left") == 0)
                                child->layout = ChildWindow::LayoutLeft;
                            else if (line.compare("right") == 0)
                                child->layout = ChildWindow::LayoutRight;
                            else
                                goto LoadingFailed;
                        }
                        else
                        {
                            // All newly created objects must be part of the panel
                            parentID.push(tgui::childWindow + 1);
                            parentPtr.push(child);

                            COMPARE_OBJECT(6, "panel:", Panel, tgui::panel)
                            else COMPARE_OBJECT(6, "label:", Label, tgui::label)
                            else COMPARE_OBJECT(7, "button:", Button, tgui::button)
                            else COMPARE_OBJECT(7, "slider:", Slider, tgui::slider)
                            else COMPARE_OBJECT(8, "picture:", Picture, tgui::picture)
                            else COMPARE_OBJECT(8, "listbox:", Listbox, tgui::listbox)
                            else COMPARE_OBJECT(8, "editbox:", EditBox, tgui::editBox)
                            else COMPARE_OBJECT(8, "textbox:", TextBox, tgui::textBox)
                            else COMPARE_OBJECT(9, "checkbox:", Checkbox, tgui::checkbox)
                            else COMPARE_OBJECT(9, "combobox:", ComboBox, tgui::comboBox)
                            else COMPARE_OBJECT(10, "scrollbar:", Scrollbar, tgui::scrollbar)
                            else COMPARE_OBJECT(11, "loadingbar:", LoadingBar, tgui::loadingBar)
                            else COMPARE_OBJECT(11, "spinbutton:", SpinButton, tgui::spinButton)
                            else COMPARE_OBJECT(12, "radiobutton:", RadioButton, tgui::radioButton)
                            else COMPARE_OBJECT(12, "childwindow:", ChildWindow, tgui::childWindow)
                            else COMPARE_OBJECT(12, "spritesheet:", SpriteSheet, tgui::spriteSheet)
                            else COMPARE_OBJECT(15, "animatedbutton:", AnimatedButton, tgui::animatedButton)
                            else COMPARE_OBJECT(16, "animatedpicture:", AnimatedPicture, tgui::animatedPicture)
                            else // The line was wrong
                                goto LoadingFailed;
                        }

                        break;
                    }
                    case tgui::spriteSheet + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the sprite sheet back
                        tgui::SpriteSheet* spriteSheet = static_cast<tgui::SpriteSheet*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("filename=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The filename must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the sprite sheet
                            spriteSheet->load(line);
                        }
                        else if (line.substr(0, 5).compare("rows=") == 0)
                        {
                            spriteSheet->setRows(atoi(line.erase(0, 5).c_str()));
                        }
                        else if (line.substr(0, 8).compare("columns=") == 0)
                        {
                            spriteSheet->setColumns(atoi(line.erase(0, 8).c_str()));
                        }
                        else if (line.substr(0, 6).compare("cells=") == 0)
                        {
                            Vector2u cells;
                            if (extractVector2u(line.erase(0, 6), cells) == false)
                                goto LoadingFailed;

                            // Set the cells
                            spriteSheet->setCells(cells.x, cells.y);
                        }
                        else if (line.substr(0, 12).compare("visiblecell=") == 0)
                        {
                            Vector2u cell;
                            if (extractVector2u(line.erase(0, 12), cell) == false)
                                goto LoadingFailed;

                            spriteSheet->setVisibleCell(cell.x, cell.y);
                        }
                        else CHECK_SHARED_PROPERTIES(spriteSheet)
                        else // The line was wrong
                            goto LoadingFailed;

                        break;
                    }
                    case tgui::animatedButton + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the animated button back
                        tgui::AnimatedButton* button = static_cast<tgui::AnimatedButton*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 9).compare("pathname=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 9);

                            // The pathname must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Load the animated button
                            if (button->load(line) == false)
                                goto LoadingFailed;
                        }
                        else if (line.substr(0, 5).compare("text=") == 0)
                        {
                            // Remove the first part of the line
                            line.erase(0, 5);

                            // The text must start and end with quotes
                            CHECK_FOR_QUOTES

                            // Change the caption
                            button->setText(line);
                        }
                        else if (line.substr(0, 9).compare("textsize=") == 0)
                        {
                            // Change the text size
                            button->setTextSize(atoi(line.erase(0, 9).c_str()));
                        }
                        else if (line.substr(0, 10).compare("textcolor=") == 0)
                        {
                            // Change the text color (black on error)
                            button->setTextColor(tgui::extractColor(line.erase(0, 10)));
                        }
                        else if (line.substr(0, 13).compare("currentframe=") == 0)
                        {
                            button->setFrame(static_cast<unsigned int>(atoi(line.erase(0, 13).c_str())));
                        }
                        else CHECK_SHARED_PROPERTIES(button)
                        else // The line was wrong
                            goto LoadingFailed;

                        break;
                    }
                    case tgui::animatedPicture + 1:
                    {
                        START_LOADING_OBJECT

                        // Get the pointer to the sprite animated picture
                        tgui::AnimatedPicture* animatedPicture = static_cast<tgui::AnimatedPicture*>(extraPtr);

                        // Find out what the next property is
                        if (line.substr(0, 6).compare("frame=") == 0)
                        {
                            line.erase(0, 6);

                            // Make sure that the string isn't empty
                            if (line.empty() == false)
                            {
                                // The first and last character have to be brackets
                                if ((line[0] == '(') && (line[line.length()-1] == ')'))
                                {
                                    // Remove the brackets
                                    line.erase(0, 1);
                                    line.erase(line.length()-1);

                                    // Search for the first comma
                                    std::string::size_type commaPos = line.find(',');
                                    if (commaPos != std::string::npos)
                                    {
                                        // Get the frame duration
                                        int duration = atoi(line.substr(commaPos+1).c_str());
                                        line.erase(commaPos);

                                        // The filename must start and end with quotes
                                        CHECK_FOR_QUOTES

                                        // Add the frame to the animated picture
                                        if (animatedPicture->addFrame(line, sf::milliseconds(duration)) == false)
                                            goto LoadingFailed;
                                    }
                                    else
                                        goto LoadingFailed;
                                }
                                else
                                    goto LoadingFailed;
                            }
                            else
                                goto LoadingFailed;
                        }
                        else if (line.substr(0, 5).compare("loop=") == 0)
                        {
                            // Remove the first part of the string
                            line.erase(0, 5);

                            // Check if the value is true or false
                            bool loop;
                            CHECK_BOOL(loop)

                            animatedPicture->loop = loop;
                        }
                        else if (line.substr(0, 8).compare("playing=") == 0)
                        {
                            // Remove the first part of the string
                            line.erase(0, 8);

                            // Check if the value is true or false
                            bool playing;
                            CHECK_BOOL(playing)

                            if (playing)
                                animatedPicture->play();
                        }
                        else if (line.substr(0, 13).compare("currentframe=") == 0)
                        {
                            animatedPicture->setFrame(static_cast<unsigned int>(atoi(line.erase(0, 13).c_str())));
                        }
                        else CHECK_SHARED_PROPERTIES(animatedPicture)
                        else // The line was wrong
                            goto LoadingFailed;

                        break;
                    }
                };
            }
        }


      LoadingSucceeded:

        m_File.close();
        return true;

      LoadingFailed:

        m_File.close();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<OBJECT*>& Group::getObjects()
    {
        return m_EventManager.m_Objects;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String>& Group::getObjectNames()
    {
        return m_ObjName;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::remove(const sf::String objectName)
    {
        // Loop through every object
        for (unsigned int i=0; i<m_ObjName.size(); ++i)
        {
            // Check if the name matches
            if (m_ObjName[i].toWideString().compare(objectName) == 0)
            {
                // Remove the object
                delete m_EventManager.m_Objects[i];
                m_EventManager.m_Objects.erase(m_EventManager.m_Objects.begin() + i);

                // Also emove the name it from the list
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::remove(OBJECT* object)
    {
        // Loop through every object
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            // Check if the pointer matches
            if (m_EventManager.m_Objects[i] == object)
            {
                // Unfocus the object, just in case it was focused
                m_EventManager.unfocusObject(object);

                // Remove the object
                delete m_EventManager.m_Objects[i];
                m_EventManager.m_Objects.erase(m_EventManager.m_Objects.begin() + i);
                object = NULL;

                // Also emove the name it from the list
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::removeAllObjects()
    {
        // Delete all objects
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
            delete m_EventManager.m_Objects[i];

        // Clear the lists
        m_EventManager.m_Objects.clear();
        m_ObjName.clear();

        // There are no more objects, so none of the objects can be focused
        m_EventManager.m_FocusedObject = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            if (m_EventManager.m_Objects[i]->m_ObjectType == radioButton)
                static_cast<RadioButton*>(m_EventManager.m_Objects[i])->m_Checked = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::focus(OBJECT* object)
    {
        m_EventManager.focusObject(object);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::unfocus(OBJECT* object)
    {
        m_EventManager.focusObject(object);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::moveObjectToFront(OBJECT* object)
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            // Check if the object is found
            if (m_EventManager.m_Objects[i] == object)
            {
                // Copy the object
                m_EventManager.m_Objects.push_back(m_EventManager.m_Objects[i]);
                m_ObjName.push_back(m_ObjName[i]);

                // Focus the correct object
                if ((m_EventManager.m_FocusedObject == 0) || (m_EventManager.m_FocusedObject == i+1))
                    m_EventManager.m_FocusedObject = m_EventManager.m_Objects.size()-1;
                else if (m_EventManager.m_FocusedObject > i+1)
                    --m_EventManager.m_FocusedObject;

                // Remove the old object
                m_EventManager.m_Objects.erase(m_EventManager.m_Objects.begin() + i);
                m_ObjName.erase(m_ObjName.begin() + i);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::moveObjectToBack(OBJECT* object)
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            // Check if the object is found
            if (m_EventManager.m_Objects[i] == object)
            {
                // Copy the object
                OBJECT* obj = m_EventManager.m_Objects[i];
                std::string name = m_ObjName[i];
                m_EventManager.m_Objects.insert(m_EventManager.m_Objects.begin(), obj);
                m_ObjName.insert(m_ObjName.begin(), name);

                // Focus the correct object
                if (m_EventManager.m_FocusedObject == i+1)
                    m_EventManager.m_FocusedObject = 1;
                else if (m_EventManager.m_FocusedObject)
                    ++m_EventManager.m_FocusedObject;

                // Remove the old object
                m_EventManager.m_Objects.erase(m_EventManager.m_Objects.begin() + i + 1);
                m_ObjName.erase(m_ObjName.begin() + i + 1);

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::updateTime(const sf::Time& elapsedTime)
    {
        m_EventManager.updateTime(elapsedTime);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::drawObjectGroup(sf::RenderTarget* target, const sf::RenderStates& states) const
    {
        // Loop through all objects
        for (unsigned int i=0; i<m_EventManager.m_Objects.size(); ++i)
        {
            // Draw the object if it is visible
            if (m_EventManager.m_Objects[i]->m_Visible)
                target->draw(*m_EventManager.m_Objects[i], states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

