#pragma once

#include "cocos2d.h"
#include "ui/UIEditBox/UIEditBox.h"
#include <functional>

class PokerApi;
class GameController;
class ServerSelectorViewController;

class LoginViewController : public cocos2d::Node, public cocos2d::ui::EditBoxDelegate {
public:
    
    CREATE_FUNC(LoginViewController);
    static LoginViewController* create(GameController *game);
    static void clearAuthenticationInformation();
    
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
    
private:
    
    cocos2d::ui::EditBox *_username;
    cocos2d::ui::EditBox *_password;
    
    void buildView();
    
    void performLogin();
    void connectToGame(const std::string& userID, const std::string& sessionID, const std::function<void(bool)>& callback);
    void showAlert(const char* message);
    
    GameController *_game;
    PokerApi *_api;
    cocos2d::Label *_statusLabel;
    ServerSelectorViewController *_selector;
};