/*
 * MENSIA TECHNOLOGIES CONFIDENTIAL
 * ________________________________
 *
 *  [2012] - [2013] Mensia Technologies SA
 *  Copyright, All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Mensia Technologies SA.
 * The intellectual and technical concepts contained
 * herein are proprietary to Mensia Technologies SA
 * and are covered copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Mensia Technologies SA.
 */
#pragma once

#define lic_studio  0x01
#define lic_archway 0x02
#define lic_engine  0x04
#define lic_private 0x08
#define lic_meddev  0x10

#if defined TARGET_OS_Windows
#include "m_ConverterUtf8.h"
#include <windows.h>
#include <ShellAPI.h>
#endif

#include "gtk/gtk.h"
#include <string>
#include <fstream>

namespace Mensia
{
	namespace ActivationTool
	{
		enum License
		{
			ELicenseStudio,
			ELicenseArchway,
			ELicenseEngine,
			ELicensePrivate,
			ELicenseMedDev,
			ELicenseCapacity
		};
		struct SLicenseInfo
		{
			bool m_bMissingLicense;
			int m_ui64LicenseId;
			GtkWidget* m_pEntryWidget;
			GtkWidget* m_pLabelWidget;
			GtkWidget* m_pImageWidget;
			bool m_bIsDependentFromStudio;
			bool m_bShowWhenAvailable;
			SLicenseInfo(){}
			SLicenseInfo(int ui64LicenseId, GtkWidget* pLabelWidget, GtkWidget* pImageWidget, 
				GtkWidget* pEntryWidget, bool bIsDependentFromStudio = false, bool bShowWhenAvailable = false) :
				m_ui64LicenseId(ui64LicenseId),
				m_pEntryWidget(pEntryWidget),
				m_pImageWidget(pImageWidget),
				m_pLabelWidget(pLabelWidget),
				m_bIsDependentFromStudio(bIsDependentFromStudio),
				m_bShowWhenAvailable(bShowWhenAvailable),
				m_bMissingLicense(false)
			{}
		} ;

		class CActivationTool
		{
		public:
			
			CActivationTool(): m_pInterface(NULL){}
			~CActivationTool(){}

			bool initialize(void);
			void launchGUI();
			bool uninitialize(void);
			void activateLicense(std::string sActivationCode);
			void refreshLicenseStatus();
			void browseLicenseFolder();
			void checkLicenseAndActivate(bool bLaunchActivate);
			void messageCloseWindow(const char* sTitle, const char* sMessage, GtkMessageType eType);
			void askForLicense();
		private:
			std::string m_sActivationTool_GUIFile;
			::GtkBuilder* m_pInterface;
			::GtkWidget* m_pDialog;
			std::string m_sOvPathRoot;
			std::string m_sActivationToolPath;
			SLicenseInfo m_vLicenseInfos[ELicenseCapacity];
		};
	};
};

