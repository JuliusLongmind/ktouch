/***************************************************************************
 *   ktouchsettings.h                                                      *
 *   ----------------                                                      *
 *   Copyright (C) 2000 by H�vard Fr�iland, 2003 by Andreas Nicolai        *
 *   haavard@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTOUCHSETTINGS_H
#define KTOUCHSETTINGS_H

#include <kurl.h>
#include <qcolor.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qfont.h>

#include "ktouchkeyboardcolor.h"

/** The KTouch configuration object.
 *  There should be only one configuration object in the program so this class
 *  is a singleton. That means you cannot create instances from it. The one and
 *  only instance of the configuration object can be retrieved by the function
 *  KTouchConfig() which returns a reference to the configuration object.<p>
 *  Wherever you need to access the configuration settings you can simply
 *  include this file and use for example
 *  @code
 *   if (KTouchConfig().m_errorBeep)
 *      // make some noise
 *  @endcode
 *  <p>The configuration data is read once (at startup) and will be written
 *  whenever you quit the preferences dialog with "Ok", hit the "Apply" button
 *  in the preferences dialog and quit the program. The member functions
 *  for reading and writing the config data are loadSettings() and saveSettings().
 */
class KTouchSettings {
  public:
    /// Possible types for the key hightlighting/animation method.
    enum KeyAnimation_t {
        KA_NONE,
        KA_SHADE,
        KA_SQUARE,
        KA_CIRCLE,
        NUM_KA
    };

    /// Loads the program settings (preferences) from the config file.
    /// Apart from reading the configuration data it builds the list of keyboard layouts, that are available.
    void loadSettings();
    /// Stores the program settings (preferences) in the config file
    void saveSettings();

    // general options
    bool            m_errorBeep;        ///< Whether to beep on error or not.
    bool            m_levelBeep;        ///< Whether to make a sound on level change or not.
    QFont           m_font;             ///< The font to be used in the slide lines.
    int             m_slideSpeed;       ///< The sliding speed (must be greater or equal 1, which means 'immediatly')

    // training page
    bool            m_autoLevelChange;  ///< Whether to use automatic level change or not.
    int             m_downCorrectLimit; ///< If user types less correct, the level will be descreased.
    int             m_downSpeedLimit;   ///< If user types slower, the level will be descreased.
    int             m_upCorrectLimit;   ///< If user types more correct, the level will be increased.
    int             m_upSpeedLimit;     ///< If user types faster, the level will be increased.
    bool            m_rememberLevel;    ///< If true, the old level will be set when starting KTouch normally.

    // keyboard page
    bool            m_useColorKeys;     ///< Whether to use colours on the keys or not.
    int             m_keyAnimationType; ///< What kind of key highlighting would you like to have.
    QString         m_keyboardLayout;   ///< The currently choosen keyboard layout.
    QStringList     m_keyboardLayouts;  ///< The list of available keyboard layouts.
    bool            m_showAnimation;    ///< If the keyboard shoud animate keys or not.

    // colors page
    bool            m_useErrorColor;       ///< Whether to use a different background for wrong text or not.
    QColor          m_teacherTextColor;    ///< The text color for the teachers line.
    QColor          m_teacherBackground;   ///< The background color for the teachers line.
    QColor          m_studentTextColor;    ///< The text color for the students line.
    QColor          m_studentBackground;   ///< The background color for the students line.
    QColor          m_errorTextColor;      ///< The text color for wrong text (students line).
    QColor          m_errorBackground;     ///< The background color for wrong text (students line).

    // other stuff not in the preferences dialog
    QStringList     m_lectureList;      ///< A list of all lecture files found in the standard dirs (filenames only).
    QValueList<KTouchKeyboardColor> m_keyboardColors;   ///< A vector holding the colour schemes for the keyboard.
    unsigned int    m_keyboardColorScheme;              ///< Indicates which colour scheme will be used.

  private:
    /// We hide our constructor so that only our friend KTouchConfig()
    /// can create the config object.
    KTouchSettings() {};
    /// Hide the copy constructor (we want only ONE config object).
    KTouchSettings(const KTouchSettings&);
    /// Creates some default keyboard colors.
    void createDefaultKeyboardColors();

    /// This function returns our one and only settings object.
    friend KTouchSettings& KTouchConfig();
};

/// Returns a pointer to the global settings object.
KTouchSettings& KTouchConfig();

#endif  // KTOUCHSETTINGS_H