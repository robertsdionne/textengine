/**
 * @enum {string}
 */
var KeyCode = {
BACKSPACE: 8,
DOWN: 40,
LEFT: 37,
RETURN: 13,
RIGHT: 39,
TILDE: 96,
UP: 38
};


/**
 * @constructor
 * @implements {ScrollBehavior}
 */
var PerWordScrollBehavior = function() {};


/**
 * @param {string} value
 * @return {number} the length
 */
PerWordScrollBehavior.prototype.length = function(value) {
  return value.split(' ').length;
};


/**
 * @param {string} value
 * @param {number} start
 * @param {number} stop
 * @return {string}
 */
PerWordScrollBehavior.prototype.slice = function(value, start, stop) {
  return value.split(' ').slice(start, stop).join(' ');
};


PerWordScrollBehavior.prototype.indexInto = function(line, cursor) {
  for (var i = 0; i < line.gameStates.length; ++i) {
    cursor -= this.length(line.gameStates[i].description);
    if (cursor < 0) {
      return i;
    }
  }
};


/**
 * @constructor
 * @param {number} timestamp
 * @param {string} description
 * @param {boolean=} opt_isCommand
 * @param {boolean=} opt_isDeath
 */
var GameState = function(timestamp, description, opt_isCommand, opt_isDeath, opt_isReport, opt_isEntity, opt_position) {
  this.timestamp = timestamp;
  this.description = description;
  this.isCommand = opt_isCommand;
  this.isDeath = opt_isDeath;
  this.isReport = opt_isReport;
  this.isEntity = opt_isEntity;
  this.position = opt_position;
};


GameState.prototype.toDomNode = function(cursor, scrollBehavior) {
  if (cursor <= 0) {
    return document.createTextNode('');
  }
  if (this.isCommand) {
    var element = document.createElement('b');
    element.className = 'command';
    element.textContent = scrollBehavior.slice(this.description, 0, cursor);
    return element;
  } else if (this.isReport) {
    var element = document.createElement('p');
    element.className = 'report';
    element.textContent = scrollBehavior.slice(this.description, 0, cursor);
    return element;
  } else if (this.isEntity) {
    var canvas = document.createElement('canvas');
    canvas.style.display = "inline";
    canvas.width = 40;
    canvas.height = 40;
    canvas.style.width = canvas.width / 2;
    canvas.style.height = canvas.height / 2;
    var context = canvas.getContext('2d');
    context.scale(1, 1);
    entities.push({canvas: canvas, context: context, position: this.position});
    return canvas;
  } else {
    return document.createTextNode(scrollBehavior.slice(this.description, 0, cursor) + '\u00A0');
  }
};


var Line = function(gameStates) {
  this.gameStates = gameStates;
};


Object.defineProperties(Line.prototype, {
                        timestamp: {
                        get: function() {
                        return this.gameStates.map(function(state) {
                                                   return state.timestamp;
                                                   }).reduce(function(timestamp0, timestamp1) {
                                                             return Math.min(timestamp0, timestamp1);
                                                             });
                        }
                        },
                        description: {
                        get: function() {
                        return this.gameStates.map(function(state) {
                                                   return state.description;
                                                   }).join(' ');
                        }
                        },
                        isDeath: {
                        get: function() {
                        return this.gameStates.map(function(state) {
                                                   return state.isDeath;
                                                   }).reduce(function(isDeath0, isDeath1) {
                                                             return isDeath0 || isDeath1;
                                                             });
                        }
                        }
                        });


Line.prototype.toDomNode = function(prefix, cursor, scrollBehavior) {
  var index = scrollBehavior.indexInto(this, cursor);
  var element = document.createElement('p');
  element.appendChild(document.createTextNode(prefix));
  this.gameStates.forEach(function(state) {
                          element.appendChild(state.toDomNode(cursor, scrollBehavior));
                          cursor -= scrollBehavior.length(state.description);
                          });
  return element;
};


var connect = function() {
  var location = window.location.toString().replace(/http/, 'ws');
  console.log(location);
  websocket = new WebSocket(location, 'interactive-fiction-protocol');
  websocket.addEventListener('open', open);
  websocket.addEventListener('message', message);
  websocket.addEventListener('error', error);
  websocket.addEventListener('close', close);
};


var open = function() {
  window.clearTimeout(reconnect);
};

var entities = [];

var target_x = 0, x = 0;
var target_y = 0, y = 0;

var position_x = 0, position_y = 0, target_position_x = 0, target_position_y = 0;

var alpha = 0.2;


var message = function(event) {
  console.log(event.data);
  var payload = JSON.parse(event.data);
  if ("step" == payload.type) {
    console.log("in step");
    lines[lines.length - 1].gameStates.push(
        new GameState(t += 1.0, ".", false, false, false));
  } else if ("entity" == payload.type) {
    lines[lines.length - 1].gameStates.push(
        new GameState(t += 1.0, "", false, false, false, true, payload.position));
  } else if ("telemetry" == payload.type) {
    console.log("in telemetry");
    target_x = canvas.width / 3 * payload.direction.x;
    target_y = canvas.width / 3 * -payload.direction.y;
    target_position_x = payload.position.x;
    target_position_y = payload.position.y;
  } else if ("report" == payload.type) {
    console.log("in report");
    lines.push(new Line([
      new GameState(t += 1.0, payload.report, false, false, true)]));
    lineCursor += 1;
  } else if ("text" == payload.type) {
    console.log("in text");
    lines.push(new Line([
      new GameState(t += 1.0, payload.text, false, false, false)]));
    lineCursor += 1;
  }
  display();
};


