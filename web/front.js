ACTION_MOVE_WEST = 2;
ACTION_MOVE_EAST = 4;
ACTION_MOVE_SOUTH = 6;
ACTION_MOVE_NORTH = 8;
ACTION_WAIT = 18;
ACTION_CAST_SPELL = 25;
ACTION_LIST_SPELLS = 31;
ACTION_TELEPORT = 34;
ACTION_IDENTIFY_TRAPS = 35;

TILE_WIDTH = 32;
TILE_HEIGHT = 32;
TILES_PER_GRAPHICS_ROW = 16;
AUTO_SAVE_INTERVAL = 100;
CENTER_CIRCLE_RADIUS = 24;
NUMBER_OF_DIRECTIONS = 4;
ANGLES_PER_DIRECTION = 360 / NUMBER_OF_DIRECTIONS;

MAP_WIDTH = _get_map_width();
MAP_HEIGHT = _get_map_height();

var $canvas = null;
var $messages = null;
var $overlay = null;
var $popup = null;
var $showGridlinesCheckbox = null;

var canvas = null;
var canvasContext = null;
var graphics = null;
var previousMap = null;
var runningMapEffect = false;
var nextAutoSaveTime = Number.MAX_VALUE;
var mouseDownTimeoutId = null;
var mouseX = 0;
var mouseY = 0;

$(function() {
  if (location.search.indexOf('ffos') == -1) {
    FastClick.attach(document.body);
  }

  $canvas = $('canvas');
  $messages = $('#messages');
  $overlay = $('#overlay');
  $popup = $('#popup');
  $showGridlinesCheckbox = $('#showGridlinesCheckbox');

  canvas = $canvas[0];
  canvasContext = canvas.getContext('2d');
  canvasContext.imageSmoothingEnabled = false;
  canvasContext.webkitImageSmoothingEnabled = false;
  canvasContext.mozImageSmoothingEnabled = false;

  graphics = $('#graphics')[0];
  $(window).resize();

  $('canvas, #messages').on('touchstart', function(event) {
    event.preventDefault();
    if (runningMapEffect) {
      return;
    }
    mouseX = event.originalEvent.touches[0].pageX;
    mouseY = event.originalEvent.touches[0].pageY;
    handleMouseDown(300);
  }).mousedown(function(event) {
    if (runningMapEffect) {
      return;
    }
    mouseX = event.pageX;
    mouseY = event.pageY;
    handleMouseDown(300);
  }).on('touchmove', function(event) {
    mouseX = event.originalEvent.touches[0].pageX;
    mouseY = event.originalEvent.touches[0].pageY;
  }).mousemove(function(event) {
    mouseX = event.pageX;
    mouseY = event.pageY;
  });

  $(document).on('touchend mouseup', clearMouseDownTimeoutId);

  $(document).keydown(function(event) {
    if (runningMapEffect || $overlay.is(':visible')) {
      return;
    }

    switch (event.which) {
      case 40:
        handleAction(ACTION_MOVE_SOUTH);
        break;
      case 37:
        handleAction(ACTION_MOVE_WEST);
        break;
      case 190:
        handleAction(ACTION_WAIT);
        break;
      case 39:
        handleAction(ACTION_MOVE_EAST);
        break;
      case 38:
        handleAction(ACTION_MOVE_NORTH);
        break;
      default:
        return;
    }
  });

  $(document).keypress(function(event) {
    if (runningMapEffect || $popup.is(':visible')) {
      return;
    }

    $('.contextAction').each(function() {
      if (String.fromCharCode($(this).data('code')) ==
          String.fromCharCode(event.which).toLowerCase()) {
        $(this).click();
      }
    });
  });

  $('#newGameButton').click(function() {
    _set_difficulty(parseInt($('#difficultySelect').val()));
    _prompt_gender();
    refreshAll();
  });

  $('#continueGameButton').click(function() {
    hidePopup();
    _ULarnSetup(0);
    Module.ccall('restoregame', 'number', ['string'],
        [localStorage['ularn.savegame']]);
    $('#difficultySelect').val(_get_difficulty());
    refreshAll();
  });

  $('#settingsButton').click(function() {
    showPopup('settings');
  });

  $('#aboutButton').click(function() {
    showPopup('about');
  });

  $('#aboutBackButton').click(function() {
    showPopup('title');
  });

  $('#statusButton').click(function() {
    refreshStatus();
    showPopup('status');
  });

  $('#inventoryButton').click(showInventory);

  $('#castButton').click(function() {
    doAction(ACTION_CAST_SPELL);
  });

  $('#teleportOption').click(function(event) {
    event.preventDefault();
    doAction(ACTION_TELEPORT);
  });

  $('#identifyTrapsOption').click(function(event) {
    event.preventDefault();
    doAction(ACTION_IDENTIFY_TRAPS);
  });

  $('#showDiscoveriesOption').click(function(event) {
    event.preventDefault();
    doAction(ACTION_LIST_SPELLS);
    refreshAll();
  });

  $('#saveGameOption').click(function(event) {
    event.preventDefault();
    saveGame();
  });

  $('#settingsOption').click(function(event) {
    event.preventDefault();
    showPopup('settings');
  });

  $('#difficultySelect').change(function() {
    saveSettings();
  });

  $showGridlinesCheckbox.change(function() {
    saveSettings();
    refreshCanvas();
  });

  $('#settingsBackButton').click(function() {
    if ($canvas.is(':visible')) {
      hidePopup();
    } else {
      showPopup('title');
    }
  });

  $('.backButton').click(hidePopup);

  $('#numberInput input').keydown(function() {
    $('#numberInput .inputError').hide();
  });

  $('#numberInput form').submit(function(event) {
    event.preventDefault();

    var number = $('#numberInput input').val().trim();
    if (!number.match(/^\d+$/)) {
      $('#numberInput .inputError').show();
      return;
    }

    $('#numberInput .inputError').hide();
    hidePopup();
    runCallback($('#numberInput').data('callback'), parseInt(number));
  });

  $('#startOverButton').click(function() {
    $messages.empty();
    showTitle();
  });

  loadSettings();
  showTitle();
});

