/*
 * Your License or Copyright can go here
 */

#include "MASSIFUtilitiesPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"

#include "MASSIFUtilities/MASSIFUtilitiesConstants.h"

// Include the MOC generated file for this class
#include "moc_MASSIFUtilitiesPlugin.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MASSIFUtilitiesPlugin::MASSIFUtilitiesPlugin() :
m_Version("0.1.0"),                            // Initialize MASSIFUtilities's Version Number Here
m_CompatibilityVersion("0.1.0"), // Initialize MASSIFUtilities's Compatibility Version Number Here
m_Vendor("Vendor Name"),                                // Initialize MASSIFUtilities's Vendor Name Here
m_URL("URL"),                                           // Initialize Company URL Here
m_Location("Location"),                                 // Initialize MASSIFUtilities library Location Here
m_Description("Description"),                           // Initialize MASSIFUtilities's Description Here
m_Copyright("Copyright"),                               // Initialize MASSIFUtilities's Copyright Here
m_Filters(QList<QString>()),                        // Initialize MASSIFUtilities's List of Dependencies Here
m_DidLoad(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MASSIFUtilitiesPlugin::~MASSIFUtilitiesPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MASSIFUtilitiesPlugin::getPluginName()
{
  return (MASSIFUtilitiesConstants::MASSIFUtilitiesPluginDisplayName);
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

  if ( licenseFileInfo.exists() )
  {
    if ( licenseFile.open(QIODevice::ReadOnly | QIODevice::Text) )
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

  if ( licenseFileInfo.exists() )
  {
    if ( licenseFile.open(QIODevice::ReadOnly | QIODevice::Text) )
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
  fileStrList.push_back(":/ThirdParty/Boost.txt");
  fileStrList.push_back(":/ThirdParty/Qt.txt");
  fileStrList.push_back(":/ThirdParty/Qwt.txt");

  for (QList<QString>::iterator iter = fileStrList.begin(); iter != fileStrList.end(); iter++)
  {
    QFile file(*iter);
    QFileInfo licenseFileInfo(file);

    if ( licenseFileInfo.exists() )
    {
      if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
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

#include "MASSIFUtilitiesFilters/RegisterKnownFilters.cpp"

#include "FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"

