#include "AppDelegate.h"
#include "Version.h"
#include "Scale.h"
#include "FacebookController.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("HDPoker " HDPOKER_VERSION);
		//glview->setFrameSize(2560, 1440);		// mac desktop
		//glview->setFrameSize(2048, 1536);		// iPad 3
		glview->setFrameSize(1136, 640);		// iPhone 6
		director->setOpenGLView(glview);
    }

    director->setDisplayStats(true);
    director->setAnimationInterval(1.0 / 60);

    register_all_packages();

#if FACEBOOK_AVAILABLE
    sdkbox::PluginFacebook::init();
#endif
    
    CCLOG("DPI: %d  Scale factor: %f", Device::getDPI(), Device::getDPI() / 160.);
	auto factor = Device::getDPI() / 160.0f;
	Scale::setFactor(std::max(2.0f, factor));

    auto scale3x = 1.940625;
//	auto scale3x = 2208 / 1136.0f;			// 3X images are this much larger than 2X images
    
    std::vector<std::string> searchPath;
	if (glview->getFrameSize().height < 640 * scale3x) {	// we can't fit 3X images
		Scale::setScale3(1.0f);
        searchPath.push_back("sprites/2x");
    } else {
		Scale::setScale3(scale3x);
        Scale::setFactor(3.8873239437f - 1);
        searchPath.push_back("sprites/3x");
        searchPath.push_back("sprites/2x"); // 2x fallback temporary while we wait for 3x assets
    }
    
    FileUtils::getInstance()->setSearchPaths(searchPath);
    
    _gameController.run();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
