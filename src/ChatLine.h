/*
 * ChatLine.h
 *
 *  Created on: 14 Nov 2018
 *      Author: rkfg
 */

#ifndef SRC_CHATLINE_H_
#define SRC_CHATLINE_H_

#include <Wt/WContainerWidget.h>
#include <regex>
#include "State.h"

class ChatLine: public Wt::WContainerWidget {
private:
    Wt::WLabel* m_label;
public:
    const Message m_msg;
    ChatLine(const Message& msg);
    Wt::EventSignal<Wt::WMouseEvent>& clicked();
};

#endif /* SRC_CHATLINE_H_ */
