/**
 * @constructor
 * @param {number} timestamp
 * @param {string} description
 * @param {boolean=} opt_isCommand
 * @param {boolean=} opt_isDeath
 */
 var GameState = function(timestamp, description, opt_isCommand, opt_isDeath, opt_isReport, opt_isEntity, opt_id, opt_isTitle) {
  this.timestamp = timestamp;
  this.description = description;
  this.isDeath = opt_isDeath;
  this.isReport = opt_isReport;
  this.isEntity = opt_isEntity;
  this.id = opt_id;
  this.isTitle = opt_isTitle;
};


GameState.prototype.toDomNode = function(cursor) {
  if (cursor <= 0) {
    return document.createTextNode('');
  }
  if (this.isReport) {
    var element = document.createElement('p');
    element.className = 'report';
    element.textContent = this.description;
    return element;
  } else if (this.isEntity) {
    var canvas = document.createElement('canvas');
    canvas.style.display = "inline";
    canvas.width = 20;
    canvas.height = 20;
    canvas.style.width = canvas.width / 2;
    canvas.style.height = canvas.height / 2;
    var context = canvas.getContext('2d');
    context.scale(1, 1);
    entities.push({canvas: canvas, context: context, id: this.id});
    return canvas;
  } else if (this.isTitle) {
    var parent = document.createElement('div');
    var span = document.createElement('span');
    parent.appendChild(span);
    var parts = this.description.split(' ');
    var title = parts[0];
    var subtitle = parts[1];
    var content = document.createTextNode(title + '\u00A0');
    span.appendChild(content);
    span.style.fontFamily = '"EVA Hand 1"';
    span.style.fontWeight = 'bold';
    span = document.createElement('span');
    span.appendChild(document.createTextNode(subtitle + '\u00A0'));
    span.style.fontWeight = 'bold';
    parent.appendChild(span);
    return parent;
  } else {
    return document.createTextNode(this.description + '\u00A0');
  }
};


var Line = function(gameStates) {
  this.gameStates = gameStates;
};


Object.defineProperties(
  Line.prototype, {
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


Line.prototype.toDomNode = function(prefix, cursor) {
  var element = document.createElement('p');
  element.appendChild(document.createTextNode(prefix));
  this.gameStates.forEach(function(state) {
    element.appendChild(state.toDomNode(cursor));
  });
  return element;
};


var connect = function() {
  var location = window.location.toString().replace(/http/, 'ws');
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

var target_x = 0;
var target_y = 0;
var smooth_x = 0;
var smooth_y = 0;

var directions = {};
var target_directions = {};

var position_x = 0, position_y = 0, target_position_x = 0, target_position_y = 0;

var alpha = 0.2;


var message = function(event) {
  var payload = JSON.parse(event.data);
  if ("step" == payload.type) {
    lines[lines.length - 1].gameStates.push(
      new GameState(t += 1.0, ".", false, false, false));
  } else if ("entity" == payload.type) {
    lines[lines.length - 1].gameStates.push(
      new GameState(t += 1.0, "", false, false, false, true, payload.id));
  } else if ("telemetry" == payload.type) {
    target_x = canvas.width / 3 * payload.direction.x;
    target_y = canvas.width / 3 * -payload.direction.y;
    target_position_x = payload.position.x;
    target_position_y = payload.position.y;
    target_directions = payload.directions;
  } else if ("report" == payload.type) {
    lines.push(new Line([
      new GameState(t += 1.0, payload.report, false, false, true)]));
    lineCursor += 1;
  } else if ("text" == payload.type) {
    lines.push(new Line([
      new GameState(t += 1.0, payload.text, false, false, false)]));
    lineCursor += 1;
  }
  display();
};


var drawArrows = function() {
  smooth_x = (1.0 - alpha) * smooth_x + alpha * target_x;
  smooth_y = (1.0 - alpha) * smooth_y + alpha * target_y;
  drawArrow(canvas, context, smooth_x, smooth_y);
  position_x = (1.0 - alpha) * position_x + alpha * target_position_x;
  position_y = (1.0 - alpha) * position_y + alpha * target_position_y;
  for (var id in target_directions) {
    if (!directions[id]) {
      directions[id] = {x: 0.0, y: 0.0};
    }
    directions[id].x = (1.0 - alpha) * directions[id].x + alpha * target_directions[id].x;
    directions[id].y = (1.0 - alpha) * directions[id].y + alpha * target_directions[id].y;
  }
  for (var i = 0; i < entities.length; ++i) {
    var dx = directions[entities[i].id].x;
    var dy = directions[entities[i].id].y;
    var x = entities[i].canvas.width / 3 * dx;
    var y = entities[i].canvas.width / 3 * -dy;
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
  context.moveTo(x / 2 + x2 / 3, y / 2 + y2 / 3);
  context.lineTo(x, y);
  context.lineTo(x / 2 - x2 / 3, y / 2 - y2 / 3);
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
  websocket.removeEventListener('open', open);
  websocket.removeEventListener('message', message);
  websocket.removeEventListener('error', error);
  websocket.removeEventListener('close', close);
//  reconnect = window.setTimeout(connect, 1000);
};


var LINE_COUNT = 30;


var SCROLL_DELAY = 80;


var t = 0.0;


var lines = [new Line([new GameState(t, '\u00A0polimpsasd /ˈpælɪmpsɛst/', false, false, false, false, null, true)])];


var lineCursor = 1;


var container;


var cursor;


var isDark = false;


var websocket;


var blink;


var reconnect;


var canvas, context;


var load = function() {
  container = document.getElementById('container');
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


var display = function () {
  entities = [];
  while (container.childNodes.length > 0) {
    container.removeChild(container.childNodes[0]);
  }
  var startIndex = lineCursor - LINE_COUNT >= 0 ? lineCursor - LINE_COUNT : 0;
  lines.slice(startIndex, lineCursor).forEach(function(line, index) {
    var content = line.toDomNode('', line.description.length);
    container.appendChild(content);
  });
  if (lineCursor >= 0 && lines.length > lineCursor) {
    var content = lines[lineCursor].toDomNode('', 0);
    var newline = document.createElement('br');
    container.appendChild(content);
    container.appendChild(newline);
  }
};
