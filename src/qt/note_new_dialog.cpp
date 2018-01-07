/*
 note_new_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "note_new_dialog.h"

using namespace std;

namespace m8r {

NoteNewDialog::GeneralTab::GeneralTab(QWidget *parent)
    : QWidget(parent)
{
    QString s{};

    this->nameLabel = new QLabel(tr("Name")+":", this),
    this->nameEdit = new QLineEdit(tr("Note"), this);

    this->typeLabel = new QLabel(tr("Type")+":", this);
    this->typeCombo = new QComboBox(this);

    this->tagLabel = new QLabel(tr("Tag")+":", this);
    this->tagCombo = new QComboBox(this);

    this->positionLabel = new QLabel(tr("Position")+":", this);
    this->positionCombo = new QComboBox(this);

    this->stencilLabel = new QLabel(tr("Stencil")+":", this);
    this->stencilCombo = new QComboBox(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(typeLabel);
    mainLayout->addWidget(typeCombo);
    mainLayout->addWidget(tagLabel);
    mainLayout->addWidget(tagCombo);
    mainLayout->addWidget(positionLabel);
    mainLayout->addWidget(positionCombo);
    mainLayout->addWidget(stencilLabel);
    mainLayout->addWidget(stencilCombo);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void NoteNewDialog::GeneralTab::clean(void)
{
    nameEdit->setText(tr("Note"));
    nameEdit->selectAll();
    nameEdit->setFocus();
}

NoteNewDialog::GeneralTab::~GeneralTab(void)
{
    delete nameLabel;
    delete nameEdit;
    delete typeLabel;
    delete typeCombo;
    delete tagLabel;
    delete tagCombo;
    delete stencilLabel;
    delete stencilCombo;
}

NoteNewDialog::NoteNewDialog(
        Ontology& ontology,
        vector<Stencil*>& stencils,
        QWidget *parent)
    : QDialog(parent), ontology(ontology)
{
    tabWidget = new QTabWidget;

    generalTab = new GeneralTab(this);
    if(ontology.getNoteTypes().size()) {
        QComboBox* combo=generalTab->getTypeCombo();
        for(const NoteType* t:ontology.getNoteTypes().values()) {
            combo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const NoteType*>(t));
        }
        combo->setCurrentText(QString::fromStdString(ontology.getDefaultNoteType()->getName()));
    }
    if(ontology.getTags().size()) {
        QComboBox* combo=generalTab->getTagCombo();
        combo->addItem("", QVariant::fromValue<const Tag*>(nullptr));
        for(const Tag* t:ontology.getTags().values()) {
            if(!stringistring(string("none"), t->getName())) {
                combo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const Tag*>(t));
            }
        }
        combo->setCurrentText("");
    }
    QComboBox* combo=generalTab->getPositionCombo();
    // IMPROVE i18n - text is localized, QVariant keeps a constant (e.g. enum)
    combo->addItem("First child");
    combo->addItem("Last child");
    if(stencils.size()) {
        QComboBox* combo=generalTab->getStencilCombo();
        combo->addItem("", QVariant::fromValue<const Stencil*>(nullptr));
        for(Stencil* t:stencils) {
            if(t->getType()==ResourceType::NOTE) {
                combo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<Stencil*>(t));
            }
        }
        combo->setCurrentText("");
    }
    tabWidget->addTab(generalTab, tr("General"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // wire signals ensuring that close & set dialog status
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("New Note Dialog"));
}

void NoteNewDialog::show()
{
    generalTab->clean();
    QDialog::show();
}

NoteNewDialog::~NoteNewDialog(void)
{
    if(generalTab) delete generalTab;
}

}
