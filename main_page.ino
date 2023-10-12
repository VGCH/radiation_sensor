void handleRoot() {
   if (captivePortal()) {  
    return;
  }
  String header;
  if (!validateToken()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  String html = "<html><head><meta charset=\"UTF-8\"><title>Система мониторинга</title>";
  html += "<link href=\"style.css\" rel=\"stylesheet\" type=\"text/css\" />";
  html +=  js;
  html += "</head><body>";
  html += divcode;
  html += "<h2> Основные данные </h2>";
  html += "<form onsubmit=\"return false\" oninput=\"level.value = flevel.valueAsNumber\">";
  String content ="";
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  html += "<text>Время работы устройства: <div class =\"live\" id=\"timew\">00:00:00:00</div></text>";
  html += "<text>Статус MQTT: <div class =\"live\" id=\"MQTT\">"+MQTT_status()+"</div></text>";
  html += "<text>Уровень радиации (сред. 10 сек), мкР/ч: <div class =\"live\" id=\"val10s\">0.00</div></text>";
  html += "<text>Уровень радиации (сред. 1 мин), мкР/ч:  <div class =\"live\" id=\"val1m\" >0.00</div></text>";
  html += "<text>Уровень радиации (сред. 5 мин), мкР/ч:  <div class =\"live\" id=\"val5m\" >0.00</div></text>";
  html += "<text>Уровень радиации (сред. 60 мин), мкР/ч: <div class =\"live\" id=\"val60m\">0.00</div></text>";
  html += "<text>Детектируемых частиц за период 10 сек:  <div class =\"live\" id=\"CP10s\" >0</div></text>";
  html += "<text>Детектируемых частиц за период 1 мин:   <div class =\"live\" id=\"CP1m\"  >0</div></text>";
  html += "<text>Детектируемых частиц за период 5 мин:   <div class =\"live\" id=\"CP5m\"  >0</div></text>";
  html += "<text>Детектируемых частиц за период 60 мин:  <div class =\"live\" id=\"CP60m\" >0</div></text>";
  html += "<input type=\"checkbox\" id=\"switch\" "+buzzer_st()+"><text> Активировать звуковой индикатор</text><br>";
  html += "</form>";
  html += "<script>";
    // при изменении состояния переключателя отправляем данные на сервер
  html += "    document.getElementById('switch').addEventListener('change', function() {";
  html += "      var status = this.checked ? 'on' : 'off';";
  html += "      var xhr = new XMLHttpRequest();";
  html += "      xhr.open('POST', '?page=indata', true);";
  html += "      xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');";
  html += "      xhr.onreadystatechange = function() {";
  html += "        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {";
  html += "          console.log(this.responseText);";
  html += "         }";
  html += "      };";
  html += "      xhr.send('status=' + status);";
  html += "    });";
  html += "  </script>";
  html += "</body>";
  html += "<center><br><a href=\"/?page=wlan_config\">Wi-Fi конфигурация</a><br>";
  html += "<a href=\"/?page=send_config\">Настройка передачи данных</a><br>";
  html += "<a href=\"/?page=coeftube\">Настройка трубки</a><br>";
  html += "<a href=\"/?page=changelp\">Изменение пароля устройства</a><br>";
  html += "<a href=\"/?page=update_fw\">Обновление прошивки</a><br>";
  html += "<a href=\"javascript:void(0)\" onclick=\"rebootdev()\">Перезагрузить устройство</a><br>";
  html += "<a href=\"/login?DISCONNECT=YES\">Выход</a></center>";
  html += "<footer>© <b>CYBEREX TECH</b>, 2023. Версия микро ПО <b>"+version_code+"</b>.</footer>";
  html += "<script src=\"script.js\"></script>"; 
  html += "</html>";
  server.send(200, "text/html", html);
}
String buzzer_st(){
    String rt ="";
    if(settings.buzzer_en){ rt = "checked"; }
  return rt;
}
