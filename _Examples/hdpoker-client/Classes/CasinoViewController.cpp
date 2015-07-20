#include "CasinoViewController.h"
#include "GameController.h"
#include "DashboardViewController.h"
#include "TableViewController.h"
#include "UI/UIButton.h"
#include "UI/UIPageView.h"
#include "SimpleAudioEngine.h"
#include "CasinoTableContainerView.h"
#include <functional>

using namespace cocos2d;
using namespace cocos2d::ui;

class PinchGestureRecognizer {
public:
    typedef std::function<void(float zoom)> PinchCallback;
    static EventListenerTouchAllAtOnce* create(const PinchCallback& callback) {
        auto listener = EventListenerTouchAllAtOnce::create();
        
//        auto isRecognizing = false;
//        const auto threshold = 2;
        listener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event *event) {
            if (touches.size() == 2) {
//                isRecognizing = true;
            }
        };
//
        listener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event *event) {
            if (touches.size() < 2) {
                return;
            }
            
            auto initialDistance = touches[0]->getStartLocation().getDistance(touches[1]->getStartLocation());
            auto currentDistance = touches[0]->getLocation().getDistance(touches[1]->getLocation());
            
            callback(initialDistance / currentDistance);
        };
//        
//        listener->onTouchesEnded = listener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event *event) {
//            isRecognizing = false;
//        };
        
        return listener;
    }
};

CasinoViewController* CasinoViewController::create(GameController *game) {
    auto controller = CasinoViewController::create();
    controller->_game = game;
    controller->_viewMode = OneUp;
    return controller;
}

void CasinoViewController::setViewMode(CasinoViewController::ViewMode mode) {
    _viewMode = mode;
}

void CasinoViewController::setOnTableRemoved(const TableRemovedCallback &callback) {
    _tableRemovedCallback = callback;
}

void CasinoViewController::setActiveTable(int index) {
    if (OneUp == _viewMode) {
        _tablesLayer->setPositionX(-index * getContentSize().width);
    }
}

void CasinoViewController::addTable(TableViewController* table) {
    if (_tables.size() >= 3) {
        _addTableButton->setVisible(false);
    } else {
        _addTableButton->setVisible(true);
    }
    
    // Find empty index
    int index;
    for (auto i = 0; i < 4; i++) {
        if (_tables.find(i) == _tables.end()) {
            index = i;
            break;
        }
    }
    
    auto tableContainer = CasinoTableContainerView::create(_game);
    tableContainer->addTable(table);
    tableContainer->setCloseCallback([=](CasinoTableContainerView* container) {
        _game->getApi()->tableUnsubscribe(container->getTable()->getModel()->getId().c_str(), NullCallback);
        
        // Stand up if standing...
        
        auto it = _tables.find(index);
        container->removeFromParent();
        _tables.erase(it);
        if (_tableRemovedCallback) {
            _tableRemovedCallback(it->second->getTable());
        }
        
        if (_tables.size() == 0) {
            // Go back to dash
            auto dash = DashboardViewController::create(_game);
            auto scene = Scene::create();
            scene->addChild(dash);
            Director::getInstance()->replaceScene(scene);
        } else {
            if (OneUp == _viewMode) {
                int i = 0;
                for (int i = 0; i < 4; i++) {
                    // Found the hole, move the tables up
//                    if (_tables.find(i) == _tables.end()) {
//                        
//                        // Walk other tables up
//                        for (int k = i + 1; k < 4; k++) {
//                            auto it = _tables.find(k);
//                            if (it != _tables.end()) {
//                                _tables[i] = _tables[k];
//                            } else {
//                                // Delete the last entry
//                                _tables.erase(_tables.find(k));
//                                break;
//                            }
//                            
//                            table.second->setPositionX(i * getContentSize().width);
//                            i++;
//                        }
//                        break;
//                    }
                }
                
                // TODO Go to next table vs start of list
                setActiveTable(0);
                
                if (_tables.size() == 1) {
                    _panGesture->setEnabled(false);
                }
            }
        }
    });
    
    _tables[index] = tableContainer;
    if (_tables.size() > 1) {
        _panGesture->setEnabled(true);
    }
    
    setActiveTable(index);
    
    if (OneUp == _viewMode) {
        auto size = getContentSize();
        tableContainer->setContentSize(size);
        tableContainer->setPosition(Vec2(index * size.width, 0));
    } else {
        auto quad = getContentSize();
        quad.width /= 2;
        quad.height /= 2;
        tableContainer->setPosition(Vec2((index % 2) * quad.width, static_cast<int>(index / 2) * quad.height));
    }
    
    _tablesLayer->addChild(tableContainer);
}

void CasinoViewController::onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
    
}

//void CasinoViewController::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
//}

bool CasinoViewController::init() {
    if (!Layer::init()) {
        return false;
    }
    
    _tablesLayer = Node::create();
    _tablesLayer->setContentSize(getContentSize());
    addChild(_tablesLayer);
    
    // Build the shell around the tables
    auto home = Button::create("sprites/button-home.png", "sprites/button-home.png");
    home->setPosition(Vec2(20, 20));
    home->setAnchorPoint(Vec2(0, 0));
    home->addClickEventListener([=] (Ref*) {
        auto index = 0;
        
        auto dash = DashboardViewController::create(_game);
        dash->transitionFromView(_tables[index]->getTable());
        
        _tables[index]->removeFromParent();
        _tables.erase(index);
        
        auto scene = Scene::create();
        scene->addChild(dash);
        Director::getInstance()->replaceScene(scene);
    });
    addChild(home);
    
    // Build the shell around the tables
    _addTableButton = Button::create("sprites/button-add-table.png", "sprites/button-add-table.png");
    _addTableButton->setPosition(Vec2(20, 20 + home->getContentSize().height + home->getPositionY()));
    _addTableButton->setAnchorPoint(Vec2(0, 0));
    _addTableButton->addClickEventListener([=] (Ref*) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/playnow.wav");
        _game->getApi()->tableSubscribeImmediate([] (const Message* message) {});
    });
    addChild(_addTableButton);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(PinchGestureRecognizer::create([] (float zoom) {
        CCLOG("Zoom %f", zoom);
    }), _tablesLayer);
    
    _panGesture = EventListenerTouchOneByOne::create();
    _panGesture->setEnabled(false);
    _panGesture->setSwallowTouches(false);
    _panGesture->onTouchBegan = [&](const Touch* touch, Event *event) {
        _tablesLayer->stopAllActions();
        return true;
    };
    _panGesture->onTouchMoved = [&](const Touch* touch, Event *event) {
        _tablesLayer->setPositionX(_tablesLayer->getPositionX() + touch->getDelta().x);
        
        const int viewWidth = _tablesLayer->getContentSize().width;
        const int halfView = viewWidth / 2;
        
        // Advance one?
        int index = (_tablesLayer->getPositionX() - halfView) / _tablesLayer->getContentSize().width;
        
        CCLOG("%d %d", (int)_tablesLayer->getPositionX(), index);
    };
    _panGesture->onTouchEnded = [&](const Touch* touch, Event *event) {
        const int viewWidth = _tablesLayer->getContentSize().width;
        const int halfView = viewWidth / 2;
        
        int index = (_tablesLayer->getPositionX() - halfView) / _tablesLayer->getContentSize().width;
        CCLOG("%d", index);
        _tablesLayer->runAction(EaseExponentialOut::create(MoveTo::create(.3, Vec2(index * viewWidth, 0))));
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_panGesture, _tablesLayer);
    
    
    return true;
}