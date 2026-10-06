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



class AppDialog : public BaseDialog
{
private:

	CContainerPtr		_ptrTextDemoCont;

    int                 _currentTextConfigIdx = 0;

private:

void initFontsDemo()
{
    _ptrTextDemoCont = std::make_shared<CContainer>();

    _root->addChild(_ptrTextDemoCont);

    _currentTextConfigIdx = 0;

    float padding = 40.0f;

    float maxCx = static_cast<float>(getDialogCx()) - padding;
    float maxCy = static_cast<float>(getDialogCy()) - padding;

    const float spawnBottomY = maxCy - 10.0f;

    float cellWidth  = 35.0f;
    float cellHeight = 30.0f;

    int numCols = std::max(1, static_cast<int>(maxCx / cellWidth));
    int numRows = std::max(1, static_cast<int>(maxCy / cellHeight));

    std::string matrixChars = "0123456789ABCDEFXYZ*#@$%";

    uint32_t greenColors[] = { 0x00FF00FF, 0x32CD32FF, 0x00FF7FFF, 0x008B00FF };

    std::vector<StaticLabelPtr> matrixGrid;
    matrixGrid.reserve(numCols * numRows);

    for (int row = numRows - 1; row >= 0; --row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            StaticLabelPtr ptrMatrix = std::make_shared<StaticLabel>();

            ptrMatrix->setFont("edugot");
            ptrMatrix->setFontSize(static_cast<float>(16 + rand() % 8));
            ptrMatrix->setRgba(greenColors[rand() % 4]);

            std::string singleChar(1, matrixChars[rand() % matrixChars.size()]);

            ptrMatrix->setText(singleChar.c_str());
            ptrMatrix->setAlign(eTRAlign::TR_ALIGN_CENTER | eTRAlign::TR_ALIGN_BOTTOM);
            ptrMatrix->setPivotCentered();

            float posX = (static_cast<float>(col) * cellWidth) + (cellWidth * 0.5f);
            float posY = (static_cast<float>(row) * cellHeight) + (cellHeight * 0.5f);

            ptrMatrix->setPos(posX, posY);
            ptrMatrix->setAlpha(0.0f);
            ptrMatrix->setScale(0.0f, 0.0f);

            _ptrTextDemoCont->addChild(ptrMatrix);
            matrixGrid.push_back(ptrMatrix);

            int topToBottomRowIdx = (numRows - 1) - row;
            float spawnDelay = static_cast<float>(topToBottomRowIdx) * 0.04f;

            ptrMatrix->addSelfTween(eTweenProp::SCALE, 0.0f, 1.0f, 0.25f, Easing::outQuad, spawnDelay);
            ptrMatrix->addSelfTween(eTweenProp::ALPHA, 0.0f, 1.0f, 0.25f, Easing::linear, spawnDelay);
        }
    }

    float matrixHoldTime = (static_cast<float>(numRows) * 0.04f) + 1.2f;
    float dropDuration   = 1.8f;

    size_t triggerElementIdx = matrixGrid.size() - 1;

    for (size_t i = 0; i < matrixGrid.size(); ++i)
    {
        auto ptrMatrix = matrixGrid[i];

        Point currentPos;
        ptrMatrix->getPos(&currentPos);

        int colIdx = static_cast<int>(currentPos.x / cellWidth);

        float cascadeDelay = matrixHoldTime + (static_cast<float>(colIdx % 6) * 0.12f);
        float targetDropY = currentPos.y + 500.0f;

        if (i == triggerElementIdx)
        {
            ptrMatrix->addSelfTween(eTweenProp::Y,
                                    currentPos.y,
                                    targetDropY,
                                    dropDuration,
                                    Easing::inCubic,
                                    cascadeDelay);

            ptrMatrix->addSelfTween(eTweenProp::ALPHA,
                                    1.0f,
                                    0.0f,
                                    dropDuration,
                                    Easing::linear,
                                    cascadeDelay,
                                    [this, w = ptrMatrix->weak_from_this(), spawnBottomY]()
                                    {
                                        if (auto ptr = std::static_pointer_cast<StaticLabel>(w.lock()))
                                        {
                                            auto parent = ptr->getParent();

                                            if (parent)
                                                parent->removeChild(ptr);

                                            this->spawnScrollText(_currentTextConfigIdx, spawnBottomY);

                                            _currentTextConfigIdx = (_currentTextConfigIdx + 1) % 15;
                                        }
                                    });
        }
        else
        {
            ptrMatrix->addSelfTween(eTweenProp::Y,
                                    currentPos.y,
                                    targetDropY,
                                    dropDuration,
                                    Easing::inCubic,
                                    cascadeDelay);

            ptrMatrix->addSelfTween(eTweenProp::ALPHA,
                                    1.0f,
                                    0.0f,
                                    dropDuration,
                                    Easing::linear,
                                    cascadeDelay,
                                    [w = ptrMatrix->weak_from_this()]()
                                    {
                                        if (auto ptr = std::static_pointer_cast<StaticLabel>(w.lock()))
                                        {
                                            auto parent = ptr->getParent();

                                            if (parent)
                                                parent->removeChild(ptr);
                                        }
                                    });
        }
    }
}

