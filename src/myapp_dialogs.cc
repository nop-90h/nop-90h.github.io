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

constexpr int SIDE_LEFT  = 0;
constexpr int SIDE_RIGHT = 1;

enum class eBehindBgKind
{
    SAKURA,
    HEARTS,
    GRENADES,
    FIREWORKS,
    BLIZZARD,
    AUTUMN,
    SOLITAIRE_STORM,
};

std::string demoLink(const char* id, const char* text, const char* color)
{
    std::string s;

    s += "<a id=\"";
    s += id;
    s += "\" hovercolor=\"#FFE9A0FF\" pressedcolor=\"#FFC040FF\" cursor=\"hand\">";
    s += "<u color=\"";
    s += color;
    s += "B0\" thickness=\"2\">";
    s += "<b><font fx=\"ripple\" repeatdelay=\"5\" color=\"";
    s += color;
    s += "\" shadow=\"1\" shadowcolor=\"#000000AA\">";
    s += text;
    s += "</font></b></u></a>";

    return s;
}

std::string demoButton(const char* id, const char* caption, int nWidth = 280)
{
    std::string s;

    s += "<ninebutton id=\"";
    s += id;
    s += "\" src=\"UI/slicedCyanBtn_normal\" hover=\"UI/slicedCyanBtn_hl\""
            " pressed=\"UI/slicedCyanBtn_normal\" a=\"20\" b=\"20\" c=\"20\" d=\"20\""
            " width=\"";
    s += std::to_string(nWidth);
    s += "\" padding=\"40\">"
            "<b><font name=\"edugot\" size=\"28\" color=\"#08272B\" shadow=\"1\" shadowcolor=\"#FFFFFF66\">";
    s += caption;
    s += "</font></b></ninebutton>";

    return s;
}

static std::string nl2br(const std::string& s)
{
    std::string out;
    out.reserve(s.size());

    for (size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == '\n')
            out += "<br>";
        else
            out += s[i];
    }

    return out;
}

static std::string locHtml(const char* key)
{
    return nl2br(L10N::getInstance().tr(key));
}

struct SakuraCont : CContainer
{
    virtual void calcNotTransBounds(Rect* p) override
    {
        p->set(0, 0, Engine::getCfg().INIT_SCR_CX, Engine::getCfg().INIT_SCR_CY);
    }
};

class DialogDemoExample final : public BaseDialog
{
public:
    bool initExample(const char* lpszTitle,
                        float cx,
                        float cy,
                        BaseDialog* pParent,
                        eScrollType eScroll = eScrollType::E_ST_NONE,
                        bool bCloseButton = true,
                        bool bClickAnywhere = false,
                        bool bPanelBg = false)
    {
        auto ptrSL = SpriteLoader::getInstance();

        Rect rc{ 30.f, 100.f, cx - 60.f, cy - 170.f };
        Rect* pRc = (eScroll != eScrollType::E_ST_NONE) ? &rc : nullptr;

        if (bPanelBg)
        {
            auto ptrNine = std::make_shared<NineSlice>();
            ptrNine->createSlices(ptrSL->getSprite("UI/nineslicedemo2"), 138, 138, 138, 138);

            if (!BaseDialog::init(cx, cy, eScroll, pRc, ptrNine, pParent))
                return false;
        }
        else
        {
            if (!BaseDialog::init(cx, cy, eScroll, pRc, ptrSL->getSprite("UI/frameBg"), pParent))
                return false;
        }

        if (bClickAnywhere)
            _eCloseStyle = eDialogCloseStyle::CLICK_ANYWHERE;

        if (!bPanelBg)
            createFgLayer(ptrSL->getSprite("UI/frameFg"), 60, 60, 46, 46);

        if (lpszTitle && *lpszTitle)
        {
            setTitle(lpszTitle, ptrSL->getSprite("UI/frameTitle"), 113, 113);
            _titleCont->setY(-20);
        }

        if (bCloseButton)
        {
            createCloseButton(ptrSL->getSprite("UI/btn_close_normal"),
                                ptrSL->getSprite("UI/btn_close_pressed"),
                                ptrSL->getSprite("UI/btn_close_hovered"),
                                ptrSL->getSprite("UI/btn_close_disabled"));
        }

        return true;
    }

    void setHtml(const char* lpszHtml,
                    float y = 90.f,
                    float widthPad = 40.f)
    {
        _ptrHtmlLabel = std::make_shared<StaticLabel>();

        _ptrHtmlLabel->setBoxMode(eTextRenderType::HTML_BOX, _fDialogCx - widthPad);
        _ptrHtmlLabel->setText(lpszHtml);
        _ptrHtmlLabel->setInteractive(true);
        _ptrHtmlLabel->setXPosCentered(_fDialogCx);
        _ptrHtmlLabel->setY(y);

        _root->addChild(_ptrHtmlLabel);
    }

    void bindClick(const char* id, SimpleCallback cb)
    {
        if (!_ptrHtmlLabel || !cb)
            return;

        _ptrHtmlLabel->getHTMLDocument().onClick(id, [cb] { cb(); });
    }

    void openSidePanel(int eSide)
    {
        if (eSide != SIDE_LEFT && eSide != SIDE_RIGHT)
            return;

        if (_sidePanels[eSide] && _sidePanels[eSide]->isVisible())
        {
            closeSidePanel(eSide);
            return;
        }

        closeSidePanel(eSide, true);

        const float pw = 400.f;
        const float ph = _fDialogCy - 40.f;

        auto panel = std::make_shared<DialogDemoExample>();

        if (!panel->initExample(nullptr, pw, ph, this, eScrollType::E_ST_NONE, false, false, true))
            return;

        std::string sHtml;

        sHtml += "<center>\n";
        sHtml += "<font name=\"edugot\" size=\"26\" color=\"#261C14\" shadow=\"1\" shadowcolor=\"#00000022\" appear=\"rise\" dur=\"0.4\">\n";
        sHtml += locHtml("DLG_SIDE_PANEL_TEXT");
        sHtml += "\n</font>\n<br><br>\n";

        sHtml += demoLink("side_close",
                            L10N::getInstance().tr("BTN_CLOSE"),
                            "#D32F2F");

        sHtml += "\n</center>";

        panel->setHtml(sHtml.c_str(), 95.f, 120.f);

        panel->bindClick("side_close", [this, eSide]()
        {
            closeSidePanel(eSide);
        });

        const float cx0 = (_fDialogCx - pw) * 0.5f;
        const float cy0 = (_fDialogCy - ph) * 0.5f;
        const float ex  = (eSide == SIDE_LEFT) ? -400 : _fDialogCx;

        panel->setPos(cx0, cy0);
        panel->showAsPanel(E_IH_DISPATCHBYPARENT, true);
        panel->bringUnderParent();

        panel->addSelfTween(eTweenProp::X, cx0, ex, 0.45f, Easing::outBack);

        _sidePanels[eSide] = panel;
    }