$(window).resize(function() {
  var canvasWidth = $(this).width();
  if (canvasWidth % 2 != 0) {
    canvasWidth--;
  }
  var canvasHeight = $(this).height();
  if (canvasHeight % 2 != 0) {
    canvasHeight--;
  }

  var backingStorePixelRatio =
      canvasContext.webkitBackingStorePixelRatio ||
      canvasContext.mozBackingStorePixelRatio ||
      canvasContext.msBackingStorePixelRatio ||
      canvasContext.oBackingStorePixelRatio ||
      canvasContext.backingStorePixelRatio ||
      1;

  var canvasPixelRatio = devicePixelRatio / backingStorePixelRatio;
  canvas.width = canvasWidth * canvasPixelRatio;
  canvas.height = canvasHeight * canvasPixelRatio;
  canvas.style.width = canvasWidth + 'px';
  canvas.style.height = canvasHeight + 'px';

  $('#popupContent').css('max-height', (canvasHeight - $('#topBar').height() -
      $('#bottomBar').height() - 50) + 'px');
  refreshCanvas();
});

function refreshCanvas(map) {
  if (map == null) {
    map = getMap();
    previousMap = map;
  }

  var xOffset = (canvas.width - TILE_WIDTH) / 2 - _get_player_x() * TILE_WIDTH;
  var yOffset = (canvas.height - TILE_HEIGHT) / 2 -
      _get_player_y() * TILE_HEIGHT;
  canvasContext.clearRect(0, 0, canvas.width, canvas.height);
  canvasContext.fillStyle = '#426b6b';
  canvasContext.fillRect(xOffset, yOffset,
      map.length * TILE_WIDTH, map[0].length * TILE_HEIGHT);
  for (var x = 0; x < map.length; x++) {
    for (var y = 0; y < map[x].length; y++) {
      if (map[x][y] != 191) {
        drawTile(map[x][y],
            x * TILE_WIDTH + xOffset, y * TILE_HEIGHT + yOffset);
      }
    }
  }

  drawGridlines();
}

