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


#ifndef TGUI_VERTICAL_LAYOUT_HPP
#define TGUI_VERTICAL_LAYOUT_HPP

#include <TGUI/Widgets/BoxLayoutRatios.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Container that automatically resizes children to fit the entire available space between children
    ///
    /// The children are stacked vertically.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API VerticalLayout : public BoxLayoutRatios
    {
    public:
        using Ptr = std::shared_ptr<VerticalLayout>; //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const VerticalLayout>; //!< Shared constant widget pointer

        static constexpr const char StaticWidgetType[] = "VerticalLayout"; //!< Type name of the widget


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /// @see create
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        VerticalLayout(const char* typeName = StaticWidgetType, bool initRenderer = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new vertical layout widget
        ///
        /// @param size  Size of the vertical layout
        ///
        /// @return The new vertical layout
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static VerticalLayout::Ptr create(const Layout2d& size = {"100%", "100%"});


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another layout
        ///
        /// @param layout  The other layout
        ///
        /// @return The new layout
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static VerticalLayout::Ptr copy(const VerticalLayout::ConstPtr& layout);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // @brief Repositions and resize the widgets
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateWidgets() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // @brief Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Widget::Ptr clone() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_VERTICAL_LAYOUT_HPP
