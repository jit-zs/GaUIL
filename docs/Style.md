# Style

Styling is managed through a filetype I created with the .simss file extension.
It works similarly to .css by making the values in the subelement override the value in the base element

Example:
```
{
  // Global section
  border_color: rgb(22, 55, 22);
  background_color: rgba(22, 88, 128, 255);
}

elememnt{
  radius: 32;
  border: 8;
}
element.subelement{
  radius: 16;
}
element.subelement.button{
  radius: 8;
}
element.subelement.button|state{
  radius: 4;
  border: 4;
}
```
- radius in element = 32
- border in element = 8
  
- radius in element.subelement = 16
- border in element.subelement = 8
  
- radius in element.subelement.button = 8
- border in element.subelement.button = 8

- radius in element.subelement.button|state = 4
- border in element.subelement.button|state = 4

## Notes
- Alignment variables are parsed from strings and if they don't specify if they are left or right (for horizontal alignment) or top or bottom (for vertical alignment) they default to center
- Padding pushes an element's content away from the border

## Subelement names for UI elements and their states
#### Label
- subelement name: label
  - no states
  - variables:
    - font > string
    - text_color > color
    - horizontal_alignment > string
    - vertical_alignment > string
    - padding
      - padding_top > float
      - padding_bottom > float
      - padding_left > float
      - padding_right > float
#### Button
- subelement name: button
  - states:
    - hover
    - active
  - variables:
    - border
      - border_top > float
      - border_bottom > float
      - border_left > float
      - border_right > float
    - padding
      - padding_top > float
      - padding_bottom > float
      - padding_left > float
      - padding_right > float
    - radius
      - radius_top_left > float
      - radius_top_right > float
      - radius_bottom_left > float
      - radius_bottom_right > float
    - border_color > color
    - background_color > color
---
- The text styling is all done under the ``button`` subelement using the variables defined in the [Label](#label) section
#### Check box
- subelement name: check_box
  - states:
    - hover
    - active
  - variables:
    - border
      - border_top > float
      - border_bottom > float
      - border_left > float
      - border_right > float
    - padding
      - padding_top > float
      - padding_bottom > float
      - padding_left > float
      - padding_right > float
    - radius
      - radius_top_left > float
      - radius_top_right > float
      - radius_bottom_left > float
      - radius_bottom_right > float
    - border_color > color
    - background_color > color
---
- The check box check's subelement name is ``check`` instead of ``check_box.check``. It uses all of the same style variables as ``check_box``
#### Slider
- subelement name: slider
  - states:
    - hover
    - active
  - variables:
    - border
      - border_top > float
      - border_bottom > float
      - border_left > float
      - border_right > float
    - padding
      - padding_top > float
      - padding_bottom > float
      - padding_left > float
      - padding_right > float
    - radius
      - radius_top_left > float
      - radius_top_right > float
      - radius_bottom_left > float
      - radius_bottom_right > float
    - border_color > color
    - background_color > color
---
- The slider handles's subelement name is ``slider_handle`` instead of ``slider.handle``. It uses all of the same style variables as ``slider``
  
  



