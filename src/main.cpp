#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto menu = this->getChildByID("editor-buttons-menu");
        
        auto syncBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
            this,
            menu_selector(MyEditorUI::onSyncLocalhost)
        );
        
        syncBtn->setID("termux-sync-btn");
        menu->addChild(syncBtn);
        menu->updateLayout();

        return true;
    }

    void onSyncLocalhost(CCObject* sender) {
        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
        if (btn) btn->setEnabled(false);

        web::AsyncWebRequest()
            .fetch("[http://127.0.0.1:5000/get_level](http://127.0.0.1:5000/get_level)")
            .text()
            .then([this, btn](std::string const& response) {
                if (btn) btn->setEnabled(true);

                // Ekstrak string pakai cara simpel
                size_t start = response.find("\"level_string\":\"") + 16;
                size_t end = response.find("\"", start);
                
                if (start != std::string::npos && end != std::string::npos) {
                    std::string levelString = response.substr(start, end - start);
                    
                    if (!levelString.empty()) {
                        LevelEditorLayer::get()->createObjectsFromString(levelString, true, true);
                        NotificationCenter::get()->sendNotification("AI Sync Berhasil!");
                    } else {
                        NotificationCenter::get()->sendNotification("Belum ada instruksi dari AI");
                    }
                }
            })
            .expect([btn](std::string const& error) {
                if (btn) btn->setEnabled(true);
                FLAlertLayer::create("Error", "Server Termux mati / nggak nyambung!", "OK")->show();
            });
    }
};
