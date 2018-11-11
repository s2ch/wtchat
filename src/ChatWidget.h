/*
 * ChatWidget.h
 *
 *  Created on: 11 Nov 2018
 *      Author: rkfg
 */

#ifndef SRC_CHATWIDGET_H_
#define SRC_CHATWIDGET_H_

#include <Wt/WContainerWidget.h>
#include "utils.h"

class ChatWidget: public Wt::WContainerWidget {
public:
    ChatWidget();
    void addLine(const Wt::WString& line);
};

#endif /* SRC_CHATWIDGET_H_ */
