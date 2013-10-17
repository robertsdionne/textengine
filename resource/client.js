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
var GameState = function(timestamp, description, opt_isCommand, opt_isDeath) {
  this.timestamp = timestamp;
  this.description = description;
  this.isCommand = opt_isCommand;
  this.isDeath = opt_isDeath;
};


GameState.prototype.toDomNode = function(cursor, scrollBehavior) {
  if (cursor <= 0) {
    return document.createTextNode('');
  }
  if (this.isCommand) {
    var element = document.createElement('b');
    element.textContent = scrollBehavior.slice(this.description, 0, cursor);
    return element;
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


var open = function() {};


var message = function(event) {
  var payload = JSON.parse(event.data);
  lines.push(new Line([new GameState(t + 1.0, payload.message)]));
  lineCursor += 1;
  display();
};


var error = function(event) {
  console.log('error');
  console.log(event.data);
};


var close = function(event) {
  console.log('closed');
};


var LINE_COUNT = 4;


var SCROLL_DELAY = 80;


var t = 0.0;


var lines = [new Line([new GameState(t, 'Text Engine')])];


var lineCursor = 1;


var container;


var commandline;


var cursor;


var isDark = false;


var scrollBehavior = new PerWordScrollBehavior();


var websocket;


var blink;


var load = function() {
  document.addEventListener('keypress', command, false);
  document.addEventListener('keydown', backspace, false);
  container = document.getElementById('container');
  commandline = document.getElementById('commandline');
  cursor = document.getElementById('cursor');
  var location = window.location.toString().replace(/http/, 'ws');
  console.log(location);
  websocket = new WebSocket(location, 'interactive-fiction-protocol');
  websocket.addEventListener('open', open);
  websocket.addEventListener('message', message);
  websocket.addEventListener('error', error);
  websocket.addEventListener('close', close);
  display();
  resetCursor();
};
window.addEventListener('load', load, false);


var blinkCursor = function() {
  cursor.textContent = cursor.textContent ? '' : '\u2588';
};

var resetCursor = function() {
  cursor.textContent = '\u2588';
  window.clearInterval(blink);
  blink = window.setInterval(blinkCursor, 1000);
}


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
  while (container.childNodes.length) {
    container.removeChild(container.childNodes[0]);
  }
  var startIndex = lineCursor - LINE_COUNT >= 0 ? lineCursor - LINE_COUNT : 0;
  lines.slice(startIndex, lineCursor).forEach(function(line, index) {
                                              var prefix = (startIndex + index) % 4 == 1 ? '\u00A0\u00A0' : '';
                                              var content = line.toDomNode(prefix, line.description.length, scrollBehavior);
                                              var newline = document.createElement('br');
                                              container.appendChild(content);
                                              container.appendChild(newline);
                                              });
  if (lineCursor >= 0 && lines.length > lineCursor) {
    var prefix = lineCursor % 4 == 1 ? '\u00A0\u00A0' : '';
    var content = lines[lineCursor].toDomNode(prefix, 0, scrollBehavior);
    var newline = document.createElement('br');
    container.appendChild(content);
    container.appendChild(newline);
  }
};