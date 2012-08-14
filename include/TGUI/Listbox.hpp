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


#ifndef _TGUI_LISTBOX_INCLUDED_
#define _TGUI_LISTBOX_INCLUDED_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct TGUI_API Listbox : public OBJECT, OBJECT_BORDERS
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Listbox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Copy constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Listbox(const Listbox& copy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~Listbox();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Overload of assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Listbox& operator= (const Listbox& right);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void initialize();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Listbox* clone();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Creates the listbox.
        ///
        /// The listbox may resize a little bit. It might be a little bit bigger or smaller than the size passed to this function.
        ///
        /// \param width              The width of the listbox (borders included).
        /// \param height             The height of the listbox (borders included).
        /// \param scrollbarPathname  The pathname needed to load the scrollbar.
        ///                           If not provided then there will be no possibility to scroll, thus no items will be added when the listbox is full.
        /// \param itemHeight         The height of a single item in the listbox. The text size will be based on this, but will be a little smaller.
        ///                           If ItemHeight is 0 then it will become a tenth of the height of the listbox.
        ///
        /// \return
        ///        - true on success
        ///        - false when scrollbar couldn't be loaded (only if \a scrollbarPathname isn't empty)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool load(unsigned int width,
                  unsigned int height,
                  const std::string scrollbarPathname = "",
                  unsigned int itemHeight = 0);


        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the size of the listbox.
        ///
        /// \param width   The new width of the listbox
        /// \param height  The new height of the listbox
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(float width, float height);


        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the size of the listbox, unaffected by scaling.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Vector2u getSize() const;


        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the size of the listbox, after the scaling transformation.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Vector2f getScaledSize() const;


        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the pathname that was used to load the scrollbar.
        ///
        /// When no scrollbar was loaded then this function will return an empty string.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string getLoadedScrollbarPathname();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the colors used in the listbox.
        ///
        /// \param backgroundColor          The color of the background of the listbox
        /// \param textColor                The color of the text
        /// \param selectedBackgroundColor  The color of the background of the selected item
        /// \param selectedTextColor        The color of the text when it is selected
        /// \param borderColor              The color of the borders
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void changeColors(const sf::Color& backgroundColor         = sf::Color::White,
                          const sf::Color& textColor               = sf::Color::Black,
                          const sf::Color& selectedBackgroundColor = sf::Color(50, 100, 200),
                          const sf::Color& selectedTextColor       = sf::Color::White,
                          const sf::Color& borderColor             = sf::Color::Black);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the background color that will be used inside the listbox.
        ///
        /// \see changeColors
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBackgroundColor(const sf::Color& backgroundColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the text color that will be used inside the listbox.
        ///
        /// \see changeColors
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextColor(const sf::Color& textColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the background color of the selected text that will be used inside the listbox.
        ///
        /// \see changeColors
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the text color of the selected text that will be used inside the listbox.
        ///
        /// \see changeColors
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSelectedTextColor(const sf::Color& selectedTextColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the border color text that will be used inside the listbox.
        ///
        /// \see changeColors
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBorderColor(const sf::Color& borderColor);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the background color that is currently being used inside the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::Color& getBackgroundColor();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the text color that is currently being used inside the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::Color& getTextColor();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the background color of the selected text that is currently being used inside the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::Color& getSelectedBackgroundColor();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the text color of the selected text that is currently being used inside the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::Color& getSelectedTextColor();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the border color that is currently being used inside the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::Color& getBorderColor();


        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the font of the items.
        ///
        /// When you don't call this function then the global font will be use.
        /// This global font can be changed by changing \a gloabalFont from the parent.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextFont(const sf::Font& font);


        //////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the font of the items.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        const sf::Font& getTextFont();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Adds an item to the list.
        ///
        /// \param itemName The name of the item you want to add (this is the text that will be displayed inside the listbox)
        ///
        /// \return
        ///         - 0 when the listbox is full (you have set a maximum item limit and you are trying to add more items)
        ///         - 0 when there is no scrollbar and you try to have more items than fit inside the listbox
        ///         - The id of the item when it was successfully added.
        ///
        /// \warning The id returned by this function is no longer correct when an item is removed. Use it immediately or don't use it at all.
        ///
        /// \see setMaximumItems
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int addItem(const sf::String itemName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Selects an item in the listbox.
        ///
        /// When adding items to the listbox with the addItem function, none of them will be selected.
        /// If you want a default item selected then you can use this function to select it.
        /// The first item that matches the name will be selected.
        ///
        /// \return
        ///         - true on success
        ///         - false when none of the items matches the name
        ///
        /// \see setSelectedItem(unsigned int)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setSelectedItem(const sf::String itemName);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Selects an item in the listbox.
        ///
        /// When adding items to the listbox with the addItem function, none of them will be selected.
        /// If you want a default item selected then you can use this function to select it.
        ///
        /// The id starts counting from 1. If you pass 0 to this function then none of the items will be selected.
        ///
        /// \return
        ///         - true on success
        ///         - false when the id was too high
        ///
        /// \see setSelectedItem(sf::String)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setSelectedItem(unsigned int id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes an item from the list with a given id.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeItem(unsigned int id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes all items from the list with a given name.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeItem(const sf::String itemName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes all items from the list.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeAllItems();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the item name of the given id.
        ///
        /// The string will be empty when the id was too high or when it was 0.
        ///
        /// \see getItemID
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::String getItem(unsigned int id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the id of the first item with the given name.
        ///
        /// If none of the items matches then the id will be 0.
        ///
        /// \warning This id will become wrong when an item is removed from the list. Use it immediately or don't use it at all.
        ///
        /// \see getItem
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getItemID(const sf::String itemName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the list that constains all the items.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<sf::String>& getItems();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the currently selected item.
        ///
        /// When no item was selected then this function will return an empty string.
        ///
        /// \see getSelectedItemID
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::String getSelectedItem() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Get the id of the selected item.
        ///
        /// When no item was selected then this function returns 0.
        ///
        /// \warning This id will become wrong when an item is removed from the list. Use it immediately or don't use it at all.
        ///
        /// \see getSelectedItem
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getSelectedItemID();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the scrollbar of the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setScrollbar(const std::string scrollbarPathname);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Removes the scrollbar from the listbox (if there is one).
        ///
        /// When there are too many items to fit in the listbox then the items will be removed.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeScrollbar();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the height of the items in the listbox.
        ///
        /// This size is always a little big greater than the text size.
        /// When there is no scrollbar then the items will be removed when they no longer fit inside the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setItemHeight(unsigned int itemHeight);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the height of the items in the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getItemHeight();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the maximum items that the listbox can contain.
        ///
        /// If this function returns 0 then there is no limit.
        /// If no scrollbar was loaded then there is always a limitation because the text can't go outside the listbox.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMaximumItems(unsigned int maximumItems = 0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the maximum items that the listbox can contain. If 0 then there is no limit.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getMaximumItems();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the size of the borders.
        ///
        /// \param leftBorder    The width of the left border
        /// \param topBorder     The height of the top border
        /// \param rightBorder   The width of the right border
        /// \param bottomBorder  The height of the bottom border
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBorders(unsigned int leftBorder   = 0,
                        unsigned int topBorder    = 0,
                        unsigned int rightBorder  = 0,
                        unsigned int bottomBorder = 0);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // These functions are used to receive callback from the EventManager.
        // You normally don't need them, but you can use them to simulate an event.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool mouseOnObject(float x, float y);
        void leftMousePressed(float x, float y);
        void leftMouseReleased(float x, float y);
        void mouseMoved(float x, float y);
        void mouseNotOnObject();
        void mouseNoLongerDown();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Because this struct is derived from sf::Drawable, you can just call the draw function from your sf::RenderTarget.
        // This function will be called and it will draw the listbox on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        // This contains the different items in the listbox
        std::vector<sf::String> m_Items;

        // What is the id of the selected item?
        unsigned int m_SelectedItem;

        // The size must be stored
        Vector2u m_Size;
        unsigned int m_ItemHeight;

        unsigned int m_TextSize;

        // This will store the maximum number of items in the listbox (zero by default, meaning that there is no limit)
        unsigned int m_MaxItems;

        // When there are too many items a scrollbar will be shown
        Scrollbar* m_Scroll;

        // The pathname of the loaded scrollbar (if there is one)
        std::string m_LoadedScrollbarPathname;

        // These colors are used to draw the listbox
        sf::Color m_BackgroundColor;
        sf::Color m_TextColor;
        sf::Color m_SelectedBackgroundColor;
        sf::Color m_SelectedTextColor;
        sf::Color m_BorderColor;

        // The font used to draw the text
        sf::Font m_TextFont;

        // ComboBox contains a list box internally and it should be able to adjust it.
        friend struct ComboBox;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_TGUI_LISTBOX_INCLUDED_
