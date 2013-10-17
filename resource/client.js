var websocket;
var load = function() {
  var location = window.location.toString().replace(/http/, 'ws');
  console.log(location);
  websocket = new WebSocket(location, 'interactive-fiction-protocol');
  websocket.addEventListener('open', open);
  websocket.addEventListener('message', message);
  websocket.addEventListener('error', error);
  websocket.addEventListener('close', close);
};
var open = function() {
  websocket.send(JSON.stringify({message: 'hello'}));
};
var message = function(event) {
  console.log(JSON.parse(event.data));
  websocket.send(JSON.stringify({message: 'hello'}));
};
var error = function(event) {
  console.log('error');
  console.log(event.data);
};
var close = function(event) {
  console.log('closed');
}
window.addEventListener('load', load);
