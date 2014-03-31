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
  canvas.className = 'entity' + this.id;
  canvas.style.display = 'inline';
  canvas.width = 20;
  canvas.height = 20;
  canvas.style.width = canvas.width / 2;
  canvas.style.height = canvas.height / 2;
  var context = canvas.getContext('2d');
  context.scale(1, 1);
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
  return document.createTextNode('\u00A0' + this.text + '\u00A0');
};



/**
 * @constructor
 * @param {Array} items
 */
var Line = function(items) {
  this.items = items;
};


Object.defineProperties(
  Line.prototype, {
    description: {
      get: function() {
        return this.items.map(function(item) {
         return item.description;
        }).join(' ');
      }
    }
  });


Line.prototype.toDomNode = function() {
  var element = document.createElement('p');
  this.items.forEach(function(item) {
    element.appendChild(item.toDomNode());
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

var target_x = 0;
var target_y = 0;
var smooth_x = 0;
var smooth_y = 0;

var directions = {};
var target_directions = {};

var position_x = 0, position_y = 0, target_position_x = 0, target_position_y = 0;

var alpha = 0.2;


var message = function(event) {
  var items = processMessage(JSON.parse(event.data));
  if (items) {
    lines.push(new Line(items));
  }
  display();
};


var processMessage = function(payload) {
  if ('composite' == payload.type) {
    var result = [];
    for (var i = 0; i < payload.messages.length; ++i) {
      var items = processMessage(payload.messages[i]);
      if (items) {
        Array.prototype.push.apply(result, items);
      }
    }
    return result;
  } else if ('entity' == payload.type) {
    return [new Entity(payload.id)];
  } else if ('telemetry' == payload.type) {
    target_x = canvas.width / 3 * payload.direction.x;
    target_y = canvas.width / 3 * -payload.direction.y;
    target_position_x = payload.position.x;
    target_position_y = payload.position.y;
    target_directions = payload.directions;
    return null;
  } else if ('report' == payload.type) {
    return [new Report(payload.report)];
  } else if ('text' == payload.type) {
    return [new Text(payload.text)];
  }
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
  for (var id in target_directions) {
    var canvases = document.getElementsByClassName('entity' + id);
    if (canvases.length) {
      var dx = directions[id].x;
      var dy = directions[id].y;
      for (var i = 0; i < canvases.length; ++i) {
        var x = canvases[i].width / 3 * dx;
        var y = canvases[i].width / 3 * -dy;
        drawArrow(canvases[i], canvases[i].getContext('2d'), x, y);
      }
    }
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
  context.scale(1, 1);
  context.translate(canvas.width / 2, canvas.height / 2);
  context.beginPath();
  context.lineJoin = 'miter';
  context.moveTo(-x * 61.0 / 90.0 + x2 / 3.0, -y * 61.0 / 90.0 + y2 / 3.0);
  context.lineTo(x / 2.0, y / 2.0);
  context.lineTo(-x * 61.0 / 90.0 - x2 / 3.0, -y * 61.0 / 90.0 - y2 / 3.0);
  context.stroke();
  context.lineJoin = 'round'
  context.moveTo(-x * 61.0 / 90.0 + x2 / 3.0, -y * 61.0 / 90.0 + y2 / 3.0);
  context.lineTo(-x * 61.0 / 90.0 - x2 / 3.0, -y * 61.0 / 90.0 - y2 / 3.0);
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
  websocket.removeEventListener('open', open);
  websocket.removeEventListener('message', message);
  websocket.removeEventListener('error', error);
  websocket.removeEventListener('close', close);
//  reconnect = window.setTimeout(connect, 1000);
};


var display = function () {
  while (container.childNodes.length > LINE_COUNT) {
    container.removeChild(container.childNodes[0]);
  }
  lines.forEach(function(line, index) {
    container.appendChild(line.toDomNode());
  });
  lines = [];
};


var LINE_COUNT = 30;


var canvas, container, context;
var lines = [];
var reconnect, websocket;


var load = function() {
  container = document.getElementById('container');
  canvas = document.getElementById('arrow');
  canvas.width = canvas.width * 2;
  canvas.height = canvas.height * 2;
  canvas.style.width = canvas.width / 2;
  canvas.style.height = canvas.height / 2;
  context = canvas.getContext('2d');
  connect();
  display();
  window.requestAnimationFrame(drawArrows);
};
window.addEventListener('load', load, false);
