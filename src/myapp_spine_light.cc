#include "pch.h"
#include "myapp.h"
#include "spinecont.h"
#include "gfx.h"
#include "basedialog.h"
#include "novel.h"
#include "button.h"
#include "beheading.h"
#include "touchhandler.h"
#include "audiomanager.h"
#include "particle.h"
#include "l10n.h"
#include "bakedspine.h"
#include "panelhtml.h"

class AppDialog : public BaseDialog
{
private:

	CContainerPtr		_ptrGameHolder;
	CContainerPtr		_ptrDriveLayer;
	CContainerPtr		_ptrShizBgCont;
	CSpinePtr			_ptrLamp;
	CSpinePtr			_ptrShiz;
	CSpinePtr			_ptrAmbulance;
	CSpritePtr			_ptrNotebook;
	CParticleSystemPtr	_ptrFlies;
	CParticleSystemPtr	_ptrFlies2;
private:

	void playDrive()
	{
		Rect rcU{ 0,   0, 1672, 580 };
		Rect rcB{ 0, 580, 1672, 941 - 580 };
		auto ptrShizBgU = CGfx::getInstance()->spriteFromTexture(CGfx::getInstance()->getTextureById("shiz_bg_u.png"));
		auto ptrShizBgB = CGfx::getInstance()->spriteFromTexture(CGfx::getInstance()->getTextureById("shiz_bg_b.png"));
		_ptrShizBgCont = std::make_shared<CContainer>();
		_ptrShizBgCont->addChild(ptrShizBgU);
		_ptrShizBgCont->addChild(ptrShizBgB);
		ptrShizBgB->setLightLayer(1);
		ptrShizBgB->setY(470);
		ptrShizBgU->setSkipLight(false);
		ptrShizBgB->setSkipLight(false);

		_ptrGameHolder		= std::make_shared<CContainer>();
		_ptrDriveLayer		= std::make_shared<CContainer>();
		_ptrLamp			= CSpineManager::getInstance()->getNewSpine("streetlamp");
		_ptrShiz			= CSpineManager::getInstance()->getNewSpine("programmer");
		_ptrAmbulance		= CSpineManager::getInstance()->getNewSpine("ambulance");
		_ptrNotebook		= SpriteLoader::getInstance()->getSprite("SHIZ/notebook");

		_root->addChild(_ptrGameHolder);

		auto strPath		= std::format("PARTICLES/{}", ParticlePresets::getPresetSpriteName(eParticlePreset::AMBIENT_DUST));
		_ptrFlies			= std::make_shared<CParticleSystem<>>(ParticlePresets::getPreset(eParticlePreset::AMBIENT_DUST), strPath.c_str());
		_ptrFlies2			= std::make_shared<CParticleSystem<>>(ParticlePresets::getPreset(eParticlePreset::FIREFLIES), strPath.c_str());
		_ptrFlies->prewarm(5);
		_ptrFlies2->prewarm(5);
		_ptrFlies->setSkipLight(false);
		_ptrFlies2->setSkipLight(false);
		_ptrFlies->setLightLayer(1);
		_ptrFlies2->setLightLayer(1);
		_ptrGameHolder->removeAll();
		_ptrDriveLayer->removeAll();
		_ptrGameHolder->addChild(_ptrDriveLayer);
		_ptrLamp->setPos(600, 700);
		_ptrShiz->setPos(630, 740);
		_ptrAmbulance->setPos(3000, 520);
		_ptrDriveLayer->addChild(_ptrNotebook);
		_ptrNotebook->setVisible(true);
		_ptrDriveLayer->addChild(_ptrShiz);
		_ptrLamp->setAnimation(0, "animation", true);
		_ptrShiz->setAnimation(0, "animation", true);
		_ptrAmbulance->setAnimation(0, "drive", true);
		_ptrLamp->setLightLayer(1);
		_ptrShiz->setLightLayer(1);

		_ptrDriveLayer->addChild(_ptrShizBgCont);
		_ptrDriveLayer->addChild(_ptrLamp);
		_ptrDriveLayer->addChild(_ptrFlies);
		_ptrDriveLayer->addChild(_ptrFlies2);
		_ptrFlies->setPos(700, 550);
		_ptrFlies2->setPos(700, 300);
		_ptrDriveLayer->addChild(_ptrAmbulance);
		_ptrDriveLayer->bringChildToBack(_ptrShizBgCont.get());
		_ptrDriveLayer->bringChildToFront(_ptrShiz.get());
		setTimeout(11.50f, [this] {
			_ptrAmbulance->setAnimation(0, "stop", false);
			_ptrAmbulance->onEvent([this](CSpine* pOwner, const char* lpccEventName) {
				assert(!strcmp("stopped", lpccEventName));
				_ptrAmbulance->removeSelfTweens();
			});
			_ptrAmbulance->addAnimation(0, "idle", true);
		});
		_ptrShiz->setScale(0.4f, 0.4f);
		_ptrLamp->setScale(0.65f, 0.65f);
		_ptrNotebook->setScale(0.5f, 0.5f);
		_ptrNotebook->setPos(730, 600);

		_ptrAmbulance->addSelfTween(eTweenProp::X, _ptrAmbulance->getX(), 1300, 12.f, Easing::linear);
		setLights();
	}

