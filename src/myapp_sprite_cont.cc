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
#include "slotmachine.h"
#include "bgeffects.h"
#include "panelspritecontdemo.h"

void MyApp::onFrame(float dt)
{
}

void MyApp::onResize(float cx, float cy)
{
}

void MyApp::init()
{
	auto& engineCfg = Engine::getCfg();
    CGfx::getInstance()->getPostProcessSettings().enabled = false;
	L10N::getInstance().load(engineCfg.LANG);
	Rect rcScreen = { 0, 0, engineCfg.INIT_SCR_CX, engineCfg.INIT_SCR_CY };
	_ptrPreloader = std::make_shared<Preloader>();

	auto cbOnLoaded = [this]
	{
		auto& engineCfg = Engine::getCfg();
		SpriteLoader::getInstance()->addAtlas("ui");
        auto ptr = std::make_shared<PanelSpriteContDemo>();
        ptr->init(engineCfg.INIT_SCR_CX, engineCfg.INIT_SCR_CY, nullptr);
        CGfx::getInstance()->getGameRoot()->addChild(ptr);
		ptr->showAsPanel(E_IH_RELAXED, false);
	};
	_ptrPreloader->filesToLoad().add("ui.png");
	_ptrPreloader->filesToLoad().add("ui.atlas");

	_ptrPreloader->filesToLoad().setSizeToLoad(63996535);
	_ptrPreloader->init(cbOnLoaded);
}