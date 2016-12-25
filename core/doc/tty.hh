namespace xtd {
namespace tty {

/**
 ** @page core_tty Format text for terminal
 **
 ** [TOC]
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Introduction {#tty_intro}
 **
 ** ANSI escape codes are an in-band signaling method that controls format,
 ** color and other display option on text terminals.
 **
 ** XTD's @ref tty namespace defines classes that help to generate ANSI sequences,
 ** providing an easy way to format text on terminals.
 **
 ** ANSI codes can control the following display parameters :
 ** - The foreground color
 ** - The background color
 ** - Some additional display attributes such as underlined text, bold text,
 **   blinking text... etc
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Example {#tty_example}
 **
 ** @code
 ** #include <iostream>
 ** #include <tty.hh>
 **
 ** using namespace xtd;
 **
 ** int main(int, char**)
 ** {
 **   tty::style l_redOnGray(color::red, color::gray);
 **   tty::Text  l_text1("I'm written in red on gray background", l_redOnGray);
 **   std::cout << l_text1 << std::endl;
 **
 **   tty::style l_blinkingYellow(color::yellow, attrs::blink);
 **   tty::Text  l_text2("I'm blinking in yellow on default background", l_blinkingYellow);
 **   std::cout << l_text2 << std::endl;
 **
 **   tty::style l_blinkingBoldBlue(color::blue, attrs::blink | attrs::bold);
 **   tty::Text  l_text3("I'm blinking in blue on default background", l_blinkingBoldBlue);
 **   std::cout << l_text3 << std::endl;
 **
 **   std::cout << tty::Text("text chunk", color::red, color::gray, attrs::bold) << std::endl;
 **   return 0;
 ** }
 ** @endcode
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** # Classes {#tty_classes}
 **
 ** XTD's tty library defines the following classes :
 ** - @ref tty::attrs : @copybrief tty::attrs
 ** - @ref tty::color : @copybrief tty::color
 ** - @ref tty::style : @copybrief tty::style
 ** - @ref tty::Text  : @copybrief tty::Text
 **
 ** Relations among these objects are described in the following diagram :
 **
 ** @image html tty_uml.png
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Output options {#tty_classes_attrs}
 **
 ** Class @ref attrs
 **
 ** @copydoc attrs
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Color {#tty_classes_colors}
 **
 ** Class @ref color
 **
 ** @copydoc color
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Style {#tty_classes_styles}
 **
 ** Class @ref style
 **
 ** @copydoc style
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 **
 ** ## Text {#tty_classes_text}
 **
 ** Class @ref Text
 **
 ** @copydoc Text
 **
 ** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
}}

// Local Variables:
// ispell-local-dictionary: "american"
// End:
