// #if defined __MY_COMPILE_ALL

#include "ovkCSimulatedBox.h"
#include "ovkCPlayer.h"
#include "ovkCBoxAlgorithmContext.h"
#include "ovkCMessageClock.h"
#include "ovkCMessageEvent.h"
#include "ovkCMessageSignal.h"

#include "../visualisation/ovkCVisualisationManager.h"
#include "ovkCOgreVisualisation.h"
#include "ovkCOgreObject.h"
#include "ovkCOgreWindow.h"
#include "ovkCOgreScene.h"
#include "../ovkGtkOVCustom.h"

#include <cstdlib>
#include <algorithm>
#if defined TARGET_OS_Windows
#  include <gdk/gdkwin32.h>
#elif defined TARGET_OS_Linux
#  include <gdk/gdkx.h>
#elif defined TARGET_OS_MacOS
#  define Cursor XCursor
#  include <gdk/gdkx.h>
#  undef Cursor
#else
#endif

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

#if defined TARGET_HAS_ThirdPartyOgre3D
typedef Ogre::Real ogre_float;
typedef Ogre::uint32 ogre_uint32;
typedef Ogre::uint64 ogre_uint64;
typedef Ogre::uint16 ogre_uint16;
typedef Ogre::uint8  ogre_uint8;
#define uint8  OpenViBE::uint8
#define uint16 OpenViBE::uint16
#define uint32 OpenViBE::uint32
#define uint64 OpenViBE::uint64
#endif // TARGET_HAS_ThirdPartyOgre3D

#define boolean OpenViBE::boolean

//#define _BoxAlgorithm_ScopeTester_
//#define _SimulatedBox_ScopeTester_
#define _MaxCrash_ 5

#define __OV_FUNC__ CString("unknown_function_name")
#define __OV_LINE__ uint32(__LINE__)
#define __OV_FILE__ CString(__FILE__)

// ________________________________________________________________________________________________________________
//

namespace
{
	template <typename T>
	T& _my_get_(deque<T>& rList, uint32 ui32Index)
	{
		typename deque<T>::iterator it=rList.begin()+ui32Index;
		return *it;
	}

	template <typename T>
	const T& _my_get_(const deque<T>& rList, uint32 ui32Index)
	{
		typename deque<T>::const_iterator it=rList.begin()+ui32Index;
		return *it;
	}
}

// ________________________________________________________________________________________________________________
//

#define _Bad_Time_ 0xffffffffffffffffll
static const CNameValuePairList s_oDummyNameValuePairList;

CSimulatedBox::CSimulatedBox(const IKernelContext& rKernelContext, CScheduler& rScheduler)
	:TKernelObject<IBoxIO>(rKernelContext)
	,m_ui32CrashCount(0)
	,m_bReadyToProcess(false)
	,m_bSuspended(false)
	,m_bCrashed(false)
	,m_bChunkConsistencyChecking(false)
	,m_eChunkConsistencyCheckingLogLevel(LogLevel_Warning)
	,m_pBoxAlgorithm(NULL)
	,m_pScenario(NULL)
	,m_pBox(NULL)
	,m_rScheduler(rScheduler)
	,m_ui64LastClockActivationDate(_Bad_Time_)
	,m_ui64ClockFrequency(0)
	,m_ui64ClockActivationStep(0)
	,m_pOgreVis(NULL)
	,m_oSceneIdentifier(OV_UndefinedIdentifier)
	,m_bIsReceivingMessage(false)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

#if defined TARGET_HAS_ThirdPartyOgre3D
	m_pOgreVis = ((CVisualisationManager*)(&rKernelContext.getVisualisationManager()))->getOgreVisualisation();
#endif // TARGET_HAS_ThirdPartyOgre3D
}

CSimulatedBox::~CSimulatedBox(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

#if defined TARGET_HAS_ThirdPartyOgre3D
	//delete OgreWidgets
	std::map<GtkWidget*, CIdentifier>::iterator it;
	for(it = m_mOgreWindows.begin(); it != m_mOgreWindows.end(); it = m_mOgreWindows.begin())
	{
		//this will destroy widget then call handleDestroyEvent to destroy COgreWindow
		//WARNING : this invalidates iterator!
		gtk_widget_destroy(it->first);
	}
#endif // TARGET_HAS_ThirdPartyOgre3D

	//clear simulated objects map
	m_mSimulatedObjects.clear();

#if defined TARGET_HAS_ThirdPartyOgre3D
	//delete OgreScene
	if(m_pOgreVis)
	{
		m_pOgreVis->deleteScene(m_oSceneIdentifier);
	}
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::handleDestroyEvent(GtkWidget* pOVCustomWidget)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	m_pOgreVis->deleteWindow(m_mOgreWindows[pOVCustomWidget]);
	m_mOgreWindows.erase(pOVCustomWidget);
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::handleRealizeEvent(GtkWidget* pOVCustomWidget)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	//create render window embedded in this widget
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}

	// added for Ogre 1.7
	gtk_widget_realize(pOVCustomWidget);
	gtk_widget_set_double_buffered(pOVCustomWidget, FALSE);

	std::string l_sExternalHandle;
