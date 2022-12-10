/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_MESSAGE_BOX_HPP
#define TGUI_MESSAGE_BOX_HPP


#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Renderers/MessageBoxRenderer.hpp>

#undef MessageBox  // windows.h defines MessageBox when NOMB isn't defined before including windows.h

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Message box widget
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API MessageBox : public ChildWindow
    {
    public:

        using Ptr = std::shared_ptr<MessageBox>; //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const MessageBox>; //!< Shared constant widget pointer

        static constexpr const char StaticWidgetType[] = "MessageBox"; //!< Type name of the widget


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The label and button alignment
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class Alignment
        {
            Left,   //!< Put the label or buttons on the left side of the window (default for label)
            Center, //!< Put the label or buttons in the middle of the window (default for buttons)
            Right   //!< Put the label or buttons on the right side of the window
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /// @see create
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MessageBox(const char* typeName = StaticWidgetType, bool initRenderer = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new message box widget
        ///
        /// @param title   Title to display in the title bar of the message box
        /// @param text    Text to display on the message box
        /// @param buttons Buttons to display on the message box
        ///
        /// @return The new message box
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static MessageBox::Ptr create(const String& title = "", const String& text = "", const std::vector<String>& buttons = {});


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MessageBox(const MessageBox& copy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Move constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MessageBox(MessageBox&& copy) noexcept;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Overload of copy assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MessageBox& operator= (const MessageBox& right);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Overload of move assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MessageBox& operator= (MessageBox&& right) noexcept;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another message box
        ///
        /// @param messageBox  The other message box
        ///
        /// @return The new message box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static MessageBox::Ptr copy(const MessageBox::ConstPtr& messageBox);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer that may be shared with other widgets using the same renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MessageBoxRenderer* getSharedRenderer();
        const MessageBoxRenderer* getSharedRenderer() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        /// @return Temporary pointer to the renderer
        /// @warning After calling this function, the widget has its own copy of the renderer and it will no longer be shared.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MessageBoxRenderer* getRenderer();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the message box
        ///
        /// @param size   Sets the new size of the message box
        ///
        /// This is the size of the entire child window, including the title bar and the borders.
        /// To set the size of the contents of the window (exluding the title bar and borders), use setClientSize instead.
        ///
        /// By default the size of the message box is determined by the text and buttons inside it.
        /// Calling this function will disable the auto-sizing of the window.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using Widget::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the client size of the child window
        ///
        /// @param size  New size of the child window contents
        ///
        /// This sets the size of the child window excluding the title bar and the borders.
        ///
        /// By default the size of the message box is determined by the text and buttons inside it.
        /// Calling this function will disable the auto-sizing of the window.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setClientSize(const Layout2d& size) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the text of the message box
        ///
        /// @param text  The text displayed by the message box
        ///
        /// The text will be placed as one long string and the message box will get the size needed to display the whole string.
        /// So if you need to display multiple lines of text then add '\n' inside the text yourself.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setText(const String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the text of the message box
        ///
        /// @return Text that is being displayed inside the message box
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const String& getText() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds a button to the message box
        ///
        /// @param buttonCaption  The caption of the button
        ///
        /// When receiving a callback with the ButtonClicked trigger then callback.text will contain this caption to identify
        /// the clicked button.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void addButton(const String& buttonCaption);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the caption of the buttons
        ///
        /// @return Button texts
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<String> getButtons() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes where the label is located inside the window (left side, centered or right side)
        ///
        /// @param buttonAlignment  Alignment for the label
        ///
        /// The label is left aligned by default.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setLabelAlignment(Alignment labelAlignment);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns where the label is located inside the window (left side, centered or right side)
        ///
        /// @return Alignment for the label
        ///
        /// The label is left aligned by default.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Alignment getLabelAlignment() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes where the buttons are located inside the window (left side, centered or right side)
        ///
        /// @param buttonAlignment  Alignment for the buttons
        ///
        /// Buttons are centered by default.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setButtonAlignment(Alignment buttonAlignment);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns where the buttons are located inside the window (left side, centered or right side)
        ///
        /// @return Alignment for the buttons
        ///
        /// Buttons are centered by default.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Alignment getButtonAlignment() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes sure all widgets lie within the window and places them on the correct position.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rearrange();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves a signal based on its name
        ///
        /// @param signalName  Name of the signal
        ///
        /// @return Signal that corresponds to the name
        ///
        /// @throw Exception when the name does not match any signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal& getSignal(String signalName) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function called when one of the properties of the renderer is changed
        ///
        /// @param property  Name of the property that was changed
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rendererChanged(const String& property) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Saves the widget as a tree node in order to save it to a file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::unique_ptr<DataIO::Node> save(SavingRenderersMap& renderers) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the widget from a tree of nodes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Called when the text size is changed (either by setTextSize or via the renderer)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateTextSize() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Widget::Ptr clone() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Figure out which widget is the label and which are the buttons after copying or loading
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void identifyLabelAndButtons();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Passes our onButtonPress signal handler to the onPress signal of a button
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void connectButtonPressSignal(std::size_t buttonIndex);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        SignalString onButtonPress = {"ButtonPressed"}; //!< One of the buttons was pressed. Optional parameter: text of the pressed button


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        String m_loadedThemeFile;
        String m_buttonClassName;
        bool m_autoSize = true;
        Alignment m_labelAlignment = Alignment::Left;
        Alignment m_buttonAlignment = Alignment::Center;

        std::vector<Button::Ptr> m_buttons;

        Label::Ptr m_label = Label::create();
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_MESSAGE_BOX_HPP
