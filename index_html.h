static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang=en>
    <head>
        <meta charset="UTF-8">
        <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 5.0">
        <title>Media Remote 0.11</title>
        <style type="text/css">
            table {
                position: absolute;
                top: 0;
                bottom: 0;
                left: 0;
                right: 0;
                height: 100%;
                width: 100%;
                border-collapse: collapse;
            }
            td {
                border: 1px solid;
                font-size: 200%;
                text-align: center;
            }
        </style>
        <script type="text/javascript">
            var websock;
            var connected = false;

            function touch_start(event) {
                event.preventDefault();
                console.log('touch_start', this.id, this.innerHTML, event);
                var json = JSON.stringify({event:'touch start', name:this.innerHTML, row:this.row, col:this.col});
                document.getElementById(this.id).style.backgroundColor = "yellow";
                if (!connected) {
                    setTimeout(function(json) {
                        websock.send(json);
                    }, 200);
                    window.location.reload();
                }
                else {
                    websock.send(json);
                }
            }
            function touch_end(event) {
                event.preventDefault();
                console.log('touch_end', this.id, this.innerHTML, event);
                document.getElementById(this.id).style.backgroundColor = "white";
                websock.send(JSON.stringify({event:'touch end', name:this.innerHTML, row:this.row, col:this.col}));
            }
            function touch_move(event) {
                event.preventDefault();
                var json = JSON.stringify({event:'touch move', name:this.innerHTML, row:this.row, col:this.col});
                if (!connected) {
                    setTimeout(function(json) {
                        websock.send(json);
                    }, 200);
                    window.location.reload();
                }
                else {
                    websock.send(json);
                }
            }
            function touch_cancel(event) {
                event.preventDefault();
                console.log('touch_cancel', this.id, this.innerHTML, event);
                document.getElementById(this.id).style.backgroundColor = "white";
                websock.send(JSON.stringify({event:'touch cancel', name:this.innerHTML, row:this.row, col:this.col}));
            }
            function button_click(event) {
                event.preventDefault()
            }
            function double_click(event) {
                event.preventDefault();
            }
            function context_menu(event) {
                event.preventDefault();
            }
            function generateGrid(cellData, id) {
                let grid = document.getElementById(id);
                let r = 0;
                for (let rowElement of cellData) {
                    let aRow = grid.insertRow();
                    let c = 0;
                    for (let colElement of rowElement) {
                        let cell = aRow.insertCell();
                        cell.id = 'r' + r + 'c' + c;
                        cell['row'] = r;
                        cell['col'] = c;
                        cell.onclick = button_click;
                        cell.ondblclick = double_click;
                        cell.oncontextmenu = context_menu;
                        cell.ontouchstart = touch_start;
                        cell.onmousedown = touch_start;
                        cell.ontouchmove = touch_move;
                        cell.onmousemove = touch_move;
                        cell.ontouchend = touch_end;
                        cell.onmouseup = touch_end;
                        cell.onmouseout = touch_end;
                        cell.onmouseleave = touch_end;
                        cell.ontouchcancel = touch_cancel;
                        cell.innerHTML = colElement["cellLabel"];
                        if (colElement["colSpan"] > 1) {
                            cell.colSpan = colElement["colSpan"];
                        }
                        c++;
                    }
                    r++;
                }
            }

            var FullPage = document.documentElement;
            function openFullscreen() {
                if (FullPage.requestFullscreen) {
                    FullPage.requestFullscreen();
                } else if (FullPage.mozRequestFullScreen) { /* Firefox */
                        FullPage.mozRequestFullScreen();
                } else if (FullPage.webkitRequestFullscreen) { /* Chrome, Safari & Opera */
                        FullPage.webkitRequestFullscreen();
                } else if (FullPage.msRequestFullscreen) { /* IE/Edge */
                        FullPage.msRequestFullscreen();
                }
            }

            function start() {
                // sending a connect request to the server.
                websock = new WebSocket('ws://' + window.location.hostname + ':81/');
                websock.onopen = function(evt) { console.log('websock onopen', evt); connected = true; };
                websock.onclose = function(evt) { console.log('websock onclose', evt); connected = false; };
                websock.onerror = function(evt) { console.log('websock onerror', evt); };
                websock.onmessage = function(evt) {
                    console.log(evt);
                    generateGrid(JSON.parse(evt.data), "my_table");
                };
            }
        </script>
    </head>
    <body onload="javascript:start();" onresize="window.location.reload();">
        <table id="my_table">
            <!-- Grid goes here -->
        </table>
    </body>
</html>
)rawliteral";