#if defined TARGET_OS_Windows
	l_sExternalHandle=Ogre::StringConverter::toString((unsigned long)GDK_WINDOW_HWND(pOVCustomWidget->window));
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	::XSync(GDK_WINDOW_XDISPLAY(pOVCustomWidget->window), False);

	::GdkDisplay* l_pGdkDisplay=gdk_drawable_get_display(GDK_DRAWABLE(pOVCustomWidget->window));
	::GdkScreen* l_pGdkScreen=gdk_drawable_get_screen(GDK_DRAWABLE(pOVCustomWidget->window));
	::GdkVisual* l_pGdkVisual=gdk_drawable_get_visual(GDK_DRAWABLE(pOVCustomWidget->window));

	::Display* l_pXDisplay=GDK_DISPLAY_XDISPLAY(l_pGdkDisplay);
	::Screen* l_pXScreen=GDK_SCREEN_XSCREEN(l_pGdkScreen);
	::XID l_pXWindow=GDK_WINDOW_XWINDOW(pOVCustomWidget->window);
	::Visual* l_pXVisual=GDK_VISUAL_XVISUAL(l_pGdkVisual);
	int l_iScreenIndex=::XScreenNumberOfScreen(l_pXScreen);

	::XVisualInfo l_oXVisualInfo;
	::memset(&l_oXVisualInfo, 0, sizeof(::XVisualInfo));
	l_oXVisualInfo.visual=l_pXVisual;
	l_oXVisualInfo.visualid=::XVisualIDFromVisual(l_pXVisual);
	l_oXVisualInfo.screen=l_iScreenIndex;
	l_oXVisualInfo.depth=24;

	l_sExternalHandle=
		Ogre::StringConverter::toString(reinterpret_cast<unsigned long>(l_pXDisplay))+":"+
		Ogre::StringConverter::toString(static_cast<unsigned int>(l_iScreenIndex))+":"+
		Ogre::StringConverter::toString(static_cast<unsigned long>(l_pXWindow))+":"+
		Ogre::StringConverter::toString(reinterpret_cast<unsigned long>(&l_oXVisualInfo));

	// added for Ogre 1.7
	l_sExternalHandle=Ogre::StringConverter::toString(static_cast<unsigned long>(l_pXWindow));
#else
		#error failed compilation
#endif

	try
	{
		l_pOgreWindow->createRenderWindow(l_sExternalHandle, pOVCustomWidget->allocation.width, pOVCustomWidget->allocation.height);
		getLogManager() << LogLevel_Trace << "Created 3D widget\n";
	}
	catch(Ogre::Exception& e)
	{
		getLogManager() << LogLevel_Error << "Could not create render window : " << e.what() << "\n";
		return false;
	}

	//handle realization
	l_pOgreWindow->handleRealizeEvent();
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::handleUnrealizeEvent(GtkWidget* pOVCustomWidget)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleUnrealizeEvent();
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::handleSizeAllocateEvent(GtkWidget* pOVCustomWidget, unsigned int uiWidth, unsigned int uiHeight)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleSizeAllocateEvent(uiWidth, uiHeight);
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::handleExposeEvent(GtkWidget* pOVCustomWidget)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleExposeEvent();
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::handleMotionEvent(GtkWidget* pOVCustomWidget, int i32X, int i32Y)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleMotionEvent(i32X, i32Y);
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::handleButtonPressEvent(GtkWidget* pOVCustomWidget, unsigned int uiButton, int i32X, int i32Y)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleButtonPressEvent(uiButton, i32X, i32Y);
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::handleButtonReleaseEvent(GtkWidget* pOVCustomWidget, unsigned int uiButton, int i32X, int i32Y)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(m_mOgreWindows[pOVCustomWidget]);
	if(l_pOgreWindow == NULL)
	{
		return false;
	}
	l_pOgreWindow->handleButtonReleaseEvent(uiButton, i32X, i32Y);
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

CIdentifier CSimulatedBox::create3DWidget(::GtkWidget*& p3DWidget)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	//don't attempt to create 3D widget if Ogre wasn't initialized properly.
	if(!m_pOgreVis || m_pOgreVis->ogreInitialized() == false || m_pOgreVis->resourcesInitialized() == false)
	{
		this->getLogManager() << LogLevel_Error << "Plugin " << m_pBox->getName() << " was disabled because the required 3D context couldn't be created!\n";
		m_bSuspended=true;
		return OV_UndefinedIdentifier;
	}

	//create Ogre widget
	::GtkWidget* l_pOVCustomWidget = gtk_ov_custom_new(this);
	p3DWidget = GTK_WIDGET(l_pOVCustomWidget);

	//create a window and generate an identifier for this widget
	CIdentifier l_oWindowIdentifier = createOgreWindow();

	//associate identifier to widget in a map
	m_mOgreWindows[l_pOVCustomWidget] = l_oWindowIdentifier;

	return l_oWindowIdentifier;
