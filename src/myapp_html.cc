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
class AppDialog : public PanelHTML
{
private:
    enum class eSpineGirlState
    {
        IDLE,
        CHARGING,
        IDLE_CHARGED,
    };

    eSpineGirlState    _eAlianna    = eSpineGirlState::IDLE;
    eSpineGirlState    _eAmarantha  = eSpineGirlState::IDLE;

private:
	std::string loadEmbedText(const char* lpszRelPath)
    {
        std::string fullPath = std::string("EMBED/HTML/") + lpszRelPath;
        std::string out;

        FILE* f = std::fopen(fullPath.c_str(), "rb");
        if (!f)
            return out;

        if (std::fseek(f, 0, SEEK_END) == 0)
        {
            long nSize = std::ftell(f);
            if (nSize > 0)
            {
                out.resize((size_t)nSize);
                std::rewind(f);

                size_t nRead = std::fread(&out[0], 1, (size_t)nSize, f);
                if (nRead != (size_t)nSize)
                    out.resize(nRead);
            }
        }

        std::fclose(f);
        return out;
    }

public:

	void init()
    {
        auto& cfg = Engine::getCfg();
        std::string html = loadEmbedText("page2.html");

        PanelHTML::init(html.c_str(), cfg.INIT_SCR_CX, cfg.INIT_SCR_CY, nullptr);

        getHTMLDocument().onSliderChanged("sld_scale", [this](float v)
        {
            auto& d = getHTMLDocument();

            // layout-твин: картинка реально занимает место (reflow)
            auto img = d["img_demo"];
            img.killTweens();
            img.tweenTo(HTMLTweenProps().width(v), 0.25f, HTMLTweenOptions{ .ease = Easing::outQuad });

            d.setTextFmt("sld_scale_v", "{:.0f}", v);
        });

        auto clickHandler = [this](bool bIsAlianna)
        {
            eSpineGirlState& egs = bIsAlianna ? _eAlianna : _eAmarantha;

            auto buttonId = bIsAlianna ? "btn_1"   : "btn_2";
            auto spineId  = bIsAlianna ? "spine_1" : "spine_2";

            switch (egs)
            {
            case eSpineGirlState::IDLE:
            {
                egs = eSpineGirlState::CHARGING;

                getHTMLDocument()[spineId].spinePlay("charge", false);
                getHTMLDocument()[spineId].spineAdd("idle_charged", true);
                getHTMLDocument()[buttonId].setVisible(false);
            }
            break;

            case eSpineGirlState::IDLE_CHARGED:
            {
                getHTMLDocument()[spineId].spinePlay("idle", false);
                getHTMLDocument()[buttonId].setVisible(false);
            }
            break;

            default:
                assert(false);
                break;
            }
        };

        getHTMLDocument()["btn_1"].onClick([clickHandler] { clickHandler(true); });
        getHTMLDocument()["btn_2"].onClick([clickHandler] { clickHandler(false); });
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
	_ptrPreloader->filesToLoad().addSpine("barbarian");
	_ptrPreloader->filesToLoad().addSpine("alianna");
	_ptrPreloader->filesToLoad().addSpine("amarantha");
	_ptrPreloader->filesToLoad().add("ui.png");
	_ptrPreloader->filesToLoad().add("ui.atlas");
	_ptrPreloader->filesToLoad().add("barbarian_idle.panm");
	_ptrPreloader->filesToLoad().add("barbarian_run.panm");
	_ptrPreloader->filesToLoad().setSizeToLoad(63996535);
	_ptrPreloader->init(cbOnLoaded);
	HTMLDom::getInstance()->parseCSSFromFile("EMBED/HTML/styles.css");

}