namespace
{
	void message(const char* sTitle, const char* sMessage, GtkMessageType eType)
	{
		::GtkWidget* l_pDialog=::gtk_message_dialog_new(
			NULL,
			::GtkDialogFlags(GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT),
			eType,
			GTK_BUTTONS_OK,
			"%s", sTitle);
		::gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(l_pDialog), "%s", sMessage);
		::gtk_dialog_run(GTK_DIALOG(l_pDialog));
		::gtk_widget_destroy(l_pDialog);
	}

	int launchProcessAndWaitExitCode(std::string sCommand)
	{
		int l_iExitCode = 0;
#if defined TARGET_OS_Windows
		PROCESS_INFORMATION l_pProcessInfo;
		DWORD l_ui32TmpExitCode;
		try
		{
			std::wstring l_sUtf16Command = Common::Converter::utf8_to_utf16(sCommand);
			LPWSTR l_sCommand = const_cast<wchar_t*>(l_sUtf16Command.c_str());
			STARTUPINFOW l_pStartupInfoW;
			ZeroMemory(&l_pStartupInfoW, sizeof(STARTUPINFOW));
			if(!CreateProcessW(NULL, l_sCommand, NULL, NULL, TRUE, 0, NULL, NULL, &l_pStartupInfoW, &l_pProcessInfo))
			{
				//std::cout << GetLastError() << std::endl;
				return GetLastError();
			}
		}
		catch(...)
		{
			// The tool mensia neurort activation is used in command line
			LPSTR l_sCommand = const_cast<char*>(sCommand.c_str());
			STARTUPINFO l_pStartupInfo;
			ZeroMemory(&l_pStartupInfo, sizeof(STARTUPINFO));
			if(!CreateProcess(NULL, l_sCommand, NULL, NULL, TRUE, 0, NULL, NULL, &l_pStartupInfo, &l_pProcessInfo))
			{
				//std::cout << GetLastError() << std::endl;
				return GetLastError();
			}
		}
		// Wait for the process to end and retrieve its exit code
		WaitForSingleObject(l_pProcessInfo.hProcess, INFINITE);
		GetExitCodeProcess(l_pProcessInfo.hProcess, &l_ui32TmpExitCode);
		CloseHandle(l_pProcessInfo.hProcess);
		l_iExitCode = static_cast<int>(l_ui32TmpExitCode);
#else
		l_iExitCode = system(sCommand.c_str());
#endif
		return l_iExitCode;
	}

	
	static void on_button_activate_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<Mensia::ActivationTool::CActivationTool*>(pUserData)->checkLicenseAndActivate(true);
	}
	static void on_button_close_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<Mensia::ActivationTool::CActivationTool*>(pUserData)->checkLicenseAndActivate(false);
	}
	static void on_button_ask_license_cb(::GtkButton* pButton, gpointer pUserData)
	{
		static_cast<Mensia::ActivationTool::CActivationTool*>(pUserData)->askForLicense();
	}
};

using namespace Mensia;
using namespace ActivationTool;
bool CActivationTool::initialize(void)
{
#if defined TARGET_OS_Windows
	char l_sOvPathRoot[1024];
	GetEnvironmentVariable("OV_PATH_ROOT", l_sOvPathRoot, 1024);
	m_sOvPathRoot = std::string(l_sOvPathRoot);
	m_sActivationToolPath = m_sOvPathRoot + "/bin/mensia-neurort-activation.exe";
#else
	m_sOvPathRoot = getenv("OV_PATH_ROOT");
	m_sActivationToolPath = m_sOvPathRoot + "/bin/mensia-neurort-activation";
#endif
	m_sActivationTool_GUIFile = m_sOvPathRoot + "/share/mensia/neurort-activation/activation-interface.ui";
	m_pInterface=gtk_builder_new();
	if(gtk_builder_add_from_file(m_pInterface, m_sActivationTool_GUIFile.c_str(), NULL) == 0)
	{
		return false;
	}
	m_vLicenseInfos[ELicenseStudio] = SLicenseInfo(lic_studio, GTK_WIDGET(gtk_builder_get_object(m_pInterface, "label_studio")),
		GTK_WIDGET(gtk_builder_get_object(m_pInterface, "image_studio")), GTK_WIDGET(gtk_builder_get_object(m_pInterface, "entry_studio")));
	m_vLicenseInfos[ELicenseArchway] = SLicenseInfo(lic_archway, GTK_WIDGET(gtk_builder_get_object(m_pInterface, "label_archway")),
		GTK_WIDGET(gtk_builder_get_object(m_pInterface, "image_archway")), GTK_WIDGET(gtk_builder_get_object(m_pInterface, "entry_archway")), true);
	m_vLicenseInfos[ELicenseEngine] = SLicenseInfo(lic_engine, GTK_WIDGET(gtk_builder_get_object(m_pInterface, "label_engine")), 
		GTK_WIDGET(gtk_builder_get_object(m_pInterface, "image_engine")), GTK_WIDGET(gtk_builder_get_object(m_pInterface, "entry_engine")), true);
	m_vLicenseInfos[ELicensePrivate] = SLicenseInfo(lic_private, GTK_WIDGET(gtk_builder_get_object(m_pInterface, "label_private")),
		GTK_WIDGET(gtk_builder_get_object(m_pInterface, "image_private")), GTK_WIDGET(gtk_builder_get_object(m_pInterface, "entry_private")), false, true);
	m_vLicenseInfos[ELicenseMedDev] = SLicenseInfo(lic_meddev, GTK_WIDGET(gtk_builder_get_object(m_pInterface, "label_meddev")),
		GTK_WIDGET(gtk_builder_get_object(m_pInterface, "image_meddev")), GTK_WIDGET(gtk_builder_get_object(m_pInterface, "entry_meddev")), false, true);

	refreshLicenseStatus();
	return true;
}

