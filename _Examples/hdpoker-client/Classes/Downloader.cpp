#include "Downloader.h"
#include "cocos2d.h"
#include "network/HttpClient.h"
#include <xxhash/xxhash.h>
#include <boost/make_shared.hpp>

using namespace cocos2d;
using namespace cocos2d::network;

CancellationToken Downloader::getSpriteFromUrl(const std::string &url, const DownloadCallback::slot_type& callback) {
    
    auto cachePath = FileUtils::getInstance()->getWritablePath() + "cache/";
    if (!FileUtils::getInstance()->isDirectoryExist(cachePath)) {
        FileUtils::getInstance()->createDirectory(cachePath);
    }
    
    // Cached on disk?
    auto extension = url.substr(url.length() - 4);
    auto hash = std::to_string(XXH32(url.c_str(), static_cast<int>(url.length()), 1337)) + extension;
    auto path = cachePath + hash;
    
    auto signal = std::make_shared<DownloadCallback>();
    auto cancelToken = signal->connect(callback);
    
//    auto cancelToken = boost::make_shared<_CancellationToken>();
    
    if (FileUtils::getInstance()->isFileExist(path)) {
        Director::getInstance()->getTextureCache()->addImageAsync(path, [=](Texture2D *texture) {
            if (texture) {
                (*signal)(Sprite::createWithTexture(texture));
            } else {
                (*signal)(nullptr);
            }
        });
    } else {
        
        // Download...
        
        HttpRequest *request = new HttpRequest();
        request->setUrl(url.c_str());
        request->setRequestType(HttpRequest::Type::GET);
        request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
            if (!response || !response->isSucceed() || response->getResponseCode() != 200) {
                (*signal)(nullptr);
            } else {
                auto buffer = response->getResponseData();
                auto image = new Image();
                if (image) {
                    image->initWithImageData((unsigned char*)&(buffer->front()), buffer->size());
                    
                    CCLOG("%s", url.c_str());
                    image->saveToFile(path, false);
                    delete image;
                    
                    Director::getInstance()->getTextureCache()->addImageAsync(path, [=](Texture2D *texture) {
                        if (texture) {
                            (*signal)(Sprite::createWithTexture(texture));
                        } else {
                            (*signal)(nullptr);
                        }
                    });
                }
            }
        });
        HttpClient::getInstance()->send(request);
        request->release();
    }
    
    return cancelToken;
}