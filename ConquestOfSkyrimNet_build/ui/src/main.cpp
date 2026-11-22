// Conquest of SkyrimNet UI bridge (CoS edition)
//
// One PrismaUI view (Data/PrismaUI/views/ConquestOfSkyrimNet/index.html) that draws:
//   * ANY Message form from the plugin (title, text, conditional buttons, <Global=..>/<Alias=..> tags,
//     %-format arguments)  -> replaces every vanilla message box menu in the mod
//   * a text input dialog and a list dialog                -> replaces SkyUILib (UILIB_1)
//   * the Conquest of SkyrimNet screens (claim, officers, war table, steward ledger)
//
// Papyrus side: CSN_Native (natives below) + CSNP (blocking helpers that mimic Message.Show()).
// Dialog results are stored here and polled by Papyrus; faction-screen commands are sent back as
// the mod event "CSN_UICommand" (strArg = "cmd|arg1|arg2...").

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include "PrismaUI_API.h"

#include <spdlog/sinks/basic_file_sink.h>

#include <atomic>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cctype>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace
{
    PRISMA_UI_API::IVPrismaUI1* g_prisma = nullptr;
    PrismaView g_view = 0;
    bool g_domReady = false;
    std::vector<std::string> g_pending;  // scripts requested before the DOM was ready

    // ------------------------------------------------------------------ dialog bookkeeping
    struct DialogResult
    {
        int index = -2;  // -2 = still open, -1 = cancelled
        std::string text;
    };
    std::mutex g_lock;
    std::unordered_map<int, DialogResult> g_results;
    std::unordered_map<int, bool> g_fireAndForget;  // notices nobody waits for
    int g_nextToken = 1;
    std::atomic<int> g_openDialogs{ 0 };
    std::atomic<int> g_generation{ 0 };
    std::atomic<bool> g_factionScreenOpen{ false };
    std::atomic<bool> g_typing{ false };  // a text box in the view has focus: the ledger key types instead of closing
    bool g_typingPaused = false;          // refocused with the game paused while typing (pause option off)
    bool g_pause = false;        // MCM "Pause the game while menus are open" (bPauseGame:Interface)
    std::string g_logPath;       // full path of ConquestOfSkyrimNet.log once created

    std::unordered_map<std::string, RE::TESGlobal*> g_globals;  // lower-case editor ID -> global

    // ------------------------------------------------------------------ helpers
    std::string Lower(std::string s)
    {
        for (auto& c : s) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return s;
    }

    // Papyrus interns strings case-insensitively, so text it builds can come back with
    // different capitalisation ("true" -> "True", "name" -> "Name"). Normalise JSON built in
    // Papyrus before it reaches JavaScript: object keys to lower case, True/False/None to JSON.
    std::string NormalizeJson(const std::string& in)
    {
        std::string out;
        out.reserve(in.size());
        size_t i = 0;
        const size_t n = in.size();
        while (i < n) {
            char c = in[i];
            if (c == '"') {
                size_t j = i + 1;
                std::string str;
                while (j < n && in[j] != '"') {
                    if (in[j] == '\\' && j + 1 < n) {
                        str += in[j];
                        str += in[j + 1];
                        j += 2;
                        continue;
                    }
                    str += in[j++];
                }
                size_t k = j + 1;
                while (k < n && std::isspace(static_cast<unsigned char>(in[k]))) {
                    ++k;
                }
                bool isKey = k < n && in[k] == ':';
                out += '"';
                out += isKey ? Lower(str) : str;
                out += '"';
                i = j + 1;
                continue;
            }
            if (std::isalpha(static_cast<unsigned char>(c))) {
                size_t j = i;
                while (j < n && std::isalpha(static_cast<unsigned char>(in[j]))) {
                    ++j;
                }
                std::string word = Lower(in.substr(i, j - i));
                if (word == "none") {
                    word = "null";
                }
                out += word;
                i = j;
                continue;
            }
            out += c;
            ++i;
        }
        return out;
    }

    // ------------------------------------------------------------------ settings (MCM Helper ini files)
    // Defaults: Data/MCM/Config/ConquestOfSkyrimNet/settings.ini, user values: Data/MCM/Settings/ConquestOfSkyrimNet.ini
    std::unordered_map<std::string, std::string> g_settings;  // "section:key" lower case -> value

    void ReadIni(const char* a_path)
    {
        std::ifstream f(a_path);
        if (!f) {
            return;
        }
        std::string line, section;
        while (std::getline(f, line)) {
            auto trim = [](std::string v) {
                size_t a = v.find_first_not_of(" \t\r\n");
                size_t b = v.find_last_not_of(" \t\r\n");
                return a == std::string::npos ? std::string() : v.substr(a, b - a + 1);
            };
            if (line.size() >= 3 && static_cast<unsigned char>(line[0]) == 0xEF && static_cast<unsigned char>(line[1]) == 0xBB &&
                static_cast<unsigned char>(line[2]) == 0xBF) {
                line = line.substr(3);  // MCM Helper writes its ini files with a UTF-8 byte order mark
            }
            line = trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }
            if (line.front() == '[' && line.back() == ']') {
                section = Lower(line.substr(1, line.size() - 2));
                continue;
            }
            auto eq = line.find('=');
            if (eq == std::string::npos) {
                continue;
            }
            g_settings[Lower(trim(line.substr(0, eq))) + ":" + section] = trim(line.substr(eq + 1));
        }
    }

    int SettingInt(const std::string& a_id, int a_default)
    {
        auto it = g_settings.find(Lower(a_id));
        if (it == g_settings.end()) {
            return a_default;
        }
        try {
            return std::stoi(it->second);
        } catch (...) {
            return a_default;
        }
    }

    std::string JsonString(const std::string& s)
    {
        std::string out = "\"";
        for (char c : s) {
            switch (c) {
            case '\\': out += "\\\\"; break;
            case '"':  out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\t': out += "\\t"; break;
            case '\r': break;
            case '<':  out += "\\u003c"; break;  // never let text close a script tag
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    break;
                }
                out += c;
            }
        }
        out += "\"";
        return out;
    }

    std::string FormatNumber(double v)
    {
        char buf[64];
        if (v == static_cast<double>(static_cast<long long>(v))) {
            std::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(v));
        } else {
            std::snprintf(buf, sizeof(buf), "%.2f", v);
        }
        return buf;
    }

    // printf-style substitution of Message.Show(afArg1..afArg9)
    std::string ApplyArgs(const std::string& text, const std::vector<float>& args)
    {
        std::string out;
        size_t argi = 0;
        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] != '%') {
                out += text[i];
                continue;
            }
            if (i + 1 < text.size() && text[i + 1] == '%') {
                out += '%';
                ++i;
                continue;
            }
            size_t j = i + 1;
            while (j < text.size() && std::strchr("-+ #0123456789.", text[j])) {
                ++j;
            }
            if (j < text.size() && std::strchr("fFgGeEdi", text[j])) {
                std::string spec = text.substr(i, j - i + 1);
                double v = argi < args.size() ? args[argi] : 0.0;
                ++argi;
                char buf[64];
                if (text[j] == 'd' || text[j] == 'i') {
                    spec.back() = 'd';
                    std::snprintf(buf, sizeof(buf), spec.c_str(), static_cast<int>(v));
                } else {
                    std::snprintf(buf, sizeof(buf), spec.c_str(), v);
                }
                out += buf;
                i = j;
            } else {
                out += '%';
            }
        }
        return out;
    }

    // ---- <Global=X>, <Global.Mod=X>, <Alias=X>, <Alias.Mod=X> tags, the same set the engine fills in.
    // Alias values come from Papyrus (CSNP.AliasText) keyed "mod|name" in lower case.
    double GlobalValue(const std::string& a_id, double a_default)
    {
        auto it = g_globals.find(Lower(a_id));
        return it != g_globals.end() && it->second ? static_cast<double>(it->second->value) : a_default;
    }

    std::string Ordinal(int n)
    {
        const char* suf = "th";
        if (n % 100 < 11 || n % 100 > 13) {
            if (n % 10 == 1) suf = "st";
            else if (n % 10 == 2) suf = "nd";
            else if (n % 10 == 3) suf = "rd";
        }
        return std::to_string(n) + suf;
    }

    std::string GlobalTag(const std::string& a_mod, const std::string& a_name)
    {
        static const char* kDays[] = { "Sundas", "Morndas", "Tirdas", "Middas", "Turdas", "Fredas", "Loredas" };
        static const char* kMonths[] = { "Morning Star", "Sun's Dawn", "First Seed", "Rain's Hand", "Second Seed", "Midyear",
            "Sun's Height", "Last Seed", "Hearthfire", "Frostfall", "Sun's Dusk", "Evening Star" };
        std::string mod = Lower(a_mod);
        double v = GlobalValue(a_name, 0.0);
        if (mod.empty()) {
            return FormatNumber(v);
        }
        // Date parts describe today's date (CoS only uses them with GameDaysPassed).
        if (mod == "weekday" || mod == "dayofweek") {
            long long d = static_cast<long long>(GlobalValue("GameDaysPassed", v));
            return kDays[((d % 7) + 7) % 7];
        }
        if (mod == "day") {
            return Ordinal(static_cast<int>(GlobalValue("GameDay", 17.0)));
        }
        if (mod == "monthword" || mod == "monthname") {
            int m = static_cast<int>(GlobalValue("GameMonth", 7.0));
            return kMonths[((m % 12) + 12) % 12];
        }
        if (mod == "month") {
            return std::to_string(static_cast<int>(GlobalValue("GameMonth", 7.0)) + 1);
        }
        if (mod == "year") {
            return std::to_string(static_cast<int>(GlobalValue("GameYear", 201.0)));
        }
        return FormatNumber(v);
    }

    const std::regex& TagRegex()
    {
        static const std::regex tag(R"(<(Global|Alias)(?:\.([A-Za-z]+))?=([^>]+)>)", std::regex::icase);
        return tag;
    }

    std::string ResolveTags(const std::string& text, const std::vector<std::string>& aliasNames,
        const std::vector<std::string>& aliasValues)
    {
        std::string out;
        size_t last = 0;
        for (auto it = std::sregex_iterator(text.begin(), text.end(), TagRegex()); it != std::sregex_iterator(); ++it) {
            const auto& m = *it;
            out += text.substr(last, m.position() - last);
            std::string kind = Lower(m[1].str());
            std::string mod = m[2].matched ? m[2].str() : "";
            std::string name = m[3].str();
            if (kind == "global") {
                out += GlobalTag(mod, name);
            } else {
                std::string key = Lower(mod) + "|" + Lower(name);
                std::string value;
                for (size_t k = 0; k < aliasNames.size(); ++k) {
                    if (Lower(aliasNames[k]) == key) {
                        value = k < aliasValues.size() ? aliasValues[k] : "";
                        break;
                    }
                }
                out += value;
            }
            last = m.position() + m.length();
        }
        out += text.substr(last);
        return out;
    }

    std::string MessageText(RE::BGSMessage* a_msg)
    {
        RE::BSString desc;
        a_msg->GetDescription(desc, a_msg);
        return desc.c_str() ? desc.c_str() : "";
    }

    // ------------------------------------------------------------------ view control (UI thread)
    void RunScript(const std::string& script)
    {
        if (!g_prisma || !g_view) {
            return;
        }
        if (!g_domReady) {
            g_pending.push_back(script);
            return;
        }
        g_prisma->Invoke(g_view, script.c_str());
    }

    void ShowAndFocus()
    {
        g_prisma->Show(g_view);
        if (!g_prisma->HasFocus(g_view)) {
            g_prisma->Focus(g_view, g_pause || g_typingPaused);
        }
    }

    void HideView()
    {
        if (!g_prisma || !g_view) {
            return;
        }
        RunScript("window.csnHideAll && window.csnHideAll();");
        if (g_prisma->HasFocus(g_view)) {
            g_prisma->Unfocus(g_view);
        }
        g_prisma->Hide(g_view);
        g_typing = false;
        g_typingPaused = false;
    }

    void OpenOnUI(const std::string& script)
    {
        ++g_generation;
        SKSE::GetTaskInterface()->AddUITask([script]() {
            ShowAndFocus();
            RunScript(script);
        });
    }

    // After a dialog answer, hide the view unless another dialog/screen opens almost immediately
    // (CoS menus usually re-open themselves, so this avoids flicker).
    void ScheduleHide()
    {
        int gen = ++g_generation;
        std::thread([gen]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            SKSE::GetTaskInterface()->AddUITask([gen]() {
                if (g_generation == gen && g_openDialogs == 0 && !g_factionScreenOpen) {
                    HideView();
                }
            });
        }).detach();
    }

    int NewToken()
    {
        std::lock_guard lk(g_lock);
        int t = g_nextToken++;
        g_results[t] = DialogResult{};
        ++g_openDialogs;
        return t;
    }

    void SendCommand(const std::string& cmd, RE::TESForm* a_sender = nullptr)
    {
        SKSE::GetTaskInterface()->AddTask([cmd, a_sender]() {
            SKSE::ModCallbackEvent evt{ "CSN_UICommand", RE::BSFixedString(cmd.c_str()), 0.0f, a_sender };
            SKSE::GetModCallbackEventSource()->SendEvent(&evt);
        });
    }

    // With "keep the game paused" off, keys typed into a text box also reach the game and other plugins' hotkeys
    // (SKSE Menu Framework and the like). While a text box has focus, hold the game paused like the pause option
    // does, which those hotkeys respect; let it run again once typing ends. Call on the UI thread.
    void ApplyTypingFocus()
    {
        if (!g_prisma || !g_view || g_pause) {
            return;
        }
        bool want = g_typing.load() && !g_prisma->IsHidden(g_view);
        if (want == g_typingPaused) {
            return;
        }
        g_typingPaused = want;
        if (g_prisma->HasFocus(g_view)) {
            g_prisma->Unfocus(g_view);
        }
        if (!g_prisma->IsHidden(g_view)) {
            g_prisma->Focus(g_view, want);
            // refocusing can take the caret out of the text box: put it back
            RunScript("window.csnRestoreTyping && window.csnRestoreTyping();");
        }
    }

    void ForceCloseScreen()
    {
        g_factionScreenOpen = false;
        g_typing = false;
        SKSE::GetTaskInterface()->AddUITask([]() {
            RunScript("window.csnCloseScreen && window.csnCloseScreen(true);");
            HideView();
        });
        SendCommand("close");
    }

    // ------------------------------------------------------------------ hotkeys
    // The faction menu key (DirectX scan code) from the MCM ini, iLedgerKey:Hotkeys. Unbound (-1) by default.
    // Both unbound by default; set them in the MCM (Hotkeys page).
    std::int32_t g_ledgerKey = -1;
    std::string g_theme = "dark";

    void LoadKeys()
    {
        g_settings.clear();
        ReadIni("Data/MCM/Config/ConquestOfSkyrimNet/settings.ini");
        ReadIni("Data/MCM/Settings/ConquestOfSkyrimNet.ini");
        g_ledgerKey = SettingInt("iLedgerKey:Hotkeys", -1);
        g_theme = SettingInt("iTheme:Interface", 0) == 1 ? "light" : "dark";
        g_pause = SettingInt("bPauseGame:Interface", 0) != 0;
        SKSE::log::info("Hotkey: ledger={} theme={}", g_ledgerKey, g_theme);
    }

    void ForceCloseScreen();

    class InputSink : public RE::BSTEventSink<RE::InputEvent*>
    {
    public:
        static InputSink* Get()
        {
            static InputSink sink;
            return &sink;
        }

        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*) override
        {
            if (!a_event) {
                return RE::BSEventNotifyControl::kContinue;
            }
            for (auto* e = *a_event; e; e = e->next) {
                auto* btn = e->AsButtonEvent();
                if (!btn || !btn->IsDown() || btn->GetDevice() != RE::INPUT_DEVICE::kKeyboard) {
                    continue;
                }
                auto key = static_cast<std::int32_t>(btn->GetIDCode());
                auto* ui = RE::UI::GetSingleton();
                // Way out of a stuck screen: Esc, or the ledger key again, always closes it.
                if (g_factionScreenOpen && g_openDialogs == 0 && (key == 0x01 || (!g_typing && g_ledgerKey > 0 && key == g_ledgerKey))) {
                    ForceCloseScreen();
                    continue;
                }
                if (!ui || ui->GameIsPaused() || ui->IsMenuOpen(RE::Console::MENU_NAME) || g_openDialogs > 0 || g_factionScreenOpen) {
                    continue;
                }
                // Opening is done by Papyrus (CSN_Core registers the same key with RegisterForKey).
            }
            return RE::BSEventNotifyControl::kContinue;
        }
    };

    // ------------------------------------------------------------------ game menus over our view
    // Conquest of Skyrim opens inventories (outfits, troop equipment) from inside its menus. While one of those
    // game menus is open our view steps aside, and comes back afterwards if a screen or dialog is still open.
    std::atomic<bool> g_suspended{ false };

    bool IsGameMenu(const RE::BSFixedString& a_name)
    {
        return a_name == RE::ContainerMenu::MENU_NAME || a_name == RE::InventoryMenu::MENU_NAME ||
               a_name == RE::BarterMenu::MENU_NAME || a_name == RE::GiftMenu::MENU_NAME ||
               a_name == RE::RaceSexMenu::MENU_NAME || a_name == RE::MagicMenu::MENU_NAME;
    }

    class MenuSink : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
    public:
        static MenuSink* Get()
        {
            static MenuSink sink;
            return &sink;
        }

        RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
        {
            if (!a_event || !g_prisma || !g_view || !IsGameMenu(a_event->menuName)) {
                return RE::BSEventNotifyControl::kContinue;
            }
            if (a_event->opening) {
                if (g_factionScreenOpen || g_openDialogs > 0) {
                    g_suspended = true;
                    SKSE::GetTaskInterface()->AddUITask([]() {
                        if (g_prisma->HasFocus(g_view)) {
                            g_prisma->Unfocus(g_view);
                        }
                        g_prisma->Hide(g_view);
                    });
                }
            } else if (g_suspended) {
                g_suspended = false;
                SKSE::GetTaskInterface()->AddUITask([]() {
                    if (g_factionScreenOpen || g_openDialogs > 0) {
                        ShowAndFocus();
                    }
                });
            }
            return RE::BSEventNotifyControl::kContinue;
        }
    };

    // ------------------------------------------------------------------ Papyrus natives
    bool IsAvailable(RE::StaticFunctionTag*)
    {
        return g_prisma != nullptr && g_view != 0;
    }

    bool IsMenuMessage(RE::StaticFunctionTag*, RE::BGSMessage* a_msg)
    {
        return a_msg && a_msg->flags.any(RE::BGSMessage::MessageFlag::kMessageBox);
    }

    RE::TESQuest* GetOwnerQuest(RE::StaticFunctionTag*, RE::BGSMessage* a_msg)
    {
        return a_msg ? a_msg->ownerQuest : nullptr;
    }

    std::vector<std::string> GetAliasTags(RE::StaticFunctionTag*, RE::BGSMessage* a_msg)
    {
        std::vector<std::string> out;
        if (!a_msg) {
            return out;
        }
        std::string all = (a_msg->GetFullName() ? a_msg->GetFullName() : std::string()) + "\n" + MessageText(a_msg);
        for (auto* btn : a_msg->menuButtons) {
            if (btn && btn->text.c_str()) {
                all += "\n";
                all += btn->text.c_str();
            }
        }
        for (auto it = std::sregex_iterator(all.begin(), all.end(), TagRegex()); it != std::sregex_iterator(); ++it) {
            if (Lower((*it)[1].str()) != "alias") {
                continue;
            }
            std::string key = Lower((*it)[2].matched ? (*it)[2].str() : "") + "|" + Lower((*it)[3].str());
            bool seen = false;
            for (auto& o : out) {
                seen = seen || o == key;
            }
            if (!seen) {
                out.push_back(key);
            }
        }
        return out;
    }

    int ShowMessage(RE::StaticFunctionTag*, RE::BGSMessage* a_msg, std::vector<std::string> a_aliasNames,
        std::vector<std::string> a_aliasValues, std::vector<float> a_args)
    {
        if (!IsAvailable(nullptr) || !a_msg) {
            return -1;
        }
        auto* player = RE::PlayerCharacter::GetSingleton();
        std::string title = ResolveTags(a_msg->GetFullName() ? a_msg->GetFullName() : "", a_aliasNames, a_aliasValues);
        std::string body = ResolveTags(ApplyArgs(MessageText(a_msg), a_args), a_aliasNames, a_aliasValues);

        std::string buttons = "[";
        int index = 0;
        bool any = false;
        for (auto* btn : a_msg->menuButtons) {
            if (!btn) {
                continue;
            }
            bool visible = btn->conditions.IsTrue(player, player);
            std::string label = ResolveTags(btn->text.c_str() ? btn->text.c_str() : "", a_aliasNames, a_aliasValues);
            if (any) {
                buttons += ",";
            }
            buttons += "{\"i\":" + std::to_string(index) + ",\"text\":" + JsonString(label) +
                       ",\"visible\":" + (visible ? "true" : "false") + "}";
            any = true;
            ++index;
        }
        if (!any) {
            buttons += "{\"i\":0,\"text\":\"Ok\",\"visible\":true}";
        }
        buttons += "]";

        int token = NewToken();
        std::string json = "{\"token\":" + std::to_string(token) + ",\"title\":" + JsonString(title) +
                           ",\"text\":" + JsonString(body) + ",\"buttons\":" + buttons + "}";
        OpenOnUI("window.csnMessage(" + json + ");");
        return token;
    }

    int OpenTextInput(RE::StaticFunctionTag*, std::string a_title, std::string a_initial)
    {
        if (!IsAvailable(nullptr)) {
            return -1;
        }
        int token = NewToken();
        OpenOnUI("window.csnTextInput({\"token\":" + std::to_string(token) + ",\"title\":" + JsonString(a_title) +
                 ",\"initial\":" + JsonString(a_initial) + "});");
        return token;
    }

    int OpenList(RE::StaticFunctionTag*, std::string a_title, std::vector<std::string> a_options, int a_start, int a_default)
    {
        if (!IsAvailable(nullptr)) {
            return -1;
        }
        std::string opts = "[";
        for (size_t i = 0; i < a_options.size(); ++i) {
            opts += (i ? "," : "") + JsonString(a_options[i]);
        }
        opts += "]";
        int token = NewToken();
        OpenOnUI("window.csnList({\"token\":" + std::to_string(token) + ",\"title\":" + JsonString(a_title) +
                 ",\"options\":" + opts + ",\"start\":" + std::to_string(a_start) +
                 ",\"default\":" + std::to_string(a_default) + "});");
        return token;
    }

    // Debug.MessageBox replacement: shows text with an Ok button, does not block.
    void OpenNotice(RE::StaticFunctionTag*, std::string a_text)
    {
        if (!IsAvailable(nullptr)) {
            RE::DebugMessageBox(a_text.c_str());
            return;
        }
        int token = NewToken();
        {
            std::lock_guard lk(g_lock);
            g_fireAndForget[token] = true;
        }
        OpenOnUI("window.csnMessage({\"token\":" + std::to_string(token) + ",\"title\":\"\",\"text\":" + JsonString(a_text) +
                 ",\"buttons\":[{\"i\":0,\"text\":\"Ok\",\"visible\":true}]});");
    }

    // Yes/No question (confirmations for officer actions). 0 = yes, 1 = no, -1 = closed.
    int OpenConfirm(RE::StaticFunctionTag*, std::string a_title, std::string a_text, std::string a_yes, std::string a_no)
    {
        if (!IsAvailable(nullptr)) {
            return -1;
        }
        int token = NewToken();
        OpenOnUI("window.csnMessage({\"token\":" + std::to_string(token) + ",\"title\":" + JsonString(a_title) +
                 ",\"text\":" + JsonString(a_text) + ",\"confirm\":true,\"buttons\":[{\"i\":0,\"text\":" +
                 JsonString(a_yes.empty() ? "Yes" : a_yes) + ",\"visible\":true},{\"i\":1,\"text\":" +
                 JsonString(a_no.empty() ? "No" : a_no) + ",\"visible\":true}]});");
        return token;
    }

    // A question with its own buttons (request decisions: Accept / Deny / Meet in the middle). Returns the button index.
    int OpenChoice(RE::StaticFunctionTag*, std::string a_title, std::string a_text, std::vector<std::string> a_buttons)
    {
        if (!IsAvailable(nullptr) || a_buttons.empty()) {
            return -1;
        }
        std::string buttons = "[";
        for (size_t i = 0; i < a_buttons.size(); ++i) {
            if (i) {
                buttons += ",";
            }
            buttons += "{\"i\":" + std::to_string(i) + ",\"text\":" + JsonString(a_buttons[i]) + ",\"visible\":" +
                       (a_buttons[i].empty() ? "false" : "true") + "}";
        }
        buttons += "]";
        int token = NewToken();
        OpenOnUI("window.csnMessage({\"token\":" + std::to_string(token) + ",\"title\":" + JsonString(a_title) +
                 ",\"text\":" + JsonString(a_text) + ",\"buttons\":" + buttons + "});");
        return token;
    }

    // Number dialog with -100/-10/-1/+1/+10/+100 steps and typing (trade confirmations). Index 0 = accepted,
    // the amount comes back through PollText.
    int OpenAmount(RE::StaticFunctionTag*, std::string a_title, std::string a_text, int a_initial, int a_min, int a_max,
        int a_unitPrice, std::string a_priceLabel, std::string a_yes, std::string a_no)
    {
        if (!IsAvailable(nullptr)) {
            return -1;
        }
        int token = NewToken();
        OpenOnUI("window.csnAmount({\"token\":" + std::to_string(token) + ",\"title\":" + JsonString(a_title) +
                 ",\"text\":" + JsonString(a_text) + ",\"initial\":" + std::to_string(a_initial) + ",\"min\":" +
                 std::to_string(a_min) + ",\"max\":" + std::to_string(a_max) + ",\"price\":" + std::to_string(a_unitPrice) +
                 ",\"priceLabel\":" + JsonString(a_priceLabel) + ",\"yes\":" + JsonString(a_yes.empty() ? "Agree" : a_yes) +
                 ",\"no\":" + JsonString(a_no.empty() ? "Cancel" : a_no) + "});");
        return token;
    }

    // Conquest of SkyrimNet's own debug log: Documents/My Games/Skyrim Special Edition/SKSE/ConquestOfSkyrimNet.log
    std::shared_ptr<spdlog::logger> g_csnLog;

    void LogLine(RE::StaticFunctionTag*, std::string a_text)
    {
        if (g_csnLog) {
            g_csnLog->info("{}", a_text);
        }
    }

    // A result line on the faction screen (green, or red when a_error), shown without reopening anything.
    void UIMessage(RE::StaticFunctionTag*, std::string a_text, bool a_error)
    {
        if (!IsAvailable(nullptr)) {
            return;
        }
        std::string js = "window.csnToast && window.csnToast(" + JsonString(a_text) + "," + (a_error ? "true" : "false") + ");";
        SKSE::GetTaskInterface()->AddUITask([js]() { RunScript(js); });
    }

    std::string GetLogPath(RE::StaticFunctionTag*)
    {
        return g_logPath;
    }

    // -2 while the dialog is open, otherwise the chosen index (-1 = cancelled).
    int PollResult(RE::StaticFunctionTag*, int a_token)
    {
        std::lock_guard lk(g_lock);
        auto it = g_results.find(a_token);
        if (it == g_results.end()) {
            return -1;
        }
        int r = it->second.index;
        if (r != -2 && it->second.text.empty()) {
            g_results.erase(it);
        }
        return r;
    }

    // Text of a finished text-input dialog (call after PollResult returned >= -1). Frees the token.
    std::string PollText(RE::StaticFunctionTag*, int a_token)
    {
        std::lock_guard lk(g_lock);
        auto it = g_results.find(a_token);
        if (it == g_results.end()) {
            return "";
        }
        std::string t = it->second.text;
        g_results.erase(it);
        return t;
    }

    bool OpenView(RE::StaticFunctionTag*, std::string a_screen, std::string a_payload)
    {
        if (!IsAvailable(nullptr)) {
            return false;
        }
        g_factionScreenOpen = true;
        auto payload = NormalizeJson(a_payload);
        SKSE::log::info("OpenView {} ({} bytes)", a_screen, payload.size());
        OpenOnUI("window.csnOpen(" + JsonString(a_screen) + "," + payload + ");");
        return true;
    }

    bool PushData(RE::StaticFunctionTag*, std::string a_screen, std::string a_payload)
    {
        if (!IsAvailable(nullptr)) {
            return false;
        }
        std::string script = "window.csnUpdate(" + JsonString(a_screen) + "," + NormalizeJson(a_payload) + ");";
        SKSE::GetTaskInterface()->AddUITask([script]() { RunScript(script); });
        return true;
    }

    bool IsScreenOpen(RE::StaticFunctionTag*)
    {
        return g_factionScreenOpen.load();
    }

    void CloseView(RE::StaticFunctionTag*)
    {
        g_factionScreenOpen = false;
        ScheduleHide();
    }

    void ApplyTheme()
    {
        std::string theme = g_theme;
        SKSE::GetTaskInterface()->AddUITask([theme]() { RunScript("window.csnSetTheme && window.csnSetTheme('" + theme + "');"); });
    }

    void ReloadSettings(RE::StaticFunctionTag*)
    {
        LoadKeys();
        ApplyTheme();
    }

    std::string NormalizeJsonNative(RE::StaticFunctionTag*, std::string a_json)
    {
        return NormalizeJson(a_json);
    }

    // Settlement map markers (CSN_Settle): the name shown on the world map. Not kept in the save, so CSN sets it
    // again on every load.
    bool SetMapMarkerName(RE::StaticFunctionTag*, RE::TESObjectREFR* a_ref, std::string a_name)
    {
        if (!a_ref) {
            return false;
        }
        auto* marker = a_ref->extraList.GetByType<RE::ExtraMapMarker>();
        if (!marker || !marker->mapData) {
            return false;
        }
        marker->mapData->locationName.fullName = a_name;
        return true;
    }

    bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
    {
        constexpr auto cls = "CSN_Native";
        a_vm->RegisterFunction("IsAvailable", cls, IsAvailable);
        a_vm->RegisterFunction("IsMenuMessage", cls, IsMenuMessage);
        a_vm->RegisterFunction("GetOwnerQuest", cls, GetOwnerQuest);
        a_vm->RegisterFunction("GetAliasTags", cls, GetAliasTags);
        a_vm->RegisterFunction("ShowMessage", cls, ShowMessage);
        a_vm->RegisterFunction("OpenTextInput", cls, OpenTextInput);
        a_vm->RegisterFunction("OpenList", cls, OpenList);
        a_vm->RegisterFunction("OpenNotice", cls, OpenNotice);
        a_vm->RegisterFunction("OpenConfirm", cls, OpenConfirm);
        a_vm->RegisterFunction("OpenChoice", cls, OpenChoice);
        a_vm->RegisterFunction("OpenAmount", cls, OpenAmount);
        a_vm->RegisterFunction("Log", cls, LogLine);
        a_vm->RegisterFunction("GetLogPath", cls, GetLogPath);
        a_vm->RegisterFunction("UIMessage", cls, UIMessage);
        a_vm->RegisterFunction("IsScreenOpen", cls, IsScreenOpen);
        a_vm->RegisterFunction("PollResult", cls, PollResult);
        a_vm->RegisterFunction("PollText", cls, PollText);
        a_vm->RegisterFunction("OpenView", cls, OpenView);
        a_vm->RegisterFunction("ReloadSettings", cls, ReloadSettings);
        a_vm->RegisterFunction("NormalizeJson", cls, NormalizeJsonNative);
        a_vm->RegisterFunction("PushData", cls, PushData);
        a_vm->RegisterFunction("CloseView", cls, CloseView);
        a_vm->RegisterFunction("SetMapMarkerName", cls, SetMapMarkerName);
        return true;
    }

    // ------------------------------------------------------------------ JS -> C++
    // "token|index|text"
    void OnDialogResult(const char* a_data)
    {
        std::string s = a_data ? a_data : "";
        auto p1 = s.find('|');
        auto p2 = p1 == std::string::npos ? std::string::npos : s.find('|', p1 + 1);
        if (p1 == std::string::npos) {
            return;
        }
        int token = std::atoi(s.substr(0, p1).c_str());
        int index = std::atoi(s.substr(p1 + 1, p2 == std::string::npos ? std::string::npos : p2 - p1 - 1).c_str());
        std::string text = p2 == std::string::npos ? "" : s.substr(p2 + 1);
        {
            std::lock_guard lk(g_lock);
            auto it = g_results.find(token);
            if (it == g_results.end() || it->second.index != -2) {
                return;
            }
            it->second.index = index;
            it->second.text = text;
            if (g_fireAndForget.erase(token)) {
                g_results.erase(it);
            }
        }
        if (g_openDialogs > 0) {
            --g_openDialogs;
        }
        ScheduleHide();
    }

    void CacheGlobals()
    {
        auto* dh = RE::TESDataHandler::GetSingleton();
        if (!dh) {
            return;
        }
        for (auto* g : dh->GetFormArray<RE::TESGlobal>()) {
            if (g && g->GetFormEditorID()) {
                g_globals[Lower(g->GetFormEditorID())] = g;
            }
        }
        SKSE::log::info("Cached {} globals for message text.", g_globals.size());
    }

    void SetupView()
    {
        CacheGlobals();
        g_prisma = static_cast<PRISMA_UI_API::IVPrismaUI1*>(
            PRISMA_UI_API::RequestPluginAPI(PRISMA_UI_API::InterfaceVersion::V1));
        if (!g_prisma) {
            SKSE::log::error("PrismaUI not found - menus fall back to vanilla message boxes.");
            return;
        }
        g_view = g_prisma->CreateView("ConquestOfSkyrimNet/index.html", [](PrismaView a_view) -> void {
            g_domReady = true;
            g_prisma->Hide(a_view);
            for (auto& s : g_pending) {
                g_prisma->Invoke(a_view, s.c_str());
            }
            g_pending.clear();
            g_prisma->Invoke(a_view, ("window.csnSetTheme && window.csnSetTheme('" + g_theme + "');").c_str());
            SKSE::log::info("Conquest of SkyrimNet view ready.");
        });
        g_prisma->RegisterJSListener(g_view, "csnLog", [](const char* a_data) -> void {
            SKSE::log::warn("[view] {}", a_data ? a_data : "");
        });
        g_prisma->RegisterJSListener(g_view, "csnTyping", [](const char* a_data) -> void {
            g_typing = a_data && a_data[0] == '1';
            SKSE::GetTaskInterface()->AddUITask([]() { ApplyTypingFocus(); });
        });
        g_prisma->RegisterJSListener(g_view, "csnSend", [](const char* a_data) -> void { SendCommand(a_data ? a_data : ""); });
        g_prisma->RegisterJSListener(g_view, "csnResult", [](const char* a_data) -> void { OnDialogResult(a_data); });
        g_prisma->RegisterJSListener(g_view, "csnClose", [](const char*) -> void {
            g_factionScreenOpen = false;
            SKSE::GetTaskInterface()->AddUITask([]() {
                if (g_openDialogs == 0) {
                    HideView();
                }
            });
            SendCommand("close");
        });
    }

    void OnMessage(SKSE::MessagingInterface::Message* a_msg)
    {
        switch (a_msg->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            SetupView();
            LoadKeys();
            if (auto* input = RE::BSInputDeviceManager::GetSingleton()) {
                input->AddEventSink(InputSink::Get());
            }
            if (auto* ui = RE::UI::GetSingleton()) {
                ui->AddEventSink<RE::MenuOpenCloseEvent>(MenuSink::Get());
            }
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
        case SKSE::MessagingInterface::kNewGame:
            SendCommand("gameLoaded");
            break;
        default:
            break;
        }
    }
}