    void closeSidePanel(int eSide, bool bImmediate = false)
    {
        if (eSide != SIDE_LEFT && eSide != SIDE_RIGHT)
            return;

        auto p = _sidePanels[eSide];
        _sidePanels[eSide].reset();

        if (!p || !p->isVisible())
            return;

        if (bImmediate)
        {
            p->close();
            return;
        }

        const float cx0 = (_fDialogCy - p->getDialogCx()) * 0.5f;

        p->removeSelfTweens();
        p->addSelfTween(eTweenProp::X, p->getX(), cx0, 0.3f, Easing::inCubic, 0.f, [p]()
        {
            p->close();
        });
    }

    void closeAllSidePanels(void)
    {
        for (int i = SIDE_LEFT; i <= SIDE_RIGHT; ++i)
            closeSidePanel(i, true);
    }

    void finalizeScroll()
    {
        if (_eScrollType != eScrollType::E_ST_NONE)
        {
            updateMaxScroll();
            updateScrollBox();
        }
    }

    void autoClose(float fSeconds)
    {
        setTimeout(fSeconds, [w = weak_from_this()]()
        {
            if (auto p = std::static_pointer_cast<BaseDialog>(w.lock()))
            {
                if (p->isVisible())
                    p->close();
            }
        });
    }

protected:
    virtual void createScrollShadow(Rect* pRcScroll) override
    {
        (void)pRcScroll;
    }

    virtual void onShow(bool bShow) override
    {
        if (bShow && _frame)
        {
            _frame->setAlpha(0.f);
            _frame->setY(25.f);

            _frame->addSelfTween(eTweenProp::ALPHA,
                                    0.f,
                                    1.f,
                                    0.22f,
                                    Easing::linear);

            _frame->addSelfTween(eTweenProp::Y,
                                    25.f,
                                    0.f,
                                    0.32f,
                                    Easing::outBack);
        }
    }

private:
    StaticLabelPtr _ptrHtmlLabel;
    BaseDialogPtr  _sidePanels[2];
};

class AppDialog : public BaseDialog
{
private:

