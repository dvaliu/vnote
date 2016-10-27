#ifndef VCONFIGMANAGER_H
#define VCONFIGMANAGER_H

#include <QFont>
#include <QPalette>
#include <QVector>
#include <QSettings>
#include "vnotebook.h"

#include "hgmarkdownhighlighter.h"
#include "vmarkdownconverter.h"

class QJsonObject;
class QString;

enum MarkdownConverterType
{
    Hoedown = 0,
    Marked
};

struct VColor
{
    QString name;
    QString rgb; // 'FFFFFF', ithout '#'
};

class VConfigManager
{
public:
    VConfigManager();
    ~VConfigManager();
    void initialize();

    // Static helper functions
    // Read config from the directory config json file into a QJsonObject
    static QJsonObject readDirectoryConfig(const QString &path);
    static bool writeDirectoryConfig(const QString &path, const QJsonObject &configJson);
    static bool deleteDirectoryConfig(const QString &path);

    // Constants
    static const QString orgName;
    static const QString appName;

    inline QFont getMdEditFont() const;

    inline QPalette getMdEditPalette() const;

    inline QVector<HighlightingStyle> getMdHighlightingStyles() const;

    inline QString getWelcomePagePath() const;

    inline QString getTemplatePath() const;

    inline QString getTemplateCssUrl() const;

    inline QFont getBaseEditFont() const;
    inline QPalette getBaseEditPalette() const;

    inline int getCurNotebookIndex() const;
    inline void setCurNotebookIndex(int index);

    inline const QVector<VNotebook>& getNotebooks() const;
    inline void setNotebooks(const QVector<VNotebook> &notebooks);

    inline hoedown_extensions getMarkdownExtensions() const;
    inline MarkdownConverterType getMdConverterType() const;
    inline void setMarkdownConverterType(MarkdownConverterType type);

    inline QString getPreTemplatePath() const;
    inline QString getPostTemplatePath() const;

    inline int getTabStopWidth() const;
    inline void setTabStopWidth(int tabStopWidth);
    inline bool getIsExpandTab() const;
    inline void setIsExpandTab(bool isExpandTab);

    inline const QVector<VColor> &getPredefinedColors() const;

    inline const QString &getCurBackgroundColor() const;
    inline void setCurBackgroundColor(const QString &colorName);

    inline const QString &getCustomBackgroundColor() const;
    inline void setCustomBackgroundColor(const QString &colorRgb);

private:
    void updateMarkdownEditStyle();
    QVariant getConfigFromSettings(const QString &section, const QString &key);
    void setConfigToSettings(const QString &section, const QString &key, const QVariant &value);
    void readNotebookFromSettings();
    void writeNotebookToSettings();
    void readPredefinedColorsFromSettings();
    // Update baseEditPalette according to curBackgroundColor
    void updatePaletteColor();

    QFont baseEditFont;
    QPalette baseEditPalette;
    QFont mdEditFont;
    QPalette mdEditPalette;
    QVector<HighlightingStyle> mdHighlightingStyles;
    QString welcomePagePath;
    QString templatePath;
    QString preTemplatePath;
    QString postTemplatePath;
    QString templateCssUrl;
    int curNotebookIndex;
    QVector<VNotebook> notebooks;

    // Markdown Converter
    hoedown_extensions markdownExtensions;
    MarkdownConverterType mdConverterType;

    // Num of spaces
    int tabStopWidth;
    // Expand tab to @tabStopWidth spaces
    bool isExpandTab;

    // App defined color
    QVector<VColor> predefinedColors;
    QString curBackgroundColor;
    QString customBackgroundColor;

    // The name of the config file in each directory
    static const QString dirConfigFileName;
    // The name of the default configuration file
    static const QString defaultConfigFilePath;
    // QSettings for the user configuration
    QSettings *userSettings;
    // Qsettings for @defaultConfigFileName
    QSettings *defaultSettings;
};


inline QFont VConfigManager::getMdEditFont() const
{
    return mdEditFont;
}

inline QPalette VConfigManager::getMdEditPalette() const
{
    return mdEditPalette;
}

inline QVector<HighlightingStyle> VConfigManager::getMdHighlightingStyles() const
{
    return mdHighlightingStyles;
}

inline QString VConfigManager::getWelcomePagePath() const
{
    return welcomePagePath;
}

inline QString VConfigManager::getTemplatePath() const
{
    return templatePath;
}

inline QString VConfigManager::getTemplateCssUrl() const
{
    return templateCssUrl;
}

inline QFont VConfigManager::getBaseEditFont() const
{
    return baseEditFont;
}

inline QPalette VConfigManager::getBaseEditPalette() const
{
    return baseEditPalette;
}

inline int VConfigManager::getCurNotebookIndex() const
{
    return curNotebookIndex;
}

inline void VConfigManager::setCurNotebookIndex(int index)
{
    if (index == curNotebookIndex) {
        return;
    }
    curNotebookIndex = index;
    setConfigToSettings("global", "current_notebook", index);
}

inline const QVector<VNotebook>& VConfigManager::getNotebooks() const
{
    return notebooks;
}

inline void VConfigManager::setNotebooks(const QVector<VNotebook> &notebooks)
{
    this->notebooks = notebooks;
    writeNotebookToSettings();
}

inline hoedown_extensions VConfigManager::getMarkdownExtensions() const
{
    return markdownExtensions;
}

inline MarkdownConverterType VConfigManager::getMdConverterType() const
{
    return mdConverterType;
}

inline QString VConfigManager::getPreTemplatePath() const
{
    return preTemplatePath;
}

inline QString VConfigManager::getPostTemplatePath() const
{
    return postTemplatePath;
}

inline void VConfigManager::setMarkdownConverterType(MarkdownConverterType type)
{
    if (mdConverterType == type) {
        return;
    }
    mdConverterType = type;
    setConfigToSettings("global", "markdown_converter", type);
}

inline int VConfigManager::getTabStopWidth() const
{
    return tabStopWidth;
}

inline bool VConfigManager::getIsExpandTab() const
{
    return isExpandTab;
}

inline void VConfigManager::setTabStopWidth(int tabStopWidth)
{
    if (tabStopWidth == this->tabStopWidth) {
        return;
    }
    this->tabStopWidth = tabStopWidth;
    setConfigToSettings("global", "tab_stop_width", tabStopWidth);
}

inline void VConfigManager::setIsExpandTab(bool isExpandTab)
{
    if (isExpandTab == this->isExpandTab) {
        return;
    }
    this->isExpandTab = isExpandTab;
    setConfigToSettings("global", "is_expand_tab", this->isExpandTab);
}

inline const QVector<VColor>& VConfigManager::getPredefinedColors() const
{
    return predefinedColors;
}

inline const QString& VConfigManager::getCurBackgroundColor() const
{
    return curBackgroundColor;
}

inline void VConfigManager::setCurBackgroundColor(const QString &colorName)
{
    if (curBackgroundColor == colorName) {
        return;
    }
    curBackgroundColor = colorName;
    setConfigToSettings("global", "current_background_color",
                        curBackgroundColor);
    updatePaletteColor();
}

inline const QString& VConfigManager::getCustomBackgroundColor() const
{
    return customBackgroundColor;
}

inline void VConfigManager::setCustomBackgroundColor(const QString &colorRgb)
{
    if (customBackgroundColor == colorRgb) {
        return;
    }
    customBackgroundColor = colorRgb;
    setConfigToSettings("global", "custom_background_color",
                        customBackgroundColor);
    updatePaletteColor();
}

#endif // VCONFIGMANAGER_H