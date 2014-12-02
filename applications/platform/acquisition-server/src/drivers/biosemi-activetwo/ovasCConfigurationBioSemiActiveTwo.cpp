/*
 * ovasCConfigurationBioSemiActiveTwo.cpp
 *
 * Copyright (c) 2012, Mensia Technologies SA. All rights reserved.
 * -- Rights transferred to Inria, contract signed 21.11.2014
 *
 */

#ifdef TARGET_HAS_ThirdPartyBioSemiAPI

#include "ovasCConfigurationBioSemiActiveTwo.h"
#include "ovasIHeader.h"
#include "mCBridgeBioSemiActiveTwo.h"

#include <system/Time.h>


#include <cstring>



using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
#define boolean OpenViBE::boolean

namespace
{
	static void gtk_combo_box_set_active_text(::GtkComboBox* pComboBox, const gchar* sActiveText)
	{
		::GtkTreeModel* l_pTreeModel=gtk_combo_box_get_model(pComboBox);
		::GtkTreeIter itComboEntry;
		int l_iIndex=0;
		gchar* l_sComboEntryName=NULL;
		if(gtk_tree_model_get_iter_first(l_pTreeModel, &itComboEntry))
		{
			do
			{
				gtk_tree_model_get(l_pTreeModel, &itComboEntry, 0, &l_sComboEntryName, -1);
				if(string(l_sComboEntryName)==string(sActiveText))
				{
					gtk_combo_box_set_active(pComboBox, l_iIndex);
					return;
				}
				else
				{
					l_iIndex++;
				}
			}
			while(gtk_tree_model_iter_next(l_pTreeModel, &itComboEntry));
		}
	}
};


CConfigurationBioSemiActiveTwo::CConfigurationBioSemiActiveTwo(const char* sGtkBuilderFileName)
	:CConfigurationBuilder(sGtkBuilderFileName)
{
}

boolean CConfigurationBioSemiActiveTwo::preConfigure(void)
{
	if(!CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	// deduced from connection to device, cannot be edited.
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_sampling_frequency")), false);

	Mensia::CBridgeBioSemiActiveTwo l_oBridge;
	boolean l_bStatus = false;
	if(l_oBridge.open() && l_oBridge.start())
	{
		// to let the device send a first packet from which the bridge can deduce the SF and channel count
		System::Time::sleep(500);
		if(l_oBridge.read() > 0)
		{
			gtk_image_set_from_stock(GTK_IMAGE(gtk_builder_get_object(m_pBuilderConfigureInterface, "image_status")),GTK_STOCK_YES,GTK_ICON_SIZE_BUTTON);
		
			gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_number_of_channels")), true);
			gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_apply")), true);
			gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_change_channel_names")), true);
		
			gtk_adjustment_set_upper(GTK_ADJUSTMENT(gtk_builder_get_object(m_pBuilderConfigureInterface, "adjustment_channel_count")), BIOSEMI_ACTIVETWO_MAXCHANNELCOUNT );
			//gtk_adjustment_set_value(GTK_ADJUSTMENT(gtk_builder_get_object(m_pBuilderConfigureInterface, "adjustment_channel_count")), l_oBridge.getChannelCount() );

			char l_sBuffer[32];
			sprintf(l_sBuffer, "%i", l_oBridge.getSamplingFrequency());
			gtk_combo_box_set_active_text(GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_sampling_frequency")), l_sBuffer);

			gtk_label_set_markup(GTK_LABEL(gtk_builder_get_object(m_pBuilderConfigureInterface, "label_device_mark")), (l_oBridge.isDeviceMarkII() ? "- <i>ActiveTwo Mark II</i> -" : "- <i>ActiveTwo Mark I</i> -"));
			gtk_label_set_markup(GTK_LABEL(gtk_builder_get_object(m_pBuilderConfigureInterface, "label_device_battery")), (l_oBridge.isBatteryLow() ? "<span color=\"red\"><b>LOW BATTERY</b></span> -" : "<b>BATTERY OK</b>"));

			// discard any data.
			l_oBridge.discard();

			l_bStatus = true;
		}
		l_oBridge.stop();
		l_oBridge.close();
	}

	if(!l_bStatus)
	{
		gtk_image_set_from_stock(GTK_IMAGE(gtk_builder_get_object(m_pBuilderConfigureInterface, "image_status")),GTK_STOCK_NO,GTK_ICON_SIZE_BUTTON);
		
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_number_of_channels")), false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_apply")), false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_change_channel_names")), false);
	}

	
	return true;
}

boolean CConfigurationBioSemiActiveTwo::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
	
	}

	if(!CConfigurationBuilder::postConfigure())
	{
		return false;
	}
	return true;
}

#endif
