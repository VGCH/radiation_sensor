void tube_setting(){
 if (!validateToken()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  String html = "<html><head><meta charset=\"UTF-8\"><title>Кофигурация трубки</title>";
  html += "<link href=\"style.css\" rel=\"stylesheet\" type=\"text/css\" />";
  html += "</head><body>";
  html += "<h2>Кофигурация трубки</h2>";
  html += "<form id=\"config-form\" >";
  html += "<div id=\"response\">";
  html += "<label for=\"number\">Коэффициент трубки:</label>";
  html += "<input type=\"number\" id=\"number\" name=\"tube_config\" placeholder=\"Пример: 10\" pattern=\"[0-9]{4,}\" value=\""+String(settings.tubecof)+"\" required><br><br>";
  html += "<input type=\"submit\" value=\"Сохранить\">";
  html += "</div>";
  html += "</form><br>";
  html += "<center><br><a href=\"/\">Вернуться назад</a><br></center>";
  html += "<footer>© <b>CYBEREX TECH</b>, 2023. Версия микро ПО <b>"+version_code+"</b>. </footer>";
  html +="<script>";
  html +="const form = document.getElementById('config-form');";
// Добавляем обработчик события отправки формы
  html +="form.addEventListener('submit', (event) => {";
  html +="  event.preventDefault();"; // Отменяем стандартное поведение формы
  html +="  const formData = new FormData(form);"; // Получаем данные формы
  html +="  const xhr = new XMLHttpRequest();"; // Создаем объект XMLHttpRequest
  html +="  xhr.open('POST', '?page=indata');"; // Указываем метод и URL для отправки данных
  html +="  xhr.addEventListener('load', () => {";
  html +="    if (xhr.status === 200) {"; // Проверяем статус ответа
  html +="      document.getElementById('response').innerHTML = xhr.responseText;"; // Выводим ответ сервера в консоль
  html +="    } else {";
  html +="      document.getElementById('response').innerHTML = 'Ошибка отправки';"; // Выводим сообщение об ошибке в консоль
  html +="    }";
  html +="  });";
  html +="  xhr.send(formData);"; // Отправляем данные формы
  html +="});";
  html +="</script>";
  html += "  <script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}
