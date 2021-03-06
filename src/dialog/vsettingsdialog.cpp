#include "vsettingsdialog.h"
#include <QtWidgets>
#include <QRegExp>
#include "vconfigmanager.h"
#include "utils/vutils.h"
#include "vconstants.h"

extern VConfigManager vconfig;

VSettingsDialog::VSettingsDialog(QWidget *p_parent)
    : QDialog(p_parent)
{
    m_tabs = new QTabWidget;
    m_tabs->addTab(new VGeneralTab(), tr("General"));
    m_tabs->addTab(new VReadEditTab(), tr("Read/Edit"));
    m_tabs->addTab(new VNoteManagementTab(), tr("Note Management"));

    m_btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_btnBox, &QDialogButtonBox::accepted, this, &VSettingsDialog::saveConfiguration);
    connect(m_btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_tabs);
    mainLayout->addWidget(m_btnBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Settings"));

    loadConfiguration();
}

void VSettingsDialog::loadConfiguration()
{
    // General Tab.
    {
        VGeneralTab *generalTab = dynamic_cast<VGeneralTab *>(m_tabs->widget(0));
        Q_ASSERT(generalTab);
        if (!generalTab->loadConfiguration()) {
            goto err;
        }
    }

    // Read/Edit Tab.
    {
        VReadEditTab *readEditTab = dynamic_cast<VReadEditTab *>(m_tabs->widget(1));
        Q_ASSERT(readEditTab);
        if (!readEditTab->loadConfiguration()) {
            goto err;
        }
    }

    // Note Management Tab.
    {
        VNoteManagementTab *noteManagementTab = dynamic_cast<VNoteManagementTab *>(m_tabs->widget(2));
        Q_ASSERT(noteManagementTab);
        if (!noteManagementTab->loadConfiguration()) {
            goto err;
        }
    }

    return;
err:
    VUtils::showMessage(QMessageBox::Warning, tr("Warning"),
                        tr("Fail to load configuration."), "",
                        QMessageBox::Ok, QMessageBox::Ok, NULL);
    QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection);
}

void VSettingsDialog::saveConfiguration()
{
    // General Tab.
    {
        VGeneralTab *generalTab = dynamic_cast<VGeneralTab *>(m_tabs->widget(0));
        Q_ASSERT(generalTab);
        if (!generalTab->saveConfiguration()) {
            goto err;
        }
    }

    // Read/Edit Tab.
    {
        VReadEditTab *readEditTab = dynamic_cast<VReadEditTab *>(m_tabs->widget(1));
        Q_ASSERT(readEditTab);
        if (!readEditTab->saveConfiguration()) {
            goto err;
        }
    }

    // Note Management Tab.
    {
        VNoteManagementTab *noteManagementTab = dynamic_cast<VNoteManagementTab *>(m_tabs->widget(2));
        Q_ASSERT(noteManagementTab);
        if (!noteManagementTab->saveConfiguration()) {
            goto err;
        }
    }

    accept();
    return;
err:
    VUtils::showMessage(QMessageBox::Warning, tr("Warning"),
                        tr("Fail to save configuration. Please try it again."), "",
                        QMessageBox::Ok, QMessageBox::Ok, NULL);
}

const QVector<QString> VGeneralTab::c_availableLangs = { "System", "English", "Chinese" };

VGeneralTab::VGeneralTab(QWidget *p_parent)
    : QWidget(p_parent)
{
    QLabel *langLabel = new QLabel(tr("&Language:"));
    m_langCombo = new QComboBox(this);
    m_langCombo->addItem(tr("System"), "System");
    auto langs = VUtils::getAvailableLanguages();
    for (auto const &lang : langs) {
        m_langCombo->addItem(lang.second, lang.first);
    }
    langLabel->setBuddy(m_langCombo);

    QFormLayout *optionLayout = new QFormLayout();
    optionLayout->addRow(langLabel, m_langCombo);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(optionLayout);

    setLayout(mainLayout);
}

bool VGeneralTab::loadConfiguration()
{
    if (!loadLanguage()) {
        return false;
    }
    return true;
}

bool VGeneralTab::saveConfiguration()
{
    if (!saveLanguage()) {
        return false;
    }
    return true;
}

bool VGeneralTab::loadLanguage()
{
    QString lang = vconfig.getLanguage();
    if (lang.isNull()) {
        return false;
    } else if (lang == "System") {
        m_langCombo->setCurrentIndex(0);
        return true;
    }
    bool found = false;
    // lang is the value, not name.
    for (int i = 0; i < m_langCombo->count(); ++i) {
        if (m_langCombo->itemData(i).toString() == lang) {
            found = true;
            m_langCombo->setCurrentIndex(i);
            break;
        }
    }
    if (!found) {
        qWarning() << "invalid language configuration (using default value)";
        m_langCombo->setCurrentIndex(0);
    }
    return true;
}

bool VGeneralTab::saveLanguage()
{
    QString curLang = m_langCombo->currentData().toString();
    vconfig.setLanguage(curLang);
    return true;
}