void CActivationTool::launchGUI()
{
	m_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pInterface, "dialog_license_activation"));

	gtk_builder_connect_signals(m_pInterface, NULL);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pInterface, "button_apply")),       "clicked", G_CALLBACK(on_button_activate_cb),    this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pInterface, "button_close")),       "clicked", G_CALLBACK(on_button_close_cb),       this);
	g_signal_connect(G_OBJECT(gtk_builder_get_object(m_pInterface, "button_ask_license")), "clicked", G_CALLBACK(on_button_ask_license_cb), this);
	gtk_dialog_run(GTK_DIALOG(m_pDialog));
}

bool CActivationTool::uninitialize(void)
{
	gtk_widget_destroy(m_pDialog);
	g_object_unref(m_pInterface);
	m_pInterface = NULL;
	return true;
}

void CActivationTool::checkLicenseAndActivate(bool bLaunchActivate)
{
	for (SLicenseInfo l_oLicenseInfo : m_vLicenseInfos)
	{
		std::string l_sActivationCode = gtk_entry_get_text(GTK_ENTRY(l_oLicenseInfo.m_pEntryWidget));
		if(l_sActivationCode != "")
		{
			if(!bLaunchActivate)
			{
				messageCloseWindow("Activation code not used", "You entered an activation code but didn't activate it, are you sure you want to close ?", GTK_MESSAGE_QUESTION);
			}
			else
			{
				activateLicense(l_sActivationCode);
				gtk_entry_set_text(GTK_ENTRY(l_oLicenseInfo.m_pEntryWidget), "");
			}
		}
	}
}

void CActivationTool::activateLicense(std::string sActivationCode)
{
	if(sActivationCode.size() != 19)
	{
		message("Malformed activation key", "The activation token you entered is incorrect.", GTK_MESSAGE_ERROR);
		return;
	}

	int l_iExitCode = ::launchProcessAndWaitExitCode(m_sActivationToolPath + " --activate " + sActivationCode);

	switch(l_iExitCode)
	{
	case 0:
		//Activation was a success, refresh license status
		refreshLicenseStatus();
		message("Activation success", "Your license was correctly activated.", GTK_MESSAGE_INFO);
		break;
	case -103:
		// There was an issue while trying to use the activation tool
		message("Connection issue", "Your network connection seems to be inactive. To activate your software you need to be connected.", GTK_MESSAGE_ERROR);
	default:
		message("License activation", "Your license could not be activated. Check your activation token or send a mail at support@mensiatech.com to get more information.", GTK_MESSAGE_ERROR);
		break;
	}
}

