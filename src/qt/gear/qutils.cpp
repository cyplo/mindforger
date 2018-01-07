/*
 qutils.cpp     MindForger thinking notebook

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
#include "qutils.h"

using namespace std;

namespace m8r {

void initRandomizer(void)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

void tagsToHtml(const vector<const Tag*>& tags, QString& html)
{
    //text += QString::fromUtf8("&nbsp;&nbsp;<table cellspacing='0' border='0' style='color: #ffffff; background-color: #00cc00; font-weight: normal;'><tr><td>urgent</td></tr></table>");
    if(!tags.empty()) {
        for(const Tag* t:tags) {
            if(!stringistring(string("none"), t->getName())) {
                html += QString::fromUtf8("&nbsp;&nbsp;<span style='color: #ffffff; background-color: ");
                html += QString::fromStdString(t->getColor().asHtml());
                html += QString::fromUtf8("; font-weight: normal;'> ");
                html += QString::fromStdString(t->getName());
                html += QString::fromUtf8(" </span>");
            }
        }
    }
}

void outlineTypeToHtml(const OutlineType* outlineType, QString& html)
{
    if(outlineType) {
        html += QString::fromUtf8("&nbsp;&nbsp;<span style='color: ");
        html += QString::fromStdString(outlineType->getColor().asHtml());
        html += QString::fromUtf8("; font-style: italic;'> ");
        html += QString::fromStdString(outlineType->getName());
        html += QString::fromUtf8(" </span>");
    }
}

void noteTypeToHtml(const NoteType* noteType, QString& html)
{
    if(noteType) {
        html += QString::fromUtf8("&nbsp;&nbsp;<span style='color: ");
        html += QString::fromStdString(noteType->getColor().asHtml());
        html += QString::fromUtf8("; font-style: italic;'> ");
        html += QString::fromStdString(noteType->getName());
        html += QString::fromUtf8(" </span>");
    }
}

}