namespace
{
    // The folder SKSE itself writes skse64.log to. CommonLib's log_directory() can pick a different
    // "My Games" subfolder on some installs (for example "Skyrim Special Edition GOG" when there is no
    // steam_api64.dll), so look for skse64.log first and only then fall back.
    std::filesystem::path LogFolder()
    {
        std::vector<std::filesystem::path> tries;
        const char* home = std::getenv("USERPROFILE");
        std::filesystem::path h = home ? std::filesystem::path(home) : std::filesystem::path(".");
        tries.push_back(h / "Documents" / "My Games" / "Skyrim Special Edition" / "SKSE");
        tries.push_back(h / "OneDrive" / "Documents" / "My Games" / "Skyrim Special Edition" / "SKSE");
        if (auto dir = SKSE::log::log_directory()) {
            tries.insert(tries.begin(), *dir);
            auto parent = dir->parent_path().parent_path();  // .../My Games
            tries.push_back(parent / "Skyrim Special Edition" / "SKSE");
        }
        std::error_code ec;
        for (auto& t : tries) {
            if (std::filesystem::exists(t / "skse64.log", ec)) {
                return t;
            }
        }
        for (auto& t : tries) {
            if (std::filesystem::is_directory(t, ec)) {
                return t;
            }
        }
        return std::filesystem::path("Data") / "SKSE" / "Plugins";
    }