function getMap() {
  var map = new Array(MAP_WIDTH);
  for (var x = 0; x < MAP_WIDTH; x++) {
    map[x] = new Array(MAP_HEIGHT);
    for (var y = 0; y < MAP_HEIGHT; y++) {
      map[x][y] = _get_map_tile(x, y);
    }
  }
  return map;
}

function drawTile(tileId, canvasX, canvasY) {
  var graphicsX = tileId % TILES_PER_GRAPHICS_ROW * TILE_WIDTH;
  var graphicsY = Math.floor(tileId / TILES_PER_GRAPHICS_ROW) * TILE_HEIGHT;
  canvasContext.drawImage(graphics, graphicsX, graphicsY,
      TILE_WIDTH, TILE_HEIGHT, canvasX, canvasY, TILE_WIDTH, TILE_HEIGHT);
}

function drawGridlines() {
  if (!$showGridlinesCheckbox.is(':checked')) {
    return;
  }

  var centerX = canvas.width / 2;
  var centerY = canvas.height / 2;
  canvasContext.strokeStyle = 'white';

  canvasContext.beginPath();
  canvasContext.arc(centerX, centerY, CENTER_CIRCLE_RADIUS, 0, 360);
  canvasContext.stroke();

  for (var i = 0; i < NUMBER_OF_DIRECTIONS; i++) {
    var angle = i * ANGLES_PER_DIRECTION - ANGLES_PER_DIRECTION / 2;
    canvasContext.beginPath();
    var radians = angle * Math.PI / 180;
    var x = Math.cos(radians);
    var y = Math.sin(radians);
    canvasContext.moveTo(centerX + CENTER_CIRCLE_RADIUS * x,
        centerY + CENTER_CIRCLE_RADIUS * y);
    var outerRadius = Math.max(canvas.width, canvas.height);
    canvasContext.lineTo(centerX + outerRadius * x, centerY + outerRadius * y);
    canvasContext.stroke();
  }
}

function refreshAll() {
  refreshMessages();
  runMapEffects();
}

function refreshMessages() {
  var messageHtml = Pointer_stringify(_get_text_html());
  if (messageHtml != '') {
    showMessage(messageHtml);
  }
}

function showMessage(messageHtml) {
  $messages.children('div:lt(' +
      Math.max($messages.children('div').length - 1, 0) + ')').remove();
  $messages.children('div').css('opacity', 0.5);
  $messages.append($('<div>').html(messageHtml));
}

function runMapEffects(effects) {
  if (effects == null) {
    effects = JSON.parse(Pointer_stringify(_get_map_effect_json()));
  }

  var playerX = _get_player_x();
  var playerY = _get_player_y();

  if (effects.length == 0) {
    runningMapEffect = false;
    if (_get_game_time() >= nextAutoSaveTime) {
      saveGame('Auto-saved game.');
      nextAutoSaveTime += AUTO_SAVE_INTERVAL;
    }
    _showcell(playerX, playerY);
    refreshCanvas();
    refreshQuickStatus();
    refreshOptions();
    refreshScore();
    return;
  }

  runningMapEffect = true;
  refreshCanvas(previousMap);
  var map = getMap();
  var xOffset = (canvas.width - TILE_WIDTH) / 2 - playerX * TILE_WIDTH;
  var yOffset = (canvas.height - TILE_HEIGHT) / 2 - playerY * TILE_HEIGHT;
  for (var i = 0; i < effects[0].length; i++) {
    var effect = effects[0][i];
    for (var j = 0; j < effect.tileIds.length; j++) {
      drawTile(effect.tileIds[j], effect.x * TILE_WIDTH + xOffset,
          effect.y * TILE_HEIGHT + yOffset);
      previousMap[effect.x][effect.y] = map[effect.x][effect.y];
    }
  }

  setTimeout(function() {
    runMapEffects(effects.slice(1));
  }, 75);
}