	CContainerPtr		_ptrDialogsDemoCont;
    StaticLabelPtr      _ptrDialogsDemoLabel;
    unsigned long long  _demoStatusNodeId = 0;

private:
static void spawnGrenade(CContainer*          pCont,
                         CParticleSystemPtr   ptrExplosion,
                         CParticleSystemPtr   ptrDebris,
                         CParticleSystemPtr   ptrBg,
                         std::shared_ptr<int> ptrAlive,
                         std::shared_ptr<int> ptrShocks)
{
    auto ptrG = SpriteLoader::getInstance()->getSprite("UI/grenade");

    if (!ptrG)
    {
        --(*ptrAlive);
        return;
    }

    ptrG->setScaleToY(64.f);
    ptrG->setPivotCentered();

    pCont->addChild(ptrG);

    const float fScrCx  = Engine::getCfg().INIT_SCR_CX;
    const float fScrCy  = Engine::getCfg().INIT_SCR_CY;

    const float fGCy    = ptrG->calcNotTransCy();
    const float fGCx    = ptrG->calcNotTransCx();

    const float fX0     = randomRange(fScrCx * 0.12f, fScrCx * 0.88f);
    const float fDrift  = randomRange(-140.f, 140.f);
    const float fStartY = -fGCy - 20.f;
    const float fFloorY = randomRange(fScrCy * 0.60f, fScrCy * 0.85f) - fGCy;

    ptrG->setPos(fX0, fStartY);

    const float fFlight = 1.87f;

    ptrG->addSelfTween(eTweenProp::X, fX0, fX0 + fDrift, fFlight, Easing::linear);

    ptrG->addSelfTween(eTweenProp::ROTATE,
                       0.f,
                       (rand() % 2 ? 1.f : -1.f) * randomRange(7.f, 13.f),
                       fFlight,
                       Easing::linear);

    CContainerWPtr wG = ptrG;

    SimpleCallback explode = [pCont, ptrExplosion, ptrDebris, ptrBg, ptrAlive, ptrShocks, wG, fGCx, fGCy]()
    {
        --(*ptrAlive);

        auto pG = wG.lock();

        if (!pG)
            return;

        const float fExX = pG->getX() + fGCx * 0.5f;
        const float fExY = pG->getY() + fGCy * 0.5f;

        if (ptrBg)
        {
            ++(*ptrShocks);

            ptrBg->repelFrom(fExX - ptrBg->getX(),
                             fExY - ptrBg->getY(),
                             3200.f,
                             580.f);

            ptrBg->setTimeout(0.5f, [ptrBg, ptrShocks]()
            {
                if (--(*ptrShocks) <= 0)
                    ptrBg->disableRepeller();
            });
        }

        pG->removeSelfTweens();

        const float fSx = pG->getScaleX();
        const float fSy = pG->getScaleY();

        pG->addSelfTween(eTweenProp::SCALE_X, fSx, fSx * 1.45f, 0.07f, Easing::outQuad);
        pG->addSelfTween(eTweenProp::SCALE_Y, fSy, fSy * 1.45f, 0.07f, Easing::outQuad);

        pG->addSelfTween(eTweenProp::ROTATE,
                         pG->getRotate(),
                         pG->getRotate() + 7.f,
                         0.26f,
                         Easing::linear);

        pG->addSelfTween(eTweenProp::SCALE_X, fSx * 1.45f, 0.f, 0.19f, Easing::inCubic, 0.07f);
        pG->addSelfTween(eTweenProp::SCALE_Y, fSy * 1.45f, 0.f, 0.19f, Easing::inCubic, 0.07f,
            [wG]()
            {
                if (auto p = wG.lock())
                    p->removeFromParent();
            });

        pG->addSelfTween(eTweenProp::ALPHA, 1.f, 0.f, 0.12f, Easing::linear, 0.14f);

        ptrExplosion->setTimeout(0.05f, [ptrExplosion, ptrDebris, fExX, fExY]()
        {
            ptrExplosion->burstAtLocal(fExX, fExY, 60);
            ptrDebris->burstAtLocal(fExX, fExY, 50);
        });

        ptrExplosion->setTimeout(0.16f, [ptrExplosion, fExX, fExY]()
        {
            ptrExplosion->burstAtLocal(fExX, fExY - 12.f, 22);
        });
    };

    auto fall = [wG](float fFrom, float fTo, float fDur, SimpleCallback cb)
    {
        if (auto pG = wG.lock())
            pG->addSelfTween(eTweenProp::Y, fFrom, fTo, fDur, Easing::inCubic, 0.f, cb);
    };

    auto rise = [wG](float fFrom, float fTo, float fDur, SimpleCallback cb)
    {
        if (auto pG = wG.lock())
            pG->addSelfTween(eTweenProp::Y, fFrom, fTo, fDur, Easing::outCubic, 0.f, cb);
    };

    const float fBounce1 = randomRange(95.f, 130.f);
    const float fBounce2 = randomRange(30.f, 55.f);

    fall(fStartY, fFloorY, 0.75f, [rise, fall, wG, fFloorY, fBounce1, fBounce2, explode]()
    {
        rise(fFloorY, fFloorY - fBounce1, 0.34f, [rise, fall, wG, fFloorY, fBounce1, fBounce2, explode]()
        {
            fall(fFloorY - fBounce1, fFloorY, 0.34f, [rise, fall, wG, fFloorY, fBounce2, explode]()
            {
                rise(fFloorY, fFloorY - fBounce2, 0.22f, [fall, wG, fFloorY, fBounce2, explode]()
                {
                    fall(fFloorY - fBounce2, fFloorY, 0.22f, explode);
                });
            });
        });
    });
}

CContainerPtr createBehindParticles(eBehindBgKind eKind, BaseDialog* pFlowDlg = nullptr)
{
    auto ptrSpr = SpriteLoader::getInstance()->getSprite("UI/whitebox");

    ptrSpr->setScaleTo(Engine::getCfg().INIT_SCR_CX, Engine::getCfg().INIT_SCR_CY);
    ptrSpr->setRgba(0xfeb9beff);

    auto ptrCont = std::make_shared<SakuraCont>();

    ptrCont->addChild(ptrSpr);

    CParticleSystemPtr ptrBackPartickes;

    if (eKind == eBehindBgKind::SOLITAIRE_STORM)
        return std::make_shared<CosmicCardStorm>();

    if (eKind == eBehindBgKind::HEARTS)
    {
        ptrBackPartickes = std::make_shared<CParticleSystem<>>(ParticlePresets::getBokehAmbientPreset(), "PARTICLES/heart");

        auto& s = ptrBackPartickes->getSettings();

        s.spawnRadius = Engine::getCfg().INIT_SCR_CX;
        s.spawnRate = 60;
        s.maxParticles = 120;

        ptrBackPartickes->setPos((Engine::getCfg().INIT_SCR_CX - s.spawnRadius) * 0.5f, 0);

        ptrCont->addChild(ptrBackPartickes);

        ptrBackPartickes->prewarm(5);
    }
    else if (eKind == eBehindBgKind::AUTUMN)
    {
        const float fScrCx = Engine::getCfg().INIT_SCR_CX;
        const float fScrCy = Engine::getCfg().INIT_SCR_CY;

        ptrSpr->setRgba(0xc98a3dff);

        auto ptrBottom = SpriteLoader::getInstance()->getSprite("UI/whitebox");

        ptrBottom->setScaleTo(fScrCx, fScrCy);
        ptrBottom->setRgba(0xf5c58cff);
        ptrBottom->setAlpha(0.55f);

        ptrCont->addChild(ptrBottom);

        const int nRays = 6;

        for (int i = 0; i < nRays; ++i)
        {
            auto ptrRay = SpriteLoader::getInstance()->getSprite("PARTICLES/light");

            if (!ptrRay)
                continue;

            ptrRay->setBlendMode(eSpriteBlendMode::ADDITIVE);
            ptrRay->setTint(1.00f, 0.88f, 0.55f);

            const float fRayCy = fScrCy * 1.8f;

            ptrRay->setScaleTo(randomRange(180.f, 420.f), fRayCy);
            ptrRay->setPivotCentered();
            ptrRay->rotate(randomRange(-0.35f, -0.15f));

            const float fSlot  = (static_cast<float>(i) + 0.5f) / static_cast<float>(nRays);
            const float fX0    = fScrCx * (fSlot + randomRange(-0.06f, 0.06f));
            const float fDrift = randomRange(80.f, 160.f);

            ptrRay->setPos(fX0 - fDrift * 0.5f, fScrCy * 0.5f);

            const float fBaseA = randomRange(0.05f, 0.10f);

            ptrRay->setAlpha(fBaseA);

            ptrCont->addChild(ptrRay);

            ptrRay->addSelfTweenYoyo(eTweenProp::ALPHA,
                                     fBaseA * 0.5f,
                                     fBaseA * 1.6f,
                                     randomRange(3.5f, 6.0f),
                                     Easing::inOutSine,
                                     Easing::inOutSine);

            ptrRay->addSelfTweenYoyo(eTweenProp::X,
                                     fX0 - fDrift * 0.5f,
                                     fX0 + fDrift * 0.5f,
                                     randomRange(7.f, 11.f),
                                     Easing::inOutSine,
                                     Easing::inOutSine);
        }

        auto ptrDust = std::make_shared<CParticleSystem<>>(
            ParticlePresets::getAmbientDustPreset(),
            std::format("PARTICLES/{}", ParticlePresets::getPresetSpriteName(eParticlePreset::AMBIENT_DUST)).c_str());

        auto& ds = ptrDust->getSettings();

        ds.spawnRate    = 14.f;
        ds.maxParticles = 90;
        ds.lifetimeMin  = 5.f;
        ds.lifetimeMax  = 9.f;
        ds.spawnRadius  = 0.f;
        ds.spawnAreaCx  = fScrCx * 1.15f;
        ds.spawnAreaCy  = fScrCy * 1.15f;
        ds.speedMin     = 4.f;
        ds.speedMax     = 14.f;
        ds.windX        = 18.f;
        ds.drag         = 0.35f;
        ds.turbulence   = 10.f;
        ds.waveFrequency = 1.2f;
        ds.waveAmplitude = 12.f;
        ds.startScaleMin = 0.18f;
        ds.startScaleMax = 0.45f;
        ds.endScaleMin   = 0.18f;
        ds.endScaleMax   = 0.45f;
        ds.use3PhaseColor = true;

        ds.startColor[0]  = 1.00f;
        ds.startColor[1]  = 0.88f;
        ds.startColor[2]  = 0.45f;
        ds.startColor[3]  = 0.90f;

        ds.middleColor[0] = 1.00f;
        ds.middleColor[1] = 0.75f;
        ds.middleColor[2] = 0.30f;
        ds.middleColor[3] = 0.70f;

        ds.endColor[0]    = 0.90f;
        ds.endColor[1]    = 0.60f;
        ds.endColor[2]    = 0.20f;
        ds.endColor[3]    = 0.00f;

        ds.colorJitter    = 0.15f;
        ds.blendMode      = eSpriteBlendMode::ADDITIVE;
        ds.useDepth       = true;
        ds.depthMin       = 0.40f;
        ds.depthScaleMin  = 0.40f;
        ds.depthSpeedMin  = 0.40f;
        ds.depthAlphaMin  = 0.45f;
        ds.fadeInTime     = 0.6f;
        ds.fadeOutTime    = 1.4f;

        ptrDust->setPos(fScrCx * 0.5f, fScrCy * 0.5f);

        ptrCont->addChild(ptrDust);

        ptrDust->prewarm(6);

        const char* leaves[] = { "PARTICLES/aleaf_01", "PARTICLES/aleaf_02", "PARTICLES/aleaf_03", "PARTICLES/aleaf_04" };

        auto ptrLeaves = std::make_shared<CParticleSystem<>>(
            ParticlePresets::getFallingLeavesPreset(), leaves);

        auto& ls = ptrLeaves->getSettings();

        ls.spawnAreaCx = fScrCx * 1.2f;
        ls.spawnAreaCy = fScrCy * 1.2f;
        ls.floorY      = fScrCy * 0.5f + 30.f;

        ptrLeaves->setPos(fScrCx * 0.5f, fScrCy * 0.5f);

        ptrCont->addChild(ptrLeaves);

        ptrLeaves->prewarm(8);

        CContainer* pContRaw = ptrCont.get();

        auto ptrGustTick = std::make_shared<std::function<void()>>();
        std::weak_ptr<std::function<void()>> wGust = ptrGustTick;

        *ptrGustTick = [pContRaw, ptrLeaves, wGust]()
        {
            if (auto sp = wGust.lock())
                pContRaw->setTimeout(randomRange(6.0f, 10.0f), [sp]() { (*sp)(); });

            ptrLeaves->getSettings().gustAmp = 320.f;

            pContRaw->setTimeout(1.6f, [ptrLeaves]()
            {
                ptrLeaves->getSettings().gustAmp = 90.f;
            });
        };

        pContRaw->setTimeout(2.5f, [sp = ptrGustTick]() { (*sp)(); });
    }
    else if (eKind == eBehindBgKind::BLIZZARD)
    {
        ptrSpr->setRgba(0x141c28ff);

        const float fScrCx = Engine::getCfg().INIT_SCR_CX;
        const float fScrCy = Engine::getCfg().INIT_SCR_CY;

        for (int i = 0; i < 4; ++i)
        {
            auto ptrDrift = SpriteLoader::getInstance()->getSprite("PARTICLES/smoke_08");

            if (!ptrDrift)
                continue;

            ptrDrift->setBlendMode(eSpriteBlendMode::NORMAL);
            ptrDrift->setTint(0.72f, 0.78f, 0.86f);
            ptrDrift->setScaleTo(randomRange(650.f, 1100.f), randomRange(140.f, 230.f));
            ptrDrift->setAlpha(0.85f);
            ptrDrift->setPos(randomRange(-200.f, fScrCx - 300.f), fScrCy - randomRange(10.f, 70.f));

            ptrCont->addChild(ptrDrift);
        }

        auto ptrSnow = std::make_shared<CParticleSystem<>>(
            ParticlePresets::getAdvancedSnowPreset(),
            std::format("PARTICLES/{}", ParticlePresets::getPresetSpriteName(eParticlePreset::ADVANCED_SNOW)).c_str());

        auto& ss = ptrSnow->getSettings();

        ss.spawnRate    = 60.f;
        ss.maxParticles = 350;
        ss.spawnRadius  = 0.f;
        ss.spawnAreaCx  = fScrCx * 1.3f;
        ss.spawnAreaCy  = fScrCy * 1.3f;
        ss.windX        = 40.f;
        ss.gustAmp      = 120.f;
        ss.gustFreq     = 0.5f;
        ss.floorY       = fScrCy * 0.5f + 20.f;

        ptrSnow->setPos(fScrCx * 0.5f, fScrCy * 0.5f);

        ptrCont->addChild(ptrSnow);

        ptrSnow->prewarm(5);

        auto ptrFog = SpriteLoader::getInstance()->getSprite("UI/whitebox");

        ptrFog->setScaleTo(fScrCx, fScrCy);
        ptrFog->setRgba(0xcfe0f2ff);
        ptrFog->setAlpha(0.f);

        ptrCont->addChild(ptrFog);

        auto ptrCharge = std::make_shared<CParticleSystem<>>(
            ParticlePresets::getSandstormPreset(),
            "PARTICLES/circle_05");

        auto& cs = ptrCharge->getSettings();

        cs.spawnRate    = 70.f;
        cs.maxParticles = 220;
        cs.lifetimeMin  = 2.0f;
        cs.lifetimeMax  = 3.5f;
        cs.spawnRadius  = 0.f;
        cs.spawnAreaCx  = 60.f;
        cs.spawnAreaCy  = fScrCy * 1.25f;
        cs.speedMin     = 400.f;
        cs.speedMax     = 700.f;
        cs.angleMin     = -6.f;
        cs.angleMax     =  6.f;
        cs.gravityX     = 0.f;
        cs.gravityY     = 8.f;
        cs.turbulence   = 30.f;
        cs.gustAmp      = 200.f;
        cs.gustFreq     = 0.5f;
        cs.velocityStretch = 0.0025f;
        cs.startScaleMin = 0.06f;
        cs.startScaleMax = 0.14f;
        cs.endScaleMin   = 0.06f;
        cs.endScaleMax   = 0.14f;
        cs.use3PhaseColor = true;

        cs.startColor[0]  = 0.85f;
        cs.startColor[1]  = 0.92f;
        cs.startColor[2]  = 1.00f;
        cs.startColor[3]  = 0.0f;

        cs.middleColor[0] = 0.85f;
        cs.middleColor[1] = 0.92f;
        cs.middleColor[2] = 1.00f;
        cs.middleColor[3] = 0.45f;

        cs.endColor[0]    = 0.85f;
        cs.endColor[1]    = 0.92f;
        cs.endColor[2]    = 1.00f;
        cs.endColor[3]    = 0.0f;

        cs.blendMode      = eSpriteBlendMode::ADDITIVE;

        ptrCharge->setPos(-60.f, fScrCy * 0.5f);

        ptrCont->addChild(ptrCharge);

        ptrCharge->prewarm(3);

        CContainer* pContRaw = ptrCont.get();

        auto ptrGustTick = std::make_shared<std::function<void()>>();
        std::weak_ptr<std::function<void()>> wGust = ptrGustTick;

        *ptrGustTick = [pContRaw, ptrSnow, ptrCharge, ptrFog, wGust]()
        {
            if (auto sp = wGust.lock())
                pContRaw->setTimeout(randomRange(4.0f, 7.5f), [sp]() { (*sp)(); });

            ptrSnow->getSettings().windX   = 150.f;
            ptrSnow->getSettings().gustAmp = 260.f;
            ptrCharge->getSettings().gustAmp = 340.f;

            ptrFog->removeSelfTweens();
            ptrFog->addSelfTween(eTweenProp::ALPHA, ptrFog->getAlpha(), 0.30f, 0.5f, Easing::linear);
            ptrFog->addSelfTween(eTweenProp::ALPHA, 0.30f, 0.f, 1.7f, Easing::outQuad, 0.6f);

            pContRaw->setTimeout(1.8f, [ptrSnow, ptrCharge]()
            {
                ptrSnow->getSettings().windX   = 40.f;
                ptrSnow->getSettings().gustAmp = 120.f;
                ptrCharge->getSettings().gustAmp = 200.f;
            });
        };

        pContRaw->setTimeout(2.0f, [sp = ptrGustTick]() { (*sp)(); });
    }
    else if (eKind == eBehindBgKind::SAKURA)
    {
        const char* arr[] = { "PARTICLES/sakura1", "PARTICLES/sakura2", "PARTICLES/sakura3" };

        ptrSpr->setRgba(0x1a1a2eff);

        const float fScrCx = Engine::getCfg().INIT_SCR_CX;
        const float fScrCy = Engine::getCfg().INIT_SCR_CY;

        const float fEmX = fScrCx * 0.5f;
        const float fEmY = fScrCy * 0.16f;

        auto ptrBranchBurst = std::make_shared<CParticleSystem<>>(ParticlePresets::getSakuraPreset(), arr);

        auto& bs = ptrBranchBurst->getSettings();

        bs.spawnRate     = 14.f;
        bs.maxParticles  = 400;
        bs.lifetimeMin   = 7.0f;
        bs.lifetimeMax   = 10.0f;
        bs.spawnRadius   = 0.f;
        bs.spawnAreaCx   = fScrCx;
        bs.spawnAreaCy   = fScrCy * 0.36f;
        bs.speedMin      = 60.f;
        bs.speedMax      = 160.f;
        bs.angleMin      = 75.f;
        bs.angleMax      = 105.f;
        bs.gravityX      = 10.f;
        bs.gravityY      = 55.f;
        bs.drag          = 0.25f;
        bs.startScaleMin = 0.22f;
        bs.startScaleMax = 0.38f;
        bs.endScaleMin   = 0.16f;
        bs.endScaleMax   = 0.28f;
        bs.fadeInTime    = 0.08f;
        bs.fadeOutTime   = 1.2f;
        bs.waveAmplitude = 22.f;
        bs.flutterSpeedMin = 2.0f;
        bs.flutterSpeedMax = 4.0f;
        bs.flutterSlip   = 35.f;
        bs.colorJitter   = 0.10f;
        bs.floorY        = (fScrCy - fEmY) + 20.f;

        ptrBranchBurst->setPos(fEmX, fEmY);
        ptrBranchBurst->setEmitting(false);

        ptrBranchBurst->setTimeout(1.3f, [ptrBranchBurst]()
        {
            ptrBranchBurst->setEmitting(true);
        });

        const float fRotFrom = -0.02f;
        const float fRotTo   =  0.03f;
        const float arrHeights[] = { 0.52f, 0.42f, 0.47f };

        float fCursorX = 0.f;
        int   nIdx     = 0;

        while (fCursorX < fScrCx)
        {
            auto ptrBranch = SpriteLoader::getInstance()->getSprite("UI/sakura");

            if (!ptrBranch)
                break;

            ptrBranch->setScaleToY(fScrCy * arrHeights[nIdx % SIZE_OF(arrHeights)]);

            const bool bFlip = (nIdx % 2) != 0;

            if (bFlip)
                ptrBranch->flipX();

            ptrCont->addChild(ptrBranch);

            const float fBranchCx = ptrBranch->calcNotTransCx();
            const float fBranchCy = ptrBranch->calcNotTransCy();

            if (fBranchCx <= 1.f || fBranchCy <= 1.f)
                break;

            const float fNextX = fCursorX + fBranchCx * 0.8f;
            const bool  bLast  = (fNextX >= fScrCx);

            float fX;

            if (bLast)
                fX = bFlip ? fScrCx : (fScrCx - fBranchCx);
            else
                fX = bFlip ? (fCursorX + fBranchCx) : fCursorX;

            const float fYFin   = -12.f;
            const float fYStart = fYFin - fBranchCy;

            ptrBranch->setPos(fX, fYStart);
            ptrBranch->setAlpha(0.f);
            ptrBranch->rotate(fRotFrom);

            const float fDelay = 0.15f + 0.10f * static_cast<float>(nIdx);

            ptrBranch->addSelfTween(eTweenProp::ALPHA, 0.f, 1.f, 0.4f, Easing::linear, fDelay);

            ptrBranch->addSelfTweenYoyo(eTweenProp::ROTATE,
                                        fRotFrom,
                                        fRotTo,
                                        2.6f,
                                        Easing::inOutSine,
                                        Easing::inOutSine,
                                        fDelay + 1.1f);

            const float fMidX = bFlip ? (fX - fBranchCx * 0.5f) : (fX + fBranchCx * 0.5f);
            const float fDx   = fBranchCx * 0.18f * (bFlip ? -1.f : 1.f);

            const float fBx0 = fMidX - fEmX;
            const float fBy0 = (fYFin + fBranchCy * 0.30f) - fEmY;

            const float fBx1 = fMidX + fDx - fEmX;
            const float fBy1 = (fYFin + fBranchCy * 0.62f) - fEmY;

            ptrBranch->addSelfTween(eTweenProp::Y,
                                    fYStart,
                                    fYFin,
                                    1.1f,
                                    Easing::outBack,
                                    fDelay,
                                    [ptrBranchBurst, fBx0, fBy0, fBx1, fBy1]()
                                    {
                                        ptrBranchBurst->burstAtLocal(fBx0, fBy0, 18);

                                        ptrBranchBurst->setTimeout(0.14f, [ptrBranchBurst, fBx1, fBy1]()
                                        {
                                            ptrBranchBurst->burstAtLocal(fBx1, fBy1, 15);
                                        });
                                    });

            fCursorX = fNextX;
            ++nIdx;
        }

        ptrCont->addChild(ptrBranchBurst);
    }
    else
    {
        ptrSpr->setRgba(0x14141cff);

        const float fScrCx = Engine::getCfg().INIT_SCR_CX;
        const float fScrCy = Engine::getCfg().INIT_SCR_CY;

        auto ptrBg = std::make_shared<CParticleSystem<>>(
            ParticlePresets::getDistantEmbersPreset(), "PARTICLES/flare_01");

        auto& bgs = ptrBg->getSettings();

        bgs.spawnAreaCx = fScrCx * 1.15f;
        bgs.spawnAreaCy = fScrCy * 1.15f;
        bgs.spawnRate = 40;
        bgs.maxParticles = 280;

        ptrBg->setPos(fScrCx * 0.5f, fScrCy * 0.5f);

        ptrCont->addChild(ptrBg);

        ptrBg->prewarm(5);

        auto ptrExplosion = std::make_shared<CParticleSystem<>>(
            ParticlePresets::getGrenadeExplosionPreset(),
            std::format("PARTICLES/{}", ParticlePresets::getPresetSpriteName(eParticlePreset::GRENADE_EXPLOSION)).c_str());

        ptrCont->addChild(ptrExplosion);

        auto ptrDebris = std::make_shared<CParticleSystem<>>(
            ParticlePresets::getDebrisPreset(),
            std::format("PARTICLES/{}", ParticlePresets::getPresetSpriteName(eParticlePreset::DEBRIS)).c_str());

        ptrDebris->getSettings().floorY = fScrCy + 40.f;

        ptrCont->addChild(ptrDebris);

        auto ptrAlive  = std::make_shared<int>(0);
        auto ptrShocks = std::make_shared<int>(0);

        CContainer* pContRaw = ptrCont.get();

        auto ptrSpawnTick = std::make_shared<std::function<void()>>();
        std::weak_ptr<std::function<void()>> wSpawnTick = ptrSpawnTick;

        *ptrSpawnTick = [pContRaw, ptrExplosion, ptrDebris, ptrBg, ptrAlive, ptrShocks, wSpawnTick]()
        {
            if (auto sp = wSpawnTick.lock())
                pContRaw->setTimeout(randomRange(0.4f, 0.9f), [sp]() { (*sp)(); });

            if (*ptrAlive >= 5)
                return;

            ++(*ptrAlive);

            spawnGrenade(pContRaw, ptrExplosion, ptrDebris, ptrBg, ptrAlive, ptrShocks);
        };

        pContRaw->setTimeout(0.4f, [sp = ptrSpawnTick]() { (*sp)(); });
    }

    return ptrCont;
}

void openStackLevel(int level)
{
    std::string strTitle = std::vformat(L10N::getInstance().tr("DLG_STACK_LEVEL_FMT"), std::make_format_args(level));

    auto dlg = std::make_shared<DialogDemoExample>();

    if (!dlg->initExample(strTitle.c_str(), 680.f, 400.f, nullptr))
        return;

    auto ptrCont = createBehindParticles(eBehindBgKind::AUTUMN, dlg.get());
    dlg->setBehindBg(ptrCont);

    std::string sHtml;

    sHtml += R"HTML(<center>
<font name="edugot" size="30" color="#FFFFFF" shadow="1" shadowcolor="#00000088" appear="rise" dur="0.5">
)HTML";