#else
	return OV_UndefinedIdentifier;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::is3DWidgetRealized(
	const CIdentifier& rWidgetIdentifier) const
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	return (m_pOgreVis!=NULL) && (m_pOgreVis->getOgreScene(m_oSceneIdentifier)!=NULL) && (m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getSceneManager()!=NULL);
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::update3DWidget(const CIdentifier& rWindowIdentifier)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	map<GtkWidget*, CIdentifier>::iterator it;

	for(it = m_mOgreWindows.begin(); it != m_mOgreWindows.end(); it++)
	{
		if(it->second == rWindowIdentifier)
		{
			m_pOgreVis->getOgreWindow(it->second)->update();
			return true;
		}
	}

	return false;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setBackgroundColor(const CIdentifier& rWindowIdentifier, float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(rWindowIdentifier);
	if(l_pOgreWindow != NULL)
	{
		l_pOgreWindow->setBackgroundColor(f32ColorRed, f32ColorGreen, f32ColorBlue);
	}
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setCameraToEncompassObjects(const CIdentifier& rWindowIdentifier)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(rWindowIdentifier);
	if(l_pOgreWindow != NULL)
	{
		l_pOgreWindow->setCameraToEncompassObjects();
	}
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setCameraSphericalCoordinates(const CIdentifier& rWindowIdentifier, float32 f32Theta, float32 f32Phi, float32 f32Radius)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(rWindowIdentifier);
	if(l_pOgreWindow != NULL)
	{
		l_pOgreWindow->setCameraSphericalCoordinates(f32Theta, f32Phi, f32Radius);
		return true;
	}
	else
	{
		return false;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::getCameraSphericalCoordinates(const CIdentifier& rWindowIdentifier, float32& rTheta, float32& rPhi, float32& rRadius)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreWindow* l_pOgreWindow = m_pOgreVis->getOgreWindow(rWindowIdentifier);
	if(l_pOgreWindow != NULL)
	{
		l_pOgreWindow->getCameraSphericalCoordinates(rTheta, rPhi, rRadius);
		return true;
	}
	else
	{
		return false;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

CIdentifier CSimulatedBox::createObject(const CString& rObjectFileName, const CNameValuePairList* pObjectParams)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	if(rObjectFileName == CString(""))
	{
		return OV_UndefinedIdentifier;
	}

	//generate a name from an identifier for this object
	CIdentifier l_oIdentifier = getUnusedIdentifier();

	string l_oSceneFileName(rObjectFileName);
	l_oSceneFileName += ".mesh";

	if(pObjectParams == NULL)
	{
		pObjectParams = &s_oDummyNameValuePairList;
	}

	if(m_pOgreVis->getOgreScene(m_oSceneIdentifier)->createObject(l_oIdentifier, l_oSceneFileName.c_str(), *pObjectParams) == NULL)
	{
		return OV_UndefinedIdentifier;
	}

	m_mSimulatedObjects[l_oIdentifier] = l_oIdentifier.toString();

	return l_oIdentifier;
#else
	return OV_UndefinedIdentifier;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

CIdentifier CSimulatedBox::createObject(const EStandard3DObject eStandard3DObject, const CNameValuePairList* pObjectParams)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	//TODO : read mapping of standard 3D objects to .scene file names from a config file
	if(eStandard3DObject == Standard3DObject_Sphere)
	{
		return createObject("sphere_ov", pObjectParams);
	}
	else if(eStandard3DObject == Standard3DObject_Cone)
	{
		return createObject("cone_ov", pObjectParams);
	}
	//TODO : create a cube_ov file!
	/*else if(eStandard3DObject == Standard3DObject_Cube)
	{
		return OV_UndefinedIdentifier;
		//return createObject("cube_ov", pObjectParams);
	}*/
	else if(eStandard3DObject == Standard3DObject_Quad)
	{
		//generate a name from an identifier for this object
		CIdentifier l_oIdentifier = getUnusedIdentifier();

		if(pObjectParams == NULL)
		{
			pObjectParams = &s_oDummyNameValuePairList;
		}

		if(m_pOgreVis->getOgreScene(m_oSceneIdentifier)->createPlane(l_oIdentifier, *pObjectParams) == NULL)
		{
			return OV_UndefinedIdentifier;
		}

		m_mSimulatedObjects[l_oIdentifier] = l_oIdentifier.toString();

		return l_oIdentifier;
	}
	else
	{
		return OV_UndefinedIdentifier;
	}
#else
	return OV_UndefinedIdentifier;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::removeObject(const CIdentifier& rObjectIdentifier)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	boolean res = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->removeObject(rObjectIdentifier);
	m_mSimulatedObjects.erase(rObjectIdentifier);
	return res;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setObjectVisible(const CIdentifier& rIdentifier, boolean bVisible)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setVisible(bVisible);
	}
	else
	{
		return false;
	}
#else
	return true;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setObjectScale(const CIdentifier& rIdentifier, float32 f32ScaleX, float32 f32ScaleY, float32 f32ScaleZ)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setScale(f32ScaleX, f32ScaleY, f32ScaleZ);
	}
	else
	{
		return false;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setObjectScale(const CIdentifier& rIdentifier, float32 f32Scale)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setScale(f32Scale, f32Scale, f32Scale);
	}
	else
	{
		return false;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setObjectPosition(const CIdentifier& rIdentifier, float32 f32PositionX, float32 f32PositionY, float32 f32PositionZ)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setPosition(f32PositionX, f32PositionY, f32PositionZ);
	}
	else
	{
		return false;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setObjectOrientation(const CIdentifier& rIdentifier, float32 f32OrientationX, float32 f32OrientationY,
	float32 f32OrientationZ, float32 f32OrientationW)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setRotation(f32OrientationX, f32OrientationY, f32OrientationZ, f32OrientationW);
	}
	else
	{
		return false;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setObjectColor(const CIdentifier& rIdentifier, float32 f32ColorRed, float32 f32ColorGreen, float32 f32ColorBlue)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setDiffuseColor(f32ColorRed, f32ColorGreen, f32ColorBlue);
	}
	else
	{
		return false;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::setObjectTransparency(const CIdentifier& rIdentifier, float32 f32Transparency)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject != NULL)
	{
		return l_pOgreObject->setTransparency(f32Transparency);
	}
	else
	{
		return false;
	}
#endif // TARGET_HAS_ThirdPartyOgre3D
	return false;
}