function refreshQuickStatus() {
  $('#stat-hp').text(_get_stat_hp());
  $('#stat-hpMax').text(_get_stat_hp_max());
  $('#stat-spells').text(_get_stat_spells());
  $('#stat-spellMax').text(_get_stat_spell_max());
  $('#stat-gold').text(_get_stat_gold());
  $('#stat-levelName').text(Pointer_stringify(_get_stat_level_name()));
}

function refreshStatus() {
  var status = JSON.parse(Pointer_stringify(_get_status_json()));
  var effects = JSON.parse(Pointer_stringify(_get_effects_json()));
  for (var key in status) {
    $('#stat-' + key).text(status[key]);
  }
  $('#stat-effects').text(effects.length > 0 ? effects.join(', ') : 'none');
}

function refreshOptions() {
  var options = JSON.parse(Pointer_stringify(_get_option_json()));
  switch (options.inputType) {
    case 'number':
      promptNumber(options.callback);
      break;
    case 'direction':
      promptDirection(options.callback);
      break;
    default:
      if (options.callback == '') {
        showContextActions(options.options);
      } else if (options.callback == 'act_on_object') {
        showOverlay();
        showContextActions(options.options);
      } else {
        showMenu(options.options, options.callback);
      }
      break;
  }
}

function promptNumber(callback) {
  refreshPopupHeader();
  $('#numberInput input').val('');
  $('#numberInput').data('callback', callback);
  showPopup('numberInput');
  $('#numberInput input').focus();
}

function promptDirection(callback) {
  $('#bottomBar').hide();
  $('#inputDirectionMessage').data('callback', callback).show();
}

function refreshPopupHeader() {
  var headerHtml = Pointer_stringify(_get_header_html());
  if (headerHtml == '') {
    $('.popupHeader').hide();
  } else {
    $('.popupHeader').html(headerHtml).show();
  }
}

function showContextActions(options) {
  $('.contextAction').remove();
  for (var i = 0; i < options.length; i++) {
    var option = options[i];
    $('<button class="btn btn-primary btn-sm contextAction">')
      .data('code', option.code)
      .html(option.text.replace(
          new RegExp(String.fromCharCode(option.code), 'i'), function(c) {
        return '<span class="shortcutKey">' + c + '</span>';
      })).click(function() {
        hideOverlay();
        _act_on_object(parseInt($(this).data('code')));
        refreshAll();
      }).appendTo($('#bottomBar'));
  }
}

function showMenu(options, callback) {
  refreshPopupHeader();
  $('#menuItems').empty();
  for (var i = 0; i < options.length; i++) {
    var option = options[i];
    $('<button class="btn btn-default btn-block">')
      .data('code', option.code)
      .data('callback', callback)
      .append($('<span>').text(option.text))
      .append($('<small>').text(option.subtext))
      .click(function() {
        hidePopup();
        var code = parseInt($(this).data('code'));
        if (code != -1) {
          runCallback($(this).data('callback'), code);
        }
      }).appendTo($('#menuItems'));
  }
  showPopup('menu');
}

function runCallback(callback, argument) {
  window['_' + callback](argument);
  _move_world();
  refreshAll();
}

function refreshScore() {
  var scoreJson = Pointer_stringify(_get_player_score_json());
  if (scoreJson == '') {
    return;
  }
  localStorage.removeItem('ularn.savegame');
  var score = JSON.parse(scoreJson);
  for (var key in score) {
    $('#score-' + key).text(score[key]);
  }
  showPopup('score');
}

function showPopup(which) {
  showOverlay();
  $('#popupContent > div').hide();
  $('#' + which).show();
  $popup.show();
  $('#popupContent').scrollTop(0);
}

function hidePopup() {
  $popup.hide();
  hideOverlay();
}

function showOverlay() {
  $overlay.show();
  $('#bottomBar button').prop('disabled', true);
}

function hideOverlay() {
  $('#bottomBar button').prop('disabled', false);
  $overlay.hide();
}