	void setLights()
	{
		LightLayerArray arrLamp;
		arrLamp[0].bActive = true;
		arrLamp[0].nLayer  = 1;
		arrLamp[1].bActive = true;
		arrLamp[1].nLayer  = 0;
		arrLamp[1].fMul    = 0.05f;
		arrLamp[2].bActive = true;
		arrLamp[2].nLayer  = 2;
		arrLamp[2].fMul    = 1.f;

		LightLayerArray arrAmbulance;
		arrAmbulance[0].bActive = true;
		arrAmbulance[0].nLayer  = 0;
		arrAmbulance[1].bActive = true;
		arrAmbulance[1].nLayer  = 1;
		arrAmbulance[1].fMul    = 0.1f;

		_ptrLamp->setLightAttachment("light",            true);
		_ptrLamp->setLightAttachment("lightspot",        true);
		_ptrLamp->setLightAttachment("lightspot2",       true);
		_ptrLamp->setLightEmmiterSettings(arrLamp);

		_ptrAmbulance->setLightAttachment("bluelight",   true);
		_ptrAmbulance->setLightAttachment("lightspot2",  true);
		_ptrAmbulance->setLightAttachment("headlight",   true);
		_ptrAmbulance->setLightAttachment("lightspot",   true);
		_ptrAmbulance->setLightAttachment("lightspot_headlights", true);
		_ptrAmbulance->setLightAttachment("STOP_1",      true);
		_ptrAmbulance->setLightAttachment("STOP_2",      true);
		_ptrAmbulance->setLightEmmiterSettings(arrAmbulance);

	}


public:

	void init()
    {
        auto& cfg = Engine::getCfg();
        BaseDialog::init(cfg.INIT_SCR_CX, cfg.INIT_SCR_CY, eScrollType::E_ST_NONE, nullptr, nullptr);

		Rect rc = { 0, 0, getDialogCx(), getDialogCy() };
		_root->setScrollBox(&rc);//clip

		playDrive();
        CGfx::getInstance()->getGameRoot()->addChild(shared_from_this());
		showAsPanel(E_IH_RELAXED, false);
    }
};

void MyApp::onFrame(float dt)
{
}

void MyApp::onResize(float cx, float cy)
{
}

void MyApp::init()
{
	SpineMixingOptions::setMixTime("ambulance", "def", "def", 0.03f);

	auto& engineCfg = Engine::getCfg();
    CGfx::getInstance()->getPostProcessSettings().enabled = false;
	L10N::getInstance().load(engineCfg.LANG);
	Rect rcScreen = { 0, 0, engineCfg.INIT_SCR_CX, engineCfg.INIT_SCR_CY };
	_ptrPreloader = std::make_shared<Preloader>();

	auto cbOnLoaded = [this]
	{
		SpriteLoader::getInstance()->addAtlas("ui");
        auto ptr = std::make_shared<AppDialog>();
        ptr->init();
	};
	_ptrPreloader->filesToLoad().addSpine("programmer");
	_ptrPreloader->filesToLoad().addSpine("ambulance");
	_ptrPreloader->filesToLoad().addSpine("streetlamp");
	_ptrPreloader->filesToLoad().add("ui.png");
	_ptrPreloader->filesToLoad().add("ui.atlas");

	_ptrPreloader->filesToLoad().add("shiz_bg_u.png");
	_ptrPreloader->filesToLoad().add("shiz_bg_b.png");

	_ptrPreloader->filesToLoad().setSizeToLoad(63996535);
	_ptrPreloader->init(cbOnLoaded);
	HTMLDom::getInstance()->parseCSSFromFile("EMBED/HTML/styles.css");

}