boolean CSimulatedBox::setObjectVertexColorArray(const CIdentifier& rIdentifier, const uint32 ui32VertexColorCount, const float32* pVertexColorArray)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}

	if(sizeof(ogre_float) == sizeof(float32))
	{
		return l_pOgreObject->setVertexColorArray((ogre_uint32)ui32VertexColorCount, (ogre_float*)pVertexColorArray);
	}
	else
	{
		ogre_float* l_pOgreVertexColorArray(new ogre_float[4*ui32VertexColorCount]);
		for(uint32 i=0; i<ui32VertexColorCount; i++)
		{
			l_pOgreVertexColorArray[4*i] = pVertexColorArray[4*i];
			l_pOgreVertexColorArray[4*i+1] = pVertexColorArray[4*i+1];
			l_pOgreVertexColorArray[4*i+2] = pVertexColorArray[4*i+2];
			l_pOgreVertexColorArray[4*i+3] = pVertexColorArray[4*i+3];
		}
		boolean l_bRes = l_pOgreObject->setVertexColorArray((ogre_uint32)ui32VertexColorCount, l_pOgreVertexColorArray);
		delete[] l_pOgreVertexColorArray;
		return l_bRes;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::getObjectAxisAlignedBoundingBox(const CIdentifier& rIdentifier, float32* pMinimum, float32* pMaximum)
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	ogre_float l_oMin[3];
	ogre_float l_oMax[3];
	l_pOgreObject->getWorldBoundingBox(l_oMin[0], l_oMin[1], l_oMin[2], l_oMax[0], l_oMax[1], l_oMax[2]);
	pMinimum[0] = l_oMin[0];
	pMinimum[1] = l_oMin[1];
	pMinimum[2] = l_oMin[2];
	pMaximum[0] = l_oMax[0];
	pMaximum[1] = l_oMax[1];
	pMaximum[2] = l_oMax[2];
	return true;
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::getObjectVertexCount(const CIdentifier& rIdentifier, uint32& ui32VertexCount) const
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	return l_pOgreObject->getVertexCount((ogre_uint32&)ui32VertexCount);
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::getObjectVertexPositionArray( const CIdentifier& rIdentifier, const uint32 ui32VertexCount, float32* pVertexPositionArray) const
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	if(sizeof(ogre_float) == sizeof(float32))
	{
		return l_pOgreObject->getVertexPositionArray((ogre_uint32)ui32VertexCount, (ogre_float*)pVertexPositionArray);
	}
	else
	{
		ogre_float* l_pOgreVertexPositionArray(new ogre_float[4*ui32VertexCount]);
		boolean l_bRes = l_pOgreObject->getVertexPositionArray((ogre_uint32)ui32VertexCount, pVertexPositionArray);
		if(l_bRes == true)
		{
			for(uint32 i=0; i<ui32VertexCount; i++)
			{
				pVertexPositionArray[4*i] = l_pOgreVertexPositionArray[4*i];
				pVertexPositionArray[4*i+1] = l_pOgreVertexPositionArray[4*i+1];
				pVertexPositionArray[4*i+2] = l_pOgreVertexPositionArray[4*i+2];
				pVertexPositionArray[4*i+3] = l_pOgreVertexPositionArray[4*i+3];
			}
		}
		delete[] l_pOgreVertexPositionArray;
		return l_bRes;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::getObjectTriangleCount(const CIdentifier& rIdentifier, uint32& ui32TriangleCount) const
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	return l_pOgreObject->getTriangleCount((ogre_uint32&)ui32TriangleCount);
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

boolean CSimulatedBox::getObjectTriangleIndexArray(const CIdentifier& rIdentifier, uint32 ui32TriangleCount, uint32* pTriangleIndexArray) const
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	COgreObject* l_pOgreObject = m_pOgreVis->getOgreScene(m_oSceneIdentifier)->getOgreObject(rIdentifier);
	if(l_pOgreObject == NULL)
	{
		return false;
	}
	if(sizeof(ogre_uint32) == sizeof(uint32))
	{
		return l_pOgreObject->getTriangleIndexArray((ogre_uint32)ui32TriangleCount, (ogre_uint32*)pTriangleIndexArray);
	}
	else
	{
		ogre_uint32* l_pOgreTriangleIndexArray(new ogre_uint32[3*ui32TriangleCount]);
		boolean l_bRes = l_pOgreObject->getTriangleIndexArray((ogre_uint32)ui32TriangleCount, pTriangleIndexArray);
		if(l_bRes == true)
		{
			for(uint32 i=0; i<ui32TriangleCount; i++)
			{
				pTriangleIndexArray[4*i] = l_pOgreTriangleIndexArray[4*i];
				pTriangleIndexArray[4*i+1] = l_pOgreTriangleIndexArray[4*i+1];
				pTriangleIndexArray[4*i+2] = l_pOgreTriangleIndexArray[4*i+2];
			}
		}
		delete[] l_pOgreTriangleIndexArray;
		return l_bRes;
	}
#else
	return false;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

CIdentifier CSimulatedBox::createOgreWindow()
{
#if defined TARGET_HAS_ThirdPartyOgre3D
	//create unique window identifier
	CIdentifier l_oWindowIdentifier = getUnusedIdentifier();

	//ensure a scene was created
	if(m_oSceneIdentifier == OV_UndefinedIdentifier)
	{
		m_oSceneIdentifier = getUnusedIdentifier();
		m_pOgreVis->createScene(m_oSceneIdentifier);
	}

	//create window
	m_pOgreVis->createWindow(l_oWindowIdentifier, l_oWindowIdentifier.toString().toASCIIString(), m_oSceneIdentifier);

	return l_oWindowIdentifier;
#else
	return OV_UndefinedIdentifier;
#endif // TARGET_HAS_ThirdPartyOgre3D
}

CIdentifier CSimulatedBox::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	std::map<CIdentifier, CString>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_mSimulatedObjects.find(l_oResult);
	}
	while(i!=m_mSimulatedObjects.end() || l_oResult==OV_UndefinedIdentifier);
	//TODO : browse window and scene maps as well to guarantee ID unicity!
	return l_oResult;
}

