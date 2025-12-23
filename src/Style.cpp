#include <GaUIL/Style.hpp>


#include <algorithm>
namespace gauil {
    Style::ButtonBase::~ButtonBase() {}

    Style::Style(){
        hovererdButton.backgroundColor = Color(156, 156, 156);
        clickedButton.backgroundColor = Color(22, 88, 122);
    }
}
