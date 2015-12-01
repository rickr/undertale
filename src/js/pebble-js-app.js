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
  // Decode and parse config data as JSON
  var config_data = JSON.parse(decodeURIComponent(e.response));
  console.log('Config window returned: ', JSON.stringify(config_data));

  // Prepare AppMessage payload
  var dict = {
    'animation_freq': config_data['animation_freq']
  };

  // Send settings to Pebble watchapp
  Pebble.sendAppMessage(dict, function(){
    console.log('Sent config data to Pebble');
  }, function() {
    console.log('Failed to send config data!');
  });
});