boolean CSimulatedBox::setScenarioIdentifier(const CIdentifier& rScenarioIdentifier)
{
	// FIXME test if rScenario is a scenario identifier
	m_pScenario=&m_rScheduler.getPlayer().getRuntimeScenarioManager().getScenario(rScenarioIdentifier);
	return m_pScenario!=NULL;
}

boolean CSimulatedBox::getBoxIdentifier(CIdentifier& rBoxIdentifier)
{
	if(m_pBox == NULL)
	{
		return false;
	}
	rBoxIdentifier = m_pBox->getIdentifier();
	return true;
}

boolean CSimulatedBox::setBoxIdentifier(const CIdentifier& rBoxIdentifier)
{
	if(!m_pScenario)
	{
		return false;
	}

	m_pBox=m_pScenario->getBoxDetails(rBoxIdentifier);
	return m_pBox!=NULL;
}

boolean CSimulatedBox::initialize(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bSuspended) return false;
	if(m_bCrashed) return false;

	// FIXME test for already initialized boxes etc
	if(!m_pBox) return false;
	if(!m_pScenario) return false;

	m_bChunkConsistencyChecking=this->getConfigurationManager().expandAsBoolean("${Kernel_CheckChunkConsistency}", true);
	m_vInput.resize(m_pBox->getInputCount());
	m_vOutput.resize(m_pBox->getOutputCount());
	m_vCurrentOutput.resize(m_pBox->getOutputCount());
	m_vLastOutputStartTime.resize(m_pBox->getOutputCount(), 0);
	m_vLastOutputEndTime.resize(m_pBox->getOutputCount(), 0);

	m_oBenchmarkChronoProcessClock.reset(1024);
	m_oBenchmarkChronoProcessInput.reset(1024);
	m_oBenchmarkChronoProcess.reset(1024);

	m_ui64LastClockActivationDate=_Bad_Time_;
	m_ui64ClockFrequency=0;
	m_ui64ClockActivationStep=0;

	m_pBoxAlgorithm=getPluginManager().createBoxAlgorithm(m_pBox->getAlgorithmClassIdentifier(), NULL);
	if(!m_pBoxAlgorithm)
	{
		getLogManager() << LogLevel_Error << "Could not create box algorithm with class id " << m_pBox->getAlgorithmClassIdentifier() << "... This box will be deactivated but this might alter the scenario behavior!\n";
		m_bSuspended=true;
		return false;
	}

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::initialize)"));
#endif
			try
			{
				if(!m_pBoxAlgorithm->initialize(l_oBoxAlgorithmContext))
				{
					getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> has been deactivated because initialize() function returned error\n";
					m_bSuspended=true;
					return false;
				}
			}
			catch (...)
			{
				this->handleCrash("initialization callback");
			}
		}
	}

	return true ;
}

boolean CSimulatedBox::uninitialize(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bCrashed) return false;
	if(!m_pBoxAlgorithm) return false;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::uninitialize)"));
#endif
			{
				try
				{
					if(!m_pBoxAlgorithm->uninitialize(l_oBoxAlgorithmContext))
					{
						getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> has been deactivated because uninitialize() function returned error\n";
						m_bSuspended=true;
					}
				}
				catch (...)
				{
					this->handleCrash("uninitialization callback");
				}
			}
		}
	}

	getPluginManager().releasePluginObject(m_pBoxAlgorithm);
	m_pBoxAlgorithm=NULL;

	return true ;
}

boolean CSimulatedBox::processClock(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bSuspended) return false;
	if(m_bCrashed) return false;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::getClockFrequency)"));
#endif
			try
			{
				uint64 l_ui64NewClockFrequency=m_pBoxAlgorithm->getClockFrequency(l_oBoxAlgorithmContext);
				if(l_ui64NewClockFrequency==0)
				{
					m_ui64ClockActivationStep=_Bad_Time_;
					m_ui64LastClockActivationDate=_Bad_Time_;
				}
				else
				{
					if(l_ui64NewClockFrequency > m_rScheduler.getFrequency()<<32)
					{
						this->getLogManager() << LogLevel_ImportantWarning << "Box " << m_pBox->getName() 
							<< " requested higher clock frequency (" << l_ui64NewClockFrequency << " == " 
							<< ITimeArithmetics::timeToSeconds(l_ui64NewClockFrequency) << "hz) "
							<< "than what the scheduler can handle (" << (m_rScheduler.getFrequency()<<32) << " == "
							<< ITimeArithmetics::timeToSeconds(m_rScheduler.getFrequency()<<32) << "hz)\n";
					}

					// note: 1LL should be left shifted 64 bits but this
					//       would result in an integer over shift (the one
					//       would exit). Thus the left shift of 63 bits
					//       and the left shift of 1 bit after the division
					m_ui64ClockActivationStep=((1LL<<63)/l_ui64NewClockFrequency)<<1;
				}
				m_ui64ClockFrequency=l_ui64NewClockFrequency;
			}
			catch (...)
			{
				this->handleCrash("clock frequency request callback");
			}
		}
	}

	if((m_ui64ClockFrequency!=0) && (m_ui64LastClockActivationDate==_Bad_Time_ || m_rScheduler.getCurrentTime()-m_ui64LastClockActivationDate>=m_ui64ClockActivationStep))
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::processClock)"));
#endif
			try
			{
				m_oBenchmarkChronoProcessClock.stepIn();

				if(m_ui64LastClockActivationDate==_Bad_Time_)
				{
					m_ui64LastClockActivationDate=m_rScheduler.getCurrentTime();
				}
				else
				{
					m_ui64LastClockActivationDate=m_ui64LastClockActivationDate+m_ui64ClockActivationStep;
				}

				CMessageClock l_oClockMessage(this->getKernelContext());
				l_oClockMessage.setTime(m_ui64LastClockActivationDate);
				if(!m_pBoxAlgorithm->processClock(l_oBoxAlgorithmContext, l_oClockMessage))
				{
					// In future, we may want to behave in a similar manner as in process(). Change not introduced for 0.18 due to insufficient testing.
					// getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> has been deactivated because processClock() function returned error\n";
					// m_bSuspended=true;
				}
				m_oBenchmarkChronoProcessClock.stepOut();
			}
			catch (...)
			{
				this->handleCrash("clock processing callback");
			}
			m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
		}
	}

	return true;
}