void CActivationTool::refreshLicenseStatus()
{
	int l_iExitCode = ::launchProcessAndWaitExitCode(m_sActivationToolPath + " --checkout-status ");
	bool l_bIsMissingLicense = false;
	for (SLicenseInfo& l_oLicenseInfo : m_vLicenseInfos)
	{
		std::string l_sTooltipMessage = "";
		if( (l_iExitCode & l_oLicenseInfo.m_ui64LicenseId) != 0x00)
		{
			l_sTooltipMessage = "Your license is activated, you can fully use your product's functionality.";
			l_oLicenseInfo.m_bMissingLicense = false;
			gtk_widget_show(l_oLicenseInfo.m_pLabelWidget);
			gtk_widget_show(l_oLicenseInfo.m_pImageWidget);
			gtk_widget_hide(l_oLicenseInfo.m_pEntryWidget);
		}
		else if(l_oLicenseInfo.m_bIsDependentFromStudio && !m_vLicenseInfos[ELicenseStudio].m_bMissingLicense)
		{
			l_bIsMissingLicense = true;
			l_sTooltipMessage = "Studio license allows you to use this product in local configuration. Ask for a new license to activate Cloud and Lan options.";
			l_oLicenseInfo.m_bMissingLicense = true;
			gtk_widget_show(l_oLicenseInfo.m_pLabelWidget);
			gtk_widget_show(l_oLicenseInfo.m_pImageWidget);
			gtk_widget_show(l_oLicenseInfo.m_pEntryWidget);
		}
		else if(l_oLicenseInfo.m_bShowWhenAvailable)
		{
			l_sTooltipMessage = "";
			l_oLicenseInfo.m_bMissingLicense = false;
			gtk_widget_hide(l_oLicenseInfo.m_pLabelWidget);
			gtk_widget_hide(l_oLicenseInfo.m_pImageWidget);
			gtk_widget_hide(l_oLicenseInfo.m_pEntryWidget);
		}
		else
		{
			l_bIsMissingLicense = true;
			l_sTooltipMessage = "You don't have any active license for this product. To use it, you need to activate a license.";
			l_oLicenseInfo.m_bMissingLicense = true;
			gtk_widget_show(l_oLicenseInfo.m_pLabelWidget);
			gtk_widget_hide(l_oLicenseInfo.m_pImageWidget);
			gtk_widget_show(l_oLicenseInfo.m_pEntryWidget);
		}
		::gtk_widget_set_tooltip_markup(l_oLicenseInfo.m_pImageWidget, l_sTooltipMessage.c_str());
		::gtk_widget_set_tooltip_markup(l_oLicenseInfo.m_pEntryWidget, l_sTooltipMessage.c_str());
		::gtk_widget_set_tooltip_markup(l_oLicenseInfo.m_pLabelWidget, l_sTooltipMessage.c_str());
	}

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pInterface, "button_apply")), l_bIsMissingLicense);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pInterface, "button_ask_license")), l_bIsMissingLicense);
	if(!l_bIsMissingLicense)
	{
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(m_pInterface, "label_explanation")));
	}
}

void CActivationTool::browseLicenseFolder()
{
	::launchProcessAndWaitExitCode(m_sActivationToolPath + " --browse");
}

	
void CActivationTool::messageCloseWindow(const char* sTitle, const char* sMessage, GtkMessageType eType)
{
	::GtkWidget* l_pDialog=::gtk_message_dialog_new(
		NULL,
		::GtkDialogFlags(GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT),
		eType,
		GTK_BUTTONS_YES_NO,
		"%s", sTitle);
	::gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(l_pDialog), "%s", sMessage);
	gint l_iResult = ::gtk_dialog_run(GTK_DIALOG(l_pDialog));
	if(l_iResult == GTK_RESPONSE_NO)
	{
		::gtk_widget_destroy(l_pDialog);
	}
	else
	{
		::gtk_widget_destroy(l_pDialog);
		uninitialize();
	}
}

void CActivationTool::askForLicense()
{
	//char l_sProgramDataPath[1024];
	//GetEnvironmentVariable("PROGRAMDATA", l_sProgramDataPath, 1024);
	//sprintf(l_sProgramDataPath, "%s%s", l_sProgramDataPath, "/mensia/openvibe/license/neurort-0001.lic");
	//std::ifstream ifs("");
	//std::string content( (std::istreambuf_iterator<char>(ifs) ),
 //                      (std::istreambuf_iterator<char>()    ) );
#if defined TARGET_OS_Windows
	std::string l_sMailContent = "mailto:licenses@mensiatech.com?\
								 								 subject=License request&body=Hello,%0A%0AI would like a license file for the product:";
	
	for each (SLicenseInfo l_oLicenseInfo in m_vLicenseInfos)
	{
		if(l_oLicenseInfo.m_bMissingLicense && !l_oLicenseInfo.m_bShowWhenAvailable)
		{
			l_sMailContent += "%0A%0A    " + std::string(gtk_label_get_text(GTK_LABEL(l_oLicenseInfo.m_pLabelWidget)));
		}
	}
	l_sMailContent += ". %0A%0A Thanks!" ;
	ShellExecute(NULL, "open",l_sMailContent.c_str(), "", "", SW_SHOWNORMAL );
#endif
}
