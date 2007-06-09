/***************************************************************************
 *   ktouchkeyboardeditordialog.cpp                                        *
 *   ------------------------------                                        *
 *   Copyright (C) 2000-2007 by Håvard Frøiland and Andreas Nicolai        *
 *   haavard@users.sourceforge.net, ghorwin@users.sourceforge.net          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktouchkeyboardeditordialog.h"
#include "ktouchkeyboardeditordialog.moc"

#include <QHeaderView>
#include <QTimer>

#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kfontdialog.h>
#include <kdebug.h>

#include "ktouch.h"
#include "ktouchopenrequestdialog.h"
#include "ktouchkey.h"
#include "ktouchkeyconnector.h"

// **************************
// ***** Public functions ***
// **************************

KTouchKeyboardEditorDialog::KTouchKeyboardEditorDialog(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent,fl)
{
	setupUi(this);

	m_keyboard = new KTouchKeyboard(this);

    m_scene = new QGraphicsScene(this);
	keyboardView->setScene(m_scene);
    keyboardView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    keyboardView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    keyboardView->setRenderHint(QPainter::TextAntialiasing);
    keyboardView->setBackgroundBrush(palette().brush(QPalette::Window));
    keyboardView->setFrameStyle(QFrame::NoFrame);

	connectorList->setMaximumSize(200000, 70);
	connectorList->setColumnCount(2);
	QHeaderView * hv = connectorList->horizontalHeader();
	hv->setMaximumSize(20000, 20);

	// setup the key edit fields
	keyTypeCombo->clear();
	for (int i=0; i<=KTouchKey::Other; ++i) {
		keyTypeCombo->addItem(KTouchKey::keyTypeString(static_cast<KTouchKey::keytype_t>(i)));
	}

	// loop over all languages supported in KDE and add the language code and language name 
	// to the langIDCombo box
	QStringList langlist = KGlobal::locale()->allLanguagesList();
	for (int i=0; i<langlist.count(); ++i) {
		QString langname = QString("%1 (%2)").arg(langlist[i]).arg(KGlobal::locale()->languageCodeToName(langlist[i]));
		langIDCombo->addItem(langname);
	}

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()) );
}
// -----------------------------------------------------------------------------

bool KTouchKeyboardEditorDialog::startEditor(const KUrl& url) {
    // call open request dialog and load a keyboard and start the dialogs event loop if
    // the user did not cancel the open request dialog 
    if (openKeyboardFile(url)==QDialog::Accepted)  {
		m_currentEditKey = NULL;
		keyClicked(NULL);
        exec();
		// Even if the user cancels the dialog we must assume that a keyboard layout 
		// was changed and save to disk. Thus, to be save, we return 'true' and
		// indicate that the keyboard has to be updated.
        return true; 
    }
    else  return false;
}
// -----------------------------------------------------------------------------


// ************************
// ***** Public slots *****
// ************************


void KTouchKeyboardEditorDialog::on_setFontButton_clicked() {
	//kDebug() << "Fontbutton clicked" << endl;
    QFont f;
    if (KFontDialog::getFont(f)==QDialog::Accepted) {
		m_keyboard->m_fontSuggestions = f.toString();
		m_keyboard->setFont(f);
		// update font
/*		titleEdit->setFont(f);  
		commentEdit->setFont(f);  
		langIDCombo->setFont(f);  
*/
		update();	// trigger repaint of the keyboard.
		setModified();
    }
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_openButton_clicked() {
    if (!saveModified()) return;  // save if modified
    openKeyboardFile(KUrl(""));
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_saveButton_clicked() {
    if (m_currentURL.isEmpty()) on_saveAsButton_clicked();
    else {
        transfer_from_dialog();
        m_keyboard->saveXML(this, m_currentURL);
        setModified(false);
    }
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_saveAsButton_clicked() {
    QString tmp = KFileDialog::getSaveFileName(m_currentURL, 
        i18n("*.keyboard.xml|KTouch Keyboard Files (*.keyboard.xml)\n*.*|All Files"), this, i18n("Save Keyboard Layout") );
    if (!tmp.isEmpty()) {
        transfer_from_dialog();
        m_currentURL = tmp;
        m_keyboard->saveXML(this, m_currentURL);
        setModified(false);
    }
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_topLeftChar_textEdited(const QString & text) {
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		if (!text.isEmpty())
			m_currentEditKey->m_keyChar[KTouchKey::TopLeft] = text[0];
		else
			m_currentEditKey->m_keyChar[KTouchKey::TopLeft] = QChar();
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_topRightChar_textEdited(const QString & text) {
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		if (!text.isEmpty())
			m_currentEditKey->m_keyChar[KTouchKey::TopRight] = text[0];
		else
			m_currentEditKey->m_keyChar[KTouchKey::TopRight] = QChar();
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_bottomLeftChar_textEdited(const QString & text) {
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		if (!text.isEmpty())
			m_currentEditKey->m_keyChar[KTouchKey::BottomLeft] = text[0];
		else
			m_currentEditKey->m_keyChar[KTouchKey::BottomLeft] = QChar();
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_bottomRightChar_textEdited(const QString & text) {
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		if (!text.isEmpty())
			m_currentEditKey->m_keyChar[KTouchKey::BottomRight] = text[0];
		else
			m_currentEditKey->m_keyChar[KTouchKey::BottomRight] = QChar();
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_keyTypeCombo_currentIndexChanged(int index) {
	if (index == KTouchKey::Other) {
		keyTextLabel->setEnabled(true);
		keyTextEdit->setEnabled(true);
	}
	else {
		keyTextLabel->setEnabled(false);
		keyTextEdit->setEnabled(false);
	}
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		m_currentEditKey->m_type = static_cast<KTouchKey::keytype_t>(index);
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_keyTextEdit_textEdited(const QString & text) {
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		m_currentEditKey->m_keyText = text;
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_deleteKeyButton_clicked(bool) {
	if (!m_keyboard->m_keys.contains(m_currentEditKey))  return; // don't mess with non existent keys
	// let user confirm to delete the key
	if (KMessageBox::questionYesNo(this, i18n("Really delete this key?")) == KMessageBox::Yes) {
		// remove key
		m_keyboard->deleteKey(m_currentEditKey);
		// disable controls
		m_currentEditKey = NULL;
		keyClicked(NULL);
		// update the graphics scene
		resizeKeyboard();
		m_scene->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_addKeyButton_clicked(bool) {
	// add a new key at coordinates 0,0
	int x = int(m_scene->sceneRect().left() + m_scene->sceneRect().width()/2);
	int y = int(m_scene->sceneRect().top() + m_scene->sceneRect().height()/2);
	KTouchKey * key = new KTouchKey(m_keyboard, KTouchKey::Normal, x, y, 80, 80, 'X');
	key->setZValue(0.1);
	m_keyboard->m_keys.append(key);
	m_scene->addItem(key);
	resizeKeyboard();
	key->setFlag(QGraphicsItem::ItemIsMovable, true);
	connect(key, SIGNAL(clicked(KTouchKey *)), this, SLOT(keyClicked(KTouchKey *)));
	connect(key, SIGNAL(positionChanged(KTouchKey *)), this, SLOT(keyPositionChanged(KTouchKey *)));	
	// select key
	keyClicked(key);
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_addConnectorButton_clicked(bool) {
	if (!m_keyboard->m_keys.contains(m_currentEditKey))  return;
	// check that we have either a character or a unicode number given
	bool error = false;
	int unicode;
	if (!conCharEdit->text().isEmpty()) {
		if (!conUnicodeEdit->text().isEmpty())
			error = true;
		else
			unicode = conCharEdit->text()[0].unicode();
	}
	else if (!conUnicodeEdit->text().isEmpty()) {
		bool ok;
		unicode = conUnicodeEdit->text().toInt(&ok,10);
		if (!ok) {
			// TODO : Fixme
			QMessageBox::warning(this, i18n("KTouch keyboard editor"), i18n("This is not a valid unicode number. Please correct the number or enter a character."));
			return;
		}
	}
	else {
		error = true;
	}
	if (error) {
			// TODO : Fixme
		QMessageBox::warning(this, i18n("KTouch keyboard editor"), i18n("Please enter either a character or a unicode number!"));
		return;
	}
	m_keyboard->m_connectors[unicode] = KTouchKeyConnector(unicode, m_currentEditKey, m_currentModifierKey);
	// update info
	keyClicked(m_currentEditKey);
	// clear input line edits to avoid accidental overwriting
	conCharEdit->clear();
	conUnicodeEdit->clear();
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_clearConnectorButton_clicked(bool) {
	if (!m_keyboard->m_keys.contains(m_currentEditKey))  return;
	if (QMessageBox::question(this, i18n("KTouch keyboard editor"), i18n("Delete all key connections for this key?"), 
		KMessageBox::Yes | KMessageBox::No) == KMessageBox::Yes)
	{
		QList<int> connectorsToRemove;
		for (QMap<int, KTouchKeyConnector>::iterator it = m_keyboard->m_connectors.begin(); 
			it != m_keyboard->m_connectors.end(); ++it) 
		{
			if ((*it).m_targetKey == m_currentEditKey) {
				connectorsToRemove.append(it.key());
				continue;
			}
		}
		for (int i=0; i<connectorsToRemove.count(); ++i) {
			//kDebug() << "Removing connector for unicode " << connectorsToRemove[i] << endl;
			m_keyboard->m_connectors.remove(connectorsToRemove[i]);
		}
	}
	keyClicked(m_currentEditKey);
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_selectFingerKeyButton_clicked(bool checked) {
	if (checked)
		selectModifierKeyButton->setChecked(false);
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_selectModifierKeyButton_clicked(bool checked) {
	if (checked)
		selectFingerKeyButton->setChecked(false);
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_leftSpinBox_valueChanged(int val) {
	if (m_keyboard->m_keys.contains(m_currentEditKey) && val != m_currentEditKey->m_x) {
		m_currentEditKey->m_x = val;
		m_currentEditKey->setPos(m_currentEditKey->m_x, m_currentEditKey->m_y);
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_topSpinBox_valueChanged(int val) {
	if (m_keyboard->m_keys.contains(m_currentEditKey) && val != m_currentEditKey->m_y) {
		m_currentEditKey->m_y = val;
		m_currentEditKey->setPos(m_currentEditKey->m_x, m_currentEditKey->m_y);
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_widthSpinBox_valueChanged(int val) {
	if (m_keyboard->m_keys.contains(m_currentEditKey) && val != m_currentEditKey->m_w) {
		m_currentEditKey->m_w = val;
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::on_heightSpinBox_valueChanged(int val) {
	if (m_keyboard->m_keys.contains(m_currentEditKey) && val != m_currentEditKey->m_h) {
		m_currentEditKey->m_h = val;
		m_currentEditKey->update();
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::resizeKeyboard() {
    QRectF sbr = m_scene->itemsBoundingRect();
	if (sbr.width() <= 0)  sbr.setWidth(1);
	if (sbr.height() <= 0)  sbr.setHeight(1);
    qreal scale = qMin(keyboardView->width()/sbr.width(), keyboardView->height()/sbr.height()) * 0.9;

    QMatrix matrix;
    matrix.scale(scale, scale);
    keyboardView->setMatrix(matrix);
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::closeEvent(QCloseEvent *event) {
	if (!saveModified())
		event->ignore();
	else
		event->accept();
}
// -----------------------------------------------------------------------------


void KTouchKeyboardEditorDialog::keyClicked(KTouchKey * k) {
	// are we currently selecting a finger key character?
	if (selectFingerKeyButton->isChecked() || selectModifierKeyButton->isChecked()) {
		// bail out if no key is currently selected
		if (!m_keyboard->m_keys.contains(m_currentEditKey)) 
			return;
		// check if the clicked key exists
		if (m_keyboard->m_keys.contains(k)) {
			// different treatment if finger key or modifier key is selected
			if (selectFingerKeyButton->isChecked()) {
				// check if the key is actually a finger key
				if (k->m_type != KTouchKey::Finger) {
					// FIXME
					QMessageBox::warning(this, i18n("KTouch keyboard editor error"), i18n("The selected key is not a finger key."));
					return;
				}
				else {
					m_currentEditKey->m_fingerKey = k;
					m_currentEditKey->m_colorIndex = k->m_colorIndex;
					selectFingerKeyButton->setChecked(false);
				}
			}
			else {
				m_currentModifierKey = k;
				QString modstr;
				if (k->m_type == KTouchKey::Other)
					modstr = k->m_keyText;
				else
					modstr = KTouchKey::keyTypeString(k->m_type);
				selectModifierKeyButton->setText( modstr );
				selectModifierKeyButton->setChecked(false);
			}
		}
		return; // stop here, don't select the key
	}
	// update the character edits
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		m_currentEditKey->m_state = KTouchKey::NormalState;
		m_currentEditKey->update();
	}
	m_currentEditKey = k;
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		keyPropertiesBox->setEnabled(true);
		keyGeometryBox->setEnabled(true);
		keyConnectorEditBox->setEnabled(true);
		m_currentEditKey->m_state = KTouchKey::HighlightedState;
		m_currentEditKey->update();
		
		// set correct combo type
		if (m_currentEditKey->m_type > KTouchKey::Other) 
			keyTypeCombo->setCurrentIndex( KTouchKey::Other );
		else 
			keyTypeCombo->setCurrentIndex( m_currentEditKey->m_type );
		if (m_currentEditKey->m_type == KTouchKey::Other)
			keyTextEdit->setText( m_currentEditKey->m_keyText );
		on_keyTypeCombo_currentIndexChanged( keyTypeCombo->currentIndex() );

		// fill in the keys in the edit lines
		if (m_currentEditKey->m_keyChar[KTouchKey::TopLeft] != QChar())
			topLeftChar->setText( m_currentEditKey->m_keyChar[KTouchKey::TopLeft] );
		else
			topLeftChar->clear();
		if (m_currentEditKey->m_keyChar[KTouchKey::TopRight] != QChar())
			topRightChar->setText( m_currentEditKey->m_keyChar[KTouchKey::TopRight] );
		else
			topRightChar->clear();
		if (m_currentEditKey->m_keyChar[KTouchKey::BottomLeft] != QChar())
			bottomLeftChar->setText( m_currentEditKey->m_keyChar[KTouchKey::BottomLeft] );
		else
			bottomLeftChar->clear();
		if (m_currentEditKey->m_keyChar[KTouchKey::BottomRight] != QChar())
			bottomRightChar->setText( m_currentEditKey->m_keyChar[KTouchKey::BottomRight] );
		else
			bottomRightChar->clear();

		// update geometry
		keyPositionChanged(k);
		
		// update key connector information
		connectorList->clear();
		int r = 0;
		for (QMap<int, KTouchKeyConnector>::iterator it = m_keyboard->m_connectors.begin();
			it != m_keyboard->m_connectors.end(); ++it)
		{
			// ignore connectors with different target character
			if ((*it).m_targetKey != m_currentEditKey) continue;

			QChar c(it.key()); // access character
			QString str;
			if (c.isPrint()) 
				str = QString("'%1' (%2)").arg(c).arg(c.unicode());
			else
				str = QString(" (%1)").arg(c.unicode());

			// do we have a modifier key?
			KTouchKey * mk = (*it).m_modifierKey;
			QString modstr;
			if (mk != NULL) {
				if (mk->m_type == KTouchKey::Other)
					modstr = mk->m_keyText;
				else
					modstr = KTouchKey::keyTypeString(mk->m_type);
			}
			connectorList->insertRow(r);
			QTableWidgetItem * w = new QTableWidgetItem(str);
			w->setFlags(Qt::ItemIsEnabled);
			connectorList->setItem(r, 0, w);
			w = new QTableWidgetItem(modstr);
			w->setFlags(Qt::ItemIsEnabled);
			connectorList->setItem(r, 1, w);
			++r;
		}
		connectorList->setRowCount(r);
		for (int i=0; i<r; ++i)
			connectorList->setRowHeight(i, 20);
		QStringList headers; headers << i18n("Character") << i18n("Modifier key");
		connectorList->setHorizontalHeaderLabels(headers);
		QHeaderView * hv = connectorList->horizontalHeader();
		hv->setMaximumSize(20000, 20);

		selectModifierKeyButton->setText( i18n("<modifier key>") );
		m_currentModifierKey = NULL;
	}
	else {
		// not a valid selection, disable the controls
		keyPropertiesBox->setEnabled(false);
		keyGeometryBox->setEnabled(false);
		keyConnectorEditBox->setEnabled(false);
	}
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::keyPositionChanged(KTouchKey * k) {
	m_currentEditKey = k;
	if (m_keyboard->m_keys.contains(m_currentEditKey)) {
		// set the geometry
		leftSpinBox->setValue(m_currentEditKey->m_x);
		topSpinBox->setValue(m_currentEditKey->m_y);
		widthSpinBox->setValue(m_currentEditKey->m_w);
		heightSpinBox->setValue(m_currentEditKey->m_h);
		// TODO : update scene boundary rect and scaling to keep keyboard centered at all times
	}
}
// -----------------------------------------------------------------------------

// ********************************
// ***** Event implementations ****
// ********************************

void KTouchKeyboardEditorDialog::resizeEvent(QResizeEvent *) {
	resizeKeyboard();
}
// -----------------------------------------------------------------------------

// ****************************
// ***** Private functions ****
// ****************************

void KTouchKeyboardEditorDialog::transfer_to_dialog() {
	if (m_currentURL.isEmpty()) {
		titleEdit->setText( i18n("untitled keyboard layout") );
		commentEdit->setText("");
	}
	else {
		// check if we have an xml extension
		QString fname = m_currentURL.fileName();
		if (!fname.endsWith(".xml")) {
			int pos = fname.indexOf('.');
			m_keyboard->m_language = fname.left(pos);
			m_keyboard->m_title = fname;
		}
		titleEdit->setText(m_keyboard->m_title);
		commentEdit->setText(m_keyboard->m_comment);
	}
	int index = langIDCombo->findText(m_keyboard->m_language);
	if (index == -1) {
		langIDCombo->addItem(m_keyboard->m_language);
		index = langIDCombo->count()-1;
	}
	langIDCombo->setCurrentIndex(index);
/*	if (!m_keyboard->m_fontSuggestions.isEmpty()) {
		QFont f;
		f.fromString(m_keyboard->m_fontSuggestions);
		titleEdit->setFont(f);
		commentEdit->setFont(f);
		langIDCombo->setFont(f);
	}
*/
	unsigned int min_x = 100000;
	unsigned int max_x = 0;
	unsigned int min_y = 100000;
	unsigned int max_y = 0;
	QList<KTouchKey*>::iterator it;
	for( it = m_keyboard->m_keys.begin(); it != m_keyboard->m_keys.end(); ++it ) {
		KTouchKey * key = *it;
        m_scene->addItem(key);
		key->setFlag(QGraphicsItem::ItemIsMovable, true);
		connect(key, SIGNAL(clicked(KTouchKey *)), this, SLOT(keyClicked(KTouchKey *)));
		connect(key, SIGNAL(positionChanged(KTouchKey *)), this, SLOT(keyPositionChanged(KTouchKey *)));

		min_x = qMin<unsigned int>(min_x, (*it)->m_x);
		max_x = qMax<unsigned int>(max_x, (*it)->m_x+(*it)->m_w);
		min_y = qMin<unsigned int>(min_y, (*it)->m_y);
		max_y = qMax<unsigned int>(max_y, (*it)->m_y+(*it)->m_h);
	}
	QTimer::singleShot(10, this, SLOT(resizeKeyboard()));
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::transfer_from_dialog() {
	m_keyboard->m_title = titleEdit->text();
	if (m_keyboard->m_title.isEmpty())  
		m_keyboard->m_title = i18n("untitled keyboard layout");
	m_keyboard->m_comment = commentEdit->toPlainText();
	m_keyboard->m_language = langIDCombo->currentText();
}
// -----------------------------------------------------------------------------

int KTouchKeyboardEditorDialog::openKeyboardFile(const KUrl& url) {
    // First setup the open request dialog
    KTouchOpenRequestDialog dlg(this);
    // Call the dialog
    KUrl new_url;
    int result = dlg.requestFileToOpen(new_url,
        i18n("Open Keyboard File"),
        i18n("Which keyboard file would you like to edit?"),
        i18n("Edit current keyboard:"),
        i18n("Open a default keyboard:"),
        i18n("Open a keyboard file:"),
        i18n("Create new keyboard"),
        url, KTouchPtr->keyboardFiles(), i18n("<no keyboard files available>"));

    if (result == QDialog::Accepted) {
        // Ok, user confirmed the dialog, now lets get the url
        m_currentURL = new_url;
        // Try to load the keyboard, if that fails, we create a new keyboard instead
		QString msg;
        if (!m_currentURL.isEmpty() && 
		    !m_keyboard->load(this, m_currentURL, msg) && !m_keyboard->loadXML(this, m_currentURL, msg)) 
		{
            KMessageBox::sorry(this, i18n("Could not open the keyboard file, creating a new one instead."));
            m_currentURL = ""; // new keyboards haven't got a URL
			m_keyboard->clear();
        }

        // If we have no URL, we create a new keyboard - can happen if either the user
        // chose "new keyboard" or the chosen keyboard could not be opened
        if (m_currentURL.isEmpty())  {
            m_keyboard->clear();
            setModified(true); // new keyboards are modified by default
        }
        else {
			// just for information, show the user possible read-warning messages
			if (!msg.isEmpty()) {
				KMessageBox::information(this,
					i18n("There were warnings while reading the keyboard file '%1':\n%2", m_currentURL.path(), msg));
			}
            setModified(false); // newly read keyboards are not modified in the begin
		}
        transfer_to_dialog();    // Update our editor with the keyboard data
        return QDialog::Accepted;
    }
    else return QDialog::Rejected;
}
// -----------------------------------------------------------------------------

void KTouchKeyboardEditorDialog::setModified(bool flag) {
    m_modified = flag;
    if (!m_currentURL.isEmpty()) {
        if (flag) this->setWindowTitle("KTouch Keyboard Editor - " + m_currentURL.url() + i18n(" (modified)"));
        else      this->setWindowTitle("KTouch Keyboard Editor - " + m_currentURL.url());
    }
	else
		this->setWindowTitle("KTouch Keyboard Editor - " + i18n("<unnamed keyboard file>"));
}
// -----------------------------------------------------------------------------

bool KTouchKeyboardEditorDialog::saveModified() {
    if (!m_modified) return true;
    // ok, ask the user to save the changes
    int result = KMessageBox::questionYesNoCancel(this, i18n("The keyboard has been changed. Do you want to save the changes?"),
		0, KStandardGuiItem::yes(), KStandardGuiItem::no(), KStandardGuiItem::cancel() );
    if (result == KMessageBox::Cancel) 
		return false; // User aborted
    if (result == KMessageBox::Yes) 
		on_saveButton_clicked();
    // if successfully saved the modified flag will be reset in the saveBtnClicked() function
    return true; // User acknowledged
}
// -----------------------------------------------------------------------------