boolean CSimulatedBox::processInput(const uint32 ui32InputIndex, const CChunk& rChunk)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bSuspended) return false;
	if(m_bCrashed) return false;

	m_vInput[ui32InputIndex].push_back(rChunk);

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::processInput)"));
#endif
			try
			{
				m_oBenchmarkChronoProcessInput.stepIn();
				if(!m_pBoxAlgorithm->processInput(l_oBoxAlgorithmContext, ui32InputIndex))
				{
					// In future, we may want to behave in a similar manner as in process(). Change not introduced for 0.18 due to insufficient testing.
					// getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> has been deactivated because processInput() function returned error\n";
					// m_bSuspended=true;
				}
				m_oBenchmarkChronoProcessInput.stepOut();
			}
			catch (...)
			{
				this->handleCrash("input processing callback");
			}
		}
		m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
	}

	return true;
}

boolean CSimulatedBox::process(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(m_bSuspended) return false;
	if(m_bCrashed) return false;

	if(!m_bReadyToProcess) return true;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::process)"));
#endif
			try
			{
				m_oBenchmarkChronoProcess.stepIn();
				if(!m_pBoxAlgorithm->process(l_oBoxAlgorithmContext))
				{
					getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> has been deactivated because process() function returned error\n";
					m_bSuspended=true;
				}
				m_oBenchmarkChronoProcess.stepOut();
			}
			catch (...)
			{
				this->handleCrash("processing callback");
			}
		}
	}

	// perform output sending
	CIdentifier l_oLinkIdentifier=m_pScenario->getNextLinkIdentifierFromBox(OV_UndefinedIdentifier, m_pBox->getIdentifier());
	while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
	{
		const ILink* l_pLink=m_pScenario->getLinkDetails(l_oLinkIdentifier);
		if(l_pLink)
		{
			CIdentifier l_oTargetBoxIdentifier=l_pLink->getTargetBoxIdentifier();
			uint32 l_ui32TargetBoxInputIndex=l_pLink->getTargetBoxInputIndex();

			uint32 l_ui32SourceOutputIndex=l_pLink->getSourceBoxOutputIndex();
			deque < CChunk >::iterator i=m_vOutput[l_ui32SourceOutputIndex].begin();
			while(i!=m_vOutput[l_ui32SourceOutputIndex].end())
			{
				m_rScheduler.sendInput(*i, l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex);
				++i;
			}
		}
		l_oLinkIdentifier=m_pScenario->getNextLinkIdentifierFromBox(l_oLinkIdentifier, m_pBox->getIdentifier());
	}

	// iterators for input and output chunks
	vector < deque< CChunk > >::iterator i;
	deque < CChunk >::iterator j;
	vector < CChunk >::iterator k;

	// perform input cleaning
	i=m_vInput.begin();
	while(i!=m_vInput.end())
	{
		j=i->begin();
		while(j!=i->end())
		{
			if(j->isDeprecated())
			{
				j=i->erase(j);
			}
			else
			{
				++j;
			}
		}
		++i;
	}

	// flushes sent output chunks
	i=m_vOutput.begin();
	while(i!=m_vOutput.end())
	{
		i->resize(0);
		++i;
	}

	// discards waiting output chunks
	k=m_vCurrentOutput.begin();
	while(k!=m_vCurrentOutput.end())
	{
		// checks buffer size
		if(k->getBuffer().getSize())
		{
			// the buffer has been (partially ?) filled but not sent
			CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
			l_oBoxAlgorithmContext.getPlayerContext()->getLogManager() << LogLevel_Warning << "Output buffer filled but not marked as ready to send\n"; // $$$ should use log
			k->getBuffer().setSize(0, true);
		}

		++k;
	}

	m_bReadyToProcess=false;

	// FIXME : might not be the most relevant place where to refresh 3D windows!
	//moved to plugins code
	/*
	map<GtkWidget*, CIdentifier>::iterator it;

	for(it = m_mOgreWindows.begin(); it != m_mOgreWindows.end(); it++)
	{
		m_pOgreVis->getOgreWindow(it->second)->update();
	}*/

