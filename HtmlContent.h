// File: HtmlContent.h
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>DMX Readout</title>
<style>
  body, html { height: 100%; margin: 0; font-family: Arial, sans-serif; }
  table { width: 100%; height: 100%; border-collapse: collapse; table-layout: fixed; }
  td, th { border: 1px solid #ddd; text-align: center; padding: 4px; }
  tr:nth-child(even){background-color: #f2f2f2;}
  th { font-weight: bold; background-color: #4CAF50; color: white; }
  @media (min-width: 600px) { td, th { min-width: 40px; } }
  @media (min-width: 1000px) { td, th { min-width: 40px; } }
</style>
</head>
<body>
<table id='dmxTable'></table>
<script>
setInterval(loadData, 100);
function loadData() {
  fetch('/data').then(response => response.json()).then(data => {
    const table = document.getElementById('dmxTable');
    let tableHtml = '';
    let count = 0;
    let maxPerRow = (window.innerWidth > 1000) ? 20 : ((window.innerWidth > 600) ? 15 : 8);
    for (const channel in data) {
      if (count % maxPerRow === 0) tableHtml += '<tr>';
      tableHtml += `<td><b>${channel}</b></td><td>${data[channel]}</td>`;
      if (++count % maxPerRow === 0) tableHtml += '</tr>';
    }
    if (count % maxPerRow !== 0) tableHtml += '</tr>';
    table.innerHTML = tableHtml;
  }).catch(err => console.error('Error fetching data:', err));
}
</script>
</body>
</html>
)rawliteral";