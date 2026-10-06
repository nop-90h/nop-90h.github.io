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

class AppDialog : public BaseDialog
{
private:

	CContainerPtr		_ptrSlotsDemoCont;
    CSlotMachinePtr     _ptrSlotMachine;
private:

    void initSlotsDemo()
    {
        _ptrSlotsDemoCont = std::make_shared<CContainer>();

        _root->addChild(_ptrSlotsDemoCont);

        _ptrSlotMachine = std::make_shared<CSlotMachine>();
        _ptrSlotMachine->initMachine(110.f, 120.f);

        _ptrSlotsDemoCont->addChild(_ptrSlotMachine);

        _ptrSlotMachine->setXPosCentered(getDialogCx());

        auto ptrSprSolid = SpriteLoader::getInstance()->getSprite("UI/mainIconsFrame");

        NineSlicePtr ptrFrameBot = std::make_shared<NineSlice>();

        ptrFrameBot->createSlices(ptrSprSolid, 67, 67, 67, 67);
        ptrFrameBot->build(_ptrSlotMachine->calcNotTransCx(), 140.f);

        _ptrSlotsDemoCont->addChild(ptrFrameBot);

        ptrFrameBot->setXPosCentered(getDialogCx());
        ptrFrameBot->setY(getDialogCy() - 180.f);
        ptrFrameBot->setAlpha(.8f);

        auto ptrSpinBtn = OrangeSlicedButton::makeInst(70, L10N::getInstance().tr("BTN_SPIN"), [this]()
        {
            if (_ptrSlotMachine->isSpinning())
                return;

            std::vector<int> fakeResult(15);

            for (int i = 0; i < 15; ++i)
                fakeResult[i] = rand() % 16;

            _ptrSlotMachine->spin();
        });

        ptrSpinBtn->setPosCentered(ptrFrameBot->calcNotTransCx(), ptrFrameBot->calcNotTransCy());

        ptrFrameBot->addChild(ptrSpinBtn);
    }

public:

	void init()
    {
        auto& cfg = Engine::getCfg();
        BaseDialog::init(cfg.INIT_SCR_CX, cfg.INIT_SCR_CY, eScrollType::E_ST_NONE, nullptr, nullptr);

		Rect rc = { 0, 0, getDialogCx(), getDialogCy() };
		_root->setScrollBox(&rc);//clip

        initSlotsDemo();
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