    sHtml += std::vformat(L10N::getInstance().tr("DLG_STACK_MODAL_FMT"), std::make_format_args(level));
    sHtml += "<br>";

    if (level < 3)
        sHtml += L10N::getInstance().tr("DLG_STACK_UNDER");
    else
        sHtml += L10N::getInstance().tr("DLG_STACK_LAST");

    sHtml += R"HTML(
</font>
<br><br>
<table padding="6" border="0" grid="none" cellspacing="2" align="center"><tr>)HTML";

    if (level < 3)
    {
        sHtml += R"HTML(<td>)HTML";
        sHtml += demoButton("btn_next", L10N::getInstance().tr("BTN_NEXT_DIALOG"));
        sHtml += R"HTML(</td>)HTML";
    }

    sHtml += R"HTML(<td>)HTML";
    sHtml += demoButton("btn_close", L10N::getInstance().tr("BTN_CLOSE"));
    sHtml += R"HTML(</td></tr></table>
</center>)HTML";

    dlg->setHtml(sHtml.c_str());

    if (level < 3)
    {
        dlg->bindClick("btn_next", [this, level]()
        {
            openStackLevel(level + 1);
        });
    }

    dlg->bindClick("btn_close", [this, w = dlg->weak_from_this()]()
    {
        if (auto p = std::static_pointer_cast<BaseDialog>(w.lock()))
        {
            p->close();
        }
    });
    setVisible(false);
    dlg->open([this, level]{ if (level == 1) setVisible(true);});
}