void spawnScrollText(size_t configIdx, float startY)
{
    struct TextDemoConfig
    {
        LPCTSTR    text;
        LPCTSTR    fontName;
        float      fontSize;
        uint32_t   colorHex;
    };

    static std::vector<TextDemoConfig> textConfigs =
    {
        { L10N::getInstance().tr("POEM_01"), "greengoth",     36.0f + 10, 0xFF4500FF },
        { L10N::getInstance().tr("POEM_02"), "creepster",     30.0f + 10, 0x00FFFFFF },
        { L10N::getInstance().tr("POEM_03"), "trigramlight",  34.0f + 10, 0x9A32CDFF },
        { L10N::getInstance().tr("POEM_04"), "edugot",        24.0f + 10, 0x32CD32FF },
        { L10N::getInstance().tr("POEM_05"), "secretorigins", 40.0f + 10, 0xFFFFFFFF },
        { L10N::getInstance().tr("POEM_06"), "edugot",        34.0f + 10, 0xFFD700FF },
        { L10N::getInstance().tr("POEM_07"), "trigramlight",  28.0f + 10, 0xDEB887FF },
        { L10N::getInstance().tr("POEM_08"), "creepster",     38.0f + 10, 0xFF00FFFF },
        { L10N::getInstance().tr("POEM_09"), "greengoth",     30.0f + 10, 0x708090FF },
        { L10N::getInstance().tr("POEM_10"), "secretorigins", 34.0f + 10, 0x00FF7FFF },
        { L10N::getInstance().tr("POEM_11"), "edugot",        28.0f + 10, 0xFF8C00FF },
        { L10N::getInstance().tr("POEM_12"), "greengoth",     42.0f + 10, 0xADFF2FFF },
        { L10N::getInstance().tr("POEM_13"), "greengoth",     32.0f + 10, 0xFF6347FF },
        { L10N::getInstance().tr("POEM_14"), "creepster",     26.0f + 10, 0x7FFFD4FF },
        { L10N::getInstance().tr("POEM_15"), "trigramlight",  36.0f + 10, 0xBA55D3FF }
    };

    if (configIdx >= textConfigs.size() || !_ptrTextDemoCont)
        return;

    const auto& config = textConfigs[configIdx];

    float maxCx = static_cast<float>(getDialogCx()) - 40.0f;
    float localCenterX = maxCx * 0.5f;

    const float scrollSpeed = 70.0f;
    const float targetTopY  = 30.0f;
    const float gapY        = 24.0f;

    float labelHeight = config.fontSize * 1.35f;
    float lineTotalStep = labelHeight + gapY;

    StaticLabelPtr ptrLabel = std::make_shared<StaticLabel>();

    ptrLabel->setFont(config.fontName);
    ptrLabel->setFontSize(config.fontSize);
    ptrLabel->setRgba(config.colorHex);
    ptrLabel->setShadow(true);
    ptrLabel->setText(config.text);
    ptrLabel->setAlign(eTRAlign::TR_ALIGN_CENTER | eTRAlign::TR_ALIGN_BOTTOM);
    ptrLabel->setPivotCentered();
    ptrLabel->setPos(localCenterX, startY);
    ptrLabel->setAlpha(0.0f);

    _ptrTextDemoCont->addChild(ptrLabel);

    float introDuration = 0.55f;

    int animationType = static_cast<int>(configIdx) % 4;

    if (animationType == 0)
    {
        ptrLabel->rotate(-1.0f);
        ptrLabel->setScale(0.2f, 0.2f);

        ptrLabel->addSelfTween(eTweenProp::SCALE, 0.2f, 1.0f, introDuration, Easing::outBack);
        ptrLabel->addSelfTween(eTweenProp::ROTATE, -1.0f, 0.0f, introDuration, Easing::outBack);
        ptrLabel->addSelfTween(eTweenProp::ALPHA, 0.0f, 1.0f, introDuration * 0.5f, Easing::linear);
    }
    else if (animationType == 1)
    {
        ptrLabel->setScale(0.0f, 0.0f);

        ptrLabel->addSelfTween(eTweenProp::SCALE, 0.0f, 1.0f, introDuration + 0.15f, Easing::outQuad);
        ptrLabel->addSelfTween(eTweenProp::ALPHA, 0.0f, 1.0f, introDuration + 0.15f, Easing::outQuad);
    }
    else if (animationType == 2)
    {
        float startLeftX = -250.0f;

        ptrLabel->setPos(startLeftX, startY);

        ptrLabel->addSelfTween(eTweenProp::X, startLeftX, localCenterX, introDuration, Easing::outCubic);
        ptrLabel->addSelfTween(eTweenProp::ALPHA, 0.0f, 1.0f, introDuration * 0.6f, Easing::linear);
    }
    else
    {
        ptrLabel->setScale(2.4f, 2.4f);

        ptrLabel->addSelfTween(eTweenProp::SCALE, 2.4f, 1.0f, introDuration, Easing::outQuart);
        ptrLabel->addSelfTween(eTweenProp::ALPHA, 0.0f, 1.0f, introDuration * 0.4f, Easing::linear);
    }

    float intermediateY = startY - lineTotalStep;
    float durationPhase1 = lineTotalStep / scrollSpeed;

    ptrLabel->addSelfTween(eTweenProp::Y,
                           startY,
                           intermediateY,
                           durationPhase1,
                           Easing::linear,
                           0.f,
                           [this, intermediateY, targetTopY, scrollSpeed, w = ptrLabel->weak_from_this()]()
                           {
                               if (auto ptr = std::static_pointer_cast<StaticLabel>(w.lock()))
                               {
                                   float maxCy = static_cast<float>(getDialogCy()) - 40.0f;

                                   this->spawnScrollText(_currentTextConfigIdx, maxCy - 10.0f);

                                   _currentTextConfigIdx = (_currentTextConfigIdx + 1) % 15;

                                   float remainingDistance = intermediateY - targetTopY;
                                   float durationPhase2 = remainingDistance / scrollSpeed;

                                   if (durationPhase2 > 0.0f)
                                   {
                                       ptr->addSelfTween(eTweenProp::Y,
                                                         intermediateY,
                                                         targetTopY,
                                                         durationPhase2,
                                                         Easing::linear);

                                       ptr->addSelfTween(eTweenProp::ALPHA,
                                                         1.0f,
                                                         0.0f,
                                                         durationPhase2 * 0.35f,
                                                         Easing::linear,
                                                         durationPhase2 * 0.65f,
                                                         [w]()
                                                         {
                                                             if (auto ptr = std::static_pointer_cast<StaticLabel>(w.lock()))
                                                             {
                                                                 auto parent = ptr->getParent();

                                                                 if (parent)
                                                                     parent->removeChild(ptr);
                                                             }
                                                         });
                                   }
                               }
                           });
}

public:

	void init()
    {
        auto& cfg = Engine::getCfg();
        BaseDialog::init(cfg.INIT_SCR_CX, cfg.INIT_SCR_CY, eScrollType::E_ST_NONE, nullptr, nullptr);

		Rect rc = { 0, 0, getDialogCx(), getDialogCy() };
		_root->setScrollBox(&rc);//clip

        initFontsDemo();
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
	_ptrPreloader->filesToLoad().add("ui.png");
	_ptrPreloader->filesToLoad().add("ui.atlas");

	_ptrPreloader->filesToLoad().setSizeToLoad(63996535);
	_ptrPreloader->init(cbOnLoaded);
}