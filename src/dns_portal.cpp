#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>

// إعدادات اسم الشبكة وكلمة المرور الخاصة بالبوردة
const char* AP_SSID = "ESP32_Local_Network";
const char* AP_PASS = "123456789";

// ضبط المنفذ القياسي العالمي لخادم الـ DNS وهو المنفذ 53
const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer webServer(80);

// واجهة الـ HTML التي ستظهر للمستخدمين فور اعتراض اتصالهم
const char* HTML_CONTENT = 
"<!DOCTYPE html><html><head><meta charset='UTF-8'><title>بوابة ESP32</title></head>"
"<body style='background:#0f172a; color:#f8fafc; font-family:sans-serif; text-align:center; padding:50px; direction:rtl;'>"
"<h1>مرحباً بك في شبكة الـ ESP32 المحلية</h1>"
"<p>تم اعتراض طلب الـ DNS وتوجيهك بنجاح إلى النظام التضميني داخل المعالج.</p>"
"</body></html>";

// دالة التحكم التي تقوم بإرسال الصفحة الترحيبية للمتصفح
void handle_captive_portal() {
    webServer.send(200, "text/html", HTML_CONTENT);
}

void setup() {
    // تفعيل شاشة المراقبة (Serial Monitor) لمتابعة النظام
    Serial.begin(115200);
    
    // 1. تفعيل وضع نقطة الوصول (Access Point) وبث شبكة واي فاي خاصة بالبوردة
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    
    IPAddress apIP = WiFi.softAPIP();
    Serial.print("[WiFi] نقطة الوصول جاهزة. عنوان الـ IP هو: ");
    Serial.println(apIP);

    // 2. تشغيل سيرفر الـ DNS وتحويل كافة النطاقات (*) إلى الـ IP الخاص بالبوردة
    dnsServer.start(DNS_PORT, "*", apIP);
    Serial.println("[DNS] سيرفر الـ DNS يعمل الآن على منفذ 53 ويقوم باعتراض الطلبات.");

    // 3. توجيه أي طلب ويب غير معروف مباشرة إلى الصفحة الترحيبية (Captive Portal)
    webServer.onNotFound(handle_captive_portal);
    webServer.begin();
    Serial.println("[HTTP] خادم الويب شغال وجاهز لاستقبال المستخدمين.");
}

void loop() {
    // معالجة طلبات الـ DNS والـ HTTP القادمة في الخلفية بشكل مستمر
    dnsServer.processNextRequest();
    webServer.handleClient();
    delay(1); // تأخير بسيط جداً لضمان استقرار المعالج
}