void initDialogsDemo()
{
    _ptrDialogsDemoCont = std::make_shared<CContainer>();
    _ptrDialogsDemoLabel = std::make_shared<StaticLabel>();
    _ptrDialogsDemoLabel.reset();
    _demoStatusNodeId = 0;
    _root->addChild(_ptrDialogsDemoCont);

    std::string html;

    html += "<font name=\"marmelad\" size=\"52\" color=\"#CCBBAA\" shadow=\"1\" shadowcolor=\"#000000AA\" appear=\"rise\" dur=\"0.8\">\n";

    html += "<i>";
    html += locHtml("DLG_DEMO_BASE_TEXT");
    html += "</i><br>";

    html += locHtml("DLG_DEMO_CLICKABLE");
    html += "<br><br>\n";

    html += demoLink("demo_modal",
                     L10N::getInstance().tr("DLG_LINK_MODAL"),
                     "#40C4D4");
    html += " ";
    html += locHtml("DLG_MODAL_CAPTURES");
    html += "<br>";

    html += demoLink("demo_panel",
                     L10N::getInstance().tr("DLG_LINK_PANEL"),
                     "#5ED95E");
    html += " ";
    html += locHtml("DLG_PANEL_LIVES");
    html += "<br>\n";

    html += demoLink("demo_cb",
                     L10N::getInstance().tr("DLG_LINK_CB"),
                     "#FF7A8E");
    html += " ";
    html += locHtml("DLG_CB_CALLED");
    html += "<br>";

    html += demoLink("demo_stack",
                     L10N::getInstance().tr("DLG_LINK_STACK"),
                     "#A594F4");
    html += " ";
    html += locHtml("DLG_STACK_OPENS");
    html += "<br>";

    html += demoLink("demo_click",
                     L10N::getInstance().tr("DLG_LINK_CLICK"),
                     "#FFD54F");

    html += "&nbsp;&nbsp;";

    html += demoLink("demo_scroll",
                     L10N::getInstance().tr("DLG_LINK_SCROLL"),
                     "#4FC3F7");

    html += "\n</font>";

    _ptrDialogsDemoLabel = std::make_shared<StaticLabel>();

    _ptrDialogsDemoLabel->setBoxMode(eTextRenderType::HTML_BOX, getDialogCx() - 100.f);
    _ptrDialogsDemoLabel->setText(html.c_str());
    _ptrDialogsDemoLabel->setInteractive(true);
    _ptrDialogsDemoLabel->setXPosCentered(getDialogCx());
    _ptrDialogsDemoLabel->setY(10.f);

    _ptrDialogsDemoCont->addChild(_ptrDialogsDemoLabel);
    _ptrDialogsDemoCont->setY(70.f);

    auto& doc = _ptrDialogsDemoLabel->getHTMLDocument();

    _demoStatusNodeId = doc.find("demo_status");

    auto bind = [&doc](const char* id, SimpleCallback cb)
    {
        doc.onClick(id, [cb] { cb(); });
    };

    bind("demo_modal", [this]()
    {
        auto dlg = std::make_shared<DialogDemoExample>();

        if (!dlg->initExample(L10N::getInstance().tr("DLG_MODAL_TITLE"), 720.f, 450.f, nullptr))
            return;

        std::string sHtml;

        sHtml += R"HTML(<center>
<font name="edugot" size="30" color="#FFFFFF" shadow="1" shadowcolor="#00000088" appear="rise" dur="0.5">
)HTML";

        sHtml += locHtml("DLG_MODAL_TEXT");

        sHtml += R"HTML(
</font>
<br><br>
<table padding="6" border="0" grid="none" cellspacing="2" align="center"><tr><td>)HTML";

        sHtml += demoButton("btn_ok", L10N::getInstance().tr("BTN_OK"));

        sHtml += R"HTML(</td><td>)HTML";

        sHtml += demoButton("btn_cancel", L10N::getInstance().tr("BTN_CANCEL"));

        sHtml += R"HTML(</td></tr></table>
</center>)HTML";

        auto ptrCont = createBehindParticles(eBehindBgKind::SAKURA);
        dlg->setBehindBg(ptrCont);

        dlg->setHtml(sHtml.c_str());

        dlg->bindClick("btn_ok", [this, w = dlg->weak_from_this()]()
        {
            if (auto p = std::static_pointer_cast<BaseDialog>(w.lock()))
                p->close();
        });

        dlg->bindClick("btn_cancel", [this, w = dlg->weak_from_this()]()
        {
            if (auto p = std::static_pointer_cast<BaseDialog>(w.lock()))
                p->close();
        });
        setVisible(false);
        dlg->open([this]{setVisible(true);});
    });

    bind("demo_cb", [this]()
    {
        auto dlg = std::make_shared<DialogDemoExample>();

        if (!dlg->initExample(L10N::getInstance().tr("DLG_CB_TITLE"), 720.f, 450.f, nullptr))
            return;

        auto ptrCont = createBehindParticles(eBehindBgKind::SOLITAIRE_STORM, dlg.get());
        dlg->setBehindBg(ptrCont, false);

        std::string sHtml;

        sHtml += R"HTML(<center>
<font name="edugot" size="30" color="#FFFFFF" shadow="1" shadowcolor="#00000088" appear="rise" dur="0.5">
)HTML";

        sHtml += locHtml("DLG_CB_TEXT");

        sHtml += R"HTML(
</font>
<br><br>
)HTML";

        sHtml += demoButton("btn_close", L10N::getInstance().tr("BTN_CLOSE"));

        sHtml += R"HTML(
