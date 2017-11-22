#define _CRT_SECURE_NO_WARNINGS

/////////////////////////////////////////////////
// Include 
#include "settingsManager.h"
#include <SFML\Graphics.hpp>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\info_parser.hpp>
#include <iostream>
#include <fstream>
#include <Windows.h>

/////////////////////////////////////////////////
// Class SettingsManager
SettingsManager::~SettingsManager()
{
	// Save new interface settings before exit
	writeData(SettingsFile_InterfaceSettings);
	writeData(SettingsFile_Staistic);
}

void SettingsManager::writeData(SettingsFile fileName)
{
	switch (fileName)
	{
	case SettingsFile_WindowSettings:
		boost::property_tree::info_parser::write_info(m_rep + "windowSettings.info", m_windowSettings);
		break;

	case SettingsFile_ObjectsSettings:
		boost::property_tree::info_parser::write_info(m_rep + "objectsParams.info", m_objectsSettings);
		break;

	case SettingsFile_InterfaceSettings:
		boost::property_tree::info_parser::write_info(m_rep + "m_interfaceSettings.info", m_interfaceSettings);
		break;

	case SettingsFile_Staistic:
		boost::property_tree::info_parser::write_info(m_rep + "statistic.info", m_statistic);
		break;
	}
}

void SettingsManager::initialize()
{
	// Get path to settings folder in AppData\Roaming
	m_rep = getenv("appdata");
	m_rep += "\\Box Jump\\Settings\\";

	// Read settings from files
	std::ifstream in(m_rep + "windowSettings.info");
	boost::property_tree::info_parser::read_info(in, m_windowSettings);
	p_windowSettings = &m_windowSettings;
	in.close();

	in.open(m_rep + "objectsParams.info");
	boost::property_tree::info_parser::read_info(in, m_objectsSettings);
	p_objectsSettings = &m_objectsSettings;
	in.close();

	in.open(m_rep + "interface.info");
	boost::property_tree::info_parser::read_info(in, m_interfaceSettings);
	p_interfaceSettings = &m_interfaceSettings;
	in.close();

	in.open(m_rep + "statistic.info");
	boost::property_tree::info_parser::read_info(in, m_statistic);
	p_statistic = &m_statistic;
	in.close();
}