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
#include "State.h"

struct Message;

class ChatWidget: public Wt::WContainerWidget {
private:
    Wt::Signal<Message> m_name_clicked;
    void cleanup();
public:
    ChatWidget();
    void addLine(const Message& msg);
    void addLine(const Wt::WString& msg);
    Wt::Signal<Message>& nameClicked();
};

#endif /* SRC_CHATWIDGET_H_ */
