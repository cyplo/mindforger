/*
 outline_view.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef M8RUI_OUTLINE_VIEW_H
#define M8RUI_OUTLINE_VIEW_H

#include <QtWidgets>

#include "lib/src/gear/lang_utils.h"
#include "outline_tree_view.h"

namespace m8r {

class OutlineView : public QWidget
{
    Q_OBJECT

private:
    // header
    QVBoxLayout headerVerticalLayout;
    QPushButton* titleLabel;
    //QLabel* labelsLabel;
    // notes tree
    OutlineTreeView *outlineTreeView;

public:
    explicit OutlineView(QWidget* parent);
    OutlineView(const OutlineView&) = delete;
    OutlineView(const OutlineView&&) = delete;
    OutlineView &operator=(const OutlineView&) = delete;
    OutlineView &operator=(const OutlineView&&) = delete;

    void refreshHeader(const std::string& title);
    const QPushButton* getTitleLabel() const { return titleLabel; }
    OutlineTreeView* getOutlineTree() const { return outlineTreeView; }

    ~OutlineView();
};

}
#endif // M8RUI_OUTLINE_VIEW_H