VReadEditTab::VReadEditTab(QWidget *p_parent)
    : QWidget(p_parent)
{
    m_readBox = new QGroupBox(tr("Read Mode (For Markdown Only)"));
    m_editBox = new QGroupBox(tr("Edit Mode"));

    // Web Zoom Factor.
    m_customWebZoom = new QCheckBox(tr("Custom Web zoom factor"), this);
    m_customWebZoom->setToolTip(tr("Set the zoom factor of the Web page when reading"));
    connect(m_customWebZoom, &QCheckBox::stateChanged,
            this, &VReadEditTab::customWebZoomChanged);
    m_webZoomFactorSpin = new QDoubleSpinBox(this);
    m_webZoomFactorSpin->setMaximum(c_webZoomFactorMax);
    m_webZoomFactorSpin->setMinimum(c_webZoomFactorMin);
    m_webZoomFactorSpin->setSingleStep(0.25);
    QHBoxLayout *zoomFactorLayout = new QHBoxLayout();
    zoomFactorLayout->addWidget(m_customWebZoom);
    zoomFactorLayout->addWidget(m_webZoomFactorSpin);

    QFormLayout *readLayout = new QFormLayout();
    readLayout->addRow(zoomFactorLayout);

    m_readBox->setLayout(readLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_readBox);
    mainLayout->addWidget(m_editBox);
    m_editBox->hide();
    setLayout(mainLayout);
}

bool VReadEditTab::loadConfiguration()
{
    if (!loadWebZoomFactor()) {
        return false;
    }
    return true;
}

bool VReadEditTab::saveConfiguration()
{
    if (!saveWebZoomFactor()) {
        return false;
    }
    return true;
}

bool VReadEditTab::loadWebZoomFactor()
{
    qreal factor = vconfig.getWebZoomFactor();
    bool customFactor = vconfig.isCustomWebZoomFactor();
    if (customFactor) {
        if (factor < c_webZoomFactorMin || factor > c_webZoomFactorMax) {
            factor = 1;
        }
        m_customWebZoom->setChecked(true);
        m_webZoomFactorSpin->setValue(factor);
    } else {
        m_customWebZoom->setChecked(false);
        m_webZoomFactorSpin->setValue(factor);
        m_webZoomFactorSpin->setEnabled(false);
    }
    return true;
}

bool VReadEditTab::saveWebZoomFactor()
{
    if (m_customWebZoom->isChecked()) {
        vconfig.setWebZoomFactor(m_webZoomFactorSpin->value());
    } else {
        vconfig.setWebZoomFactor(-1);
    }
    return true;
}

void VReadEditTab::customWebZoomChanged(int p_state)
{
    m_webZoomFactorSpin->setEnabled(p_state == Qt::Checked);
}

VNoteManagementTab::VNoteManagementTab(QWidget *p_parent)
    : QWidget(p_parent)
{
    // Image folder.
    m_customImageFolder = new QCheckBox(tr("Custom image folder"), this);
    m_customImageFolder->setToolTip(tr("Set the global name of the image folder to store images "
                                       "of notes (restart VNote to make it work)"));
    connect(m_customImageFolder, &QCheckBox::stateChanged,
            this, &VNoteManagementTab::customImageFolderChanged);

    m_imageFolderEdit = new QLineEdit(this);
    m_imageFolderEdit->setPlaceholderText(tr("Name of the image folder"));
    QValidator *validator = new QRegExpValidator(QRegExp(VUtils::c_fileNameRegExp), this);
    m_imageFolderEdit->setValidator(validator);

    QHBoxLayout *imageFolderLayout = new QHBoxLayout();
    imageFolderLayout->addWidget(m_customImageFolder);
    imageFolderLayout->addWidget(m_imageFolderEdit);

    QFormLayout *mainLayout = new QFormLayout();
    mainLayout->addRow(imageFolderLayout);

    setLayout(mainLayout);
}

bool VNoteManagementTab::loadConfiguration()
{
    if (!loadImageFolder()) {
        return false;
    }

    return true;
}

bool VNoteManagementTab::saveConfiguration()
{
    if (!saveImageFolder()) {
        return false;
    }

    return true;
}

bool VNoteManagementTab::loadImageFolder()
{
    bool isCustom = vconfig.isCustomImageFolder();

    m_customImageFolder->setChecked(isCustom);
    m_imageFolderEdit->setText(vconfig.getImageFolder());
    m_imageFolderEdit->setEnabled(isCustom);

    return true;
}

bool VNoteManagementTab::saveImageFolder()
{
    if (m_customImageFolder->isChecked()) {
        vconfig.setImageFolder(m_imageFolderEdit->text());
    } else {
        vconfig.setImageFolder("");
    }

    return true;
}

void VNoteManagementTab::customImageFolderChanged(int p_state)
{
    if (p_state == Qt::Checked) {
        m_imageFolderEdit->setEnabled(true);
        m_imageFolderEdit->selectAll();
        m_imageFolderEdit->setFocus();
    } else {
        m_imageFolderEdit->setEnabled(false);
    }
}
