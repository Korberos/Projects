#include "BuyInView.h"
#include "TableModel.h"
#include "GameModel.h"
#include "Shared.h"
#include "UI/UISlider.h"
#include "UI/UICheckbox.h"
#include "UI/UIButton.h"
#include "UI/UIScale9Sprite.h"
#include "Text.h"

using namespace cocos2d;
using namespace cocos2d::ui;

BuyInView* BuyInView::create(int time, const TableModel* tableModel, const GameModel* gameModel) {
    auto view = BuyInView::create();
    view->buildView(time, tableModel, gameModel);
    return view;
}

void BuyInView::setBuyInClickedCallback(const BuyInClickedCallback& callback) {
    _buyInClickedCallback = callback;
}

void BuyInView::setBuyInDismissedCallback(const BuyInDismissedCallback &callback) {
    _buyInDismissedCallback = callback;
}

int64_t BuyInView::getBuyInAmount() const {
    return _minBuyIn + _slider->getPercent() * .01 * (_maxBuyIn - _minBuyIn);
}

void BuyInView::buildView(int time, const TableModel* tableModel, const GameModel* gameModel) {
    _timeLeft = getTimeInMilliseconds();
    _minBuyIn = tableModel->getMinBuyIn();
	_maxBuyIn = tableModel->getMaxBuyIn();

	auto size = getContentSize();
	auto viewCenter = Vec2(size.width / 2.0f, size.height / 2.0f);

    auto background = Scale9Sprite::create("background-9.png");
	background->setContentSize(PT(Size(968, 558)));
	background->setPosition(PT(Vec2(viewCenter.x, viewCenter.y + 5)));
	background->setAnchorPoint(Vec2(0.5f, 0.5f));
    setContentSize(background->getContentSize());
	addChild(background);
    
    auto titleBar = Sprite::create("hdp-shared-panel-title.png");
	titleBar->setPosition(Vec2(viewCenter.x - 33, viewCenter.y + 227));
    addChild(titleBar);
    
    auto buyInInset = Sprite::create("hdp-inset-buyin-1.png");
	buyInInset->setPosition(Vec2(viewCenter.x + 149, viewCenter.y + 54));
    addChild(buyInInset);
    
    auto amountInset = Sprite::create("hdp-inset-buyin-amount.png");
	amountInset->setPosition(Vec2(viewCenter.x + 151, viewCenter.y + 132));
    addChild(amountInset);
    
    auto buyInLine = Sprite::create("hdp-panel-buyin-line.png");
	buyInLine->setPosition(Vec2(viewCenter.x - 310, viewCenter.y - 29));
    addChild(buyInLine);
    
    auto title = Text::create(tableModel->getName().c_str(), UniSansRegular, 32);
	title->setPosition(Vec2(viewCenter.x - 32, viewCenter.y + 226));
    addChild(title);
    
    auto chips = Text::create("", UniSansBold, 38);
	chips->setPosition(Vec2(viewCenter.x + 151, viewCenter.y + 132));
    addChild(chips);
    
    _slider = Slider::create();
    _slider->loadBarTexture("slider-track-background.png");
    _slider->loadSlidBallTextures("hdp-btn-buyin-slidergrip.png", "hdp-btn-buyin-slidergrip.png", "");
    _slider->loadProgressBarTexture("slider-track-background.png");
	_slider->setPosition(Vec2(viewCenter.x + 149, viewCenter.y + 28));
    _slider->setPercent(100);
    _slider->addEventListener([=](Ref*,Slider::EventType event) {
        chips->setString(numberWithCommas(getBuyInAmount()).c_str());
    });
    addChild(_slider);
    
    chips->setString(numberWithCommas(getBuyInAmount()));
    
    auto min = Button::create("hdp-btn-buyin-min.png", "");
	min->setPosition(Vec2(viewCenter.x - 87, viewCenter.y - 170));
    min->setZoomScale(-0.05f);
    min->addClickEventListener([=](Ref*) {
        _slider->setPercent(0);
        chips->setString(numberWithCommas(getBuyInAmount()).c_str());
    });
    addChild(min);
    
    auto minLabel = Text::create(shortStyleNumber(tableModel->getMinBuyIn()), UniSansBold, 14);
    addChild(minLabel);
    
    auto max = Button::create("hdp-btn-buyin-max.png", "");
	max->setPosition(Vec2(viewCenter.x + 395, viewCenter.y - 170));
    max->setZoomScale(-0.05f);
    max->addClickEventListener([=](Ref*) {
        _slider->setPercent(100);
        chips->setString(numberWithCommas(getBuyInAmount()).c_str());
    });
    addChild(max);
    
    auto maxLabel = Text::create(shortStyleNumber(tableModel->getMaxBuyIn()), UniSansBold, 14);
    addChild(maxLabel);
    
    auto autoRebuy = CheckBox::create("hdp_btn_buyin_autorebuy_off.png", "hdp_btn_buyin_autorebuy_on.png");
	autoRebuy->setPosition(Vec2(viewCenter.x - 311, viewCenter.y - 176));
    autoRebuy->setZoomScale(-0.05f);
    addChild(autoRebuy);
    
    auto buyIn = Button::create("hdp-btn-buyin-buyin.png", "");
	buyIn->setPosition(Vec2(viewCenter.x + 149, viewCenter.y - 171));
    buyIn->setZoomScale(-0.05f);
    buyIn->addClickEventListener([=](Ref*) {
        if (_buyInClickedCallback) {
            _buyInClickedCallback(getBuyInAmount(), autoRebuy->isSelected());
        }
    });
    addChild(buyIn);
    
    auto buyChips = Button::create("hdp-btn-buyin-buychips.png", "");
	buyChips->setPosition(Vec2(viewCenter.x - 311, viewCenter.y + 29));
    buyChips->setZoomScale(-0.05f);
    addChild(buyChips);
    
    auto chipIcon = Sprite::create("hdp-element-shared-chip.png");
	chipIcon->setPosition(Vec2(viewCenter.x - 388, viewCenter.y + 103));
    addChild(chipIcon);
    
    auto bank = Text::create(shortStyleNumber(gameModel->getChips()), UniSansBold, 38);
	bank->setPosition(Vec2(viewCenter.x - 310, viewCenter.y + 99));
    bank->setAnchorPoint(Vec2(0.5f, 0.5f));
    addChild(bank);
    
    auto remember = CheckBox::create("hdp_btn_buyin_remember_off.png", "hdp_btn_buyin_remember_on.png");
	remember->setPosition(Vec2(viewCenter.x - 311, viewCenter.y - 88));
    remember->setZoomScale(-0.05f);
    addChild(remember);
    
    auto timeLeft = Text::create("", UniSansBold, 29);
	timeLeft->setPosition(Vec2(viewCenter.x - 310, viewCenter.y + 162));
    timeLeft->setAnchorPoint(Vec2(0.5f, 0.5f));
    addChild(timeLeft);
    
    auto closeButton = Button::create("button-decline.png", "");
	closeButton->setPosition(Vec2(viewCenter.x + 427, viewCenter.y + 226));
	closeButton->setScale(0.93f, 1.06f);
    closeButton->setZoomScale(-0.05f);
    closeButton->addClickEventListener([=](Ref*) {
        if (_buyInDismissedCallback) {
            _buyInDismissedCallback();
        }
    });
    addChild(closeButton);

    Director::getInstance()->getScheduler()->schedule([](float) {
        // Count down
    }, this, 1, time / 1000, 0, false, "Countdown");
}