#include "utils.h"

void doJs(Wt::WWidget* widget, Wt::WString jsText) {
    widget->doJavaScript(jsText.arg(widget->jsRef()).toUTF8());
}