</center>)HTML";

        dlg->setHtml(sHtml.c_str());

        dlg->bindClick("btn_close", [this, w = dlg->weak_from_this()]()
        {
            if (auto p = std::static_pointer_cast<BaseDialog>(w.lock()))
                p->close();
        });
        setVisible(false);
        dlg->open([this]()
        {
            setVisible(true);
        });
    });

    bind("demo_panel", [this]()
    {
        auto dlg = std::make_shared<DialogDemoExample>();

        if (!dlg->initExample(nullptr,
                              800.f,
                              550.f,
                              nullptr,
                              eScrollType::E_ST_NONE,
                              false,
                              false,
                              true))
        {
            return;
        }

        std::string sHtml;

        sHtml += R"HTML(<center>
<font name="edugot" size="35" color="#000000FF" shadow="1" shadowcolor="#00000044" appear="rise" dur="0.5">
)HTML";

        sHtml += locHtml("DLG_PANEL_MAIN_TEXT");

        sHtml += R"HTML(
</font>
<br><br>
<font size="45">
)HTML";

        sHtml += demoLink("panel_left",
                          L10N::getInstance().tr("DLG_PANEL_LEFT"),
                          "#2E7D32");

        sHtml += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";

        sHtml += demoLink("panel_right",
                          L10N::getInstance().tr("DLG_PANEL_RIGHT"),
                          "#007A87");

        sHtml += "<br><br>";

        sHtml += demoLink("dlg_close",
                          L10N::getInstance().tr("BTN_CLOSE"),
                          "#D32F2F");

        sHtml += R"HTML(