#if 1
/*-----------------------------------------------*/
/* TODO send this messages with better frequency */
	if(m_oBenchmarkChronoProcessClock.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process clock" << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcessClock.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
	if(m_oBenchmarkChronoProcessInput.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process input" << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcessInput.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
	if(m_oBenchmarkChronoProcess.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process      " << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcess.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
/* TODO Thank you for reading :)                 */
/*-----------------------------------------------*/
#endif

	return true;
}

boolean CSimulatedBox::isReadyToProcess(void) const
{
	return m_bReadyToProcess;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// - --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

CString CSimulatedBox::getName(void) const
{
	return m_pBox->getName();
}

const IScenario& CSimulatedBox::getScenario(void) const
{
	return *m_pScenario;
}

// ________________________________________________________________________________________________________________
//

namespace
{
	void __out_of_bound_input(ILogManager& rLogManager, const CString& sName, boolean& bSuspended, uint32 ui32InputIndex, uint32 ui32InputCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound input (" << ui32InputIndex << "/" << ui32InputCount << ") - Disabled box\n";
		bSuspended = true;
	}

	void __out_of_bound_input_chunk(ILogManager& rLogManager, const CString& sName, boolean& bSuspended, uint32 ui32InputIndex, uint32 ui32InputChunkIndex, uint32 ui32InputChunkCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound input (" << ui32InputIndex << ":" << ui32InputChunkIndex << "/" << ui32InputChunkCount << ") - Disabled box\n";
		bSuspended = true;
	}
}

// ________________________________________________________________________________________________________________
//

uint32 CSimulatedBox::getInputChunkCount(
	const uint32 ui32InputIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, m_vInput.size());
		return 0;
	}
	return m_vInput[ui32InputIndex].size();
}

boolean CSimulatedBox::getInputChunk(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex,
	uint64& rStartTime,
	uint64& rEndTime,
	uint64& rChunkSize,
	const uint8*& rpChunkBuffer) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	rStartTime=l_rChunk.getStartTime();
	rEndTime=l_rChunk.getEndTime();
	rChunkSize=l_rChunk.getBuffer().getSize();
	rpChunkBuffer=l_rChunk.getBuffer().getDirectPointer();
	return true;
}

const IMemoryBuffer* CSimulatedBox::getInputChunk(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, m_vInput.size());
		return NULL;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return NULL;
	}
	return &_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex).getBuffer();
}

uint64 CSimulatedBox::getInputChunkStartTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	return l_rChunk.getStartTime();
}

uint64 CSimulatedBox::getInputChunkEndTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	return l_rChunk.getEndTime();
}

boolean CSimulatedBox::markInputAsDeprecated(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex)
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), m_bSuspended, ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}
	_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex).markAsDeprecated(true);
	return true;
}

// ________________________________________________________________________________________________________________
//

namespace
{
	void __out_of_bound_output(ILogManager& rLogManager, const CString& sName, boolean& bSuspended, uint32 ui32OutputIndex, uint32 ui32OutputCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound output (" << ui32OutputIndex << "/" << ui32OutputCount << ") - Disabled box\n";
		bSuspended = true;
	}
}

// ________________________________________________________________________________________________________________
//

uint64 CSimulatedBox::getOutputChunkSize(
	const uint32 ui32OutputIndex) const
{
	if(ui32OutputIndex>=m_vCurrentOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), m_bSuspended, ui32OutputIndex, m_vCurrentOutput.size());
		return 0;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().getSize();
}

boolean CSimulatedBox::setOutputChunkSize(
	const uint32 ui32OutputIndex,
	const uint64 ui64Size,
	const boolean bDiscard)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), m_bSuspended, ui32OutputIndex, m_vCurrentOutput.size());
		return false;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().setSize(ui64Size, bDiscard);
}

uint8* CSimulatedBox::getOutputChunkBuffer(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), m_bSuspended, ui32OutputIndex, m_vCurrentOutput.size());
		return NULL;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().getDirectPointer();
}

boolean CSimulatedBox::appendOutputChunkData(
	const uint32 ui32OutputIndex,
	const uint8* pBuffer,
	const uint64 ui64BufferSize)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), m_bSuspended, ui32OutputIndex, m_vCurrentOutput.size());
		return false;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().append(pBuffer, ui64BufferSize);
}

IMemoryBuffer* CSimulatedBox::getOutputChunk(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), m_bSuspended, ui32OutputIndex, m_vCurrentOutput.size());
		return NULL;
	}
	return &m_vCurrentOutput[ui32OutputIndex].getBuffer();
}

