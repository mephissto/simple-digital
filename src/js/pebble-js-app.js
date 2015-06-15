Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL('http://public.msl.re/simple-digital-config.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  
  var configuration = JSON.parse(decodeURIComponent(e.response));

  var options = {
    "PERSIST_KEY_DATE": '' + configuration.date,
  };

  Pebble.sendAppMessage(options,
    function(e) {
      console.log('Settings update successful!');
    },
    function(e) {
      console.log('Settings update failed: ' + JSON.stringify(e));
    });
  
}); 