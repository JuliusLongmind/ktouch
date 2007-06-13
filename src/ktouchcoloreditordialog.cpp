/***************************************************************************
 *   ktouchcoloreditor.cpp                                                 *
 *   ---------------------                                                 *
 *   Copyright (C) 2006 by Andreas Nicolai                                 *
 *   ghorwin@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktouchcoloreditordialog.h"
#include "ktouchcoloreditordialog.moc"

#include <QCloseEvent>

#include <kcolorbutton.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kmessagebox.h>

KTouchColorEditorDialog::KTouchColorEditorDialog(QWidget* parent)
: QDialog(parent)
{
	setupUi(this);
	connect(addBtn,SIGNAL(clicked()),SLOT(addBtnClicked()));
	connect(removeBtn,SIGNAL(clicked()),SLOT(removeBtnClicked()));
	connect(schemeListWidget,SIGNAL(itemClicked(QListWidgetItem*)),SLOT(colorSchemeChanged(QListWidgetItem*)));
	connect(updateBtn,SIGNAL(clicked()),SLOT(updateClicked()));
}
// ----------------------------------------------------------------------------

KTouchColorEditorDialog::~KTouchColorEditorDialog()
{
}
// ----------------------------------------------------------------------------

void KTouchColorEditorDialog::startEditor(QList<KTouchColorScheme>& schemes, int active, int & selected) {
	m_schemes = schemes;
	m_currentItem = qMax(0, active);
	updateListWidget();

	if (active >= 0)	m_currentItem = active;
	else				m_currentItem = -1;

	exec();

	if (m_saveChanges) {
		selected = m_currentItem;
		schemes = m_schemes;
	}
	else {
		selected = active;
	}
}
// ----------------------------------------------------------------------------

void KTouchColorEditorDialog::colorSchemeChanged(QListWidgetItem *) {
//	kdDebug() << "[KTouchColorEditor::colorSchemeChanged]" << endl;
    int num = schemeListWidget->currentRow();
	if (num < 0 || num >= static_cast<int>(m_schemes.count())) {
		updateControls(NULL);
		return;
	}
	QList<KTouchColorScheme>::const_iterator it = m_schemes.begin();
	while (--num >= 0) ++it;
	updateControls(&(*it));
}
// ----------------------------------------------------------------------------

void KTouchColorEditorDialog::updateClicked() {
	// first check that we have a valid color scheme selected
    int num = schemeListWidget->currentRow();
	int old_num = num;
	if (num < 0 || num >= static_cast<int>(m_schemes.count())) {
		updateListWidget();
		return;
	}
	QList<KTouchColorScheme>::iterator it = m_schemes.begin();
	while (--num >= 0) ++it;
	(*it).m_name = nameEdit->text();
	if ((*it).m_name.isEmpty())
		(*it).m_name = i18n("New color scheme");
	// slide line colors
	(*it).m_teacherTextColor = teacherTextBtn->color();
	(*it).m_teacherBackground = teacherBackBtn->color();
	(*it).m_studentTextColor = studentTextBtn->color();
	(*it).m_studentBackground = studentBackBtn->color();
	(*it).m_errorTextColor = studentErrorTextBtn->color();
	(*it).m_errorBackground = studentErrorBackBtn->color();
	// keyboard colors
    (*it).m_frame = keyFrameBtn->color();
    (*it).m_text = keyTextBtn->color();
    (*it).m_textH = keyTextHighBtn->color();
    (*it).m_backgroundH = keyBackHighBtn->color();
    (*it).m_cText = cTextBtn->color();
    (*it).m_cBackground = cBackgroundBtn->color();
    (*it).m_cTextH = cTextHighBtn->color();
    (*it).m_cBackgroundH = cBackHighBtn->color();

    (*it).m_background[0] = back1Btn->color();
    (*it).m_background[1] = back2Btn->color();
    (*it).m_background[2] = back3Btn->color();
    (*it).m_background[3] = back4Btn->color();
    (*it).m_background[4] = back5Btn->color();
    (*it).m_background[5] = back6Btn->color();
    (*it).m_background[6] = back7Btn->color();
    (*it).m_background[7] = back8Btn->color();

	updateListWidget();
	schemeListWidget->setCurrentRow( qMin(old_num, static_cast<int>(m_schemes.count())-1) );
	colorSchemeChanged(NULL);
}
// ----------------------------------------------------------------------------

void KTouchColorEditorDialog::removeBtnClicked() {
    int num = schemeListWidget->currentRow();
	int old_num = num;
	if (num >=0 && num < static_cast<int>(m_schemes.count())) {
		QList<KTouchColorScheme>::iterator it = m_schemes.begin();
		while (--num >= 0) ++it;
		m_schemes.erase(it);
	}
	updateListWidget();
	schemeListWidget->setCurrentRow( qMin(old_num, static_cast<int>(m_schemes.count())-1) );
	colorSchemeChanged(NULL);
}
// ----------------------------------------------------------------------------

void KTouchColorEditorDialog::addBtnClicked() {
	KTouchColorScheme s;
	s.m_name = i18n("New color scheme");
	m_schemes.append(s);
	updateListWidget();
	schemeListWidget->setCurrentRow( m_schemes.count() - 1 );
	colorSchemeChanged(NULL);
}
// ----------------------------------------------------------------------------

void KTouchColorEditorDialog::closeEvent( QCloseEvent* ce ) {
	int result = KMessageBox::questionYesNoCancel(this,
		i18n("Save modified color schemes?"));
	switch (result) {
	  case KMessageBox::Cancel : m_saveChanges = false; ce->ignore(); return;
	  case KMessageBox::Yes :
		m_currentItem = schemeListWidget->currentRow();
		updateClicked();
		m_saveChanges = true; break;
	  default : m_saveChanges = false; break;
	}
	ce->accept();
}
// ----------------------------------------------------------------------------

void KTouchColorEditorDialog::updateListWidget() {
//	kdDebug() << "[KTouchColorEditor::updateListBox]" << endl;
	schemeListWidget->clear();
	for (QList<KTouchColorScheme>::const_iterator it = m_schemes.constBegin();
		it != m_schemes.constEnd(); ++it)
	{
		schemeListWidget->addItem( (*it).m_name );
	}
	if (m_schemes.isEmpty()) {
		editGroupBox->setEnabled(false);
		removeBtn->setEnabled(false);
	}
	else {
		editGroupBox->setEnabled(true);
		removeBtn->setEnabled(true);
		// select the first one by default
	}
}
// ----------------------------------------------------------------------------

void KTouchColorEditorDialog::updateControls(const KTouchColorScheme * cs) {
//	kdDebug() << "[KTouchColorEditor::updateControls]" << endl;
	if (cs==NULL) {
		// TODO : also set all colors back to defaults
		editGroupBox->setEnabled(false);
		return;
	}
	editGroupBox->setEnabled(true);
	nameEdit->setText(cs->m_name);
	// slide line colors
	teacherTextBtn->setColor(cs->m_teacherTextColor);
	teacherBackBtn->setColor(cs->m_teacherBackground);
	studentTextBtn->setColor(cs->m_studentTextColor);
	studentBackBtn->setColor(cs->m_studentBackground);
	studentErrorTextBtn->setColor(cs->m_errorTextColor);
	studentErrorBackBtn->setColor(cs->m_errorBackground);
	// keyboard colors
    keyFrameBtn->setColor(cs->m_frame);
    keyTextBtn->setColor(cs->m_text);
    keyTextHighBtn->setColor(cs->m_textH);
    keyBackHighBtn->setColor(cs->m_backgroundH);
    cTextBtn->setColor(cs->m_cText);
    cBackgroundBtn->setColor(cs->m_cBackground);
    cTextHighBtn->setColor(cs->m_cTextH);
    cBackHighBtn->setColor(cs->m_cBackgroundH);

    back1Btn->setColor(cs->m_background[0]);
    back2Btn->setColor(cs->m_background[1]);
    back3Btn->setColor(cs->m_background[2]);
    back4Btn->setColor(cs->m_background[3]);
    back5Btn->setColor(cs->m_background[4]);
    back6Btn->setColor(cs->m_background[5]);
    back7Btn->setColor(cs->m_background[6]);
    back8Btn->setColor(cs->m_background[7]);
}
// ----------------------------------------------------------------------------


