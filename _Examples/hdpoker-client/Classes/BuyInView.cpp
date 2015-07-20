#include "BuyInView.h"
#include "TableModel.h"
#include "GameModel.h"
#include "Shared.h"
#include "UI/UISlider.h"
#include "UI/UICheckbox.h"
#include "UI/UIButton.h"

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

    auto background = Sprite::create("sprites/hdp-panel-bg-buyin.png");
    background->setPosition(Vec2::ZERO);
    background->setAnchorPoint(Vec2::ZERO);
    setContentSize(background->getContentSize());
    addChild(background);
    
    auto size = getContentSize();
    
    auto titleBar = Sprite::create("sprites/hdp-shared-panel-title.png");
    titleBar->setPosition(Vec2(431, 484));
    addChild(titleBar);
    
    auto buyInInset = Sprite::create("sprites/hdp-inset-buyin-1.png");
    buyInInset->setPosition(Vec2(612, 312));
    addChild(buyInInset);
    
    auto amountInset = Sprite::create("sprites/hdp-inset-buyin-amount.png");
    amountInset->setPosition(Vec2(614, 390));
    addChild(amountInset);
    
    auto buyInLine = Sprite::create("sprites/hdp-panel-buyin-line.png");
    buyInLine->setPosition(Vec2(153, 229));
    addChild(buyInLine);
    
    auto title = Label::createWithTTF(tableModel->getName().c_str(), UniSansRegular, 32);
    title->setPosition(Vec2(431, 484));
    addChild(title);
    
    auto chips = Label::createWithTTF("", UniSansBold, 38);
    chips->setPosition(Vec2(614, 390));
    addChild(chips);
    
    _slider = Slider::create();
    _slider->loadBarTexture("sprites/slider-track-background.png");
    _slider->loadSlidBallTextures("sprites/hdp-btn-buyin-slidergrip.png", "sprites/hdp-btn-buyin-slidergrip.png", "");
    _slider->loadProgressBarTexture("sprites/slider-track-background.png");
    _slider->setPosition(Vec2(612, 286));
    _slider->setPercent(100);
    _slider->addEventListener([=](Ref*,Slider::EventType event) {
        chips->setString(numberWithCommas(getBuyInAmount()).c_str());
    });
    addChild(_slider);
    
    chips->setString(numberWithCommas(getBuyInAmount()));
    
    auto min = Button::create("sprites/hdp-btn-buyin-min.png", "");
    min->setPosition(Vec2(370, 88));
    min->setZoomScale(-.05);
    addChild(min);
    
    auto minLabel = Label::createWithTTF(shortStyleNumber(tableModel->getMinBuyIn()), UniSansBold, 14);
    addChild(minLabel);
    
    auto max = Button::create("sprites/hdp-btn-buyin-max.png", "");
    max->setPosition(Vec2(858, 88));
    max->setZoomScale(-.05);
    addChild(max);
    
    auto maxLabel = Label::createWithTTF(shortStyleNumber(tableModel->getMaxBuyIn()), UniSansBold, 14);
    addChild(maxLabel);
    
    auto autoRebuy = CheckBox::create("sprites/hdp_btn_buyin_autorebuy_off.png", "sprites/hdp_btn_buyin_autorebuy_on.png");
    autoRebuy->setPosition(Vec2(153, 81));
    autoRebuy->setZoomScale(-.05);
    addChild(autoRebuy);
    
    auto buyIn = Button::create("sprites/hdp-btn-buyin-buyin.png", "");
    buyIn->setPosition(Vec2(612, 87));
    buyIn->setZoomScale(-.05);
    buyIn->addClickEventListener([=](Ref*) {
        if (_buyInClickedCallback) {
            _buyInClickedCallback(getBuyInAmount(), autoRebuy->isSelected());
        }
    });
    addChild(buyIn);
    
    auto buyChips = Button::create("sprites/hdp-btn-buyin-buychips.png", "");
    buyChips->setPosition(Vec2(153, 286));
    buyChips->setZoomScale(-.05);
    addChild(buyChips);
    
    auto chipIcon = Sprite::create("sprites/hdp-element-shared-chip.png");
    chipIcon->setPosition(Vec2(75, 361));
    addChild(chipIcon);
    
    auto bank = Label::createWithTTF(shortStyleNumber(gameModel->getChips()), UniSansBold, 38);
    bank->setPosition(Vec2(153, 357));
    bank->setAnchorPoint(Vec2(.5, .5));
    addChild(bank);
    
    auto remember = CheckBox::create("sprites/hdp_btn_buyin_remember_off.png", "sprites/hdp_btn_buyin_remember_on.png");
    remember->setPosition(Vec2(153, 169));
    remember->setZoomScale(-.05);
    addChild(remember);
    
    auto timeLeft = Label::createWithTTF("timer shit", UniSansBold, 29);
    timeLeft->setPosition(Vec2(153, 420));
    timeLeft->setAnchorPoint(Vec2(.5, .5));
    addChild(timeLeft);
    
    auto closeButton = Button::create("sprites/button-decline.png", "");
    closeButton->setPosition(Vec2(890, 484));
    closeButton->setZoomScale(-.05);
    closeButton->addClickEventListener([=](Ref*) {
        removeFromParent();
        if (_buyInDismissedCallback) {
            _buyInDismissedCallback();
        }
    });
    addChild(closeButton);

    Director::getInstance()->getScheduler()->schedule([](float) {
        // Count down
    }, this, 1, time / 1000, 0, false, "Countdown");
}