    std::shared_ptr<spdlog::logger> MakeLogger(const char* a_name, const char* a_file)
    {
        try {
            auto dir = LogFolder();
            std::error_code ec;
            std::filesystem::create_directories(dir, ec);
            std::shared_ptr<spdlog::sinks::basic_file_sink_mt> sink;
            try {
                sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>((dir / a_file).string(), true);
            } catch (...) {
                dir = std::filesystem::path("Data") / "SKSE" / "Plugins";  // always writable for SKSE plugins here
                sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>((dir / a_file).string(), true);
            }
            if (std::string(a_file) == "ConquestOfSkyrimNet.log") {
                g_logPath = std::filesystem::absolute(dir / a_file, ec).string();
            }
            auto log = std::make_shared<spdlog::logger>(a_name, std::move(sink));
            log->set_level(spdlog::level::info);
            log->flush_on(spdlog::level::info);
            return log;
        } catch (...) {
            return nullptr;
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    SKSE::Init(a_skse);
    if (auto log = MakeLogger("global", "ConquestOfSkyrimNetUI.log")) {
        spdlog::set_default_logger(std::move(log));
    }
    g_csnLog = MakeLogger("csn", "ConquestOfSkyrimNet.log");
    SKSE::log::info("Conquest of SkyrimNet UI loaded (log: {})", g_logPath);
    if (g_csnLog) {
        g_csnLog->info("Conquest of SkyrimNet log started. Turn on Debug logging in the MCM for script messages.");
    }
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SKSE::GetPapyrusInterface()->Register(RegisterFuncs);
    return true;
}
