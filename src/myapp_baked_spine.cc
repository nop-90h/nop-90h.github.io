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

class AppDialog : public BaseDialog
{
private:
    CContainerPtr _ptrBakedSpines;
    CContainerPtr _ptrBakedDemoCont;

public:
    void init()
    {
        auto& cfg = Engine::getCfg();
		BaseDialog::init(cfg.INIT_SCR_CX, cfg.INIT_SCR_CY, E_ST_NONE, nullptr, nullptr);

        CGfx::getInstance()->getGameRoot()->addChild(shared_from_this());
		Rect rc = { 0, 0, getDialogCx(), getDialogCy() };
		_root->setScrollBox(&rc);
        initBakedSpine();
        _root->addChild(_ptrBakedDemoCont);
		showAsPanel(E_IH_RELAXED, false);
    }

private:

    void initBakedSpine()
    {
        _ptrBakedDemoCont = std::make_shared<CContainer>();
        _ptrBakedSpines   = std::make_shared<CContainer>();

        auto ptrCount = std::make_shared<int>(0);
        auto ptrShown = std::make_shared<std::set<int>>();

        auto ptrCountLabel = std::make_shared<StaticLabel>();

        ptrCountLabel->setBoxMode(eTextRenderType::HTML_BOX, getDialogCx() - 80.f);

        auto updateCountLabel = [ptrCountLabel, ptrCount]()
        {
            if (!ptrCountLabel)
                return;

            std::string sText = std::vformat(L10N::getInstance().tr("BAKED_RUNNING_FMT"), std::make_format_args(*ptrCount));

            ptrCountLabel->setText(std::format(
                "<center><font name=\"edugot\" size=\"34\" color=\"#9ADCFF\" shadow=\"1\" shadowcolor=\"#000000AA\">"
                "<b><font color=\"#FFE9A0\">{}</font></b></font></center>", sText).c_str());
        };

        auto showMilestone = [this](int nCount, const char* lpszText)
        {
            if (!_ptrBakedDemoCont)
                return;

            const int nTier = (nCount >= 2000) ? 2 : (nCount >= 1000) ? 1 : 0;

            auto ptrBig = std::make_shared<StaticLabel>();

            ptrBig->setBoxMode(eTextRenderType::HTML_BOX, getDialogCx() - 60.f);

            std::string sHtml;

            switch (nTier)
            {
                case 2:
                    sHtml = std::format(
                        "<center><font name=\"edugot\" size=\"132\" color=\"#FFE9A0\" shadow=\"2\" shadowcolor=\"#000000FF\""
                        " appear=\"comet\" dur=\"1.2\" fx=\"rainbow\"><b>{}</b></font></center>", lpszText);
                    break;

                case 1:
                    sHtml = std::format(
                        "<center><font name=\"edugot\" size=\"120\" color=\"#FFD54F\" shadow=\"1\" shadowcolor=\"#000000EE\""
                        " appear=\"shatter\" dur=\"1.8\" fx=\"fire\"><b>{}</b></font></center>", lpszText);
                    break;

                default:
                    sHtml = std::format(
                        "<center><font name=\"edugot\" size=\"86\" color=\"#010161\" shadow=\"1\" shadowcolor=\"#000000EE\""
                        " appear=\"rise\" dur=\"0.8\"><b>{}</b></font></center>", lpszText);
                    break;
            }

            ptrBig->setText(sHtml.c_str());
            ptrBig->setXPosCentered(getDialogCx());
            ptrBig->setY(getDialogCy() * (nTier == 2 ? 0.34f : 0.40f));
            ptrBig->setScale(0.2f, 0.2f);
            ptrBig->setAlpha(0.f);

            _ptrBakedDemoCont->addChild(ptrBig);

            const float fHold = (nTier == 2) ? 6.5f : (nTier == 1) ? 5.2f : 2.6f;
            const float fBaseX = ptrBig->getX();
            const float fBaseY = ptrBig->getY();

            ptrBig->addSelfTween(eTweenProp::ALPHA, 0.f, 1.f, 0.3f, Easing::outQuad);
            ptrBig->addSelfTween(eTweenProp::SCALE, 0.2f, 1.f, 0.5f, Easing::outBack);

            if (nTier == 2)
            {
                auto ptrFlash = SpriteLoader::getInstance()->getSprite("UI/whitebox");

                if (ptrFlash)
                {
                    ptrFlash->setBlendMode(eSpriteBlendMode::ADDITIVE);
                    ptrFlash->setScaleTo(getDialogCx(), getDialogCy());
                    ptrFlash->setPivotCentered();
                    ptrFlash->setPos(getDialogCx() * 0.5f, getDialogCy() * 0.5f);
                    ptrFlash->setAlpha(0.f);

                    _ptrBakedDemoCont->addChild(ptrFlash);

                    ptrFlash->addSelfTween(eTweenProp::ALPHA, 0.f, 0.9f, 0.07f, Easing::outQuad);
                    ptrFlash->addSelfTween(eTweenProp::ALPHA, 0.9f, 0.f, 1.1f, Easing::outQuad, 0.08f,
                        [w = ptrFlash->weak_from_this()]()
                        {
                            if (auto p = w.lock())
                                p->removeFromParent();
                        });
                }

                for (int i = 0; i < 2; ++i)
                {
                    auto ptrRing = SpriteLoader::getInstance()->getSprite("UI/whitebox");

                    if (!ptrRing)
                        break;

                    ptrRing->setBlendMode(eSpriteBlendMode::ADDITIVE);
                    ptrRing->setPivotCentered();

                    const float fSize = 300.f + i * 160.f;

                    ptrRing->setScaleTo(fSize, fSize);

                    const float fS = std::min(ptrRing->getScaleX(), ptrRing->getScaleY());

                    ptrRing->setPos(getDialogCx() * 0.5f, getDialogCy() * 0.5f);
                    ptrRing->setAlpha(0.f);

                    _ptrBakedDemoCont->addChild(ptrRing);

                    const float fDelay = 1.0f + i * 0.18f;

                    ptrRing->addSelfTween(eTweenProp::SCALE, fS * 0.3f, fS * 5.5f, 1.0f, Easing::outCubic, fDelay);
                    ptrRing->addSelfTween(eTweenProp::ALPHA, 0.85f, 0.f, 1.0f, Easing::outQuad, fDelay,
                        [w = ptrRing->weak_from_this()]()
                        {
                            if (auto p = w.lock())
                                p->removeFromParent();
                        });
                }

                ptrBig->addSelfTween(eTweenProp::SCALE, 1.f, 2.8f, 0.35f, Easing::outQuad, 0.45f);
                ptrBig->addSelfTween(eTweenProp::SCALE, 2.8f, 1.0f, 1.4f, Easing::outElastic, 0.80f);

                ptrBig->rotate(-12.566f);
                ptrBig->addSelfTween(eTweenProp::ROTATE, -12.566f, 0.f, 1.6f, Easing::outCubic, 0.45f);

                float fPrevOff = 0.f;

                for (int i = 0; i < 8; ++i)
                {
                    const float fOff    = (i % 2 == 0) ? 1.f : -1.f;
                    const float fShakeX = fOff * randomRange(10.f, 22.f);
                    const float fShakeY = fOff * randomRange(6.f, 14.f);
                    const float fT      = 2.3f + i * 0.08f;

                    ptrBig->addSelfTween(eTweenProp::X, fBaseX + fPrevOff * 16.f, fBaseX + fShakeX, 0.045f, Easing::linear, fT);
                    ptrBig->addSelfTween(eTweenProp::Y, fBaseY - fPrevOff * 10.f, fBaseY + fShakeY, 0.045f, Easing::linear, fT);

                    fPrevOff = fOff;
                }

                ptrBig->addSelfTween(eTweenProp::X, fBaseX + fPrevOff * 16.f, fBaseX, 0.06f, Easing::outQuad, 2.3f + 8 * 0.08f);
                ptrBig->addSelfTween(eTweenProp::Y, fBaseY - fPrevOff * 10.f, fBaseY, 0.06f, Easing::outQuad, 2.3f + 8 * 0.08f);
            }
            else if (nTier == 1)
            {
                auto ptrFlash = SpriteLoader::getInstance()->getSprite("UI/whitebox");

                if (ptrFlash)
                {
                    ptrFlash->setBlendMode(eSpriteBlendMode::ADDITIVE);
                    ptrFlash->setScaleTo(getDialogCx(), getDialogCy());
                    ptrFlash->setPivotCentered();
                    ptrFlash->setPos(getDialogCx() * 0.5f, getDialogCy() * 0.5f);
                    ptrFlash->setAlpha(0.f);

                    _ptrBakedDemoCont->addChild(ptrFlash);

                    ptrFlash->addSelfTween(eTweenProp::ALPHA, 0.f, 0.65f, 0.08f, Easing::outQuad);
                    ptrFlash->addSelfTween(eTweenProp::ALPHA, 0.65f, 0.f, 0.9f, Easing::outQuad, 0.1f,
                        [w = ptrFlash->weak_from_this()]()
                        {
                            if (auto p = w.lock())
                                p->removeFromParent();
                        });
                }

                ptrBig->addSelfTween(eTweenProp::SCALE, 1.f, 1.8f, 0.28f, Easing::outQuad, 0.45f);
                ptrBig->addSelfTween(eTweenProp::SCALE, 1.8f, 1.0f, 1.1f, Easing::outElastic, 0.73f);

                ptrBig->rotate(-0.2f);
                ptrBig->addSelfTween(eTweenProp::ROTATE, -0.2f, 0.05f, 0.6f, Easing::outBack, 0.45f);
                ptrBig->addSelfTween(eTweenProp::ROTATE, 0.05f, 0.f, 0.8f, Easing::outElastic, 1.05f);

                float fPrevOff = 0.f;

                for (int i = 0; i < 6; ++i)
                {
                    const float fOff    = (i % 2 == 0) ? 1.f : -1.f;
                    const float fShakeX = fOff * randomRange(6.f, 14.f);
                    const float fShakeY = fOff * randomRange(3.f, 8.f);
                    const float fT      = 1.5f + i * 0.09f;

                    ptrBig->addSelfTween(eTweenProp::X, fBaseX + fPrevOff * 10.f, fBaseX + fShakeX, 0.05f, Easing::linear, fT);
                    ptrBig->addSelfTween(eTweenProp::Y, fBaseY - fPrevOff * 6.f,  fBaseY + fShakeY, 0.05f, Easing::linear, fT);

                    fPrevOff = fOff;
                }

                ptrBig->addSelfTween(eTweenProp::X, fBaseX + fPrevOff * 10.f, fBaseX, 0.06f, Easing::outQuad, 1.5f + 6 * 0.09f);
                ptrBig->addSelfTween(eTweenProp::Y, fBaseY - fPrevOff * 6.f,  fBaseY, 0.06f, Easing::outQuad, 1.5f + 6 * 0.09f);
            }
            else
            {
                ptrBig->addSelfTween(eTweenProp::SCALE, 1.f, 1.12f, 0.3f, Easing::outQuad, 0.55f);
                ptrBig->addSelfTween(eTweenProp::SCALE, 1.12f, 1.f, 0.35f, Easing::inOutQuad, 0.85f);
            }

            ptrBig->addSelfTween(eTweenProp::ALPHA, 1.f, 0.f, 0.6f, Easing::inQuad, fHold,
                [w = ptrBig->weak_from_this()]()
                {
                    if (auto p = w.lock())
                        p->removeFromParent();
                });

            ptrBig->addSelfTween(eTweenProp::SCALE, 1.f, (nTier == 2) ? 1.6f : 1.35f, 0.6f, Easing::inQuad, fHold);
        };

        auto createSpines = [this, ptrCount, ptrShown, updateCountLabel, showMilestone](CContainerPtr ptrTo, int numOfSpines)
        {
            if (numOfSpines > 0)
            {
                auto ptrBarbTex = CGfx::getInstance()->getTextureById("barbarian.png");

                for (auto i = 0; i < numOfSpines; i++)
                {
                    static const char* fileNames[] = { "barbarian_idle.panm", "barbarian_run.panm" };
                    static const char* animNames[] = { "idle", "run" };

                    auto ptrSpine = std::make_shared<CBakedSpine>(fileNames, animNames, ptrBarbTex);

                    ptrSpine->setAnimationByName(getRandomElement(animNames), true);
                    ptrSpine->setPos(fastRandU32() % static_cast<int>(Engine::getCfg().INIT_SCR_CX - 300) + 150 , 
                        fastRandU32() % static_cast<int>(Engine::getCfg().INIT_SCR_CY - 350) + 205);
                    ptrSpine->setSkipLight(true);

                    ptrTo->addChild(ptrSpine);
                }

                *ptrCount += numOfSpines;

                updateCountLabel();

                static const std::pair<int, const char*> arrMilestones[] =
                {
                    { 100,  L10N::getInstance().tr("MILESTONE_100")  },
                    { 200,  L10N::getInstance().tr("MILESTONE_200")  },
                    { 300,  L10N::getInstance().tr("MILESTONE_300")  },
                    { 500,  L10N::getInstance().tr("MILESTONE_500")  },
                    { 1000, L10N::getInstance().tr("MILESTONE_1000") },
                    { 2000, L10N::getInstance().tr("MILESTONE_2000") },
                };

                for (const auto& ms : arrMilestones)
                {
                    if (*ptrCount >= ms.first && !ptrShown->count(ms.first))
                    {
                        ptrShown->insert(ms.first);
                        showMilestone(ms.first, ms.second);
                    }
                }
            }
            else
            {
                ptrTo->removeAll();

                *ptrCount = 0;
                ptrShown->clear();

                updateCountLabel();
            }
        };

        auto ptrBtn1 = OrangeSlicedButton::makeInst(
            55,
            L10N::getInstance().tr("BAKED_ADD1"),
            [createSpines, ptrTo = _ptrBakedSpines->weak_from_this()]
            {
                if (auto p = ptrTo.lock())
                    createSpines(p, 1);
            },
            nullptr,
            true);

        auto ptrBtn10 = OrangeSlicedButton::makeInst(
            55,
            L10N::getInstance().tr("BAKED_ADD10"),
            [createSpines, ptrTo = _ptrBakedSpines->weak_from_this()]
            {
                if (auto p = ptrTo.lock())
                    createSpines(p, 10);
            },
            nullptr,
            true);

        auto ptrBtnClear = OrangeSlicedButton::makeInst(
            55,
            L10N::getInstance().tr("BAKED_CLEAR"),
            [createSpines, ptrTo = _ptrBakedSpines->weak_from_this()]
            {
                if (auto p = ptrTo.lock())
                    createSpines(p, 0);
            },
            nullptr,
            true);

        auto ptrBtnsCont = std::make_shared<CContainer>();

        ptrBtnsCont->addChild(ptrBtn1);
        ptrBtnsCont->addChild(ptrBtn10);
        ptrBtnsCont->addChild(ptrBtnClear);
        ptrBtnsCont->addChild(ptrCountLabel);

        ptrBtnsCont->alignChildren(eChildrenAlign::HORIZONTAL, 20.f, getDialogCx());

        updateCountLabel();

        _ptrBakedSpines->setPos(0, 0);

        _ptrBakedDemoCont->addChild(_ptrBakedSpines);
        _ptrBakedDemoCont->addChild(ptrBtnsCont);

        ptrBtnsCont->setXPosCentered(getDialogCx());
        ptrBtnsCont->setY(getDialogCy() - (ptrBtnsCont->calcNotTransCy() + 50));
        createSpines(_ptrBakedSpines, 1);
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
	_ptrPreloader->filesToLoad().addSpine("barbarian");;
	_ptrPreloader->filesToLoad().add("ui.png");
	_ptrPreloader->filesToLoad().add("ui.atlas");
	_ptrPreloader->filesToLoad().add("barbarian_idle.panm");
	_ptrPreloader->filesToLoad().add("barbarian_run.panm");
	_ptrPreloader->filesToLoad().setSizeToLoad(63996535);
	_ptrPreloader->init(cbOnLoaded);
	HTMLDom::getInstance()->parseCSSFromFile("EMBED/HTML/styles.css");

}