/**
 * @constructor
 * @param {string} title
 */
var Title = function(title) {
  this.title = title;
};


Title.prototype.toDomNode = function() {
  var parent = document.createElement('div');
  var span = document.createElement('span');
  parent.appendChild(span);
  var parts = this.title.split(' ');
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
};


/**
 * @constructor
 * @param {string} report
 */
var Report = function(report) {
  this.report = report;
};


Report.prototype.toDomNode = function() {
  var element = document.createElement('p');
  element.className = 'report';
  element.textContent = this.report;
  return element;
};


/**
 * @constructor
 * @param {number} id
 */
var Entity = function(id) {
  this.id = id;
};


Entity.prototype.toDomNode = function() {
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
};


/**
 * @constructor
 * @param {string} text
 */
var Text = function(text) {
  this.text = text;
};


Text.prototype.toDomNode = function() {
  return document.createTextNode(this.text + '\u00A0');
};


var Line = function(gameStates) {
  this.gameStates = gameStates;
};


Object.defineProperties(
  Line.prototype, {
    description: {
      get: function() {
        return this.gameStates.map(function(state) {
         return state.description;
        }).join(' ');
      }
    }
  });


Line.prototype.toDomNode = function(prefix) {
  var element = document.createElement('p');
  element.appendChild(document.createTextNode(prefix));
  this.gameStates.forEach(function(state) {
    element.appendChild(state.toDomNode());
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
    lines[lines.length - 1].gameStates.push(new Text("."));
  } else if ("entity" == payload.type) {
    lines[lines.length - 1].gameStates.push(new Entity(payload.id));
  } else if ("telemetry" == payload.type) {
    target_x = canvas.width / 3 * payload.direction.x;
    target_y = canvas.width / 3 * -payload.direction.y;
    target_position_x = payload.position.x;
    target_position_y = payload.position.y;
    target_directions = payload.directions;
  } else if ("report" == payload.type) {
    lines.push(new Line([new Report(payload.report)]));
    lineCursor += 1;
  } else if ("text" == payload.type) {
    lines.push(new Line([new Text(payload.text)]));
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


var lines = [new Line([new Title('\u00A0polimpsasd /ˈpælɪmpsɛst/')])];


var lineCursor = 1;


var container;


var websocket;


var reconnect;


var canvas, context;


var load = function() {
  container = document.getElementById('container');
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
};
window.addEventListener('load', load, false);


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
