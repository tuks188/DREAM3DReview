/*
 * Your License or Copyright can go here
 */

#include "MASSIFUtilitiesPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"

#include "MASSIFUtilities/MASSIFUtilitiesConstants.h"
#include "MASSIFUtilities/MASSIFUtilitiesVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MASSIFUtilitiesPlugin::MASSIFUtilitiesPlugin()
: m_Version(MASSIFUtilities::Version::Package())
, m_CompatibilityVersion(MASSIFUtilities::Version::Package())
, m_Vendor("Open-Source")
, // Initialize MASSIFUtilities's Vendor Name Here
    m_URL("http://www.github.com/dream3D/MASSIFUtilities")
, // Initialize Company URL Here
    m_Location("")
, // Initialize MASSIFUtilities library Location Here
    m_Description("")
, // Initialize MASSIFUtilities's Description Here
    m_Copyright("")
, // Initialize MASSIFUtilities's Copyright Here
    m_Filters(QList<QString>())
, // Initialize MASSIFUtilities's List of Dependencies Here
    m_DidLoad(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MASSIFUtilitiesPlugin::~MASSIFUtilitiesPlugin() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getPluginFileName()
{
  return MASSIFUtilitiesConstants::MASSIFUtilitiesPluginFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getPluginDisplayName()
{
  return MASSIFUtilitiesConstants::MASSIFUtilitiesPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getPluginBaseName()
{
  return MASSIFUtilitiesConstants::MASSIFUtilitiesBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getDescription()
{
  /* PLEASE UPDATE YOUR PLUGIN'S DESCRIPTION FILE.
  It is located at MASSIFUtilities/Resources/MASSIFUtilities/MASSIFUtilitiesDescription.txt */

  QFile licenseFile(":/MASSIFUtilities/MASSIFUtilitiesDescription.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--Description was not read-->>";

  if(licenseFileInfo.exists())
  {
    if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getLicense()
{
  /* PLEASE UPDATE YOUR PLUGIN'S LICENSE FILE.
  It is located at MASSIFUtilities/Resources/MASSIFUtilities/MASSIFUtilitiesLicense.txt */

  QFile licenseFile(":/MASSIFUtilities/MASSIFUtilitiesLicense.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--License was not read-->>";

  if(licenseFileInfo.exists())
  {
    if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> MASSIFUtilitiesPlugin::getThirdPartyLicenses()
{
  QMap<QString, QString> licenseMap;
  QList<QString> fileStrList;
  fileStrList.push_back(":/ThirdParty/HDF5.txt");

  fileStrList.push_back(":/ThirdParty/Qt.txt");
  fileStrList.push_back(":/ThirdParty/Qwt.txt");

  for(QList<QString>::iterator iter = fileStrList.begin(); iter != fileStrList.end(); iter++)
  {
    QFile file(*iter);
    QFileInfo licenseFileInfo(file);

    if(licenseFileInfo.exists())
    {
      if(file.open(QIODevice::ReadOnly | QIODevice::Text))
      {
        QTextStream in(&file);
        licenseMap.insert(licenseFileInfo.baseName(), in.readAll());
      }
    }
  }

  return licenseMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MASSIFUtilitiesPlugin::getDidLoad()
{
  return m_DidLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MASSIFUtilitiesPlugin::setDidLoad(bool didLoad)
{
  m_DidLoad = didLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MASSIFUtilitiesPlugin::setLocation(QString filePath)
{
  m_Location = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MASSIFUtilitiesPlugin::writeSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MASSIFUtilitiesPlugin::readSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MASSIFUtilitiesPlugin::registerFilterWidgets(FilterWidgetManager* fwm)
{
}

#include "MASSIFUtilitiesFilters/RegisterKnownFilters.cpp"
