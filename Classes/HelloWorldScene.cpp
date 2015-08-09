#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto renderTexture = RenderTexture::create(visibleSize.width, visibleSize.height);
    renderTexture->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    this->addChild(renderTexture, 2);
    //drawSegment(スタートポイント,エンドポイント,太さ,色);
    
    auto httpRequest = new HttpRequest();
    
    httpRequest->setUrl("http://www.google.co.jp");
    httpRequest->setRequestType(HttpRequest::Type::GET);
    httpRequest->setResponseCallback([this](HttpClient* client, HttpResponse* response){
        std::vector<char>* data = response->getResponseData();
        std::string result(data->begin(), data->end());
        //CCLOG(result.c_str());
    });
    
    network::HttpClient::getInstance()->send(httpRequest);
    
    posList = std::vector<Vec2>();
    
    //イベントリスナーを作成
    auto listener = EventListenerTouchOneByOne::create();

    //タッチ開始
    listener->onTouchBegan = [=](Touch* touch, Event* event){
        prevPoint = touch->getLocation();
        posList.push_back(prevPoint);
        log("TouchBegan");
        return true;
    };
    
    //タッチ中
    listener->onTouchMoved = [=](Touch* touch, Event* event){
        renderTexture->begin();
        auto drawNode = DrawNode::create();
        drawNode->drawSegment(prevPoint, touch->getLocation(), 2.0f, Color4F(.5f, 1.0f, .7f, 1.0f));
        drawNode->visit(); // RenderTextureにレンダリング
        renderTexture->end();
        prevPoint = touch->getLocation();
        posList.push_back(prevPoint);

        log("TouchMoved");
    };
    
    //タッチ終了
    listener->onTouchEnded = [=](Touch* touch, Event* event){
        //CCLOG(HelloWorld::createJson().c_str());
        //posList
        /*
        for (auto pos : posList) {
          CCLOG("%f:%f", pos.x, pos.y);
        }*/
        posList.clear();
        log("TouchEnded");
    };
    
    //イベントリスナーを登録
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

std::string HelloWorld::createJson() {
    // JSON生成
    Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    
    // JSON配列生成
    rapidjson::Value jsonArray(rapidjson::kArrayType);
    // 要素生成
    for (auto pos : posList) {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember<const float>("x",  pos.x, allocator);
        object.AddMember<const float>("y", pos.y, allocator);
        // 配列に追加
        jsonArray.PushBack(object, allocator);
    }
    // JSON配列追加
    //document.AddMember(ARRY_ROOT, jsonArray, allocator);
    
    // StringBufferへ書き出し
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    jsonArray.Accept(writer);
    
    // JSON文字列取得
    const char* result = buffer.GetString();
    
    // string生成
    std::string str(result);
    
    return str;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