function handleAction(action) {
  if ($('#inputDirectionMessage').is(':visible')) {
    if (action == ACTION_WAIT) {
      return;
    }
    $('#inputDirectionMessage').hide();
    $('#bottomBar').show();
    runCallback($('#inputDirectionMessage').data('callback'), action);
  } else {
    doAction(action);
  }
}

function doAction(action) {
  if (_act(action)) {
    refreshAll();
  }
}

function showInventory() {
  var inventory = JSON.parse(Pointer_stringify(_get_inventory_json()));
  $('.popupHeader').hide();
  $('#menuItems').empty();
  for (var i = 0; i < inventory.length; i++) {
    var item = inventory[i];
    $('<button class="btn btn-default btn-block">')
      .data('code', item.code)
      .data('options', item.options)
      .text(item.name)
      .click(function() {
        var code = $(this).data('code');
        var options = $(this).data('options');
        $('#menuItems').empty();
        for (var i = 0; i < options.length; i++) {
          var option = options[i];
          $('<button class="btn btn-default btn-block">')
            .data('code', code)
            .data('action', option.action)
            .text(option.text)
            .click(function() {
              hidePopup();
              runCallback($(this).data('action'),
                  parseInt($(this).data('code')));
            }).appendTo($('#menuItems'));
        }
        $('<button class="btn btn-default btn-block">Back</button>')
            .click(showInventory).appendTo($('#menuItems'));
      }).appendTo($('#menuItems'));
  }
  $('<button class="btn btn-default btn-block backButton">Back</button>')
      .click(hidePopup).appendTo($('#menuItems'));
  showPopup('menu');
}

function saveGame(message) {
  localStorage['ularn.savegame'] = Pointer_stringify(_get_savegame());
  _free_savegame();
  showMessage(message || 'Game saved.');
}

function showTitle() {
  $('#topBar, canvas, #bottomBar').hide();
  $('#continueGameButton').toggle(localStorage['ularn.savegame'] != null);
  $('#difficultySelect').prop('disabled', false);
  nextAutoSaveTime = Number.MAX_VALUE;
  showPopup('title');
}

function showGame() {
  $('#topBar, canvas, #bottomBar').show();
  $('#difficultySelect').prop('disabled', true);
  nextAutoSaveTime = _get_game_time() + AUTO_SAVE_INTERVAL;
}

function saveSettings() {
  localStorage['ularn.settings'] = JSON.stringify({
    difficulty: $('#difficultySelect').val(),
    showGridlines: $showGridlinesCheckbox.prop('checked')
  });
}

function loadSettings() {
  var settings = localStorage['ularn.settings'];
  if (settings == null) {
    return;
  }
  settings = JSON.parse(settings);
  $('#difficultySelect').val(settings.difficulty);
  $showGridlinesCheckbox.prop('checked', settings.showGridlines);
}

function clearMouseDownTimeoutId() {
  if (mouseDownTimeoutId != null) {
    clearTimeout(mouseDownTimeoutId);
    mouseDownTimeoutId = null;
  }
}

function handleMouseDown(repeatDelay) {
  if ($overlay.is(':visible')) {
    return;
  }
  var x = mouseX - $canvas.width() / 2;
  var y = mouseY - $canvas.height() / 2;
  var distance = Math.sqrt(x * x + y * y);
  if (distance <= CENTER_CIRCLE_RADIUS) {
    handleAction(ACTION_WAIT);
  } else {
    var angle = Math.atan2(y, x) * 180 / Math.PI + 180;
    var direction = Math.floor((angle - ANGLES_PER_DIRECTION / 2) /
        ANGLES_PER_DIRECTION) % NUMBER_OF_DIRECTIONS;
    switch (direction) {
      case 0:
        handleAction(ACTION_MOVE_NORTH);
        break;
      case 1:
        handleAction(ACTION_MOVE_EAST);
        break;
      case 2:
        handleAction(ACTION_MOVE_SOUTH);
        break;
      case 3:
        handleAction(ACTION_MOVE_WEST);
        break;
    }
  }
  clearMouseDownTimeoutId();
  mouseDownTimeoutId = setTimeout(function() {
    handleMouseDown(20);
  }, repeatDelay);
}