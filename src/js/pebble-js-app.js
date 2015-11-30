var config = {};

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) { console.log('PebbleKit JS ready!'); }
);

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  console.log('JavaScript config ready and running!');
  Pebble.openURL('https://rickr.github.io/undertale');
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('Configuration window returned: ' + e.response);
});