var drawArrows = function() {
  x = (1.0 - alpha) * x + alpha * target_x;
  y = (1.0 - alpha) * y + alpha * target_y;
  position_x = (1.0 - alpha) * position_x + alpha * target_position_x;
  position_y = (1.0 - alpha) * position_y + alpha * target_position_y;
  drawArrow(canvas, context, target_x, target_y);
  for (var i = 0; i < entities.length; ++i) {
    var dx = entities[i].position.x - position_x;
    var dy = entities[i].position.y - position_y;
    var d = Math.sqrt(dx * dx + dy * dy);
    var x = entities[i].canvas.width / 3 * dx / d;
    var y = entities[i].canvas.width / 3 * -dy / d;
    drawArrow(entities[i].canvas, entities[i].context, x, y);
  }
  window.requestAnimationFrame(drawArrows);
};


var drawArrow = function(canvas, context, x, y) {
  var x2 = y;
  var y2 = -x;
  context.clearRect(0, 0, canvas.width, canvas.height);
  context.strokeStyle = 'rgb(0, 0, 0)';
  context.lineWidth = 4.0;
  context.lineCap = 'round';
  context.save();
  context.translate(canvas.width / 2, canvas.height / 2);
  context.beginPath();
  context.moveTo(-x, -y);
  context.lineTo(x, y);
  context.moveTo(x / 2 + x2 / 4, y / 2 + y2 / 4);
  context.lineTo(x, y);
  context.lineTo(x / 2 - x2 / 4, y / 2 - y2 / 4);
  context.stroke();
  context.closePath();
  context.restore();
};


var error = function(event) {
  console.log('error');
  console.log(event.data);
};


var close = function(event) {
  console.log('closed');
  clearCursor();
  document.removeEventListener('keypress', command, false);
  document.removeEventListener('keydown', backspace, false);
  websocket.removeEventListener('open', open);
  websocket.removeEventListener('message', message);
  websocket.removeEventListener('error', error);
  websocket.removeEventListener('close', close);
//  reconnect = window.setTimeout(connect, 1000);
};


var LINE_COUNT = 10;


var SCROLL_DELAY = 80;


var t = 0.0;


var lines = [new Line([new GameState(t, 'Text Field')])];


var lineCursor = 1;


var container;


var commandline;


var cursor;


var isDark = false;


var scrollBehavior = new PerWordScrollBehavior();


var websocket;


var blink;


var reconnect;


var canvas, context;


var load = function() {
  container = document.getElementById('container');
  commandline = document.getElementById('commandline');
  cursor = document.getElementById('cursor');
  canvas = document.getElementById('arrow');
  canvas.width = canvas.width * 2;
  canvas.height = canvas.height * 2;
  canvas.style.width = canvas.width / 2;
  canvas.style.height = canvas.height / 2;
  context = canvas.getContext('2d');
  context.scale(1, 1);
  connect();
  display();
  window.requestAnimationFrame(drawArrows);
//  resetCursor();
};
window.addEventListener('load', load, false);


var blinkCursor = function() {
  cursor.textContent = cursor.textContent ? '' : '\u258b';
};

var resetCursor = function() {
  cursor.textContent = '\u258b';
  window.clearInterval(blink);
  blink = window.setInterval(blinkCursor, 1000);
};


var clearCursor = function() {
  cursor.textContent = '';
  window.clearInterval(blink);
};


var toggleScheme = function() {
  if (isDark) {
    document.body.style.background = 'white';
    document.body.style.color = 'black';
  } else {
    document.body.style.background = 'black';
    document.body.style.color = 'white';
  }
  isDark = !isDark;
};


var backspace = function(e) {
  if (e.keyCode == KeyCode.BACKSPACE) {
    commandline.textContent = commandline.textContent.slice(0, -1);
    resetCursor();
    e.preventDefault();
  }
};


var command = function(e) {
  if (e.keyCode == KeyCode.RETURN) {
    if (commandline.textContent) {
      websocket.send(JSON.stringify({message: commandline.textContent}));
      lines.push(new Line([new GameState(t += 1.0, commandline.textContent, true)]));
      lineCursor += 1;
      commandline.textContent = '';
      resetCursor();
      display();
    }
  } else if (e.keyCode == KeyCode.TILDE) {
    toggleScheme();
  } else {
    commandline.textContent = commandline.textContent + String.fromCharCode(e.keyCode);
    resetCursor();
  }
};


var display = function () {
  console.log("in display");
  while (container.childNodes.length) {
    container.removeChild(container.childNodes[0]);
  }
  var startIndex = lineCursor - LINE_COUNT >= 0 ? lineCursor - LINE_COUNT : 0;
  lines.slice(startIndex, lineCursor).forEach(function(line, index) {
                                              var content = line.toDomNode('', line.description.length, scrollBehavior);
                                              var newline = document.createElement('br');
                                              container.appendChild(content);
                                              container.appendChild(newline);
                                              });
  if (lineCursor >= 0 && lines.length > lineCursor) {
    var content = lines[lineCursor].toDomNode('', 0, scrollBehavior);
    var newline = document.createElement('br');
    container.appendChild(content);
    container.appendChild(newline);
  }
};
