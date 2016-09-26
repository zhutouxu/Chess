#include "AppDelegate.h"
#include "SceneStart.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    //CCDirector* pDirector = CCDirector::sharedDirector();
    //CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
	auto pDirector = Director::getInstance();
	auto* pEGLView = pDirector->getOpenGLView();
	if (!pEGLView) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                            \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		pEGLView = GLViewImpl::createWithRect("shentu", Rect(0, 0, 800, 400));
#else
		glview = GLViewImpl::create("shentu");
#endif
		pDirector->setOpenGLView(pEGLView);
	}


    pDirector->setOpenGLView(pEGLView);

    pEGLView->setDesignResolutionSize(800, 480, kResolutionExactFit);
	
    // turn on display FPS
    pDirector->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = SceneStart::scene();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