boolean CSimulatedBox::markOutputAsReadyToSend(
	const uint32 ui32OutputIndex,
	const uint64 ui64StartTime,
	const uint64 ui64EndTime)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), m_bSuspended, ui32OutputIndex, m_vCurrentOutput.size());
		return false;
	}

	if(m_bChunkConsistencyChecking)
	{
		boolean l_bIsConsistent = true;
		const char* l_sSpecificMessage = NULL;

		// checks chunks consistency
		CIdentifier l_oType;
		m_pBox->getOutputType(ui32OutputIndex, l_oType);
		if(l_oType == OV_TypeId_Stimulations)
		{
			if(m_vLastOutputEndTime[ui32OutputIndex] != ui64StartTime)
			{
				l_bIsConsistent = false;
				l_sSpecificMessage = "'Stimulations' streams should have continuously dated chunks";
			}
		}

		if(m_vLastOutputEndTime[ui32OutputIndex] > ui64EndTime)
		{
			l_bIsConsistent = false;
			l_sSpecificMessage = "Current 'end time' can not be earlier than previous 'end time'";
		}

		if(m_vLastOutputStartTime[ui32OutputIndex] > ui64StartTime)
		{
			l_bIsConsistent = false;
			l_sSpecificMessage = "Current 'start time' can not be earlier than previous 'start time'";
		}

		if(!l_bIsConsistent)
		{
			this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << "Box <" << m_pBox->getName() << "> sends inconsistent chunk dates on output [" << ui32OutputIndex << "] (current chunk dates are [" << ui64StartTime << "," << ui64EndTime << "] whereas previous chunk dates were [" << m_vLastOutputStartTime[ui32OutputIndex] << "," << m_vLastOutputEndTime[ui32OutputIndex] << "])\n";
			if(l_sSpecificMessage) this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << l_sSpecificMessage << "\n";
			this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << "Please report to box author and attach your scenario\n";
			this->getLogManager() << LogLevel_Trace                      << "Previous warning can be disabled setting Kernel_CheckChunkConsistency to false\n";
			m_eChunkConsistencyCheckingLogLevel = LogLevel_Trace;
		}

		// sets last times
		m_vLastOutputStartTime[ui32OutputIndex] = ui64StartTime;
		m_vLastOutputEndTime[ui32OutputIndex] = ui64EndTime;
	}

	// sets start and end time
	m_vCurrentOutput[ui32OutputIndex].setStartTime(std::min(ui64StartTime, ui64EndTime));
	m_vCurrentOutput[ui32OutputIndex].setEndTime(std::max(ui64StartTime, ui64EndTime));

	// copies chunk
	m_vOutput[ui32OutputIndex].push_back(m_vCurrentOutput[ui32OutputIndex]);

	// resets chunk size
	m_vCurrentOutput[ui32OutputIndex].getBuffer().setSize(0, true);

	return true;
}

// ________________________________________________________________________________________________________________
//

void CSimulatedBox::handleCrash(const char* sHintName)
{
	m_ui32CrashCount++;

	this->getLogManager() << LogLevel_Error << "At time " << time64(m_rScheduler.getCurrentTime()) << ", plugin code caused a crash " << m_ui32CrashCount << " time(s)\n";
	this->getLogManager() << LogLevel_Error << "  [name:" << m_pBox->getName() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [identifier:" << m_pBox->getIdentifier() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [algorithm class identifier:" << m_pBox->getAlgorithmClassIdentifier() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [location:" << sHintName << "]\n";

	if(m_ui32CrashCount>=_MaxCrash_)
	{
		this->getLogManager() << LogLevel_Fatal << "  This plugin has been disabled !\n";
		m_bCrashed=true;
	}
}


boolean CSimulatedBox::sendMessage(const IMessageWithData &msg, uint32 outputIndex)
{
	if(m_bIsReceivingMessage) {
		this->getLogManager() << LogLevel_Error << "Message sending prohibited while receiving message.\n";
		return false;
	}

	this->getLogManager() << LogLevel_Debug << "SimulatedBox sendMessage on output" << outputIndex <<"\n";

	//get the message links originating from this box
	CIdentifier l_oLinkIdentifier=m_pScenario->getNextMessageLinkIdentifierFromBox(OV_UndefinedIdentifier, m_pBox->getIdentifier());
	while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
	{
		const ILink* l_pLink=m_pScenario->getMessageLinkDetails(l_oLinkIdentifier);
		if(l_pLink)
		{
			if ( l_pLink->getSourceBoxOutputIndex()==outputIndex)
			{
				CIdentifier l_oTargetBoxIdentifier=l_pLink->getTargetBoxIdentifier();
				uint32 l_ui32TargetBoxInputIndex=l_pLink->getTargetBoxInputIndex();
				boolean l_bMessageReceived = m_rScheduler.sendMessage(msg, l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex);
				if (!l_bMessageReceived)
				{
					this->getLogManager() << LogLevel_ImportantWarning << "Box " << m_pScenario->getBoxDetails(l_oTargetBoxIdentifier)->getName()
									<< " with ID " << l_oTargetBoxIdentifier << " failed to receive the message. Check the connection and/or the message content\n";
				}
			}
		}
		l_oLinkIdentifier=m_pScenario->getNextMessageLinkIdentifierFromBox(l_oLinkIdentifier, m_pBox->getIdentifier());
	}
	return true; // if success
}

boolean CSimulatedBox::receiveMessage(const IMessageWithData &msg, uint32 inputIndex)
{
	m_bIsReceivingMessage = true;
	CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
	this->getLogManager() << LogLevel_Debug << "simulated box" << m_pBox->getName() <<" receiving message on input " << inputIndex <<"\n";

	bool l_bReturnValue = m_pBoxAlgorithm->processMessage(l_oBoxAlgorithmContext, msg, inputIndex);

	m_bIsReceivingMessage = false;

	return l_bReturnValue;
}


boolean CSimulatedBox::cleanupMessages() {
	// ...
//	this->getLogManager() << LogLevel_Debug << "cleaning messages " << "\n";

	for(std::vector<OpenViBE::Kernel::CMessageWithData*>::iterator it = m_vPreparedMessages.begin();
		it!=m_vPreparedMessages.end(); it++)
	{
		delete (*it);
	}

	m_vPreparedMessages.clear();
	return true; // if success
}

IMessageWithData& CSimulatedBox::createMessage()
{
	CMessageWithData* newMessage = new CMessageWithData(getKernelContext());
	m_vPreparedMessages.push_back(newMessage);

	return *newMessage;
}

// #endif // __MY_COMPILE_ALL
