#include "Downloader.h"
#include "cocos2d.h"
#include "network/HttpClient.h"
#include <xxhash/xxhash.h>
#include <boost/make_shared.hpp>
#include "os.h"
#include "Callback.h"

using namespace cocos2d;
using namespace cocos2d::network;

_CancelationToken::_CancelationToken(const boost::signals2::connection& connection) : _connection(connection) {}

void _CancelationToken::cancel() {
    _connection.disconnect();
}

bool _CancelationToken::isCancelled() const {
    return !_connection.connected();
}

CancelationToken Downloader::getSpriteFromUrl(const std::string &url, const DownloadCallback& callback) {
    
    auto cachePath = FileUtils::getInstance()->getWritablePath() + "cache/";
    if (!FileUtils::getInstance()->isDirectoryExist(cachePath)) {
        FileUtils::getInstance()->createDirectory(cachePath);
    }
    
    // Cached on disk?
    auto extension = url.substr(url.length() - 4);
    auto hash = std::to_string(XXH32(url.c_str(), static_cast<int>(url.length()), 1337)) + extension;
    auto path = cachePath + hash;
    
    auto signal = std::make_shared<DownloadSignal>();
    auto connection = signal->connect(callback);
    auto cancelToken = boost::make_shared<_CancelationToken>(connection);
    
    if (FileUtils::getInstance()->isFileExist(path)) {
        
        // From disk...
        Director::getInstance()->getTextureCache()->addImageAsync(path, [=](Texture2D *texture) {
            if (cancelToken->isCancelled()) {
                CCLOG("Cancelled: Disk");
                return;
            }
            
            if (texture) {
				(*signal)(Sprite::createWithTexture(texture), path, true);
            } else {
				(*signal)(nullptr, path, true);
            }
        });
    } else {
        
        // Download...
        HttpRequest *request = new HttpRequest();
        request->setUrl(url.c_str());
        request->setRequestType(HttpRequest::Type::GET);
        request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {

            // With cancelation tokens, we want to ensure successful downloads are cached to disk...
            
            if (!response || !response->isSucceed() || response->getResponseCode() != 200) {
                if (cancelToken->isCancelled()) {
                    return;
                }
                (*signal)(nullptr, path, false);
            } else {
                auto buffer = response->getResponseData();
                auto image = new Image();
                if (image) {
                    bool isValidImage = image->initWithImageData((unsigned char*)&(buffer->front()), buffer->size());
                    if (isValidImage) {
                        image->saveToFile(path, false);
                    
                        if (cancelToken->isCancelled()) {
                            return;
                        }
                        Director::getInstance()->getTextureCache()->addImageAsync(path, [=](Texture2D *texture) {
                            if (cancelToken->isCancelled()) {
                                return;
                            }
                            
                            if (texture) {
                                (*signal)(Sprite::createWithTexture(texture), path, false);
                            } else {
                                (*signal)(nullptr, path, false);
                            }
                        });
                    } else {
                        if (cancelToken->isCancelled()) {
                            return;
                        }
                        
                        (*signal)(nullptr, path, false);
                    }
                    delete image;
                }
            }
        });
        HttpClient::getInstance()->send(request);
        request->release();
    }
    
    return cancelToken;
}