</font></center>)HTML";

        dlg->setHtml(sHtml.c_str(), 90.f, 120.f);

        auto ptrCont = createBehindParticles(eBehindBgKind::HEARTS);
        dlg->setBehindBg(ptrCont, false);

        dlg->bindClick("dlg_close", [w = dlg->weak_from_this()]()
        {
            if (auto p = std::static_pointer_cast<DialogDemoExample>(w.lock()))
            {
                p->close();
            }
        });

        dlg->bindClick("panel_left", [w = dlg->weak_from_this()]()
        {
            if (auto p = std::static_pointer_cast<DialogDemoExample>(w.lock()))
            {
                p->openSidePanel(SIDE_LEFT);
            }
        });

        dlg->bindClick("panel_right", [w = dlg->weak_from_this()]()
        {
            if (auto p = std::static_pointer_cast<DialogDemoExample>(w.lock()))
                p->openSidePanel(SIDE_RIGHT);
        });
        setVisible(false);
        dlg->open([this, w = dlg->weak_from_this()]()
        {
            setVisible(true);
            if (auto p = std::static_pointer_cast<DialogDemoExample>(w.lock()))
                p->closeAllSidePanels();
        });
    });

    bind("demo_stack", [this]()
    {
        openStackLevel(1);
    });

    bind("demo_click", [this]()
    {
        auto dlg = std::make_shared<DialogDemoExample>();

        if (!dlg->initExample(L10N::getInstance().tr("DLG_CLICK_TITLE"),
                              620.f,
                              340.f,
                              nullptr,
                              eScrollType::E_ST_NONE,
                              false,
                              true))
        {
            return;
        }

        auto ptrCont = createBehindParticles(eBehindBgKind::GRENADES, dlg.get());
        dlg->setBehindBg(ptrCont, false);

        std::string sHtml;

        sHtml += R"HTML(<center>
<font name="edugot" size="30" color="#FFFFFF" shadow="1" shadowcolor="#00000088" appear="rise" dur="0.5">
)HTML";

        sHtml += locHtml("DLG_CLICK_TEXT");

        sHtml += R"HTML(
</font>
</center>)HTML";

        dlg->setHtml(sHtml.c_str(), 110.f);
        setVisible(false);
        dlg->open([this]{setVisible(true);});
    });

    bind("demo_scroll", [this]()
    {
        auto dlg = std::make_shared<DialogDemoExample>();

        if (!dlg->initExample(L10N::getInstance().tr("DLG_SCROLL_TITLE"),
                              760.f,
                              600.f,
                              nullptr,
                              eScrollType::E_ST_NONE,
                              true,
                              false))
        {
            return;
        }

        std::string sHtml;

        sHtml += "<font name=\"edugot\" size=\"28\" color=\"#FFFFFF\" shadow=\"1\" shadowcolor=\"#00000088\">";
        sHtml += L10N::getInstance().tr("DLG_SCROLL_HELP");
        sHtml += "<br><br>";

        for (int i = 1; i <= 24; ++i)
        {
            sHtml += "<b>";
            sHtml += std::vformat(L10N::getInstance().tr("DLG_SCROLL_LINE_FMT"), std::make_format_args(i));
            sHtml += "</b><br>";
        }

        sHtml += "</font>";

        auto ptrCont = createBehindParticles(eBehindBgKind::BLIZZARD, dlg.get());
        dlg->setBehindBg(ptrCont, false);

        const float fPanelCx = dlg->getDialogCx() - 40.f;
        const float fPanelCy = dlg->getDialogCy();

        auto ptrPanel = std::make_shared<PanelHTML>();

        ptrPanel->init(sHtml.c_str(),
                       fPanelCx,
                       fPanelCy,
                       std::static_pointer_cast<CContainer>(dlg),
                       true);

        ptrPanel->setPos(20, -10);
        ptrPanel->showAsPanel(E_IH_DISPATCHBYPARENT);
        ptrPanel->setVisible(true);
        ptrPanel->finalizeScroll();

        dlg->bringChildUnderForeground(ptrPanel);

        setVisible(false);
        dlg->open([this, ptrPanel]()
        {
            setVisible(true);
            if (ptrPanel && ptrPanel->isVisible())
                ptrPanel->close();
        });
    });
}

public:

	void init()
    {
        auto& cfg = Engine::getCfg();
        BaseDialog::init(cfg.INIT_SCR_CX, cfg.INIT_SCR_CY, eScrollType::E_ST_NONE, nullptr, nullptr);

		Rect rc = { 0, 0, getDialogCx(), getDialogCy() };
		_root->setScrollBox(&rc);//clip

        initDialogsDemo();
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